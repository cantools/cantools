# Load and dump a CAN database in DBC format.

import re
from collections import OrderedDict as odict
from decimal import Decimal

import textparser
from textparser import Sequence
from textparser import choice
from textparser import ZeroOrMore
from textparser import OneOrMore
from textparser import OneOrMoreDict
from textparser import DelimitedList
from textparser import Any
from textparser import tokenize_init
from textparser import Token
from textparser import TokenizeError
from textparser import Optional

from ..attribute_definition import AttributeDefinition
from ..attribute import Attribute
from ..signal import Signal
from ..message import Message
from ..node import Node
from ..internal_database import InternalDatabase

from .utils import num


DBC_FMT = (
    'VERSION "{version}"\r\n'
    '\r\n'
    '\r\n'
    'NS_ : \r\n'
    '\tNS_DESC_\r\n'
    '\tCM_\r\n'
    '\tBA_DEF_\r\n'
    '\tBA_\r\n'
    '\tVAL_\r\n'
    '\tCAT_DEF_\r\n'
    '\tCAT_\r\n'
    '\tFILTER\r\n'
    '\tBA_DEF_DEF_\r\n'
    '\tEV_DATA_\r\n'
    '\tENVVAR_DATA_\r\n'
    '\tSGTYPE_\r\n'
    '\tSGTYPE_VAL_\r\n'
    '\tBA_DEF_SGTYPE_\r\n'
    '\tBA_SGTYPE_\r\n'
    '\tSIG_TYPE_REF_\r\n'
    '\tVAL_TABLE_\r\n'
    '\tSIG_GROUP_\r\n'
    '\tSIG_VALTYPE_\r\n'
    '\tSIGTYPE_VALTYPE_\r\n'
    '\tBO_TX_BU_\r\n'
    '\tBA_DEF_REL_\r\n'
    '\tBA_REL_\r\n'
    '\tBA_DEF_DEF_REL_\r\n'
    '\tBU_SG_REL_\r\n'
    '\tBU_EV_REL_\r\n'
    '\tBU_BO_REL_\r\n'
    '\tSG_MUL_VAL_\r\n'
    '\r\n'
    'BS_:\r\n'
    '\r\n'
    'BU_: {bu}\r\n'
    '\r\n'
    '\r\n'
    '{bo}\r\n'
    '\r\n'
    '\r\n'
    '\r\n'
    '{cm}\r\n'
    '{ba_def}\r\n'
    '{ba_def_def}\r\n'
    '{ba}\r\n'
    '{val}\r\n'
    '\r\n'
)


