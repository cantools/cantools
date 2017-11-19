# Load and dump a CAN database in SYM format.

import logging
from collections import OrderedDict
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
from ..signal import Signal
from ..message import Message
from ..database import Database


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
    type_ = (Keyword('unsigned')
             | Keyword('signed')
             | Keyword('float')
             | Keyword('double'))

    version = Group(Keyword('FormatVersion')
                    + assign
                    + Keyword('6.0'))

    title = Group(Keyword('Title')
                  + assign
                  + QuotedString('"'))

    enum_value = Group(number
                       + assign
                       + QuotedString('"'))

    enum = Group(Suppress(Keyword('Enum'))
                 + assign
                 + name
                 + Suppress(lp)
                 + Group(delimitedList(enum_value))
                 + Suppress(rp))

    sig_unit = Group(Literal('/u:') + word)
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
                           + Optional(sig_offset)
                           + Optional(sig_min)
                           + Optional(sig_max)
                           + Optional(sig_default)
                           + Optional(sig_long_name)
                           + Optional(sig_enum)))

    symbol = Group(Suppress(lb)
                   + name
                   + Suppress(rb)
                   + Group(Keyword('ID')
                           + assign
                           + word)
                   + Group(Keyword('Len')
                           + assign
                           + positive_integer)
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
               + title
               + Group(OneOrMore(section))
               + StringEnd())
    grammar.ignore(dblSlashComment)

    return grammar


def _get_section(tokens, name):
    for section in tokens[2]:
        if section[0] == name:
            return section[1]


def _load_enums(tokens):
    enums = {}

    section = _get_section(tokens, '{ENUMS}')

    for name, values in section:
        enums[name] = OrderedDict(
            (int(v[0]), v[1]) for v in values)

    return enums


def _load_signal(tokens, enums):
    # Default values.
    name = tokens[0]
    is_signed = False
    is_float = False
    byte_order = 'big_endian'
    offset = 0
    factor = 1
    unit = None
    minimum = None
    maximum = None
    enum = None
    length = 0

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
            byte_order = 'little_endian'
    except IndexError:
        pass

    # The rest.
    for key, value in tokens[4]:
        if key == '/u:':
            unit = value
        elif key == '/f:':
            factor = int(value)
        elif key == '/o:':
            offset = int(value)
        elif key == '/min:':
            minimum = int(value)
        elif key == '/max:':
            maximum = int(value)
        elif key == '/e:':
            enum = enums[value]
        else:
            LOGGER.debug("Ignoring unsupported message attribute '%s'.", key)

    return Signal(name=name,
                  start=offset,
                  length=length,
                  nodes=[],
                  byte_order=byte_order,
                  is_signed=is_signed,
                  scale=factor,
                  offset=offset,
                  minimum=minimum,
                  maximum=maximum,
                  unit=unit,
                  choices=enum,
                  comment=None,
                  is_multiplexer=False,
                  multiplexer_id=None,
                  is_float=is_float)


def _load_signals(tokens, enums):
    signals = {}

    section = _get_section(tokens, '{SIGNALS}')

    for signal in section:
        signal = _load_signal(signal, enums)
        signals[signal.name] = signal

    return signals


def _load_message_signal(tokens, signals):
    signal = signals[tokens[1]]

    return Signal(name=signal.name,
                  start=int(tokens[2]),
                  length=signal.length,
                  nodes=signal.nodes,
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
                  multiplexer_id=signal.multiplexer_id,
                  is_float=signal.is_float)


def _load_message_signals(tokens, signals):
    return [_load_message_signal(signal, signals)
            for signal in tokens]


def _load_message(frame_id, tokens, signals):
    # Default values.
    name = tokens[0]
    length = 0
    cycle_time = None
    is_extended_frame = False

    # Length.
    try:
        length = int(tokens[2][1])
    except IndexError:
        pass

    # Cycle time.
    if len(tokens[3]) == 1:
        cycle_time = int(tokens[3][1])

    return Message(frame_id=frame_id,
                   is_extended_frame=is_extended_frame,
                   name=name,
                   length=length,
                   nodes=[],
                   send_type=None,
                   cycle_time=cycle_time,
                   signals=_load_message_signals(tokens[6], signals),
                   comment=None,
                   bus_name=None)


def _load_messages(tokens, signals):
    messages = []

    def load_section(name):
        def message_frame_ids(message):
            def to_int(string):
                return int(string[:-1], 16)

            if '-' in message[1][1]:
                minimum, maximum = message[1][1].split('-')
                return range(to_int(minimum), to_int(maximum) + 1)
            else:
                return [int(to_int(message[1][1]))]

        section = _get_section(tokens, name)

        for message in section:
            for frame_id in message_frame_ids(message):
                messages.append(_load_message(frame_id,
                                              message,
                                              signals))

    load_section('{SEND}')
    load_section('{RECEIVE}')
    load_section('{SENDRECEIVE}')

    return messages


def _load_version(tokens):
    return tokens[0][1]


def load_string(string):
    """Parse given string.

    """

    if not string.startswith('FormatVersion=6.0'):
        raise ValueError('Only SYM version 6.0 is supported.')

    grammar = _create_grammar_6_0()
    tokens = grammar.parseString(string)

    version = _load_version(tokens)
    enums = _load_enums(tokens)
    signals = _load_signals(tokens, enums)
    messages = _load_messages(tokens, signals)

    return Database(messages,
                    [],
                    [],
                    version,
                    [],
                    [])
