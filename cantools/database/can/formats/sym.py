# Load and dump a CAN database in SYM format.

import re
import logging
from collections import OrderedDict as odict
from decimal import Decimal

import textparser
from textparser import Sequence
from textparser import choice
from textparser import ZeroOrMore
from textparser import ZeroOrMoreDict
from textparser import DelimitedList
from textparser import tokenize_init
from textparser import Token
from textparser import TokenizeError
from textparser import Optional

from ..signal import Signal
from ..signal import Decimal as SignalDecimal
from ..message import Message
from ..internal_database import InternalDatabase

from .utils import num
from ...errors import ParseError


LOGGER = logging.getLogger(__name__)


class Parser60(textparser.Parser):
    """Create the SYM 6.0 parser.

    """

    KEYWORDS = set([
        'FormatVersion',
        'Title',
        'UniqueVariables',
        'FloatDecimalPlaces',
        'BRS',
        'Enum',
        'Sig',
        'ID',
        'Len',
        'Mux',
        'CycleTime',
        'Timeout',
        'MinInterval',
        'Sig',
        'Color',
        'Var'
    ])

    def tokenize(self, string):

        names = {
            'LPAREN':      '(',
            'RPAREN':      ')',
            'LBRACE':      '[',
            'RBRACE':      ']',
            'COMMA':       ',',
            'ASSIGN':      '=',
            'ENUMS':       '{ENUMS}',
            'SIGNALS':     '{SIGNALS}',
            'SEND':        '{SEND}',
            'RECEIVE':     '{RECEIVE}',
            'SENDRECEIVE': '{SENDRECEIVE}',
            'U':           '/u:',
            'F':           '/f:',
            'O':           '/o:',
            'MIN':         '/min:',
            'MAX':         '/max:',
            'D':           '/d:',
            'LN':          '/ln:',
            'E':           '/e:',
            'P':           '/p:',
            'M':           '-m'
        }

        token_specs = [
            ('SKIP',        r'[ \r\n\t]+|//.*?\n'),
            ('NUMBER',      r'-?\d+\.?\d*([eE][+-]?\d+)?'),
            ('WORD',        r'[A-Za-z0-9_\*]+'),
            ('STRING',      r'"(\\"|[^"])*?"'),
            ('LPAREN',      r'\('),
            ('RPAREN',      r'\)'),
            ('LBRACE',      r'\['),
            ('RBRACE',      r'\]'),
            ('COMMA',       r','),
            ('ASSIGN',      r'='),
            ('ENUMS',       r'\{ENUMS\}'),
            ('SIGNALS',     r'\{SIGNALS\}'),
            ('SEND',        r'\{SEND\}'),
            ('RECEIVE',     r'\{RECEIVE\}'),
            ('SENDRECEIVE', r'\{SENDRECEIVE\}'),
            ('U',           r'/u:'),
            ('F',           r'/f:'),
            ('O',           r'/o:'),
            ('MIN',         r'/min:'),
            ('MAX',         r'/max:'),
            ('D',           r'/d:'),
            ('LN',          r'/ln:'),
            ('E',           r'/e:'),
            ('P',           r'/p:'),
            ('M',           r'\-m'),
            ('MISMATCH',    r'.')
        ]

        tokens, token_regex = tokenize_init(token_specs)

        for mo in re.finditer(token_regex, string, re.DOTALL):
            kind = mo.lastgroup

            if kind == 'SKIP':
                pass
            elif kind == 'STRING':
                value = mo.group(kind)[1:-1].replace('\\"', '"')
                tokens.append(Token(kind, value, mo.start()))
            elif kind != 'MISMATCH':
                value = mo.group(kind)

                if value in self.KEYWORDS:
                    kind = value

                if kind in names:
                    kind = names[kind]

                tokens.append(Token(kind, value, mo.start()))
            else:
                raise TokenizeError(string, mo.start())

        return tokens

    def grammar(self):
        word = choice('WORD', *list(self.KEYWORDS))
        version = Sequence('FormatVersion', '=', 'NUMBER')
        title = Sequence('Title' , '=', 'STRING')
        unique_variables = Sequence('UniqueVariables' , '=', word)
        float_decimal_places = Sequence('FloatDecimalPlaces' , '=', 'NUMBER')
        bit_rate_switch = Sequence('BRS' , '=', word)

        enum_value = Sequence('NUMBER', '=', 'STRING')
        enum = Sequence('Enum', '=', word,
                        '(', Optional(DelimitedList(enum_value)), ')')

        sig_unit = Sequence('/u:', word)
        sig_factor = Sequence('/f:', 'NUMBER')
        sig_offset = Sequence('/o:', 'NUMBER')
        sig_min = Sequence('/min:', 'NUMBER')
        sig_max = Sequence('/max:', 'NUMBER')
        sig_default = Sequence('/d:', choice('NUMBER', 'WORD'))
        sig_long_name = Sequence('/ln:', 'STRING')
        sig_enum = Sequence('/e:', word)
        sig_places = Sequence('/p:', 'NUMBER')

        signal = Sequence('Sig', '=', word, word,
                          Optional('NUMBER'),
                          Optional('-m'),
                          ZeroOrMore(choice(sig_unit,
                                            sig_factor,
                                            sig_offset,
                                            sig_min,
                                            sig_max,
                                            sig_default,
                                            sig_long_name,
                                            sig_enum,
                                            sig_places)))

        variable = Sequence('Var', '=', word, word,
                            'NUMBER', ',', 'NUMBER',
                            Optional('-m'),
                            ZeroOrMore(choice(sig_unit,
                                              sig_factor,
                                              sig_offset,
                                              sig_min,
                                              sig_max,
                                              sig_default,
                                              sig_long_name,
                                              sig_enum,
                                              sig_places)))

        symbol = Sequence('[', word, ']',
                          Optional(Sequence('ID', '=', 'NUMBER', word,
                                            Optional(Sequence('NUMBER', word)))),
                          Sequence('Len', '=', 'NUMBER'),
                          ZeroOrMoreDict(choice(
                              Sequence('Mux', '=', word, 'NUMBER', ',',
                                       'NUMBER', 'NUMBER'),
                              Sequence('CycleTime', '=', 'NUMBER'),
                              Sequence('Timeout', '=', 'NUMBER'),
                              Sequence('MinInterval', '=', 'NUMBER'),
                              Sequence('Color', '=', 'NUMBER', 'WORD'),
                              variable,
                              Sequence('Sig', '=', word, 'NUMBER'))))

        enums = Sequence('{ENUMS}', ZeroOrMore(enum))
        signals = Sequence('{SIGNALS}', ZeroOrMore(signal))
        send = Sequence('{SEND}', ZeroOrMore(symbol))
        receive = Sequence('{RECEIVE}', ZeroOrMore(symbol))
        sendreceive = Sequence('{SENDRECEIVE}', ZeroOrMore(symbol))

        section = choice(enums,
                         signals,
                         send,
                         receive,
                         sendreceive)

        grammar = Sequence(version,
                           ZeroOrMore(choice(unique_variables,
                                             float_decimal_places,
                                             title,
                                             bit_rate_switch)),
                           ZeroOrMore(section))

        return grammar


