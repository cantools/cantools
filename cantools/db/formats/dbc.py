# Load and dump a CAN database in DBC format.

from collections import OrderedDict
import pyparsing
from pyparsing import Word
from pyparsing import Literal
from pyparsing import Keyword
from pyparsing import Optional
from pyparsing import Suppress
from pyparsing import Group
from pyparsing import StringEnd
from pyparsing import printables
from pyparsing import nums
from pyparsing import alphas
from pyparsing import ZeroOrMore
from pyparsing import OneOrMore
from pyparsing import delimitedList
from pyparsing import LineEnd
from pyparsing import Empty
from pyparsing import ParseException
from pyparsing import ParseSyntaxException

from ..signal import Signal
from ..message import Message
from ..node import Node
from ..internal_database import InternalDatabase

from .utils import num
from .utils import ParseError


# DBC section types.
VERSION = 'VERSION'
NODES = 'BU_'
NODES_REL = 'BU_SG_REL_'
COMMENT = 'CM_'
MESSAGE = 'BO_'
MESSAGE_TX_NODE = 'BO_TX_BU_'
SIGNAL = 'SG_'
CHOICE = 'VAL_'
VALUE_TABLE = 'VAL_TABLE_'
ATTRIBUTE = 'BA_'
ATTRIBUTE_DEFINITION = 'BA_DEF_'
ATTRIBUTE_DEFINITION_DEFAULT = 'BA_DEF_DEF_'
ATTRIBUTE_REL = 'BA_REL_'
ATTRIBUTE_DEFINITION_REL = 'BA_DEF_REL_'
ATTRIBUTE_DEFINITION_DEFAULT_REL = 'BA_DEF_DEF_REL_'
EVENT = 'EV_'
SIGNAL_TYPE = 'SIG_VALTYPE_'
SIGNAL_MULTIPLEXER_VALUES = 'SG_MUL_VAL_'
SIGNAL_GROUP = 'SIG_GROUP_'

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


class QuotedString(pyparsing.QuotedString):
    """Quoted string accepting escaped quotation marks.

    """

    def __init__(self):
        super(QuotedString, self).__init__('"',
                                           multiline=True,
                                           escChar='\\')


