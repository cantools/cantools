# Load and dump a CAN database in DBC format.

import re
from collections import OrderedDict as odict
from collections import defaultdict
from decimal import Decimal
from copy import deepcopy

import textparser
from textparser import Sequence
from textparser import choice
from textparser import ZeroOrMore
from textparser import OneOrMore
from textparser import OneOrMoreDict
from textparser import DelimitedList
from textparser import Any
from textparser import AnyUntil
from textparser import tokenize_init
from textparser import Token
from textparser import TokenizeError
from textparser import Optional

from ..attribute_definition import AttributeDefinition
from ..attribute import Attribute
from ..signal import Signal
from ..signal import Decimal as SignalDecimal
from ..signal_group import SignalGroup
from ..message import Message
from ..node import Node
from ..bus import Bus
from ..internal_database import InternalDatabase
from ..environment_variable import EnvironmentVariable

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
    '{val_table}'
    '\r\n'
    '\r\n'
    '{bo}\r\n'
    '\r\n'
    '{bo_tx_bu}\r\n'
    '\r\n'
    '\r\n'
    '{cm}\r\n'
    '{ba_def}\r\n'
    '{ba_def_def}\r\n'
    '{ba}\r\n'
    '{val}\r\n'
    '{signal_types}\r\n'
    '{sig_group}\r\n'
    '{sig_mux_values}\r\n'
)


# Signal types.
SIGNAL_TYPE_FLOAT = 1
SIGNAL_TYPE_DOUBLE = 2

FLOAT_SIGNAL_TYPES = [
    SIGNAL_TYPE_FLOAT,
    SIGNAL_TYPE_DOUBLE
]

FLOAT_LENGTH_TO_SIGNAL_TYPE = {
    32: SIGNAL_TYPE_FLOAT,
    64: SIGNAL_TYPE_DOUBLE
}

ATTRIBUTE_DEFINITION_LONG_NODE_NAME = AttributeDefinition(
    'SystemNodeLongSymbol',
    default_value='',
    kind='BU_',
    type_name='STRING')

ATTRIBUTE_DEFINITION_LONG_MESSAGE_NAME = AttributeDefinition(
    'SystemMessageLongSymbol',
    default_value='',
    kind='BO_',
    type_name='STRING')

ATTRIBUTE_DEFINITION_LONG_SIGNAL_NAME = AttributeDefinition(
    'SystemSignalLongSymbol',
    default_value='',
    kind='SG_',
    type_name='STRING')


def to_int(value):
    return int(Decimal(value))


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
            ('NUMBER',   r'[-+]?\d+\.?\d*([eE][+-]?\d+)?'),
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

        ns = Sequence('NS_', ':', AnyUntil(Sequence(Any(), ':')))

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
                              Sequence('BU_', 'WORD'),
                              Sequence('EV_', 'WORD'))),
            choice('NUMBER', 'STRING'),
            ';')

        attribute_definition_rel = Sequence(
            'BA_DEF_REL_',
            Optional('BU_SG_REL_'),
            'STRING',
            'WORD',
            choice(DelimitedList('STRING'), OneOrMore('NUMBER')),
            ';')

        attribute_definition_default_rel = Sequence(
            'BA_DEF_DEF_REL_', 'STRING', choice('NUMBER', 'STRING'), ';')

        attribute_rel = Sequence(
            'BA_REL_', 'STRING', 'BU_SG_REL_', 'WORD', 'SG_', 'NUMBER',
            'WORD', choice('NUMBER', 'STRING'), ';')

        choice_ = Sequence(
            'VAL_',
            Optional('NUMBER'),
            'WORD',
            ZeroOrMore(Sequence('NUMBER', 'STRING')),
            ';')

        value_table = Sequence(
            'VAL_TABLE_', 'WORD', ZeroOrMore(Sequence('NUMBER', 'STRING')), ';')

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
            'SIG_GROUP_', 'NUMBER', 'WORD', 'NUMBER', ':', ZeroOrMore('WORD'), ';')

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
                version))