def _get_section_tokens(tokens, name):
    for section in tokens[2]:
        if section[0] == name:
            return section[1]

    return []


def _get_enum(enums, name):
    try:
        return enums[name]
    except KeyError:
        raise ParseError("Enum '{}' is not defined.".format(name))


def _load_enums(tokens):
    section = _get_section_tokens(tokens, '{ENUMS}')
    enums = {}

    for _, _, name, _, values, _ in section:
        if values:
            values = values[0]

        enums[name] = odict((num(v[0]), v[2]) for v in values)

    return enums


def _load_signal_type_and_length(type_, tokens, enums):
    # Default values.
    is_signed = False
    is_float = False
    length = 0
    enum = None
    minimum = None
    maximum = None
    decimal = SignalDecimal()

    if type_ == 'signed':
        is_signed = True
        length = int(tokens[0])
    elif type_ == 'unsigned':
        length = int(tokens[0])
    elif type_ == 'float':
        is_float = True
        length = 32
    elif type_ == 'double':
        is_float = True
        length = 64
    elif type_ == 'bit':
        # As unsigned integer for now.
        length = 1
        minimum = 0
        maximum = 1
        decimal.minimum = Decimal('0')
        decimal.maximum = Decimal('1')
    elif type_ == 'char':
        # As unsigned integer for now.
        length = 8
    elif type_ in ['string', 'raw']:
        # As unsigned integer for now.
        length = int(tokens[0])
    else:
        # Enum. As unsigned integer for now.
        length = int(tokens[0])
        enum = _get_enum(enums, type_)

    return is_signed, is_float, length, enum, minimum, maximum, decimal


def _load_signal_attributes(tokens, enum, enums, minimum, maximum, decimal):
    # Default values.
    factor = 1
    offset = 0
    unit = None
    decimal.scale = Decimal(factor)
    decimal.offset = Decimal(offset)

    for key, value in tokens:
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
            enum = _get_enum(enums, value)
        else:
            LOGGER.debug("Ignoring unsupported message attribute '%s'.", key)

    return unit, factor, offset, enum, minimum, maximum, decimal


def _load_signal(tokens, enums):
    # Default values.
    name = tokens[2]
    byte_order = 'little_endian'

    # Type and length.
    (is_signed,
     is_float,
     length,
     enum,
     minimum,
     maximum,
     decimal) = _load_signal_type_and_length(tokens[3],
                                             tokens[4],
                                             enums)

    # Byte order.
    if tokens[5] == ['-m']:
        byte_order = 'big_endian'

    # The rest.
    unit, factor, offset, enum, minimum, maximum, decimal = _load_signal_attributes(
        tokens[6],
        enum,
        enums,
        minimum,
        maximum,
        decimal)

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
    signal = signals[tokens[2]]
    start = int(tokens[3])

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