def _create_grammar():
    """Create the DBC grammar.

    """

    word = Word(printables.replace(';', '').replace(':', ''))
    integer = Group(Optional('-') + Word(nums))
    positive_integer = Word(nums).setName('positive integer')
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
    frame_id = Word(nums).setName('frame id')

    version = Group(Keyword('VERSION')
                    - QuotedString())
    version.setName(VERSION)

    symbol = Word(alphas + '_') + Suppress(LineEnd())

    symbols = Group(Keyword('NS_')
                    - colon
                    - Group(ZeroOrMore(symbol)))
    symbols.setName('NS_')

    discard = Suppress(Keyword('BS_') - colon).setName('BS_')

    nodes = Group(Keyword('BU_')
                  - colon
                  - Group(ZeroOrMore(node)))
    nodes.setName('BU_')

    signal = Group(Keyword(SIGNAL)
                   - Group(word + Optional(word))
                   - colon
                   - Group(positive_integer
                           - pipe
                           - positive_integer
                           - at
                           - positive_integer
                           - sign)
                   - Group(lp
                           - number
                           - comma
                           - number
                           - rp)
                   - Group(lb
                           - number
                           - pipe
                           - number
                           - rb)
                   - QuotedString()
                   - Group(delimitedList(node)))
    signal.setName(SIGNAL)

    message = Group(Keyword(MESSAGE)
                    - frame_id
                    - word
                    - colon
                    - positive_integer
                    - word
                    - Group(ZeroOrMore(signal)))
    message.setName(MESSAGE)

    event = Suppress(Keyword(EVENT)
                     - word
                     - colon
                     - positive_integer
                     - lb
                     - number
                     - pipe
                     - number
                     - rb
                     - QuotedString()
                     - number
                     - number
                     - word
                     - node
                     - scolon)
    event.setName(EVENT)

    comment = Group(Keyword(COMMENT)
                    - ((Keyword(SIGNAL)
                          - frame_id
                          - word
                          - QuotedString()
                          - scolon).setName(SIGNAL)
                       | (Keyword(MESSAGE)
                        - frame_id
                        - QuotedString()
                        - scolon).setName(MESSAGE)
                       | (Keyword(EVENT)
                          - word
                          - QuotedString()
                          - scolon).setName(EVENT)
                       | (Keyword(NODES)
                          - word
                          - QuotedString()
                          - scolon).setName(NODES)
                       | (QuotedString()
                          - scolon).setName('QuotedString')))
    comment.setName(COMMENT)

    attribute_definition = Group(Keyword(ATTRIBUTE_DEFINITION)
                                 - ((QuotedString())
                                    | (Keyword(SIGNAL)
                                       | Keyword(MESSAGE)
                                       | Keyword(EVENT)
                                       | Keyword(NODES))
                                    + QuotedString())
                                 - word
                                 - (scolon
                                    | (Group(ZeroOrMore(Group(
                                        (comma | Empty())
                                        + QuotedString())))
                                       + scolon)
                                    | (Group(ZeroOrMore(number))
                                       + scolon)))
    attribute_definition.setName(ATTRIBUTE_DEFINITION)

    attribute_definition_default = Group(Keyword(ATTRIBUTE_DEFINITION_DEFAULT)
                                         - QuotedString()
                                         - (number | QuotedString())
                                         - scolon)
    attribute_definition_default.setName(ATTRIBUTE_DEFINITION_DEFAULT)

    attribute = Group(Keyword(ATTRIBUTE)
                      - QuotedString()
                      - Group(Optional((Keyword(MESSAGE) + frame_id)
                                       | (Keyword(SIGNAL) + frame_id + word)
                                       | (Keyword(NODES) + word)))
                      - (QuotedString() | number)
                      - scolon)
    attribute.setName(ATTRIBUTE)

    choice = Group(Keyword(CHOICE)
                   - Group(Optional(frame_id))
                   - word
                   - Group(OneOrMore(Group(integer
                                           + QuotedString())))
                   - scolon)
    choice.setName(CHOICE)

    value_table = Group(Keyword(VALUE_TABLE)
                        - word
                        - Group(OneOrMore(Group(integer
                                                + QuotedString())))
                        - scolon)
    value_table.setName(VALUE_TABLE)

    signal_type = Group(Keyword(SIGNAL_TYPE)
                        - frame_id
                        - word
                        - colon
                        - positive_integer
                        - scolon)
    signal_type.setName(SIGNAL_TYPE)

    signal_multiplexer_values = Group(Keyword(SIGNAL_MULTIPLEXER_VALUES)
                                       - frame_id
                                       - word
                                       - word
                                       - Group(delimitedList(positive_integer
                                                             - Suppress('-')
                                                             - Suppress(positive_integer)))
                                       - scolon)
    signal_multiplexer_values.setName(SIGNAL_MULTIPLEXER_VALUES)

    message_add_sender = Group(Keyword(MESSAGE_TX_NODE)
                               - frame_id
                               - colon
                               - Group(delimitedList(node))
                               - scolon)
    message_add_sender.setName(MESSAGE_TX_NODE)

    attribute_definition_rel = Group(Keyword(ATTRIBUTE_DEFINITION_REL)
                                     - (QuotedString()
                                        | (Keyword(NODES_REL)
                                           + QuotedString()))
                                     - word
                                     - (scolon
                                        | (Group(ZeroOrMore(Group(
                                            (comma | Empty())
                                            + QuotedString())))
                                           + scolon)
                                        | (Group(ZeroOrMore(number))
                                           + scolon)))
    attribute_definition_rel.setName(ATTRIBUTE_DEFINITION_REL)

    attribute_definition_default_rel = Group(Keyword(ATTRIBUTE_DEFINITION_DEFAULT_REL)
                                             - QuotedString()
                                             - (number | QuotedString())
                                             - scolon)
    attribute_definition_default_rel.setName(ATTRIBUTE_DEFINITION_DEFAULT_REL)

    attribute_rel = Group(Keyword(ATTRIBUTE_REL)
                          - QuotedString()
                          - Keyword(NODES_REL)
                          - word
                          - Keyword(SIGNAL)
                          - frame_id
                          - word
                          - (positive_integer | QuotedString())
                          - scolon)
    attribute_rel.setName(ATTRIBUTE_REL)

    signal_group = Group(Keyword(SIGNAL_GROUP)
                         - frame_id
                         - word
                         - integer
                         - colon
                         - OneOrMore(word)
                         - scolon)
    signal_group.setName(SIGNAL_GROUP)

    entry = (message
             | comment
             | attribute
             | choice
             | attribute_definition
             | attribute_definition_default
             | attribute_rel
             | attribute_definition_rel
             | attribute_definition_default_rel
             | signal_group
             | event
             | message_add_sender
             | value_table
             | signal_type
             | signal_multiplexer_values
             | discard
             | nodes
             | symbols
             | version)

    frame_id.setParseAction(lambda _s, _l, t: int(t[0]))

    return OneOrMore(entry) + StringEnd()