class Parser(textparser.Parser):

    def tokenize(self, string):
        keywords = set([
            'BA_',
            'BA_DEF_',
            'BA_DEF_DEF_',
            'BA_DEF_DEF_REL_',
            'BA_DEF_REL_',
            'BA_DEF_SGTYPE_',
            'BA_REL_',
            'BA_SGTYPE_',
            'BO_',
            'BO_TX_BU_',
            'BS_',
            'BU_',
            'BU_BO_REL_',
            'BU_EV_REL_',
            'BU_SG_REL_',
            'CAT_',
            'CAT_DEF_',
            'CM_',
            'ENVVAR_DATA_',
            'EV_',
            'EV_DATA_',
            'FILTER',
            'NS_',
            'NS_DESC_',
            'SG_',
            'SG_MUL_VAL_',
            'SGTYPE_',
            'SGTYPE_VAL_',
            'SIG_GROUP_',
            'SIG_TYPE_REF_',
            'SIG_VALTYPE_',
            'SIGTYPE_VALTYPE_',
            'VAL_',
            'VAL_TABLE_',
            'VERSION'
        ])

        names = {
            'LPAREN': '(',
            'RPAREN': ')',
            'LBRACE': '[',
            'RBRACE': ']',
            'COMMA':  ',',
            'AT':     '@',
            'SCOLON': ';',
            'COLON':  ':',
            'PIPE':   '|',
            'SIGN':   '+/-'
        }

        token_specs = [
            ('SKIP',     r'[ \r\n\t]+|//.*?\n'),
            ('NUMBER',   r'-?\d+(\.\d+)?([eE][+-]?\d+)?'),
            ('WORD',     r'[A-Za-z0-9_]+'),
            ('STRING',   r'"(\\"|[^"])*?"'),
            ('LPAREN',   r'\('),
            ('RPAREN',   r'\)'),
            ('LBRACE',   r'\['),
            ('RBRACE',   r'\]'),
            ('COMMA',    r','),
            ('PIPE',     r'\|'),
            ('AT',       r'@'),
            ('SIGN',     r'[+-]'),
            ('SCOLON',   r';'),
            ('COLON',    r':'),
            ('MISMATCH', r'.')
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

                if value in keywords:
                    kind = value

                if kind in names:
                    kind = names[kind]

                tokens.append(Token(kind, value, mo.start()))
            else:
                raise TokenizeError(string, mo.start())

        return tokens

    def grammar(self):
        version = Sequence('VERSION', 'STRING')

        ns = Sequence('NS_', ':', ZeroOrMore(Any(), Sequence(Any(), ':')))

        bs = Sequence('BS_', ':')

        nodes = Sequence('BU_', ':', ZeroOrMore('WORD'))

        signal = Sequence(
            'SG_', choice(Sequence('WORD', 'WORD'), Sequence('WORD')), ':',
            'NUMBER', '|', 'NUMBER', '@', 'NUMBER', '+/-',
            '(', 'NUMBER', ',', 'NUMBER', ')',
            '[', 'NUMBER', '|', 'NUMBER', ']',
            'STRING',
            DelimitedList('WORD'))

        message = Sequence(
            'BO_', 'NUMBER', 'WORD', ':', 'NUMBER', 'WORD', ZeroOrMore(signal))

        environment_variable = Sequence(
            'EV_', 'WORD', ':', 'NUMBER',
            '[', 'NUMBER', '|', 'NUMBER', ']',
            'STRING', 'NUMBER', 'NUMBER', 'WORD', 'WORD', ';')

        comment = Sequence(
            'CM_',
            choice(
                Sequence('SG_', 'NUMBER', 'WORD', 'STRING'),
                Sequence('BO_', 'NUMBER', 'STRING'),
                Sequence('EV_', 'WORD', 'STRING'),
                Sequence('BU_', 'WORD', 'STRING'),
                'STRING'),
            ';')

        attribute_definition = Sequence(
            'BA_DEF_',
            Optional(choice('SG_', 'BO_', 'EV_', 'BU_')),
            'STRING',
            'WORD',
            Optional(choice(DelimitedList('STRING'), ZeroOrMore('NUMBER'))),
            ';')

        attribute_definition_default = Sequence(
            'BA_DEF_DEF_', 'STRING', choice('NUMBER', 'STRING'), ';')

        attribute = Sequence(
            'BA_', 'STRING',
            ZeroOrMore(choice(Sequence('BO_', 'NUMBER'),
                              Sequence('SG_', 'NUMBER', 'WORD'),
                              Sequence('BU_', 'WORD'))),
            choice('NUMBER', 'STRING'),
            ';')

        attribute_definition_rel = Sequence(
            'BA_DEF_REL_',
            choice('STRING', Sequence('BU_SG_REL_', 'STRING')),
            'WORD',
            choice(';',
                   Sequence(DelimitedList('STRING'), ';'),
                   Sequence(OneOrMore('NUMBER'), ';')))

        attribute_definition_default_rel = Sequence(
            'BA_DEF_DEF_REL_','STRING', choice('NUMBER', 'STRING'), ';')

        attribute_rel = Sequence(
            'BA_REL_', 'STRING', 'BU_SG_REL_', 'WORD', 'SG_', 'NUMBER',
            'WORD', choice('NUMBER', 'STRING'), ';')

        choice_ = Sequence(
            'VAL_',
            choice(Sequence('NUMBER', 'WORD'),
                   Sequence('WORD')),
            OneOrMore(Sequence('NUMBER', 'STRING')),
            ';')

        value_table = Sequence(
            'VAL_TABLE_', 'WORD', OneOrMore(Sequence('NUMBER', 'STRING')), ';')

        signal_type = Sequence(
            'SIG_VALTYPE_', 'NUMBER', 'WORD', ':', 'NUMBER', ';')

        signal_multiplexer_values = Sequence(
            'SG_MUL_VAL_',
            'NUMBER',
            'WORD',
            'WORD',
            DelimitedList(Sequence('NUMBER', 'NUMBER')),
            ';')

        message_add_sender = Sequence(
            'BO_TX_BU_', 'NUMBER', ':', DelimitedList('WORD'), ';')

        signal_group = Sequence(
            'SIG_GROUP_', 'NUMBER', 'WORD', 'NUMBER', ':', OneOrMore('WORD'), ';')

        return OneOrMoreDict(
            choice(
                message,
                comment,
                attribute_definition,
                value_table,
                choice_,
                attribute,
                attribute_rel,
                attribute_definition_rel,
                attribute_definition_default,
                attribute_definition_default_rel,
                signal_group,
                signal_type,
                signal_multiplexer_values,
                message_add_sender,
                environment_variable,
                nodes,
                ns,
                bs,
                version
            )
        )


class DbcSpecifics(object):

    def __init__(self,
                 attributes = None,
                 attribute_definitions = None):
        self._attributes = attributes
        self._attribute_definitions = attribute_definitions

    @property
    def attributes(self):
        """The dbc specific attributes of the parent object (database, node,
        message or signal) as a dictionary.

        """

        return self._attributes

    @property
    def attribute_definitions(self):
        """The dbc specific attribute definitions as dictionary.

        """

        return self._attribute_definitions


def get_dbc_frame_id(message):
    frame_id = message.frame_id

    if message.is_extended_frame:
        frame_id |= 0x80000000

    return frame_id


def _dump_nodes(database):
    bu = []

    for node in database.nodes:
        bu.append(node.name)

    return bu


def _dump_messages(database):
    bo = []

    def format_mux(signal):
        if signal.is_multiplexer:
            return ' M'
        elif signal.multiplexer_ids is not None:
            return ' m{}'.format(signal.multiplexer_ids[0])
        else:
            return ''

    def format_receivers(signal):
        if signal.receivers:
            return ' ' + ','.join(signal.receivers)
        else:
            return 'Vector__XXX'

    def format_senders(message):
        if message.senders:
            return ' '.join(message.senders)
        else:
            return 'Vector__XXX'

    for message in database.messages:
        msg = []
        fmt = 'BO_ {frame_id} {name}: {length} {senders}'
        msg.append(fmt.format(frame_id=get_dbc_frame_id(message),
                              name=message.name,
                              length=message.length,
                              senders=format_senders(message)))

        for signal in message.signals[::-1]:
            fmt = (' SG_ {name}{mux} : {start}|{length}@{byte_order}{sign}'
                   ' ({scale},{offset})'
                   ' [{minimum}|{maximum}] "{unit}" {receivers}')
            msg.append(fmt.format(
                name=signal.name,
                mux=format_mux(signal),
                start=signal.start,
                length=signal.length,
                receivers=format_receivers(signal),
                byte_order=(0 if signal.byte_order == 'big_endian' else 1),
                sign=('-' if signal.is_signed else '+'),
                scale=signal.scale,
                offset=signal.offset,
                minimum=(0 if signal.minimum is None else signal.minimum),
                maximum=(0 if signal.maximum is None else signal.maximum),
                unit='' if signal.unit is None else signal.unit))

        bo.append('\r\n'.join(msg))

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
            cm.append(fmt.format(frame_id=get_dbc_frame_id(message),
                                 comment=message.comment))

        for signal in message.signals[::-1]:
            if signal.comment is not None:
                fmt = 'CM_ SG_ {frame_id} {name} "{comment}";'
                cm.append(fmt.format(frame_id=get_dbc_frame_id(message),
                                     name=signal.name,
                                     comment=signal.comment))

    return cm


def _dump_attribute_definitions(database):
    ba_def = []

    if database.dbc is None:
        return ba_def

    definitions = database.dbc.attribute_definitions

    def get_value(definition, value):
        if definition.minimum is None:
            value = ''
        else:
            value = ' {}'.format(value)

        return value

    def get_minimum(definition):
        return get_value(definition, definition.minimum)

    def get_maximum(definition):
        return get_value(definition, definition.maximum)

    def get_kind(definition):
        return '' if definition.kind is None else definition.kind + ' '

    for definition in definitions.values():
        if definition.type_name == 'ENUM':
            fmt = 'BA_DEF_ {kind}  "{name}" {type_name}  {choices};'
            choices = ','.join(['"{}"'.format(choice)
                                for choice in definition.choices])
            ba_def.append(fmt.format(kind=definition.kind,
                                     name=definition.name,
                                     type_name=definition.type_name,
                                     choices=choices))
        elif definition.type_name in ['INT', 'FLOAT', 'HEX']:
            fmt = 'BA_DEF_ {kind} "{name}" {type_name}{minimum}{maximum};'
            ba_def.append(fmt.format(kind=get_kind(definition),
                                     name=definition.name,
                                     type_name=definition.type_name,
                                     minimum=get_minimum(definition),
                                     maximum=get_maximum(definition)))
        elif definition.type_name == 'STRING':
            fmt = 'BA_DEF_ {kind} "{name}" {type_name} ;'
            ba_def.append(fmt.format(kind=get_kind(definition),
                                     name=definition.name,
                                     type_name=definition.type_name))

    return ba_def


def _dump_attribute_definition_defaults(database):
    ba_def_def = []

    if database.dbc is None:
        return ba_def_def

    definitions = database.dbc.attribute_definitions

    for definition in definitions.values():
        if definition.default_value is not None:
            if definition.type_name in ["STRING", "ENUM"]:
                fmt = 'BA_DEF_DEF_  "{name}" "{value}";'
            else:
                fmt = 'BA_DEF_DEF_  "{name}" {value};'

            ba_def_def.append(fmt.format(name=definition.name,
                                         value=definition.default_value))

    return ba_def_def


def _dump_attributes(database):
    ba = []

    def get_value(attribute):
        result = attribute.value

        if attribute.definition.type_name == "STRING":
            result = '"' + attribute.value + '"'

        return result

    if database.dbc is not None:
        if database.dbc.attributes is not None:
            for attribute in database.dbc.attributes.values():
                fmt = 'BA_ "{name}" {value};'
                ba.append(fmt.format(name=attribute.definition.name,
                                    value=get_value(attribute)))

    for node in database.nodes:
        if node.dbc is not None:
            if node.dbc.attributes is not None:
                for attribute in node.dbc.attributes.values():
                    fmt = 'BA_ "{name}" {kind} {node_name} {value};'
                    ba.append(fmt.format(name=attribute.definition.name,
                                        kind=attribute.definition.kind,
                                        node_name=node.name,
                                        value=get_value(attribute)))

    for message in database.messages:
        if message.dbc is not None:
            if message.dbc.attributes is not None:
                for attribute in message.dbc.attributes.values():
                    fmt = 'BA_ "{name}" {kind} {frame_id} {value};'
                    ba.append(fmt.format(name=attribute.definition.name,
                                        kind=attribute.definition.kind,
                                        frame_id=get_dbc_frame_id(message),
                                        value=get_value(attribute)))

        for signal in message.signals[::-1]:
            if signal.dbc is not None:
                if signal.dbc.attributes is not None:
                    for attribute in signal.dbc.attributes.values():
                        fmt = 'BA_ "{name}" {kind} {frame_id} {signal_name} {value};'
                        ba.append(fmt.format(name=attribute.definition.name,
                                            kind=attribute.definition.kind,
                                            frame_id=get_dbc_frame_id(message),
                                            signal_name=signal.name,
                                            value=get_value(attribute)))

    return ba


def _dump_choices(database):
    val = []

    for message in database.messages:
        for signal in message.signals[::-1]:
            if signal.choices is None:
                continue

            fmt = 'VAL_ {frame_id} {name} {choices} ;'
            val.append(fmt.format(
                frame_id=get_dbc_frame_id(message),
                name=signal.name,
                choices=' '.join(['{value} "{text}"'.format(value=value,
                                                            text=text)
                                  for value, text in signal.choices.items()])))

    return val


def _load_comments(tokens):
    comments = {}

    for comment in tokens.get('CM_', []):
        if not isinstance(comment[1], list):
            continue

        item = comment[1]
        kind = item[0]

        if kind == 'SG_':
            frame_id = int(item[1])

            if frame_id not in comments:
                comments[frame_id] = {}

            if 'signal' not in comments[frame_id]:
                comments[frame_id]['signal'] = {}

            comments[frame_id]['signal'][item[2]] = item[3]
        elif kind == 'BO_':
            frame_id = int(item[1])

            if frame_id not in comments:
                comments[frame_id] = {}

            comments[frame_id]['message'] = item[2]
        elif kind == 'BU_':
            node_name = item[1]
            comments[node_name] = item[2]

    return comments


def _load_attribute_definitions(tokens):
    return tokens.get('BA_DEF_', [])


def _load_attribute_definition_defaults(tokens):
    defaults = odict()

    for default_attr in tokens.get('BA_DEF_DEF_', []):
        defaults[default_attr[1]] = default_attr[2]

    return defaults


def _load_attributes(tokens, definitions):
    attributes = odict()

    def to_object(attribute):
        value = attribute[3]

        definition = definitions[attribute[1]]

        if definition.type_name in ['INT', 'HEX', 'ENUM']:
            value = int(value)
        elif definition.type_name == 'FLOAT':
            value = Decimal(value)

        return Attribute(value=value,
                         definition=definition)

    for attribute in tokens.get('BA_', []):
        name = attribute[1]

        if len(attribute[2]) > 0:
            item = attribute[2][0]
            kind = item[0]

            if kind == 'SG_':
                frame_id_dbc = int(item[1])
                signal = item[2]

                if frame_id_dbc not in attributes:
                    attributes[frame_id_dbc] = {}
                    attributes[frame_id_dbc]['message'] = odict()

                if 'signal' not in attributes[frame_id_dbc]:
                    attributes[frame_id_dbc]['signal'] = odict()

                if signal not in attributes[frame_id_dbc]['signal']:
                    attributes[frame_id_dbc]['signal'][signal] = odict()

                attributes[frame_id_dbc]['signal'][signal][name] = to_object(attribute)
            elif kind == 'BO_':
                frame_id_dbc = int(item[1])

                if frame_id_dbc not in attributes:
                    attributes[frame_id_dbc] = {}
                    attributes[frame_id_dbc]['message'] = odict()

                attributes[frame_id_dbc]['message'][name] = to_object(attribute)
            elif kind == 'BU_':
                node = item[1]

                if 'node' not in attributes:
                    attributes['node'] = odict()

                if node not in attributes['node']:
                    attributes['node'][node] = odict()

                attributes['node'][node][name] = to_object(attribute)
        else:
            if 'database' not in attributes:
                attributes['database'] = odict()

            attributes['database'][name] = to_object(attribute)

    return attributes


def _load_choices(tokens):
    choices = {}

    for choice in tokens.get('VAL_', []):
        if len(choice[1]) == 1:
            continue

        frame_id = int(choice[1][0])

        if frame_id not in choices:
            choices[frame_id] = {}

        choices[frame_id][choice[1][1]] = odict(
            (int(''.join(v[0])), v[1]) for v in choice[2])

    return choices


def _load_message_senders(tokens):
    """Load additional message senders.

    """

    message_senders = {}

    for senders in tokens.get('BO_TX_BU_', []):
        frame_id = int(senders[1])

        if frame_id not in message_senders:
            message_senders[frame_id] = []

        message_senders[frame_id] += list(senders[3])

    return message_senders


def _load_signal_types(tokens):
    """Load signal types.

    """

    signal_types = {}

    for signal_type in tokens.get('SIG_VALTYPE_', []):
        frame_id = int(signal_type[1])

        if frame_id not in signal_types:
            signal_types[frame_id] = {}

        signal_name = signal_type[2]
        signal_types[frame_id][signal_name] = int(signal_type[4])

    return signal_types


def _load_signal_multiplexer_values(tokens):
    """Load additional signal multiplexer values.

    """

    signal_multiplexer_values = {}

    for signal_multiplexer_value in tokens.get('SG_MUL_VAL_', []):
        frame_id = int(signal_multiplexer_value[1])
        signal_name = signal_multiplexer_value[2]
        multiplexer_signal = signal_multiplexer_value[3]
        multiplexer_ids = [int(v[0]) for v in signal_multiplexer_value[4]]

        if frame_id not in signal_multiplexer_values:
            signal_multiplexer_values[frame_id] = {}

        if multiplexer_signal not in signal_multiplexer_values[frame_id]:
            signal_multiplexer_values[frame_id][multiplexer_signal] = {}

        multiplexer_signal = signal_multiplexer_values[frame_id][multiplexer_signal]
        multiplexer_signal[signal_name] = multiplexer_ids

    return signal_multiplexer_values


def _load_messages(tokens,
                   comments,
                   attributes,
                   definitions,
                   choices,
                   message_senders,
                   signal_types,
                   signal_multiplexer_values,
                   strict):
    """Load messages.

    """

    def get_attributes(frame_id_dbc, signal=None):
        """Get attributes for given message or signal.

        """

        try:
            if signal is None:
                return attributes[frame_id_dbc]['message']
            else:
                return attributes[frame_id_dbc]['signal'][signal]
        except KeyError:
            return None

    def get_comment(frame_id_dbc, signal=None):
        """Get comment for given message or signal.

        """

        try:
            if signal is None:
                return comments[frame_id_dbc]['message']
            else:
                return comments[frame_id_dbc]['signal'][signal]
        except:
            return None

    def get_send_type(frame_id_dbc):
        """Get send type for a given message

        """

        result = None
        messageAttributes = get_attributes(frame_id_dbc)

        try:
            result = messageAttributes['GenMsgSendType'].value
            # Resolve ENUM index to ENUM text
            result = definitions['GenMsgSendType'].choices[int(result)]
        except (KeyError, TypeError):
            try:
                result = definitions['GenMsgSendType'].default_value
            except (KeyError, TypeError):
                result = None

        return result

    def get_cycle_time(frame_id_dbc):
        """Get cycle time for a given message

        """

        messageAttributes = get_attributes(frame_id_dbc)

        try:
            return int(messageAttributes['GenMsgCycleTime'].value)
        except (KeyError, TypeError):
            try:
                return int(definitions['GenMsgCycleTime'].default_value)
            except (KeyError, TypeError):
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

    def get_receivers(receivers):
        if receivers == ['Vector__XXX']:
            receivers = []

        return receivers

    def get_minimum(minimum, maximum):
        if minimum == maximum == 0:
            return None
        else:
            return minimum

    def get_maximum(minimum, maximum):
        if minimum == maximum == 0:
            return None
        else:
            return maximum

    messages = []

    for message in tokens.get('BO_', []):
        # Frame id.
        frame_id_dbc = int(message[1])
        frame_id = frame_id_dbc & 0x7fffffff
        is_extended_frame = bool(frame_id_dbc & 0x80000000)

        # Senders.
        senders = [message[5]]

        for node in message_senders.get(frame_id_dbc, []):
            if node not in senders:
                senders.append(node)

        if senders == ['Vector__XXX']:
            senders = []

        # Signal multiplexing.
        multiplexer_signal = None

        for signal in message[6]:
            if len(signal[1]) == 2:
                if signal[1][1] == 'M':
                    multiplexer_signal = signal[1][0]
                    break

        message = Message(
            frame_id=frame_id,
            is_extended_frame=is_extended_frame,
            name=message[2],
            length=int(message[4], 0),
            senders=senders,
            send_type=get_send_type(frame_id_dbc),
            cycle_time=get_cycle_time(frame_id_dbc),
            dbc_specifics=DbcSpecifics(attributes=get_attributes(frame_id_dbc),
                                       attribute_definitions=definitions),
            signals=[Signal(name=signal[1][0],
                            start=int(signal[3]),
                            length=int(signal[5]),
                            receivers=get_receivers(signal[20]),
                            byte_order=('big_endian'
                                        if signal[7] == '0'
                                        else 'little_endian'),
                            is_signed=(signal[8] == '-'),
                            scale=num(signal[10]),
                            offset=num(signal[12]),
                            minimum=get_minimum(num(signal[15]),
                                                num(signal[17])),
                            maximum=get_maximum(num(signal[15]),
                                                num(signal[17])),
                            unit=None if signal[19] == '' else signal[19],
                            choices=get_choices(frame_id_dbc,
                                                signal[1][0]),
                            dbc_specifics=DbcSpecifics(attributes=get_attributes(
                                                                    frame_id_dbc,
                                                                    signal[1][0]),
                                                       attribute_definitions=definitions),
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
                     for signal in message[6]],
            comment=get_comment(frame_id_dbc),
            strict=strict)
        messages.append(message)

    return messages


def _load_version(tokens):
    return tokens.get('VERSION', [[None, None]])[0][1]


def _load_nodes(tokens, comments, attributes, definitions):
    def get_node_comment(node_name):
        """Get comment for a given node_name

        """

        try:
            return comments[node_name]
        except KeyError:
            return None

    def get_node_attributes(node_name):
        """Get attributes for given node.

        """

        try:
            return attributes['node'][node_name]
        except KeyError:
            return None

    nodes = None

    for token in tokens.get('BU_', []):
        nodes = [Node(name=node,
                      comment=get_node_comment(node),
                      dbc_specifics=DbcSpecifics(get_node_attributes(node),
                                                 definitions))
                 for node in token[2]]

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
                          bo='\r\n\r\n'.join(bo),
                          cm='\r\n'.join(cm),
                          ba_def='\r\n'.join(ba_def),
                          ba_def_def='\r\n'.join(ba_def_def),
                          ba='\r\n'.join(ba),
                          val='\r\n'.join(val))


def get_definitions_dict(definitions, defaults):
    result = odict()

    def convert_value(definition, value):
        if definition.type_name in ['INT', 'HEX']:
            value = int(value)
        elif definition.type_name == 'FLOAT':
            value = Decimal(value)

        return value

    for item in definitions:
        if len(item[1]) > 0:
            kind = item[1][0]
        else:
            kind = None

        definition = AttributeDefinition(name=item[2],
                                         kind=kind,
                                         type_name=item[3])
        values = item[4][0]

        if len(values) > 0:
            if definition.type_name == "ENUM":
                definition.choices = values
            elif definition.type_name in ['INT', 'FLOAT', 'HEX']:
                definition.minimum = convert_value(definition, values[0])
                definition.maximum = convert_value(definition, values[1])

        try:
            value = defaults[definition.name]
            definition.default_value = convert_value(definition, value)
        except KeyError:
            definition.default_value = None

        result[definition.name] = definition

    return result


def load_string(string, strict=True):
    """Parse given string.

    """

    def get_database_attributes():
        """Get attributes for the database.

        """

        try:
            return attributes['database']
        except KeyError:
            return None


    tokens = Parser().parse(string)

    comments = _load_comments(tokens)
    definitions = _load_attribute_definitions(tokens)
    defaults = _load_attribute_definition_defaults(tokens)
    attribute_definitions = get_definitions_dict(definitions, defaults)
    attributes = _load_attributes(tokens, attribute_definitions)
    choices = _load_choices(tokens)
    message_senders = _load_message_senders(tokens)
    signal_types = _load_signal_types(tokens)
    signal_multiplexer_values = _load_signal_multiplexer_values(tokens)
    messages = _load_messages(tokens,
                              comments,
                              attributes,
                              attribute_definitions,
                              choices,
                              message_senders,
                              signal_types,
                              signal_multiplexer_values,
                              strict)
    nodes = _load_nodes(tokens, comments, attributes, attribute_definitions)
    version = _load_version(tokens)
    dbc_specifics = DbcSpecifics(attributes=get_database_attributes(),
                                  attribute_definitions=attribute_definitions)

    return InternalDatabase(messages,
                            nodes,
                            [],
                            version,
                            dbc_specifics)
