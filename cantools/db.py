# CAN database.

import bitstruct
from collections import namedtuple
from pyparsing import Word, Literal, Keyword, Optional, Suppress, Group, QuotedString
from pyparsing import printables, nums, alphas, LineEnd, ZeroOrMore, OneOrMore

__author__ = 'Erik Moqvist'

# DBC section types
VERSION = 'VERSION'
ECU     = 'BU_'
COMMENT = 'CM_'
MESSAGE = 'BO_'
SIGNAL  = 'SG_'
CHOICE  = 'VAL_'

dbc_fmt = """VERSION "{version}"

NS_ :
\tNS_DESC_
\tCM_
\tBA_DEF_
\tBA_
\tVAL_
\tCAT_DEF_
\tCAT_
\tFILTER
\tBA_DEF_DEF_
\tEV_DATA_
\tENVVAR_DATA_
\tSGTYPE_
\tSGTYPE_VAL_
\tBA_DEF_SGTYPE_
\tBA_SGTYPE_
\tSIG_TYPE_REF_
\tVAL_TABLE_
\tSIG_GROUP_
\tSIG_VALTYPE_
\tSIGTYPE_VALTYPE_

BS_:

BU_: {bu}

{bo}

{cm}

{val}
"""

def num(s):
    """Convert a string to an integer or a float.

    """

    try:
        return int(s)
    except ValueError:
        return float(s)
    else:
        raise ValueError('Expected integer or floating point number.')

def create_dbc_grammar():
    """Create DBC grammar.

    """

    # DBC file grammar
    word = Word(printables)
    integer = Optional(Literal('-')) + Word(nums)
    number = Word(nums + '.Ee-')
    colon = Suppress(Literal(':'))
    scolon = Suppress(Literal(';'))
    pipe = Suppress(Literal('|'))
    at = Suppress(Literal('@'))
    sign = Literal('+') | Literal('-')
    lp = Suppress(Literal('('))
    rp = Suppress(Literal(')'))
    lb = Suppress(Literal('['))
    rb = Suppress(Literal(']'))
    comma = Suppress(Literal(','))

    version = Group(Keyword('VERSION') +
                    QuotedString('"', multiline=True))
    symbol = Word(alphas + '_') + Suppress(LineEnd())
    symbols = Group(Keyword('NS_') +
                    colon +
                    Group(ZeroOrMore(symbol)))
    discard = Suppress(Keyword('BS_') + colon)
    ecu = Group(Keyword('BU_') +
                colon +
                ZeroOrMore(Word(printables).setWhitespaceChars(' \t')))
    signal = Group(Keyword(SIGNAL) +
                   word +
                   colon +
                   Group(integer +
                         pipe +
                         integer +
                         at +
                         integer +
                         sign) +
                   Group(lp +
                         number +
                         comma +
                         number +
                         rp) +
                   Group(lb +
                         number +
                         pipe +
                         number +
                         rb) +
                   QuotedString('"', multiline=True) +
                   word)
    message = Group(Keyword(MESSAGE) +
                    integer +
                    word +
                    integer +
                    word +
                    Group(ZeroOrMore(signal)))
    comment = Group(Keyword(COMMENT) +
                    ((Keyword(MESSAGE) +
                      integer +
                      QuotedString('"', multiline=True) +
                      scolon) |
                     (Keyword(SIGNAL) +
                      integer +
                      word +
                      QuotedString('"', multiline=True) +
                      scolon)))
    choice = Group(Keyword(CHOICE) +
                   integer +
                   word +
                   Group(OneOrMore(Group(integer + QuotedString('"', multiline=True)))) +
                   scolon)
    entry = version | symbols | discard | ecu | message | comment | choice
    grammar = OneOrMore(entry)

    return grammar

def as_dbc(db):
    """Format database in DBC file format.

    """

    # messages
    bo = []
    for message in db.messages:
        msg = []
        fmt = 'BO_ {frame_id} {name}: {length} {ecu}'
        msg.append(fmt.format(frame_id=message.frame_id,
                              name=message.name,
                              length=message.length,
                              ecu=db.ecu))
        for signal in message.signals:
            fmt = (' SG_ {name} : {start}|{length}@{byte_order}{_type} ({scale},{offset})'
                   ' [{_min}|{_max}] "{unit}" Vector__XXX')
            msg.append(fmt.format(name=signal.name,
                                  start=signal.start,
                                  length=signal.length,
                                  byte_order=(0 if signal.byte_order == 'big_endian' else 1),
                                  _type=('-' if signal.type == 'signed' else '+'),
                                  scale=signal.scale,
                                  offset=signal.offset,
                                  _min=signal.min,
                                  _max=signal.max,
                                  unit=signal.unit))
        bo.append('\n'.join(msg))

    # comments
    cm = []
    for message in db.messages:
        if message.comment != None:
            fmt = 'CM_ BO_ {frame_id} "{comment}";'
            cm.append(fmt.format(frame_id=message.frame_id,
                                 comment=message.comment))
        for signal in message.signals:
            if signal.comment == None:
                continue
            fmt = 'CM_ SG_ {frame_id} {name} "{comment}";'
            cm.append(fmt.format(frame_id=message.frame_id,
                                 name=signal.name,
                                 comment=signal.comment))

    # choices
    val = []
    for message in db.messages:
        for signal in message.signals:
            if signal.choices == None:
                continue
            fmt = 'VAL_ {frame_id} {name} {choices} ;'
            val.append(fmt.format(frame_id=message.frame_id,
                                  name=signal.name,
                                  choices=' '.join([ '{value} "{text}"'.format(value=choice[0],
                                                                               text=choice[1])
                                                       for choice in signal.choices ])))

    return dbc_fmt.format(version=db.version,
                          bu=db.ecu,
                          bo='\n\n'.join(bo),
                          cm='\n'.join(cm),
                          val='\n'.join(val))