class DbcSpecifics(object):

    def __init__(self,
                 attributes=None,
                 attribute_definitions=None,
                 environment_variables=None,
                 value_tables=None):
        if attributes is None:
            attributes = odict()

        if attribute_definitions is None:
            attribute_definitions = odict()

        if environment_variables is None:
            environment_variables = odict()

        if value_tables is None:
            value_tables = odict()

        self._attributes = attributes
        self._attribute_definitions = attribute_definitions
        self._environment_variables = environment_variables
        self._value_tables = value_tables

    @property
    def attributes(self):
        """The DBC specific attributes of the parent object (database, node,
        message or signal) as a dictionary.

        """

        return self._attributes

    @attributes.setter
    def attributes(self, value):
        self._attributes = value

    @property
    def attribute_definitions(self):
        """The DBC specific attribute definitions as dictionary.

        """

        return self._attribute_definitions

    @property
    def value_tables(self):
        """An ordered dictionary of all value tables. Only valid for DBC
        specifiers on database level.

        """

        return self._value_tables

    @property
    def environment_variables(self):
        """An ordered dictionary of all environment variables. Only valid for
        DBC specifiers on database level.

        """

        return self._environment_variables


class LongNamesConverter(object):

    def __init__(self, database):
        self._database = database
        self._next_index_per_cut_name = defaultdict(int)
        self._short_names = set()

    def convert(self, name):
        short_name = None

        if len(name) == 32:
            self._short_names.add(name)
        elif len(name) > 32:
            cut_name = name[:27]
            short_name = name[:32]

            if short_name in self._short_names:
                index = self._next_index_per_cut_name[cut_name]
                self._next_index_per_cut_name[cut_name] += 1
                short_name = '{}_{:04d}'.format(name[:27], index)
            else:
                self._next_index_per_cut_name[cut_name] = 0
                self._short_names.add(short_name)

        return short_name


def get_dbc_frame_id(message):
    frame_id = message.frame_id

    if message.is_extended_frame:
        frame_id |= 0x80000000

    return frame_id


def _get_node_name(attributes, name):
    try:
        return attributes['node'][name]['SystemNodeLongSymbol'].value
    except (KeyError, TypeError):
        return name


def _get_environment_variable_name(attributes, name):
    try:
        return attributes['envvar'][name]['SystemEnvVarLongSymbol'].value
    except (KeyError, TypeError):
        return name


def _dump_version(database):
    return '' if database.version is None else database.version


def _dump_nodes(database):
    bu = []

    for node in database.nodes:
        bu.append(node.name)

    return bu