def _dump_nodes(database):
    bu = []

    for node in database.nodes:
        bu.append(node.name)

    return bu


def _dump_messages(database):
    bo = []

    def get_frame_id(message):
        frame_id = message.frame_id

        if message.is_extended_frame:
            frame_id |= 0x80000000

        return frame_id

    def get_mux(signal):
        if signal.is_multiplexer:
            result = ' M'
        elif signal.multiplexer_ids is not None:
            result = ' m{}'.format(signal.multiplexer_ids[0])
        else:
            result = ''

        return result

    for message in database.messages:
        msg = []
        fmt = 'BO_ {frame_id} {name}: {length} {senders}'
        msg.append(fmt.format(frame_id=get_frame_id(message),
                              name=message.name,
                              length=message.length,
                              senders=' '.join(message.senders)))

        for signal in message.signals[::-1]:
            fmt = (' SG_ {name}{mux} : {start}|{length}@{byte_order}{sign}'
                   ' ({scale},{offset})'
                   ' [{minimum}|{maximum}] "{unit}" {receivers}')
            msg.append(fmt.format(
                name=signal.name,
                mux=get_mux(signal),
                start=signal.start,
                length=signal.length,
                receivers=', '.join(signal.receivers),
                byte_order=(0 if signal.byte_order == 'big_endian' else 1),
                sign=('-' if signal.is_signed else '+'),
                scale=signal.scale,
                offset=signal.offset,
                minimum=signal.minimum,
                maximum=signal.maximum,
                unit='' if signal.unit is None else signal.unit))

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

        for signal in message.signals[::-1]:
            if signal.comment is not None:
                fmt = 'CM_ SG_ {frame_id} {name} "{comment}";'
                cm.append(fmt.format(frame_id=message.frame_id,
                                     name=signal.name,
                                     comment=signal.comment))

    return cm


def _dump_attribute_definitions(database):
    ba_def = []

    for attribute in database.attribute_definitions:
        if attribute[1] in [SIGNAL, MESSAGE]:
            fmt = 'BA_DEF_ {kind} "{name}" {type_} {choices};'
            if attribute[3] == 'ENUM':
                choices = ','.join(['"{}"'.format(choice[0])
                                    for choice in attribute[4]])
                ba_def.append(fmt.format(kind=attribute[1],
                                         name=attribute[2],
                                         type_=attribute[3],
                                         choices=choices))
            elif attribute[3] == 'INT':
                choices = ' '.join(['{}'.format(choice) for choice in attribute[4]])
                ba_def.append(fmt.format(kind=attribute[1],
                                         name=attribute[2],
                                         type_=attribute[3],
                                         choices=choices))
        elif attribute[0] == ATTRIBUTE_DEFINITION:
            fmt = 'BA_DEF_ "{name}" {type_};'
            ba_def.append(fmt.format(name=attribute[1],
                                     type_=attribute[2]))

    return ba_def