class Signal(object):
    """CAN signal.

    """

    def __init__(self,
                 name,
                 start,
                 length,
                 byte_order,
                 _type,
                 scale,
                 offset,
                 _min,
                 _max,
                 unit,
                 choices,
                 comment):
        self.name = name
        self.start = start
        self.length = length
        self.byte_order = byte_order
        self.type = _type
        self.scale = scale
        self.offset = offset
        self.min = _min
        self.max = _max
        self.unit = unit
        self.choices = choices
        self.comment = comment

    def __repr__(self):
        fmt = 'signal(' + ', '.join(12 * ['{}']) + ')'
        return fmt.format(self.name,
                          self.start,
                          self.length,
                          self.byte_order,
                          self.type,
                          self.scale,
                          self.offset,
                          self.min,
                          self.max,
                          self.unit,
                          self.choices,
                          self.comment)


class Message(object):
    """CAN message.

    """

    def __init__(self,
                 frame_id,
                 name,
                 length,
                 signals,
                 comment):
        self.frame_id = frame_id
        self.name = name
        self.length = length
        self.signals = signals
        self.signals.sort(key=lambda s: s.start)
        self.signals.reverse()
        self.comment = comment
        self.Decoded = namedtuple(name, [signal.name for signal in signals])
        self.fmt = ''
        end = 64
        for signal in signals:
            padding = end - (signal.start + signal.length)
            if padding > 0:
                self.fmt += 'p{}'.format(padding)
            self.fmt += '{}{}'.format(signal.type[0], signal.length)
            end = signal.start

    def decode(self, data):
        return self.Decoded(*[v[0].scale * v[1] + v[0].offset
                              for v in zip(self.signals,
                                           bitstruct.unpack(self.fmt, data))])


class File(object):
    """CAN database file.

    """

    def __init__(self, messages=None):
        self.messages = messages if messages else []
        self.grammar = create_dbc_grammar()
        self.frame_id_to_message = {}
        self.version = None
        self.ecu = None

    def add_dbc(self, dbc):
        """Add information from dbc io stream.

        """

        tokens = self.grammar.parseString(dbc.read())

        comments = {}
        for comment in tokens:
            if comment[0] == COMMENT:
                frame_id = int(comment[2])
                if frame_id not in comments:
                    comments[frame_id] = {}
                if comment[1] == MESSAGE:
                    comments[frame_id]['message'] = comment[3]
                if comment[1] == SIGNAL:
                    if 'signals' not in comments[frame_id]:
                        comments[frame_id]['signals'] = {}
                    comments[frame_id]['signals'][comment[3]] = comment[4]

        choices = {}
        for choice in tokens:
            if choice[0] == CHOICE:
                frame_id = int(choice[1])
                if frame_id not in choices:
                    choices[frame_id] = {}
                choices[frame_id][choice[2]] = [ (int(v[0]), v[1])
                                                 for v in choice[3] ]

        def get_comment(frame_id, signal=None):
            """Get comment for given message or signal.

            """

            try:
                if signal == None:
                    return comments[frame_id]['message']
                else:
                    return comments[frame_id]['signals'][signal]
            except KeyError:
                return None

        def get_choices(frame_id, signal):
            """Get choices for given signal.

            """

            try:
                return choices[frame_id][signal]
            except KeyError:
                return None

        self.version = [token[1]
                        for token in tokens
                        if token[0] == VERSION][0]
        self.ecu = [token[1]
                    for token in tokens
                    if token[0] == ECU][0]

        for message in tokens:
            if message[0] != MESSAGE:
                continue
            message = Message(frame_id=int(message[1]),
                              name=message[2][0:-1],
                              length=int(message[3], 0),
                              signals=[ Signal(name=signal[1],
                                               start=int(signal[2][0]),
                                               length=int(signal[2][1]),
                                               byte_order=('big_endian'
                                                           if signal[2][2] == '0'
                                                           else 'little_endian'),
                                               _type=('signed'
                                                     if signal[2][3] == '-'
                                                     else 'unsigned'),
                                               scale=num(signal[3][0]),
                                               offset=num(signal[3][1]),
                                               _min=num(signal[4][0]),
                                               _max=num(signal[4][1]),
                                               unit=signal[5],
                                               choices=get_choices(int(message[1]),
                                                                   signal[1]),
                                               comment=get_comment(int(message[1]),
                                                                   signal[1]))
                                        for signal in message[5] ],
                              comment=get_comment(int(message[1])))
            self.add_message(message)

    def add_message(self, message):
        self.messages.append(message)
        self.frame_id_to_message[message.frame_id] = message

    def as_dbc(self):
        """Return database in dbc file format.

        """

        return as_dbc(self)

    def decode_message(self, frame_id, data):
        """Decode a message.

        """

        message = self.frame_id_to_message[frame_id]
        return message.decode(data)