def _dump_value_tables(database):
    if database.dbc is None:
        return []

    val_table = []

    for name, choices in database.dbc.value_tables.items():
        choices = [
            '{} "{}"'.format(number, text)
            for number, text in reversed(sorted(choices.items()))
        ]
        val_table.append('VAL_TABLE_ {} {} ;'.format(name, ' '.join(choices)))

    return val_table + ['']


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
            return message.senders[0]
        else:
            return 'Vector__XXX'

    for message in database.messages:
        msg = []
        msg.append(
            'BO_ {frame_id} {name}: {length} {senders}'.format(
                frame_id=get_dbc_frame_id(message),
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


def _dump_senders(database):
    bo_tx_bu = []

    for message in database.messages:
        if len(message.senders) > 1:
            bo_tx_bu.append(
                'BO_TX_BU_ {frame_id} : {senders};'.format(
                    frame_id=get_dbc_frame_id(message),
                    senders=','.join(message.senders)))

    return bo_tx_bu


def _dump_comments(database):
    cm = []

    for bus in database.buses:
        if bus.comment is not None:
            cm.append(
                'CM_ "{comment}";'.format(
                    comment=bus.comment) )

    for node in database.nodes:
        if node.comment is not None:
            cm.append(
                'CM_ BU_ {name} "{comment}";'.format(
                    name=node.name,
                    comment=node.comment.replace('"', '\\"')))

    for message in database.messages:
        if message.comment is not None:
            cm.append(
                'CM_ BO_ {frame_id} "{comment}";'.format(
                    frame_id=get_dbc_frame_id(message),
                    comment=message.comment.replace('"', '\\"')))

        for signal in message.signals[::-1]:
            if signal.comment is not None:
                cm.append(
                    'CM_ SG_ {frame_id} {name} "{comment}";'.format(
                        frame_id=get_dbc_frame_id(message),
                        name=signal.name,
                        comment=signal.comment.replace('"', '\\"')))

    return cm


def _dump_signal_types(database):
    valtype = []

    for message in database.messages:
        for signal in message.signals:
            if not signal.is_float:
                continue

            valtype.append(
                'SIG_VALTYPE_ {} {} : {};'.format(
                    get_dbc_frame_id(message),
                    signal.name,
                    FLOAT_LENGTH_TO_SIGNAL_TYPE[signal.length]))

    return valtype


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
            choices = ','.join(['"{}"'.format(choice)
                                for choice in definition.choices])
            ba_def.append(
                'BA_DEF_ {kind} "{name}" {type_name}  {choices};'.format(
                    kind=get_kind(definition),
                    name=definition.name,
                    type_name=definition.type_name,
                    choices=choices))
        elif definition.type_name in ['INT', 'FLOAT', 'HEX']:
            ba_def.append(
                'BA_DEF_ {kind} "{name}" {type_name}{minimum}{maximum};'.format(
                    kind=get_kind(definition),
                    name=definition.name,
                    type_name=definition.type_name,
                    minimum=get_minimum(definition),
                    maximum=get_maximum(definition)))
        elif definition.type_name == 'STRING':
            ba_def.append(
                'BA_DEF_ {kind} "{name}" {type_name} ;'.format(
                    kind=get_kind(definition),
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
                ba.append(
                    'BA_ "{name}" {value};'.format(name=attribute.definition.name,
                                                   value=get_value(attribute)))

    for node in database.nodes:
        if node.dbc is not None:
            if node.dbc.attributes is not None:
                for attribute in node.dbc.attributes.values():
                    ba.append(
                        'BA_ "{name}" {kind} {node_name} {value};'.format(
                            name=attribute.definition.name,
                            kind=attribute.definition.kind,
                            node_name=node.name,
                            value=get_value(attribute)))

    for message in database.messages:
        if message.dbc is not None:
            if message.dbc.attributes is not None:
                for attribute in message.dbc.attributes.values():
                    ba.append(
                        'BA_ "{name}" {kind} {frame_id} {value};'.format(
                            name=attribute.definition.name,
                            kind=attribute.definition.kind,
                            frame_id=get_dbc_frame_id(message),
                            value=get_value(attribute)))

        for signal in message.signals[::-1]:
            if signal.dbc is not None:
                if signal.dbc.attributes is not None:
                    for attribute in signal.dbc.attributes.values():
                        ba.append(
                            'BA_ "{name}" {kind} {frame_id} {signal_name} {value};'.format(
                                name=attribute.definition.name,
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

            val.append(
                'VAL_ {frame_id} {name} {choices} ;'.format(
                    frame_id=get_dbc_frame_id(message),
                    name=signal.name,
                    choices=' '.join(['{value} "{text}"'.format(value=value,
                                                                text=text)
                                      for value, text in signal.choices.items()])))

    return val


def _dump_signal_groups(database):
    sig_group = []

    for message in database.messages:
        if message.signal_groups is None:
            continue

        for signal_group in message.signal_groups:
            all_sig_names = list(map(lambda sig: sig.name, message.signals))
            signal_group.signal_names = list(filter(lambda sig_name: sig_name in all_sig_names, signal_group.signal_names))
            sig_group.append(
                'SIG_GROUP_ {frame_id} {signal_group_name} {repetitions} : {signal_names};'.format(
                    frame_id=get_dbc_frame_id(message),
                    signal_group_name=signal_group.name,
                    repetitions=signal_group.repetitions,
                    signal_names=' '.join(signal_group.signal_names)
                ))

    return sig_group


def _is_extended_mux_needed(messages):
    """Check for messages with more than one mux signal or signals with
    more than one multiplexer value.

    """

    for message in messages:
        multiplexers = [
            signal.name
            for signal in message.signals
            if signal.is_multiplexer
        ]

        if len(multiplexers) > 1:
            return True

        for signal in message.signals:
            if signal.multiplexer_ids:
                if len(signal.multiplexer_ids) > 1:
                    return True

    return False


def _create_mux_ranges(multiplexer_ids):
    """Create a list of ranges based on a list of single values.

    Example:
        Input:  [1, 2, 3, 5,      7, 8, 9]
        Output: [[1, 3], [5, 5], [7, 9]]

    """

    ordered = sorted(multiplexer_ids)
    # Anything but ordered[0] - 1
    prev_value = ordered[0]
    ranges = []

    for value in ordered:
        if value == prev_value + 1:
            ranges[-1][1] = value
        else:
            ranges.append([value, value])

        prev_value = value

    return ranges


def _dump_signal_mux_values(database):
    """Create multiplex entries ("SG_MUL_VAL_") if extended multiplexing
    is used.

    """

    if not _is_extended_mux_needed(database.messages):
        return []

    sig_mux_values = []

    for message in database.messages:
        for signal in message.signals:
            if not signal.multiplexer_ids:
                continue

            ranges = ', '.join([
                '{}-{}'.format(minimum, maximum)
                for minimum, maximum in _create_mux_ranges(signal.multiplexer_ids)
            ])

            sig_mux_values.append(
                'SG_MUL_VAL_ {frame_id} {name} {multiplexer} {ranges};'.format(
                    frame_id=get_dbc_frame_id(message),
                    name=signal.name,
                    multiplexer=signal.multiplexer_signal,
                    ranges=ranges))

    return sig_mux_values


def _load_comments(tokens):
    comments = defaultdict(dict)

    for comment in tokens.get('CM_', []):
        if not isinstance(comment[1], list):
            comments['database']['bus'] = comment[1]
            continue

        item = comment[1]
        kind = item[0]

        if kind == 'SG_':
            frame_id = int(item[1])

            if 'signal' not in comments[frame_id]:
                comments[frame_id]['signal'] = {}

            comments[frame_id]['signal'][item[2]] = item[3]
        elif kind == 'BO_':
            frame_id = int(item[1])
            comments[frame_id]['message'] = item[2]
        elif kind == 'BU_':
            node_name = item[1]
            comments[node_name] = item[2]
        elif kind == 'EV_':
            environment_variable_name = item[1]
            comments[environment_variable_name] = item[2]

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
    attributes['node'] = odict()

    def to_object(attribute):
        value = attribute[3]

        definition = definitions[attribute[1]]

        if definition.type_name in ['INT', 'HEX', 'ENUM']:
            value = to_int(value)
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

                if node not in attributes['node']:
                    attributes['node'][node] = odict()

                attributes['node'][node][name] = to_object(attribute)
            elif kind == 'EV_':
                envvar = item[1]

                if 'envvar' not in attributes:
                    attributes['envvar'] = odict()

                if envvar not in attributes['envvar']:
                    attributes['envvar'][envvar] = odict()

                attributes['envvar'][envvar][name] = to_object(attribute)
        else:
            if 'database' not in attributes:
                attributes['database'] = odict()

            attributes['database'][name] = to_object(attribute)

    return attributes


def _load_value_tables(tokens):
    """Load value tables, that is, choice definitions.

    """

    value_tables = odict()

    for value_table in tokens.get('VAL_TABLE_', []):
        name = value_table[1]
        choices = {int(number): text for number, text in value_table[2]}
        value_tables[name] = choices

    return value_tables


def _load_environment_variables(tokens, comments, attributes):
    environment_variables = odict()

    for env_var in tokens.get('EV_', []):
        name = _get_environment_variable_name(attributes, env_var[1])
        environment_variables[name] = EnvironmentVariable(
            name=name,
            env_type=int(env_var[3]),
            minimum=num(env_var[5]),
            maximum=num(env_var[7]),
            unit=env_var[9],
            initial_value=num(env_var[10]),
            env_id=int(env_var[11]),
            access_type=env_var[12],
            access_node=env_var[13],
            comment=comments.get(env_var[1], None))

    return environment_variables

def _load_choices(tokens):
    choices = defaultdict(dict)

    for choice in tokens.get('VAL_', []):
        if len(choice[1]) == 0:
            continue

        od = odict((int(''.join(v[0])), v[1]) for v in choice[3])

        if len(od) == 0:
            continue

        frame_id = int(choice[1][0])
        choices[frame_id][choice[2]] = od

    return choices

def _load_message_senders(tokens, attributes):
    """Load additional message senders.

    """

    message_senders = defaultdict(list)

    for senders in tokens.get('BO_TX_BU_', []):
        frame_id = int(senders[1])
        message_senders[frame_id] += [
            _get_node_name(attributes, sender) for sender in senders[3]
        ]

    return message_senders


def _load_signal_types(tokens):
    """Load signal types.

    """

    signal_types = defaultdict(dict)

    for signal_type in tokens.get('SIG_VALTYPE_', []):
        frame_id = int(signal_type[1])
        signal_name = signal_type[2]
        signal_types[frame_id][signal_name] = int(signal_type[4])

    return signal_types


def _load_signal_multiplexer_values(tokens):
    """Load additional signal multiplexer values.

    """

    signal_multiplexer_values = defaultdict(dict)

    for signal_multiplexer_value in tokens.get('SG_MUL_VAL_', []):
        frame_id = int(signal_multiplexer_value[1])
        signal_name = signal_multiplexer_value[2]
        multiplexer_signal = signal_multiplexer_value[3]
        multiplexer_ids = []

        for lower, upper in signal_multiplexer_value[4]:
            lower = int(lower)
            upper = int(upper[1:])
            # ToDo: Probably store ranges as tuples to not run out of
            #       memory on huge ranges.
            multiplexer_ids.extend(range(lower, upper + 1))

        if multiplexer_signal not in signal_multiplexer_values[frame_id]:
            signal_multiplexer_values[frame_id][multiplexer_signal] = {}

        multiplexer_signal = signal_multiplexer_values[frame_id][multiplexer_signal]
        multiplexer_signal[signal_name] = multiplexer_ids

    return signal_multiplexer_values


def _load_signal_groups(tokens):
    """Load signal groups.

    """

    signal_groups = defaultdict(list)

    for signal_group in tokens.get('SIG_GROUP_',[]):
        frame_id = int(signal_group[1])
        signal_groups[frame_id].append(SignalGroup(name=signal_group[2],
                                                   repetitions=int(signal_group[3]),
                                                   signal_names=signal_group[5]))

    return signal_groups


def _load_signals(tokens,
                  comments,
                  attributes,
                  definitions,
                  choices,
                  signal_types,
                  signal_multiplexer_values,
                  frame_id_dbc,
                  multiplexer_signal):
    signal_to_multiplexer = {}

    try:
        signal_multiplexer_values = signal_multiplexer_values[frame_id_dbc]

        for multiplexer_name, items in signal_multiplexer_values.items():
            for name in items:
                signal_to_multiplexer[name] = multiplexer_name
    except KeyError:
        pass

    def get_attributes(frame_id_dbc, signal):
        """Get attributes for given signal.

        """

        try:
            return attributes[frame_id_dbc]['signal'][signal]
        except KeyError:
            return None

    def get_comment(frame_id_dbc, signal):
        """Get comment for given signal.

        """

        try:
            return comments[frame_id_dbc]['signal'][signal]
        except:
            return None

    def get_choices(frame_id_dbc, signal):
        """Get choices for given signal.

        """

        try:
            return choices[frame_id_dbc][signal]
        except KeyError:
            return None

    def get_is_multiplexer(signal):
        if len(signal[1]) == 2:
            return signal[1][1].endswith('M')
        else:
            return False

    def get_multiplexer_ids(signal, multiplexer_signal):
        ids = []

        if multiplexer_signal is not None:
            if len(signal) == 2 and not signal[1].endswith('M'):
                value = signal[1][1:].rstrip('M')
                ids.append(int(value))
        else:
            multiplexer_signal = get_multiplexer_signal(signal,
                                                        multiplexer_signal)

        try:
            ids.extend(
                signal_multiplexer_values[multiplexer_signal][signal[0]])
        except KeyError:
            pass

        if ids:
            return list(set(ids))

    def get_multiplexer_signal(signal, multiplexer_signal):
        if len(signal) != 2:
            return

        if multiplexer_signal is None:
            try:
                return signal_to_multiplexer[signal[0]]
            except KeyError:
                pass
        elif signal[0] != multiplexer_signal:
            return multiplexer_signal

    def get_receivers(receivers):
        if receivers == ['Vector__XXX']:
            receivers = []

        return [_get_node_name(attributes, receiver) for receiver in receivers]

    def get_minimum(minimum, maximum):
        if minimum == maximum == '0':
            return None
        else:
            return num(minimum)

    def get_maximum(minimum, maximum):
        if minimum == maximum == '0':
            return None
        else:
            return num(maximum)

    def get_minimum_decimal(minimum, maximum):
        if minimum == maximum == '0':
            return None
        else:
            return Decimal(minimum)

    def get_maximum_decimal(minimum, maximum):
        if minimum == maximum == '0':
            return None
        else:
            return Decimal(maximum)

    def get_is_float(frame_id_dbc, signal):
        """Get is_float for given signal.

        """

        try:
            return signal_types[frame_id_dbc][signal] in FLOAT_SIGNAL_TYPES
        except KeyError:
            return False

    def get_signal_name(frame_id_dbc, name):
        signal_attributes = get_attributes(frame_id_dbc, name)

        try:
            return signal_attributes['SystemSignalLongSymbol'].value
        except (KeyError, TypeError):
            return name

    def get_signal_initial_value(frame_id_dbc, name):
        signal_attributes = get_attributes(frame_id_dbc, name)

        try:
            return signal_attributes['GenSigStartValue'].value
        except (KeyError, TypeError):
            return None

    def get_signal_spn(frame_id_dbc, name):
        signal_attributes = get_attributes(frame_id_dbc, name)
        
        try:
            return signal_attributes['SPN'].value
        except (KeyError, TypeError):
            return None

    signals = []

    for signal in tokens:
        signals.append(
            Signal(name=get_signal_name(frame_id_dbc, signal[1][0]),
                   start=int(signal[3]),
                   length=int(signal[5]),
                   receivers=get_receivers(signal[20]),
                   byte_order=('big_endian'
                               if signal[7] == '0'
                               else 'little_endian'),
                   is_signed=(signal[8] == '-'),
                   initial=get_signal_initial_value(frame_id_dbc, signal[1][0]),
                   scale=num(signal[10]),
                   offset=num(signal[12]),
                   minimum=get_minimum(signal[15], signal[17]),
                   maximum=get_maximum(signal[15], signal[17]),
                   decimal=SignalDecimal(Decimal(signal[10]),
                                         Decimal(signal[12]),
                                         get_minimum_decimal(signal[15],
                                                             signal[17]),
                                         get_maximum_decimal(signal[15],
                                                             signal[17])),
                   unit=(None if signal[19] == '' else signal[19]),
                   spn=get_signal_spn(frame_id_dbc, signal[1][0]),
                   choices=get_choices(frame_id_dbc,
                                       signal[1][0]),
                   dbc_specifics=DbcSpecifics(get_attributes(frame_id_dbc, signal[1][0]),
                                              definitions),
                   comment=get_comment(frame_id_dbc,
                                       signal[1][0]),
                   is_multiplexer=get_is_multiplexer(signal),
                   multiplexer_ids=get_multiplexer_ids(signal[1],
                                                       multiplexer_signal),
                   multiplexer_signal=get_multiplexer_signal(signal[1],
                                                             multiplexer_signal),
                   is_float=get_is_float(frame_id_dbc, signal[1][0])))

    return signals


def _load_messages(tokens,
                   comments,
                   attributes,
                   definitions,
                   choices,
                   message_senders,
                   signal_types,
                   signal_multiplexer_values,
                   strict,
                   bus_name,
                   signal_groups):
    """Load messages.

    """

    def get_attributes(frame_id_dbc):
        """Get attributes for given message.

        """

        try:
            return attributes[frame_id_dbc]['message']
        except KeyError:
            return None

    def get_comment(frame_id_dbc):
        """Get comment for given message.

        """

        try:
            return comments[frame_id_dbc]['message']
        except:
            return None

    def get_send_type(frame_id_dbc):
        """Get send type for a given message.

        """

        result = None
        message_attributes = get_attributes(frame_id_dbc)

        try:
            result = message_attributes['GenMsgSendType'].value
            # Resolve ENUM index to ENUM text
            result = definitions['GenMsgSendType'].choices[int(result)]
        except (KeyError, TypeError):
            try:
                result = definitions['GenMsgSendType'].default_value
            except (KeyError, TypeError):
                result = None

        return result

    def get_cycle_time(frame_id_dbc):
        """Get cycle time for a given message.

        """

        message_attributes = get_attributes(frame_id_dbc)

        try:
            return int(message_attributes['GenMsgCycleTime'].value)
        except (KeyError, TypeError):
            try:
                return int(definitions['GenMsgCycleTime'].default_value)
            except (KeyError, TypeError):
                return None

    def get_protocol(frame_id_dbc):
        """Get protocol for a given message.

        """

        message_attributes = get_attributes(frame_id_dbc)

        try:
            frame_format = message_attributes['VFrameFormat'].value
            frame_format = definitions['VFrameFormat'].choices[frame_format]
        except (KeyError, TypeError):
            try:
                frame_format = definitions['VFrameFormat'].default_value
            except (KeyError, TypeError):
                frame_format = None

        if frame_format == 'J1939PG':
            return 'j1939'
        else:
            return None

    def get_message_name(frame_id_dbc, name):
        message_attributes = get_attributes(frame_id_dbc)

        try:
            return message_attributes['SystemMessageLongSymbol'].value
        except (KeyError, TypeError):
            return name

    def get_signal_groups(frame_id_dbc):
        try:
            return signal_groups[frame_id_dbc]
        except:
            return None

    messages = []

    for message in tokens.get('BO_', []):
        # Any message named VECTOR__INDEPENDENT_SIG_MSG contains
        # signals not assigned to any message. Cantools does not yet
        # support unassigned signals. Discard them for now.
        if message[2] == 'VECTOR__INDEPENDENT_SIG_MSG':
            continue

        # Frame id.
        frame_id_dbc = int(message[1])
        frame_id = frame_id_dbc & 0x7fffffff
        is_extended_frame = bool(frame_id_dbc & 0x80000000)

        # Senders.
        senders = [_get_node_name(attributes, message[5])]

        for node in message_senders.get(frame_id_dbc, []):
            if node not in senders:
                senders.append(node)

        if senders == ['Vector__XXX']:
            senders = []

        # Signal multiplexing.
        multiplexer_signal = None

        for signal in message[6]:
            if len(signal[1]) == 2:
                if signal[1][1].endswith('M'):
                    if multiplexer_signal is None:
                        multiplexer_signal = signal[1][0]
                    else:
                        multiplexer_signal = None
                        break

        signals = _load_signals(message[6],
                                comments,
                                attributes,
                                definitions,
                                choices,
                                signal_types,
                                signal_multiplexer_values,
                                frame_id_dbc,
                                multiplexer_signal)

        messages.append(
            Message(frame_id=frame_id,
                    is_extended_frame=is_extended_frame,
                    name=get_message_name(frame_id_dbc, message[2]),
                    length=int(message[4], 0),
                    senders=senders,
                    send_type=get_send_type(frame_id_dbc),
                    cycle_time=get_cycle_time(frame_id_dbc),
                    dbc_specifics=DbcSpecifics(get_attributes(frame_id_dbc),
                                               definitions),
                    signals=signals,
                    comment=get_comment(frame_id_dbc),
                    strict=strict,
                    protocol=get_protocol(frame_id_dbc),
                    bus_name=bus_name,
                    signal_groups=get_signal_groups(frame_id_dbc)))

    return messages


def _load_version(tokens):
    return tokens.get('VERSION', [[None, None]])[0][1]


def _load_bus(attributes, comments):
    try:
        bus_name = attributes['database']['DBName'].value
    except KeyError:
        return None

    try:
        bus_baudrate = attributes['database']['Baudrate'].value
    except KeyError:
        bus_baudrate = None

    try:
        bus_comment = comments['database']['bus']
    except KeyError:
        bus_comment = None

    return Bus(bus_name, baudrate=bus_baudrate, comment=bus_comment)


def _load_nodes(tokens, comments, attributes, definitions):
    nodes = None

    for token in tokens.get('BU_', []):
        nodes = [Node(name=_get_node_name(attributes, node),
                      comment=comments.get(node, None),
                      dbc_specifics=DbcSpecifics(attributes['node'].get(node, None),
                                                 definitions))
                 for node in token[2]]

    return nodes


def get_attribute_definition(database, name, default):
    if database.dbc is None:
        database.dbc = DbcSpecifics()

    if name not in database.dbc.attribute_definitions:
        database.dbc.attribute_definitions[name] = default

    return database.dbc.attribute_definitions[name]


def get_long_node_name_attribute_definition(database):
    return get_attribute_definition(database,
                                    'SystemNodeLongSymbol',
                                    ATTRIBUTE_DEFINITION_LONG_NODE_NAME)


def get_long_message_name_attribute_definition(database):
    return get_attribute_definition(database,
                                    'SystemMessageLongSymbol',
                                    ATTRIBUTE_DEFINITION_LONG_MESSAGE_NAME)


def get_long_signal_name_attribute_definition(database):
    return get_attribute_definition(database,
                                    'SystemSignalLongSymbol',
                                    ATTRIBUTE_DEFINITION_LONG_SIGNAL_NAME)


def try_remove_attribute(dbc, name):
    try:
        dbc.attributes.pop(name)
    except (KeyError, AttributeError):
        pass


def make_node_names_unique(database):
    converter = LongNamesConverter(database)

    for node in database.nodes:
        name = converter.convert(node.name)
        try_remove_attribute(node.dbc, 'SystemNodeLongSymbol')

        if name is None:
            continue

        for message in database.messages:
            for index, sender in enumerate(message.senders):
                if sender == node.name:
                    message.senders[index] = name

            for signal in message.signals:
                for index, receiver in enumerate(signal.receivers):
                    if receiver == node.name:
                        signal.receivers[index] = name

        if node.dbc is None:
            node.dbc = DbcSpecifics()

        node.dbc.attributes['SystemNodeLongSymbol'] = Attribute(
            node.name,
            get_long_node_name_attribute_definition(database))
        node.name = name


def make_message_names_unique(database):
    converter = LongNamesConverter(database)

    for message in database.messages:
        name = converter.convert(message.name)
        try_remove_attribute(message.dbc, 'SystemMessageLongSymbol')

        if name is None:
            continue

        if message.dbc is None:
            message.dbc = DbcSpecifics()

        message.dbc.attributes['SystemMessageLongSymbol'] = Attribute(
            message.name,
            get_long_message_name_attribute_definition(database))
        message.name = name


def make_signal_names_unique(database):
    converter = LongNamesConverter(database)

    for message in database.messages:
        for signal in message.signals:
            name = converter.convert(signal.name)
            try_remove_attribute(signal.dbc, 'SystemSignalLongSymbol')

            if name is None:
                continue

            if signal.dbc is None:
                signal.dbc = DbcSpecifics()

            signal.dbc.attributes['SystemSignalLongSymbol'] = Attribute(
                signal.name,
                get_long_signal_name_attribute_definition(database))
            signal.name = name


def make_names_unique(database):
    """Make message, signal and node names unique and add attributes for
    their long names.

    """

    make_node_names_unique(database)
    make_message_names_unique(database)
    make_signal_names_unique(database)

    return database


def dump_string(database):
    """Format database in DBC file format.

    """

    # Make a deep copy of the database as names and attributes will be
    # modified for items with long names.
    database = deepcopy(database)

    database = make_names_unique(database)
    bu = _dump_nodes(database)
    val_table = _dump_value_tables(database)
    bo = _dump_messages(database)
    bo_tx_bu = _dump_senders(database)
    cm = _dump_comments(database)
    signal_types = _dump_signal_types(database)
    ba_def = _dump_attribute_definitions(database)
    ba_def_def = _dump_attribute_definition_defaults(database)
    ba = _dump_attributes(database)
    val = _dump_choices(database)
    sig_group = _dump_signal_groups(database)
    sig_mux_values = _dump_signal_mux_values(database)

    return DBC_FMT.format(version=_dump_version(database),
                          bu=' '.join(bu),
                          val_table='\r\n'.join(val_table),
                          bo='\r\n\r\n'.join(bo),
                          bo_tx_bu='\r\n'.join(bo_tx_bu),
                          cm='\r\n'.join(cm),
                          signal_types='\r\n'.join(signal_types),
                          ba_def='\r\n'.join(ba_def),
                          ba_def_def='\r\n'.join(ba_def_def),
                          ba='\r\n'.join(ba),
                          val='\r\n'.join(val),
                          sig_group='\r\n'.join(sig_group),
                          sig_mux_values='\r\n'.join(sig_mux_values))


def get_definitions_dict(definitions, defaults):
    result = odict()

    def convert_value(definition, value):
        if definition.type_name in ['INT', 'HEX']:
            value = to_int(value)
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

    tokens = Parser().parse(string)

    comments = _load_comments(tokens)
    definitions = _load_attribute_definitions(tokens)
    defaults = _load_attribute_definition_defaults(tokens)
    attribute_definitions = get_definitions_dict(definitions, defaults)
    attributes = _load_attributes(tokens, attribute_definitions)
    bus = _load_bus(attributes, comments)
    value_tables = _load_value_tables(tokens)
    choices = _load_choices(tokens)
    message_senders = _load_message_senders(tokens, attributes)
    signal_types = _load_signal_types(tokens)
    signal_multiplexer_values = _load_signal_multiplexer_values(tokens)
    signal_groups = _load_signal_groups(tokens)
    messages = _load_messages(tokens,
                              comments,
                              attributes,
                              attribute_definitions,
                              choices,
                              message_senders,
                              signal_types,
                              signal_multiplexer_values,
                              strict,
                              bus.name if bus else None,
                              signal_groups)
    nodes = _load_nodes(tokens, comments, attributes, attribute_definitions)
    version = _load_version(tokens)
    environment_variables = _load_environment_variables(tokens, comments, attributes)
    dbc_specifics = DbcSpecifics(attributes.get('database', None),
                                 attribute_definitions,
                                 environment_variables,
                                 value_tables)

    return InternalDatabase(messages,
                            nodes,
                            [bus] if bus else [],
                            version,
                            dbc_specifics)