def _dump_attribute_definition_defaults(database):
    ba_def_def = []

    for default in database.attribute_definition_defaults:
        try:
            int(database.attribute_definition_defaults[default])
            fmt = 'BA_DEF_DEF_ "{name}" {value};'
        except ValueError:
            fmt = 'BA_DEF_DEF_ "{name}" "{value}";'

        ba_def_def.append(fmt.format(name=default,
                                     value=database.attribute_definition_defaults[default]))

    return ba_def_def


def _dump_attributes(database):
    ba = []

    try:
        default_send_type = database.attribute_definition_defaults['GenMsgSendType']
    except KeyError:
        default_send_type = None

    try:
        default_cycle_time = int(database.attribute_definition_defaults['GenMsgCycleTime'])
    except KeyError:
        default_cycle_time = None

    for message in database.messages:
        if message.send_type != default_send_type:
            fmt = 'BA_ "GenMsgSendType" BO_ {frame_id} "{send_type}";'
            ba.append(fmt.format(frame_id=message.frame_id,
                                 send_type=message.send_type))

        if message.cycle_time != default_cycle_time:
            fmt = 'BA_ "GenMsgCycleTime" BO_ {frame_id} {cycle_time};'
            ba.append(fmt.format(frame_id=message.frame_id,
                                 cycle_time=message.cycle_time))

    return ba


def _dump_choices(database):
    val = []

    for message in database.messages:
        for signal in message.signals[::-1]:
            if signal.choices is None:
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
        if comment[0] != COMMENT:
            continue

        if comment[1] == NODES:
            node_name = comment[2]
            comments[node_name] = comment[3]
        elif comment[1] == MESSAGE:
            frame_id = comment[2]

            if frame_id not in comments:
                comments[frame_id] = {}

            comments[frame_id]['message'] = comment[3]
        elif comment[1] == SIGNAL:
            frame_id = comment[2]

            if frame_id not in comments:
                comments[frame_id] = {}

            if 'signals' not in comments[frame_id]:
                comments[frame_id]['signals'] = {}

            comments[frame_id]['signals'][comment[3]] = comment[4]

    return comments


def _load_attribute_definitions(tokens):
    definitions = []

    for attribute in tokens:
        if attribute[0] == ATTRIBUTE_DEFINITION:
            definitions.append(attribute)

    return definitions


def _load_attribute_definition_defaults(tokens):
    defaults = OrderedDict()

    for default_attr in tokens:
        if default_attr[0] == ATTRIBUTE_DEFINITION_DEFAULT:
            defaults[default_attr[1]] = default_attr[2]

    return defaults


def _load_attributes(tokens):
    attributes = {}

    for attribute in tokens:
        if attribute[0] != ATTRIBUTE:
            continue

        name = attribute[1]

        if len(attribute[2]) == 2:
            if attribute[2][0] == MESSAGE:
                frame_id = attribute[2][1]

                if frame_id not in attributes:
                    attributes[frame_id] = {}

                attributes[frame_id][name] = attribute[3]

    return attributes


def _load_choices(tokens):
    choices = {}

    for choice in tokens:
        if choice[0] != CHOICE:
            continue

        if not choice[1]:
            continue

        frame_id = choice[1][0]

        if frame_id not in choices:
            choices[frame_id] = {}

        choices[frame_id][choice[2]] = OrderedDict(
            (int(''.join(v[0])), v[1]) for v in choice[3])

    return choices


