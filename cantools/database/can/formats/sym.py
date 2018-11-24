# Load and dump a CAN database in SYM format.

import logging
from collections import OrderedDict as odict
from decimal import Decimal

from pyparsing import Word
from pyparsing import Literal
from pyparsing import Keyword
from pyparsing import Optional
from pyparsing import Suppress
from pyparsing import Group
from pyparsing import QuotedString
from pyparsing import StringEnd
from pyparsing import printables
from pyparsing import nums
from pyparsing import alphas
from pyparsing import ZeroOrMore
from pyparsing import OneOrMore
from pyparsing import delimitedList
from pyparsing import dblSlashComment
from pyparsing import ParseException
from pyparsing import ParseSyntaxException

from ..signal import Signal
from ..signal import Decimal as SignalDecimal
from ..message import Message
from ..internal_database import InternalDatabase

from .utils import num
from ...errors import ParseError


LOGGER = logging.getLogger(__name__)


def _create_grammar_6_0():
    """Create the SYM 6.0 grammar.

    """

    word = Word(printables.replace(';', '').replace(':', ''))
    positive_integer = Word(nums)
    number = Word(nums + '.Ee-+')
    lp = Suppress(Literal('('))
    rp = Suppress(Literal(')'))
    lb = Suppress(Literal('['))
    rb = Suppress(Literal(']'))
    name = Word(alphas + nums + '_-').setWhitespaceChars(' ')
    assign = Suppress(Literal('='))
    comma = Suppress(Literal(','))
    type_ = name

    version = Group(Keyword('FormatVersion')
                    - assign
                    - Keyword('6.0'))

    title = Group(Keyword('Title')
                  - assign
                  - QuotedString('"'))

    enum_value = Group(number
                       + assign
                       + QuotedString('"'))

    enum = Group(Suppress(Keyword('Enum'))
                 - assign
                 - name
                 - Suppress(lp)
                 + Group(delimitedList(enum_value))
                 - Suppress(rp))

    sig_unit = Group(Literal('/u:') + word)
    sig_factor = Group(Literal('/f:') + word)
    sig_offset = Group(Literal('/o:') + word)
    sig_min = Group(Literal('/min:') + word)
    sig_max = Group(Literal('/max:') + word)
    sig_default = Group(Literal('/d:') + word)
    sig_long_name = Group(Literal('/ln:') + word)
    sig_enum = Group(Literal('/e:') + word)

    signal = Group(Suppress(Keyword('Sig'))
                   - Suppress(assign)
                   - name
                   - type_
                   + Group(Optional(positive_integer))
                   + Group(Optional(Keyword('-m')))
                   + Group(Optional(sig_unit)
                           + Optional(sig_factor)
                           + Optional(sig_offset)
                           + Optional(sig_min)
                           + Optional(sig_max)
                           + Optional(sig_default)
                           + Optional(sig_long_name)
                           + Optional(sig_enum)))

    symbol = Group(Suppress(lb)
                   - name
                   - Suppress(rb)
                   - Group(Optional(Keyword('ID')
                                    + assign
                                    + word))
                   - Group(Keyword('Len')
                           + assign
                           + positive_integer)
                   + Group(Optional(Keyword('Mux')
                                    + assign
                                    + word
                                    + positive_integer
                                    + comma
                                    + positive_integer
                                    + positive_integer))
                   + Group(Optional(Keyword('CycleTime')
                                    + assign
                                    + positive_integer))
                   + Group(Optional(Keyword('Timeout')
                                    + assign
                                    + positive_integer))
                   + Group(Optional(Keyword('MinInterval')
                                    + assign
                                    + positive_integer))
                   + Group(ZeroOrMore(Group(Keyword('Sig')
                                            + assign
                                            + name
                                            + positive_integer))))

    enums = Group(Keyword('{ENUMS}')
                  + Group(ZeroOrMore(enum)))
    signals = Group(Keyword('{SIGNALS}')
                    + Group(ZeroOrMore(signal)))
    send = Group(Keyword('{SEND}')
                 + Group(ZeroOrMore(symbol)))
    receive = Group(Keyword('{RECEIVE}')
                    + Group(ZeroOrMore(symbol)))
    sendreceive = Group(Keyword('{SENDRECEIVE}')
                        + Group(ZeroOrMore(symbol)))

    section = (enums
               | signals
               | send
               | receive
               | sendreceive)

    grammar = (version
               - title
               + Group(OneOrMore(section))
               + StringEnd())
    grammar.ignore(dblSlashComment)

    return grammar


