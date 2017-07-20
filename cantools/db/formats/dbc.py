# Load and dump a CAN database in DBC format.

from collections import OrderedDict
from pyparsing import Word, Literal, Keyword, Optional, Suppress
from pyparsing import Group, QuotedString, StringEnd
from pyparsing import printables, nums, alphas, LineEnd, Empty
from pyparsing import ZeroOrMore, OneOrMore, delimitedList
from ..signal import Signal
from ..message import Message
from ..node import Node
from ..database import Database

# DBC section types.
VERSION = 'VERSION'
NODES = 'BU_'
COMMENT = 'CM_'
MESSAGE = 'BO_'
SIGNAL = 'SG_'
CHOICE = 'VAL_'
VALUE_TABLE = 'VAL_TABLE_'
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


def _num(number_as_string):
    """Convert given string to an integer or a float.

    """

    try:
        return int(number_as_string)

    except ValueError:
        return float(number_as_string)

    else:
        raise ValueError('Expected integer or floating point number.')


def _create_dbc_grammar():
    """Create DBC grammar.

    """

    # DBC file grammar.
    word = Word(printables.replace(';', ''))
    integer = Group(Optional('-') + Word(nums))
    positive_integer = Word(nums)
    number = Word(nums + '.Ee-+')
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
    node = Word(alphas + nums + '_-').setWhitespaceChars(' ')

    version = Group(Keyword('VERSION') +
                    QuotedString('"', multiline=True))
    symbol = Word(alphas + '_') + Suppress(LineEnd())
    symbols = Group(Keyword('NS_') +
                    colon +
                    Group(ZeroOrMore(symbol)))
    discard = Suppress(Keyword('BS_') + colon)
    nodes = Group(Keyword('BU_') +
                 colon +
                 Group(ZeroOrMore(node)))
    signal = Group(Keyword(SIGNAL) +
                   word +
                   colon +
                   Group(positive_integer +
                         pipe +
                         positive_integer +
                         at +
                         positive_integer +
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
                   Group(delimitedList(node)))
    message = Group(Keyword(MESSAGE) +
                    positive_integer +
                    word +
                    positive_integer +
                    word +
                    Group(ZeroOrMore(signal)))
    event = Suppress(Keyword(EVENT)
                     + word
                     + positive_integer
                     + lb
                     + number
                     + pipe
                     + number
                     + rb
                     + QuotedString('"', multiline=True)
                     + number
                     + number
                     + word
                     + node
                     + scolon)
    comment = Group(Keyword(COMMENT) +
                    ((Keyword(MESSAGE) +
                      positive_integer +
                      QuotedString('"', multiline=True) +
                      scolon) |
                     (Keyword(SIGNAL) +
                      positive_integer +
                      word +
                      QuotedString('"', multiline=True) +
                      scolon) |
                     (Keyword(NODES) +
                      word +
                      QuotedString('"', multiline=True) +
                      scolon) |
                     (Keyword(EVENT) +
                      word +
                      QuotedString('"', multiline=True) +
                      scolon)))
    attribute = Group(Keyword(ATTRIBUTE) +
                      ((QuotedString('"', multiline=True)) |
                       (Keyword(SIGNAL) +
                        QuotedString('"', multiline=True)) |
                       (Keyword(MESSAGE) +
                        QuotedString('"', multiline=True)) |
                       (Keyword(EVENT) +
                        QuotedString('"', multiline=True)) |
                       (Keyword(NODES) +
                        QuotedString('"', multiline=True))) +
                       word +
                       ((scolon) |
                        (Group(ZeroOrMore(Group(
                         (comma | Empty()) + QuotedString('"', multiline=True)))) +
                         scolon) |
                         (Group(ZeroOrMore(number)) +
                         scolon)))
    default_attr = Group(Keyword(DEFAULT_ATTR) +
                         QuotedString('"', multiline=True) +
                         (positive_integer | QuotedString('"', multiline=True)) +
                         scolon)
    attr_definition = Group(Keyword(ATTR_DEFINITION) +
                            QuotedString('"', multiline=True) +
                            Group(Optional((Keyword(MESSAGE) + positive_integer) |
                                           (Keyword(SIGNAL) + positive_integer + word) |
                                           (Keyword(NODES) + word))) +
                            (QuotedString('"', multiline=True) | positive_integer) +
                            scolon)
    choice = Group(Keyword(CHOICE) +
                   Optional(positive_integer) +
                   word +
                   Group(OneOrMore(Group(
                       integer + QuotedString('"', multiline=True)))) +
                   scolon)
    value_table = Group(Keyword(VALUE_TABLE) +
                        word +
                        Group(OneOrMore(Group(
                            integer + QuotedString('"', multiline=True)))) +
                        scolon)
    entry = (version
             | symbols
             | discard
             | nodes
             | message
             | comment
             | attribute
             | default_attr
             | attr_definition
             | choice
             | value_table
             | event)
    grammar = OneOrMore(entry) + StringEnd()

    return grammar


def _dump_nodes(database):
    bu = []

    for node in database.nodes:
        bu.append(node.name)

    return bu

def _dump_messages(database):
    bo = []

    for message in database.messages:
        msg = []
        fmt = 'BO_ {frame_id} {name}: {length} {nodes}'
        msg.append(fmt.format(frame_id=message.frame_id,
                              name=message.name,
                              length=message.length,
                              nodes=message.nodes))

        for signal in message.signals:
            fmt = (' SG_ {name} : {start}|{length}@{byte_order}{sign}'
                   ' ({scale},{offset})'
                   ' [{minimum}|{maximum}] "{unit}" {nodes}')
            msg.append(fmt.format(
                name=signal.name,
                start=signal.start,
                length=signal.length,
                nodes=', '.join(signal.nodes),
                byte_order=(0 if signal.byte_order == 'big_endian' else 1),
                sign=('-' if signal.is_signed else '+'),
                scale=signal.scale,
                offset=signal.offset,
                minimum=signal.minimum,
                maximum=signal.maximum,
                unit=signal.unit))

        bo.append('\n'.join(msg))

    return bo


def _dump_comments(database):
    cm = []

    for node in database.nodes:
        if node.comment is not None:
            fmt = 'CM_ BU_ {name} "{comment}";'
            cm.append(fmt.format(name=node.name,
                                 comment=node.comment))

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

    return cm


def _dump_attributes(database):
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

    return ba_def


def _dump_attribute_defaults(database):
    ba_def_def = []

    for default_attr in database.default_attrs:
        try:
            int(database.default_attrs[default_attr])
            fmt = 'BA_DEF_DEF_ "{name}" {value};'

        except ValueError:
            fmt = 'BA_DEF_DEF_ "{name}" "{value}";'

        ba_def_def.append(fmt.format(name=default_attr,
                                     value=database.default_attrs[default_attr]))

    return ba_def_def


def _dump_attribute_definitions(database):
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

    return ba


def _dump_choices(database):
    val = []

    for message in database.messages:
        for signal in message.signals:
            if signal.choices == None:
                continue

            fmt = 'VAL_ {frame_id} {name} {choices} ;'
            val.append(fmt.format(
                frame_id=message.frame_id,
                name=signal.name,
                choices=' '.join(['{value} "{text}"'.format(value=value,
                                                            text=text)
                                  for value, text in signal.choices.items()])))

    return val


def _load_comments(tokens):
    comments = {}

    for comment in tokens:
        if comment[0] == COMMENT:
            if comment[1] == NODES:
                node_name = comment[2]
                if node_name not in comments:
                    comments[node_name] = {}
                comments[node_name] = comment[3]

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

    return comments


def _load_attributes(tokens):
    attributes = []

    for attribute in tokens:
        if attribute[0] == ATTRIBUTE:
            attributes.append(attribute)

    return attributes


def _load_attribute_defaults(tokens):
    default_attrs = {}

    for default_attr in tokens:
        if default_attr[0] == DEFAULT_ATTR:
            default_attrs[default_attr[1]] = default_attr[2]

    return default_attrs


def _load_attribute_definitions(tokens):
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

    return msg_attributes


def _load_choices(tokens):
    choices = {}

    for choice in tokens:
        if choice[0] == CHOICE:
            try:
                frame_id = int(choice[1])
            except ValueError:
                print('warning: discarding tokens {}'.format(choice))
                continue

            if frame_id not in choices:
                choices[frame_id] = {}

            choices[frame_id][choice[2]] = OrderedDict(
                (int(''.join(v[0])), v[1]) for v in choice[3])

    return choices


def _load_messages(tokens,
                   comments,
                   default_attrs,
                   msg_attributes,
                   choices):

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

    messages = []

    for message in tokens:
        if message[0] != MESSAGE:
            continue

        frame_id = int(message[1]) & 0x7fffffff
        is_extended_frame = (int(message[1]) & 0x80000000 != 0)

        message = Message(
            frame_id=frame_id,
            is_extended_frame=is_extended_frame,
            name=message[2][0:-1],
            length=int(message[3], 0),
            nodes=message[4],
            send_type=get_send_type(int(message[1])),
            cycle_time=get_cycle_time(int(message[1])),
            signals=[Signal(name=signal[1],
                            start=int(signal[2][0]),
                            length=int(signal[2][1]),
                            nodes=signal[6],
                            byte_order=('big_endian'
                                        if signal[2][2] == '0'
                                        else 'little_endian'),
                            is_signed=(signal[2][3] == '-'),
                            scale=_num(signal[3][0]),
                            offset=_num(signal[3][1]),
                            minimum=_num(signal[4][0]),
                            maximum=_num(signal[4][1]),
                            unit=signal[5],
                            choices=get_choices(int(message[1]),
                                                signal[1]),
                            comment=get_comment(int(message[1]),
                                                signal[1]))
                     for signal in message[5]],
            comment=get_comment(int(message[1])))

        messages.append(message)

    return messages


def _load_version(tokens):
    return [token[1]
            for token in tokens
            if token[0] == VERSION][0]


def _load_nodes(tokens, comments):
    def get_node_comment(node_name):
        """Get comment for a given node_name

        """

        try:
            return comments[node_name]

        except KeyError:
            return None

    nodes = None

    for token in tokens:
        if token[0] == NODES:
            nodes = [Node(name=node,
                          comment=get_node_comment(node))
                     for node in token[1]]

    return nodes


def dumps(database):
    """Format database in DBC file format.

    """

    bu = _dump_nodes(database)
    bo = _dump_messages(database)
    cm = _dump_comments(database)
    ba_def = _dump_attributes(database)
    ba_def_def = _dump_attribute_defaults(database)
    ba = _dump_attribute_definitions(database)
    val = _dump_choices(database)

    return DBC_FMT.format(version=database.version,
                          bu=' '.join(bu),
                          bo='\n\n'.join(bo),
                          cm='\n'.join(cm),
                          ba_def='\n'.join(ba_def),
                          ba_def_def='\n'.join(ba_def_def),
                          ba='\n'.join(ba),
                          val='\n'.join(val))

def loads(string):
    """Parse given string.

    """

    grammar = _create_dbc_grammar()
    tokens = grammar.parseString(string)

    comments = _load_comments(tokens)
    attributes = _load_attributes(tokens)
    default_attrs = _load_attribute_defaults(tokens)
    msg_attributes = _load_attribute_definitions(tokens)
    choices = _load_choices(tokens)
    messages = _load_messages(tokens,
                              comments,
                              default_attrs,
                              msg_attributes,
                              choices)
    nodes = _load_nodes(tokens, comments)
    version = _load_version(tokens)

    return Database(messages,
                    nodes,
                    attributes,
                    default_attrs,
                    version)