def _load_message_senders(tokens):
    """Load additional message senders.

    """

    message_senders = {}

    for senders in tokens:
        if senders[0] != MESSAGE_TX_NODE:
            continue

        frame_id = senders[1]

        if frame_id not in message_senders:
            message_senders[frame_id] = []

        message_senders[frame_id] += list(senders[2])

    return message_senders


def _load_signal_types(tokens):
    """Load signal types.

    """

    signal_types = {}

    for signal_type in tokens:
        if signal_type[0] != SIGNAL_TYPE:
            continue

        frame_id = signal_type[1]

        if frame_id not in signal_types:
            signal_types[frame_id] = {}

        signal_name = signal_type[2]
        signal_types[frame_id][signal_name] = int(signal_type[3])

    return signal_types


def _load_signal_multiplexer_values(tokens):
    """Load additional signal multiplexer values.

    """

    signal_multiplexer_values = {}

    for signal_multiplexer_value in tokens:
        if signal_multiplexer_value[0] != SIGNAL_MULTIPLEXER_VALUES:
            continue

        frame_id = signal_multiplexer_value[1]
        signal_name = signal_multiplexer_value[2]
        multiplexer_signal = signal_multiplexer_value[3]
        multiplexer_ids = [int(v) for v in signal_multiplexer_value[4]]

        if frame_id not in signal_multiplexer_values:
            signal_multiplexer_values[frame_id] = {}

        if multiplexer_signal not in signal_multiplexer_values[frame_id]:
            signal_multiplexer_values[frame_id][multiplexer_signal] = {}

        signal_multiplexer_values[frame_id][multiplexer_signal][signal_name] = multiplexer_ids

    return signal_multiplexer_values


def _load_messages(tokens,
                   comments,
                   attribute_definition_defaults,
                   message_attributes,
                   choices,
                   message_senders,
                   signal_types,
                   signal_multiplexer_values):
    """Load messages.

    """

    def get_comment(frame_id_dbc, signal=None):
        """Get comment for given message or signal.

        """

        try:
            if signal is None:
                return comments[frame_id_dbc]['message']
            else:
                return comments[frame_id_dbc]['signals'][signal]
        except KeyError:
            return None

    def get_send_type(frame_id_dbc):
        """Get send type for a given message

        """
        try:
            return message_attributes[frame_id_dbc]['GenMsgSendType']
        except KeyError:
            try:
                return attribute_definition_defaults['GenMsgSendType']
            except KeyError:
                return None

    def get_cycle_time(frame_id_dbc):
        """Get cycle time for a given message

        """

        try:
            return int(message_attributes[frame_id_dbc]['GenMsgCycleTime'])
        except KeyError:
            try:
                return int(attribute_definition_defaults['GenMsgCycleTime'])
            except KeyError:
                return None

    def get_choices(frame_id_dbc, signal):
        """Get choices for given signal.

        """

        try:
            return choices[frame_id_dbc][signal]
        except KeyError:
            return None

    def get_multiplexer_ids(frame_id_dbc, signal, multiplexer_signal):
        ids = []

        if len(signal) == 2 and signal[1] != 'M':
            # ToDo: Extended multiplexing not yet supported.
            value = signal[1][1:].strip('M')
            ids.append(int(value))

        try:
            ids.extend(
                signal_multiplexer_values[frame_id_dbc][multiplexer_signal][signal[0]])
        except KeyError:
            pass

        if ids:
            return list(set(ids))
        else:
            return None

    def get_is_float(frame_id_dbc, signal):
        """Get is_float for given signal.

        """

        try:
            return signal_types[frame_id_dbc][signal] == 1
        except KeyError:
            return False

    messages = []

    for message in tokens:
        if message[0] != MESSAGE:
            continue

        # Frame id.
        frame_id_dbc = message[1]
        frame_id = frame_id_dbc & 0x7fffffff
        is_extended_frame = bool(frame_id_dbc & 0x80000000)

        # Senders.
        senders = [message[4]]

        for node in message_senders.get(frame_id_dbc, []):
            if node not in senders:
                senders.append(node)

        # Signal multiplexing.
        multiplexer_signal = None

        for signal in message[5]:
            if len(signal[1]) == 2:
                if signal[1][1] == 'M':
                    multiplexer_signal = signal[1][0]
                    break

        message = Message(
            frame_id=frame_id,
            is_extended_frame=is_extended_frame,
            name=message[2],
            length=int(message[3], 0),
            senders=senders,
            send_type=get_send_type(frame_id_dbc),
            cycle_time=get_cycle_time(frame_id_dbc),
            signals=[Signal(name=signal[1][0],
                            start=int(signal[2][0]),
                            length=int(signal[2][1]),
                            receivers=list(signal[6]),
                            byte_order=('big_endian'
                                        if signal[2][2] == '0'
                                        else 'little_endian'),
                            is_signed=(signal[2][3] == '-'),
                            scale=num(signal[3][0]),
                            offset=num(signal[3][1]),
                            minimum=num(signal[4][0]),
                            maximum=num(signal[4][1]),
                            unit=None if signal[5] == '' else signal[5],
                            choices=get_choices(frame_id_dbc,
                                                signal[1][0]),
                            comment=get_comment(frame_id_dbc,
                                                signal[1][0]),
                            is_multiplexer=(signal[1][1] == 'M'
                                            if len(signal[1]) == 2
                                            else False),
                            multiplexer_ids=get_multiplexer_ids(frame_id_dbc,
                                                                signal[1],
                                                                multiplexer_signal),
                            multiplexer_signal=(multiplexer_signal
                                                if (signal[1][0] != multiplexer_signal
                                                    and len(signal[1]) == 2)
                                                else None),
                            is_float=get_is_float(frame_id_dbc,
                                                  signal[1][0]))
                     for signal in message[5]],
            comment=get_comment(frame_id_dbc))

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


