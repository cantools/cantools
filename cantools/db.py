# CAN database.

import bitstruct
from collections import namedtuple
from pyparsing import Word, Literal, Keyword, Optional, Suppress
from pyparsing import Group, QuotedString, StringEnd
from pyparsing import printables, nums, alphas, LineEnd, Empty
from pyparsing import ZeroOrMore, OneOrMore, delimitedList


# DBC section types.
VERSION = 'VERSION'
ECUS = 'BU_'
COMMENT = 'CM_'
MESSAGE = 'BO_'
SIGNAL = 'SG_'
CHOICE = 'VAL_'
ATTRIBUTE = 'BA_DEF_'
DEFAULT_ATTR = 'BA_DEF_DEF_'
ATTR_DEFINITION = 'BA_'
EVENT = 'EV_'

DBC_FMT = """VERSION "{version}"

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
\tBO_TX_BU_
\tBA_DEF_REL_
\tBA_REL_
\tBA_DEF_DEF_REL_
\tBU_SG_REL_
\tBU_EV_REL_
\tBU_BO_REL_
\tSG_MUL_VAL_

BS_:

BU_: {bu}

{bo}

{cm}

{ba_def}

{ba_def_def}

{ba}

{val}
"""


def num(number_as_string):
    """Convert given string to an integer or a float.

    """

    try:
        return int(number_as_string)

    except ValueError:
        return float(number_as_string)

    else:
        raise ValueError('Expected integer or floating point number.')


def create_dbc_grammar():
    """Create DBC grammar.

    """

    # DBC file grammar.
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
    ecu = Word(alphas + nums + '_-').setWhitespaceChars(' ')

    version = Group(Keyword('VERSION') +
                    QuotedString('"', multiline=True))
    symbol = Word(alphas + '_') + Suppress(LineEnd())
    symbols = Group(Keyword('NS_') +
                    colon +
                    Group(ZeroOrMore(symbol)))
    discard = Suppress(Keyword('BS_') + colon)
    ecus = Group(Keyword('BU_') +
                 colon +
                 Group(ZeroOrMore(ecu)))
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
                   Group(delimitedList(ecu)))
    message = Group(Keyword(MESSAGE) +
                    integer +
                    word +
                    integer +
                    word +
                    Group(ZeroOrMore(signal)))
    event = Suppress(Keyword(EVENT)
                     + word
                     + integer
                     + lb
                     + number
                     + pipe
                     + number
                     + rb
                     + QuotedString('"', multiline=True)
                     + number
                     + number
                     + word
                     + ecu
                     + scolon)
    comment = Group(Keyword(COMMENT) +
                    ((Keyword(MESSAGE) +
                      integer +
                      QuotedString('"', multiline=True) +
                      scolon) |
                     (Keyword(SIGNAL) +
                      integer +
                      word +
                      QuotedString('"', multiline=True) +
                      scolon) |
                     (Keyword(ECUS) +
                      word +
                      QuotedString('"', multiline=True) +
                      scolon)))
    attribute = Group(Keyword(ATTRIBUTE) +
                      ((QuotedString('"', multiline=True)) |
                       (Keyword(SIGNAL) +
                        QuotedString('"', multiline=True)) |
                       (Keyword(MESSAGE) +
                        QuotedString('"', multiline=True))) +
                       word +
                       ((scolon) |
                        (Group(ZeroOrMore(Group(
                         (comma | Empty()) + QuotedString('"', multiline=True)))) +
                         scolon) |
                         (Group(ZeroOrMore(integer)) +
                         scolon)))
    default_attr = Group(Keyword(DEFAULT_ATTR) +
                         QuotedString('"', multiline=True) +
                         (integer | QuotedString('"', multiline=True)) +
                         scolon)
    attr_definition = Group(Keyword(ATTR_DEFINITION) +
                            QuotedString('"', multiline=True) +
                            (Keyword(MESSAGE) | Keyword(SIGNAL)) +
                            integer +
                            integer +
                            scolon)
    choice = Group(Keyword(CHOICE) +
                   integer +
                   word +
                   Group(OneOrMore(Group(
                       integer + QuotedString('"', multiline=True)))) +
                   scolon)
    entry = version | symbols | discard | ecus | message | comment | \
            attribute | default_attr | attr_definition | choice | event
    grammar = OneOrMore(entry) + StringEnd()

    return grammar