def _get_section_tokens(tokens, name):
    for section in tokens[2]:
        if section[0] == name:
            return section[1]


def _load_enums(tokens):
    section = _get_section_tokens(tokens, '{ENUMS}')
    enums = {}

    for name, values in section:
        enums[name] = odict((num(v[0]), v[1]) for v in values)

    return enums


def _load_signal(tokens, enums):
    # Default values.
    name = tokens[0]
    is_signed = False
    is_float = False
    byte_order = 'little_endian'
    offset = 0
    factor = 1
    unit = None
    minimum = None
    maximum = None
    enum = None
    length = 0
    decimal = SignalDecimal(Decimal(factor), Decimal(offset))
    
    # Type and length.
    type_ = tokens[1]

    if type_ in 'signed':
        is_signed = True
        length = int(tokens[2][0])
    elif type_ == 'unsigned':
        length = int(tokens[2][0])
    elif type_ == 'float':
        is_float = True
        length = 32
    elif type_ == 'double':
        is_float = True
        length = 64
    else:
        LOGGER.debug("Ignoring unsupported type '%s'.", type_)

    # Byte order.
    try:
        if tokens[3][0] == '-m':
            byte_order = 'big_endian'
    except IndexError:
        pass

    # The rest.
    for key, value in tokens[4]:
        if key == '/u:':
            unit = value
        elif key == '/f:':
            factor = num(value)
            decimal.scale = Decimal(value)
        elif key == '/o:':
            offset = num(value)
            decimal.offset = Decimal(value)
        elif key == '/min:':
            minimum = num(value)
            decimal.minimum = Decimal(value)
        elif key == '/max:':
            maximum = num(value)
            decimal.maximum = Decimal(value)
        elif key == '/e:':
            enum = enums[value]
        else:
            LOGGER.debug("Ignoring unsupported message attribute '%s'.", key)

    return Signal(name=name,
                  start=offset,
                  length=length,
                  receivers=[],
                  byte_order=byte_order,
                  is_signed=is_signed,
                  scale=factor,
                  offset=offset,
                  minimum=minimum,
                  maximum=maximum,
                  unit=unit,
                  choices=enum,
                  is_multiplexer=False,
                  is_float=is_float,
                  decimal=decimal)


def _load_signals(tokens, enums):
    section = _get_section_tokens(tokens, '{SIGNALS}')
    signals = {}

    for signal in section:
        signal = _load_signal(signal, enums)
        signals[signal.name] = signal

    return signals