def dump_string(database):
    """Format database in DBC file format.

    """

    bu = _dump_nodes(database)
    bo = _dump_messages(database)
    cm = _dump_comments(database)
    ba_def = _dump_attribute_definitions(database)
    ba_def_def = _dump_attribute_definition_defaults(database)
    ba = _dump_attributes(database)
    val = _dump_choices(database)

    return DBC_FMT.format(version=database.version,
                          bu=' '.join(bu),
                          bo='\n\n'.join(bo),
                          cm='\n'.join(cm),
                          ba_def='\n'.join(ba_def),
                          ba_def_def='\n'.join(ba_def_def),
                          ba='\n'.join(ba),
                          val='\n'.join(val))


def load_string(string):
    """Parse given string.

    """

    grammar = _create_grammar()

    try:
        tokens = grammar.parseString(string)
    except (ParseException, ParseSyntaxException) as e:
        raise ParseError(
            "Invalid DBC syntax at line {}, column {}: '{}': {}.".format(
                e.lineno,
                e.column,
                e.markInputline(),
                e.msg))

    comments = _load_comments(tokens)
    attribute_definitions = _load_attribute_definitions(tokens)
    attribute_definition_defaults = _load_attribute_definition_defaults(tokens)
    message_attributes = _load_attributes(tokens)
    choices = _load_choices(tokens)
    message_senders = _load_message_senders(tokens)
    signal_types = _load_signal_types(tokens)
    signal_multiplexer_values = _load_signal_multiplexer_values(tokens)
    messages = _load_messages(tokens,
                              comments,
                              attribute_definition_defaults,
                              message_attributes,
                              choices,
                              message_senders,
                              signal_types,
                              signal_multiplexer_values)
    nodes = _load_nodes(tokens, comments)
    version = _load_version(tokens)

    return InternalDatabase(messages,
                            nodes,
                            [],
                            version,
                            attribute_definitions,
                            attribute_definition_defaults)