def as_dbc(database):
    """Format database in DBC file format.

    """

    # Ecus.
    bu = []

    for ecu in database.ecus:
        bu.append(ecu.name)

    # Messages.
    bo = []

    for message in database.messages:
        msg = []
        fmt = 'BO_ {frame_id} {name}: {length} {ecus}'
        msg.append(fmt.format(frame_id=message.frame_id,
                              name=message.name,
                              length=message.length,
                              ecus=message.ecus))

        for signal in message.signals:
            fmt = (' SG_ {name} : {start}|{length}@{byte_order}{sign}'
                   ' ({scale},{offset})'
                   ' [{minimum}|{maximum}] "{unit}" {ecus}')
            msg.append(fmt.format(
                name=signal.name,
                start=signal.start,
                length=signal.length,
                ecus=', '.join(signal.ecus),
                byte_order=(0 if signal.byte_order == 'big_endian' else 1),
                sign=('-' if signal.is_signed else '+'),
                scale=signal.scale,
                offset=signal.offset,
                minimum=signal.minimum,
                maximum=signal.maximum,
                unit=signal.unit))

        bo.append('\n'.join(msg))

    # Comments.
    cm = []

    for ecu in database.ecus:
        if ecu.comment is not None:
            fmt = 'CM_ BU_ {name} "{comment}";'
            cm.append(fmt.format(name=ecu.name,
                                 comment=ecu.comment))

    for message in database.messages:
        if message.comment is not None:
            fmt = 'CM_ BO_ {frame_id} "{comment}";'
            cm.append(fmt.format(frame_id=message.frame_id,
                                 comment=message.comment))

        for signal in message.signals:
            if signal.comment is not None:
                fmt = 'CM_ SG_ {frame_id} {name} "{comment}";'
                cm.append(fmt.format(frame_id=message.frame_id,
                                     name=signal.name,
                                     comment=signal.comment))
    # Attributes.
    ba_def = []

    for attribute in database.attributes:
        if attribute[1] == SIGNAL or attribute[1] == MESSAGE:
            fmt = 'BA_DEF_ {kind} "{name}" {type_} {choices};'
            if attribute[3] == 'ENUM':
                ba_def.append(fmt.format(kind=attribute[1],
                                         name=attribute[2],
                                         type_=attribute[3],
                                         choices=','.join(['"{text}"'.format(text=choice[0])
                                                           for choice in attribute[4]])))
            elif attribute[3] == 'INT':
                ba_def.append(fmt.format(kind=attribute[1],
                                         name=attribute[2],
                                         type_=attribute[3],
                                         choices=' '.join(['{num}'.format(num=choice[0])
                                                           for choice in attribute[4]])))
    # Attribute defaults.
    ba_def_def = []

    for default_attr in database.default_attrs:
        try:
            int(database.default_attrs[default_attr])
            fmt = 'BA_DEF_DEF_ "{name}" {value};'

        except ValueError:
            fmt = 'BA_DEF_DEF_ "{name}" "{value}";'

        ba_def_def.append(fmt.format(name=default_attr,
                                     value=database.default_attrs[default_attr]))

    # Attribute definitions.
    ba = []

    for message in database.messages:
        if message.cycle_time is not None:
            fmt = 'BA_ "GenMsgCycleTime" BO_ {frame_id} {cycle_time};'
            ba.append(fmt.format(frame_id=message.frame_id,
                                 cycle_time=message.cycle_time))

    ba.append('')

    for message in database.messages:
        try:
            if message.send_type is not database.default_attrs['GenMsgSendType']:
                fmt = 'BA_ "GenMsgSendType" BO_ {frame_id} {send_type};'
                ba.append(fmt.format(frame_id=message.frame_id,
                                     send_type=message.send_type))

        except KeyError:
            continue

    # Choices.
    val = []

    for message in database.messages:
        for signal in message.signals:
            if signal.choices == None:
                continue

            fmt = 'VAL_ {frame_id} {name} {choices} ;'
            val.append(fmt.format(
                frame_id=message.frame_id,
                name=signal.name,
                choices=' '.join(['{value} "{text}"'.format(value=choice[0],
                                                            text=choice[1])
                                  for choice in signal.choices])))

    return DBC_FMT.format(version=database.version,
                          bu=' '.join(bu),
                          bo='\n\n'.join(bo),
                          cm='\n'.join(cm),
                          ba_def='\n'.join(ba_def),
                          ba_def_def='\n'.join(ba_def_def),
                          ba='\n'.join(ba),
                          val='\n'.join(val))