def _load_message_signal(tokens,
                         signals,
                         multiplexer_signal,
                         multiplexer_ids):
    signal = signals[tokens[1]]
    start = int(tokens[2])

    if signal.byte_order == 'big_endian':
        start = (8 * (start // 8) + (7 - (start % 8)))

    return Signal(name=signal.name,
                  start=start,
                  length=signal.length,
                  receivers=signal.receivers,
                  byte_order=signal.byte_order,
                  is_signed=signal.is_signed,
                  scale=signal.scale,
                  offset=signal.offset,
                  minimum=signal.minimum,
                  maximum=signal.maximum,
                  unit=signal.unit,
                  choices=signal.choices,
                  comment=signal.comment,
                  is_multiplexer=signal.is_multiplexer,
                  multiplexer_ids=multiplexer_ids,
                  multiplexer_signal=multiplexer_signal,
                  is_float=signal.is_float,
                  decimal=signal.decimal)


def _load_message_signals_inner(message_tokens,
                                signals,
                                multiplexer_signal=None,
                                multiplexer_ids=None):
    return [
        _load_message_signal(signal,
                             signals,
                             multiplexer_signal,
                             multiplexer_ids)
        for signal in message_tokens[7]
    ]


def _load_muxed_message_signals(message_tokens,
                                message_section_tokens,
                                signals):
    mux_tokens = message_tokens[3]
    multiplexer_signal = mux_tokens[1]
    result = [
        Signal(name=multiplexer_signal,
               start=int(mux_tokens[2]),
               length=int(mux_tokens[3]),
               byte_order='little_endian',
               is_multiplexer=True)
    ]

    multiplexer_ids = [int(mux_tokens[4])]
    result += _load_message_signals_inner(message_tokens,
                                          signals,
                                          multiplexer_signal,
                                          multiplexer_ids)

    for tokens in message_section_tokens:
        if tokens[0] == message_tokens[0] and tokens != message_tokens:
            multiplexer_ids = [int(tokens[3][4])]
            result += _load_message_signals_inner(tokens,
                                                  signals,
                                                  multiplexer_signal,
                                                  multiplexer_ids)

    return result


def _is_multiplexed(message_tokens):
    return len(message_tokens[3]) > 0


def _load_message_signals(message_tokens,
                          message_section_tokens,
                          signals):
    if _is_multiplexed(message_tokens):
        return _load_muxed_message_signals(message_tokens,
                                           message_section_tokens,
                                           signals)
    else:
        return _load_message_signals_inner(message_tokens,
                                           signals)


def _load_message(frame_id,
                  is_extended_frame,
                  message_tokens,
                  message_section_tokens,
                  signals,
                  strict):
    # Default values.
    name = message_tokens[0]
    length = int(message_tokens[2][1])
    cycle_time = None

    # Cycle time.
    try:
        cycle_time = num(message_tokens[4][1])
    except IndexError:
        pass

    return Message(frame_id=frame_id,
                   is_extended_frame=is_extended_frame,
                   name=name,
                   length=length,
                   senders=[],
                   send_type=None,
                   cycle_time=cycle_time,
                   signals=_load_message_signals(message_tokens,
                                                 message_section_tokens,
                                                 signals),
                   comment=None,
                   bus_name=None,
                   strict=strict)


def _parse_message_frame_ids(message):
    def to_int(string):
        return int(string[:-1], 16)

    def is_extended_frame(string):
        return len(string) == 9

    if '-' in message[1][1]:
        minimum, maximum = message[1][1].split('-')
    else:
        minimum = maximum = message[1][1]

    frame_ids = range(to_int(minimum), to_int(maximum) + 1)

    return frame_ids, is_extended_frame(minimum)


def _load_message_section(section_name, tokens, signals, strict):
    def has_frame_id(message):
        return len(message[1]) > 0

    message_section_tokens = _get_section_tokens(tokens, section_name)
    messages = []

    for message_tokens in message_section_tokens:
        if not has_frame_id(message_tokens):
            continue

        frame_ids, is_extended_frame = _parse_message_frame_ids(message_tokens)

        for frame_id in frame_ids:
            message = _load_message(frame_id,
                                    is_extended_frame,
                                    message_tokens,
                                    message_section_tokens,
                                    signals,
                                    strict)
            messages.append(message)

    return messages


def _load_messages(tokens, signals, strict):
    messages = _load_message_section('{SEND}', tokens, signals, strict)
    messages += _load_message_section('{RECEIVE}', tokens, signals, strict)
    messages += _load_message_section('{SENDRECEIVE}', tokens, signals, strict)

    return messages


def _load_version(tokens):
    return tokens[0][1]


def load_string(string, strict=True):
    """Parse given string.

    """

    if not string.startswith('FormatVersion=6.0'):
        raise ParseError('Only SYM version 6.0 is supported.')

    grammar = _create_grammar_6_0()

    try:
        tokens = grammar.parseString(string)
    except (ParseException, ParseSyntaxException) as e:
        raise ParseError(
            "Invalid SYM syntax at line {}, column {}: '{}': {}.".format(
                e.lineno,
                e.column,
                e.markInputline(),
                e.msg))

    version = _load_version(tokens)
    enums = _load_enums(tokens)
    signals = _load_signals(tokens, enums)
    messages = _load_messages(tokens, signals, strict)

    return InternalDatabase(messages,
                            [],
                            [],
                            version,
                            [])