def _load_message_variable(tokens, enums):
    # Default values.
    name = tokens[2]
    byte_order = 'little_endian'
    start = int(tokens[4])

    # Type and length.
    (is_signed,
     is_float,
     length,
     enum,
     minimum,
     maximum,
     decimal) = _load_signal_type_and_length(tokens[3],
                                             [tokens[6]],
                                             enums)

    # Byte order.
    if tokens[7] == ['-m']:
        byte_order = 'big_endian'

    # The rest.
    unit, factor, offset, enum, minimum, maximum, decimal = _load_signal_attributes(
        tokens[8],
        enum,
        enums,
        minimum,
        maximum,
        decimal)

    if byte_order == 'big_endian':
        start = (8 * (start // 8) + (7 - (start % 8)))

    return Signal(name=name,
                  start=start,
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


def _load_message_signals_inner(message_tokens,
                                signals,
                                enums,
                                multiplexer_signal=None,
                                multiplexer_ids=None):
    return [
        _load_message_signal(signal,
                             signals,
                             multiplexer_signal,
                             multiplexer_ids)
        for signal in message_tokens[5].get('Sig', [])
    ] + [
        _load_message_variable(variable, enums)
        for variable in message_tokens[5].get('Var', [])
    ]


def _load_muxed_message_signals(message_tokens,
                                message_section_tokens,
                                signals,
                                enums):
    mux_tokens = message_tokens[5]['Mux'][0]
    multiplexer_signal = mux_tokens[2]
    result = [
        Signal(name=multiplexer_signal,
               start=int(mux_tokens[3]),
               length=int(mux_tokens[5]),
               byte_order='little_endian',
               is_multiplexer=True)
    ]

    multiplexer_ids = [int(mux_tokens[6])]
    result += _load_message_signals_inner(message_tokens,
                                          signals,
                                          enums,
                                          multiplexer_signal,
                                          multiplexer_ids)

    for tokens in message_section_tokens:
        if tokens[1] == message_tokens[1] and tokens != message_tokens:
            mux_tokens = tokens[5]['Mux'][0]
            multiplexer_ids = [int(mux_tokens[6])]
            result += _load_message_signals_inner(tokens,
                                                  signals,
                                                  enums,
                                                  multiplexer_signal,
                                                  multiplexer_ids)

    return result


def _is_multiplexed(message_tokens):
    return 'Mux' in message_tokens[5]


def _load_message_signals(message_tokens,
                          message_section_tokens,
                          signals,
                          enums):
    if _is_multiplexed(message_tokens):
        return _load_muxed_message_signals(message_tokens,
                                           message_section_tokens,
                                           signals,
                                           enums)
    else:
        return _load_message_signals_inner(message_tokens,
                                           signals,
                                           enums)


def _load_message(frame_id,
                  is_extended_frame,
                  message_tokens,
                  message_section_tokens,
                  signals,
                  enums,
                  strict):
    # Default values.
    name = message_tokens[1]
    length = int(message_tokens[4][2])
    cycle_time = None

    # Cycle time.
    try:
        cycle_time = num(message_tokens[5]['CycleTime'][0][2])
    except (KeyError, IndexError):
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
                                                 signals,
                                                 enums),
                   comment=None,
                   bus_name=None,
                   strict=strict)


def _parse_message_frame_ids(message):
    def to_int(string):
        return int(string, 16)

    def is_extended_frame(string):
        return len(string) == 8

    message = message[3][0]
    minimum = to_int(message[2])

    if message[4]:
        maximum = -to_int(message[4][0][0])
    else:
        maximum = minimum

    frame_ids = range(minimum, maximum + 1)

    return frame_ids, is_extended_frame(message[2])


def _load_message_section(section_name, tokens, signals, enums, strict):
    def has_frame_id(message):
        return len(message[3]) > 0

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
                                    enums,
                                    strict)
            messages.append(message)

    return messages


def _load_messages(tokens, signals, enums, strict):
    messages = _load_message_section('{SEND}', tokens, signals, enums, strict)
    messages += _load_message_section('{RECEIVE}', tokens, signals, enums, strict)
    messages += _load_message_section('{SENDRECEIVE}', tokens, signals, enums, strict)

    return messages


def _load_version(tokens):
    return tokens[0][2]


def load_string(string, strict=True):
    """Parse given string.

    """

    if not string.startswith('FormatVersion=6.0'):
        raise ParseError('Only SYM version 6.0 is supported.')

    tokens = Parser60().parse(string)

    version = _load_version(tokens)
    enums = _load_enums(tokens)
    signals = _load_signals(tokens, enums)
    messages = _load_messages(tokens, signals, enums, strict)

    return InternalDatabase(messages,
                            [],
                            [],
                            version,
                            [])