class Signal(object):
    """A CAN signal.

    """

    def __init__(self,
                 name,
                 start,
                 length,
                 byte_order,
                 is_signed,
                 scale,
                 offset,
                 minimum,
                 maximum,
                 unit,
                 choices,
                 comment,
                 ecus=None):
        self.name = name
        self.start = start
        self.length = length
        self.byte_order = byte_order
        self.is_signed = is_signed
        self.scale = scale
        self.offset = offset
        self.minimum = minimum
        self.maximum = maximum
        self.unit = unit
        self.choices = choices
        self.comment = comment
        self.ecus = ecus

    def __repr__(self):
        return 'signal("{}", {}, {}, "{}", "{}", {}, {}, {}, {}, "{}", {}, {})'.format(
            self.name,
            self.start,
            self.length,
            self.byte_order,
            self.is_signed,
            self.scale,
            self.offset,
            self.minimum,
            self.maximum,
            self.unit,
            self.choices,
            '"' + self.comment + '"' if self.comment is not None else None)


class Message(object):
    """CAN message.

    """

    class Signals(object):

        def __init__(self, message, *args, **kwargs):
            self.message = message

            if args:
                for signal, value in zip(message.signals, args):
                    setattr(self, signal.name, value)

            self.__dict__.update(kwargs)

        def __str__(self):
            signals = []

            for signal in self.message.signals:
                unit = ' ' + signal.unit if signal.unit else ''
                signals.append(
                    '{name}: {value}{unit}'.format(name=signal.name,
                                                   value=getattr(self, signal.name),
                                                   unit=unit))

            return '{}({})'.format(self.message.name, ', '.join(signals))

    def __init__(self,
                 frame_id,
                 name,
                 length,
                 signals,
                 comment,
                 ecus=None,
                 send_type=None,
                 cycle_time=None):
        self.frame_id = frame_id
        self.name = name
        self.length = length
        self.signals = signals
        self.signals.sort(key=lambda s: s.start)
        self.signals.reverse()
        self.comment = comment
        self.ecus = ecus
        self.send_type = send_type
        self.cycle_time = cycle_time
        self.fmt = ''
        end = 64

        for signal in signals:
            padding = end - (signal.start + signal.length)

            if padding > 0:
                self.fmt += 'p{}'.format(padding)

            self.fmt += '{}{}'.format('s' if signal.is_signed else 'u',
                                      signal.length)
            end = signal.start

    def encode(self, data):
        """Encode given data as a message of this type.

        """

        if isinstance(data, dict):
            data = self.Signals(self, **data)

        return bitstruct.pack(self.fmt,
                              *[int((getattr(data, signal.name)
                                     - signal.offset) / signal.scale)
                                for signal in self.signals])[::-1]

    def decode(self, data):
        """Decode given data as a message of this type.

        """

        data += b'\x00' * (8 - len(data))

        return self.Signals(self,
                            *[s.scale * v + s.offset
                              for s, v in zip(self.signals,
                                              bitstruct.unpack(self.fmt,
                                                               data[::-1]))])

    def __repr__(self):
        return 'message("{}", 0x{:x}, {}, {})'.format(
            self.name,
            self.frame_id,
            self.length,
            '"' + self.comment + '"' if self.comment is not None else None)


class Ecu(object):
    """An ECU on the CAN bus.

    """

    def __init__(self,
                 name,
                 comment):
        self.name = name
        self.comment = comment

    def __repr__(self):
        return 'ecu("{}", {})'.format(
            self.name,
            '"' + self.comment + '"' if self.comment is not None else None)


class File(object):
    """CAN database file.

    """

    def __init__(self,
                 messages=None,
                 ecus=None,
                 attributes=None,
                 default_attrs=None):
        self.messages = messages if messages else []
        self.ecus = ecus if ecus else []
        self.attributes = attributes if attributes else []
        self.default_attrs = default_attrs if default_attrs else []
        self._frame_id_to_message = {}
        self.version = None
        self._grammar = create_dbc_grammar()

    def add_dbc(self, dbc):
        """Add information from given DBC iostream.

        """

        tokens = self._grammar.parseString(dbc.read())

        comments = {}

        for comment in tokens:
            if comment[0] == COMMENT:
                if comment[1] == ECUS:
                    ecu_name = comment[2]
                    if ecu_name not in comments:
                        comments[ecu_name] = {}
                    comments[ecu_name] = comment[3]

                if comment[1] == MESSAGE:
                    frame_id = int(comment[2])
                    if frame_id not in comments:
                        comments[frame_id] = {}
                    comments[frame_id]['message'] = comment[3]

                if comment[1] == SIGNAL:
                    frame_id = int(comment[2])
                    if frame_id not in comments:
                        comments[frame_id] = {}
                    if 'signals' not in comments[frame_id]:
                        comments[frame_id]['signals'] = {}

                    comments[frame_id]['signals'][comment[3]] = comment[4]

        attributes = []

        for attribute in tokens:
            if attribute[0] == ATTRIBUTE:
                attributes.append(attribute)
        self.attributes = attributes

        default_attrs = {}

        for default_attr in tokens:
            if default_attr[0] == DEFAULT_ATTR:
                default_attrs[default_attr[1]] = default_attr[2]
        self.default_attrs = default_attrs

        msg_attributes = {}

        for attr_definition in tokens:
            if attr_definition[0] == ATTR_DEFINITION and \
               attr_definition[1] == "GenMsgCycleTime" and \
               attr_definition[2] == MESSAGE:
                frame_id = int(attr_definition[3])
                if frame_id not in msg_attributes:
                    msg_attributes[frame_id] = {}
                if 'cycle_time' not in msg_attributes[frame_id]:
                    msg_attributes[frame_id]['cycle_time'] = {}
                msg_attributes[frame_id]['cycle_time'] = attr_definition[4]
            if attr_definition[0] == ATTR_DEFINITION and \
               attr_definition[1] == "GenMsgSendType" and \
               attr_definition[2] == MESSAGE:
                frame_id = int(attr_definition[3])
                if frame_id not in msg_attributes:
                    msg_attributes[frame_id] = {}
                if 'send_type' not in msg_attributes[frame_id]:
                    msg_attributes[frame_id]['send_type'] = {}
                msg_attributes[frame_id]['send_type'] = 1 #TODO

        choices = {}

        for choice in tokens:
            if choice[0] == CHOICE:
                frame_id = int(choice[1])

                if frame_id not in choices:
                    choices[frame_id] = {}

                choices[frame_id][choice[2]] = [(int(v[0]), v[1])
                                                for v in choice[3]]

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

        def get_ecu_comment(ecu_name):
            """Get comment for a given ecu_name

            """

            try:
                return comments[ecu_name]

            except KeyError:
                return None

        def get_send_type(frame_id):
            """Get send type for a given message

            """
            try:
                return msg_attributes[frame_id]['send_type']

            except KeyError:
                try:
                    return default_attrs['GenMsgSendType']

                except KeyError:
                    return None

        def get_cycle_time(frame_id):
            """Get cycle time for a given message

            """

            try:
                return msg_attributes[frame_id]['cycle_time']

            except KeyError:
                try:
                    return default_attrs['GenMsgCycleTime']

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

        for ecus in tokens:
            if ecus[0] == ECUS:
                self.ecus = [Ecu(name=ecu,
                                 comment=get_ecu_comment(ecu))
                             for ecu in ecus[1]]

        for message in tokens:
            if message[0] != MESSAGE:
                continue

            message = Message(
                frame_id=int(message[1]),
                name=message[2][0:-1],
                length=int(message[3], 0),
                ecus=message[4],
                send_type=get_send_type(int(message[1])),
                cycle_time=get_cycle_time(int(message[1])),
                signals=[Signal(name=signal[1],
                                start=int(signal[2][0]),
                                length=int(signal[2][1]),
                                ecus=signal[6],
                                byte_order=('big_endian'
                                            if signal[2][2] == '0'
                                            else 'little_endian'),
                                is_signed=(signal[2][3] == '-'),
                                scale=num(signal[3][0]),
                                offset=num(signal[3][1]),
                                minimum=num(signal[4][0]),
                                maximum=num(signal[4][1]),
                                unit=signal[5],
                                choices=get_choices(int(message[1]),
                                                    signal[1]),
                                comment=get_comment(int(message[1]),
                                                    signal[1]))
                         for signal in message[5]],
                comment=get_comment(int(message[1])))

            self.add_message(message)

    def add_dbc_file(self, filename):
        """Add information from given DBC-file.

        """

        with open(filename, 'r') as fin:
            self.add_dbc(fin)

    def add_message(self, message):
        """Add given message to the database.

        """

        self.messages.append(message)
        self._frame_id_to_message[message.frame_id] = message

    def as_dbc(self):
        """Return a string of the database in DBC-file format.

        """

        return as_dbc(self)

    def lookup_message(self, frame_id):
        """Find the message object for given frame id `frame_id`.

        """

        return self._frame_id_to_message[frame_id]

    def encode_message(self, frame_id, data):
        """Encode given signal data `data` as a message of given
        `frame_id`. `data` can be a dictionary or a named tuple with
        signal values.

        """

        message = self._frame_id_to_message[frame_id]

        return message.encode(data)

    def decode_message(self, frame_id, data):
        """Decode given signal data `data` as a message of given frame id
        `frame_id`.

        """

        message = self._frame_id_to_message[frame_id]

        return message.decode(data)
