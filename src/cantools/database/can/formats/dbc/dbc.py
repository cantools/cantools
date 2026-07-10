# Load and dump a CAN database in DBC format.

import re
import typing
from collections import OrderedDict, defaultdict
from copy import deepcopy
from dataclasses import dataclass, field
from decimal import Decimal
from typing import TypeVar

from textparser import (
    Any,
    AnyUntil,
    DelimitedList,
    Grammar,
    MatchObject,
    OneOrMore,
    OneOrMoreDict,
    Optional,
    Parser,
    Sequence,
    Token,
    TokenizeError,
    ZeroOrMore,
    choice,
    tokenize_init,
)

from cantools.database.can.internal_database import InternalDatabase
from cantools.typechecking import Choices

from ....conversion import BaseConversion
from ....errors import ParseError
from ....namedsignalvalue import NamedSignalValue
from ....utils import (
    SORT_SIGNALS_DEFAULT,
    sort_signals_by_start_bit,
    sort_signals_by_start_bit_reversed,
    type_sort_attribute,
    type_sort_attributes,
    type_sort_choices,
    type_sort_signals,
)
from ...bus import Bus
from ...message import Message
from ...node import Node
from ...signal import Signal
from ...signal_group import SignalGroup
from ..utils import num
from .attribute import Attribute, AttributeType
from .attribute_definition import (
    AttributeDefinition,
    AttributeDefinitionType,
)
from .dbc_specifics import DbcSpecifics
from .environment_variable import EnvironmentVariable

# Type alias for the parsed token dict produced by DbcParser.parse()
DbcTokens = dict[str, list[MatchObject]]

# A single parsed token item: textparser always produces list-shaped
# nodes for Sequence/ZeroOrMore/OneOrMoreDict rules, so we cast to
# this at loop entry
TokenList = list[MatchObject]

# Attribute value types produced during parsing (before they become
# Attribute objects)
DbcAttributeValue = str | int | float

# attribute_name -> attribute_object
DbcAttributeMap = OrderedDict[str, AttributeType]

# The internalized attributes for the whole dataset returned by
# _load_attributes()
@dataclass(slots=True, kw_only=True)
class DbcAttributes:
    # global database attributes:
    #     database[attribute_name] -> attribute_object
    database: DbcAttributeMap = field(default_factory=OrderedDict)

    # message-specific attributes:
    #     messages[dbc_frame_id][attribute_name] -> attribute_object
    messages: OrderedDict[int, DbcAttributeMap] = field(default_factory=OrderedDict)

    # signal-specific attributes:
    #     signals[dbc_frame_id][signal_name][attribute_name] -> attribute_object
    signals: OrderedDict[int, OrderedDict[str, DbcAttributeMap]] = field(default_factory=OrderedDict)

    # node-specific attributes:
    #     nodes[node_name][attribute_name] -> attribute_object
    nodes: OrderedDict[str, DbcAttributeMap] = field(default_factory=OrderedDict)

    # environment-variable-specific attributes:
    #     envvars[envvar_name][attribute_name] -> attribute_object
    envvars: OrderedDict[str, DbcAttributeMap] = field(default_factory=OrderedDict)

# The type returned by _load_relation_attributes()
@dataclass(slots=True, kw_only=True)
class DbcRelationAttributes:
    # BU_SG_REL_ attributes:
    #     node_signal_relations[frame_id][signal_name][node_name][attribute_name] -> attribute_object
    node_signal_relations: OrderedDict[int, OrderedDict[str, OrderedDict[str, DbcAttributeMap]]] = field(default_factory=OrderedDict)

    # BU_BO_REL_ attributes:
    #     node_message_relations[frame_id][node_name][attribute_name] -> attribute_object
    node_message_relations: OrderedDict[int, OrderedDict[str, DbcAttributeMap]] = field(default_factory=OrderedDict)

# The type returned by _load_comments()
@dataclass(slots=True, kw_only=True)
class DbcComments:
    # comment for the database as a whole
    database: str|None = None

    # comment for the CAN bus described by the database
    bus: str|None = None

    # comments for all nodes on the CAN bus:
    #     nodes[node_name] -> node_comment
    nodes: dict[str, str] = field(default_factory=dict)

    # comments for environment variables mentioned in the database:
    #     envvars[envvar_name] -> envvar_comment
    envvars: dict[str, str] = field(default_factory=dict)

    # comments for messages featured by the database:
    #     messages[message_arbitration_id] -> message_comment
    messages: dict[int, str] = field(default_factory=dict)

    # comments for all signals featured by the database:
    #     signals[message_arbitration_id][signal_name] -> signal_comment
    signals: dict[int, dict[str, str]] = field(default_factory=dict)

# Signal multiplexer values:
#   mux_values_dict[frame_id][mux_signal_name][signal_name] -> list[mux_id]
MuxSignalValues = dict[str, dict[str, list[int]]]
MuxValues = dict[int, MuxSignalValues]

# Attribute definition defaults from BA_DEF_DEF_ tokens keyed on the
# attribute name
DbcAttributeDefaults = OrderedDict[str, DbcAttributeValue]

# Set of value tables: name -> { int_value: NamedSignalValue }
ValueTables = OrderedDict[str, Choices]

# Choices dict: choices[frame_id][signal_name] -> {choice_int_value: choice_string_value}
ChoicesDict = defaultdict[int, dict[str, Choices]]



_T = TypeVar('_T')
def dbc_assert_type(x: object, t: type[_T]) -> _T:
    """Ensure that object `x` is of type `t`

    Return `x` if this is the case, or raise a `ParseError` if it is not.
    """
    if not isinstance(x, t):
        raise ParseError(
            f'Expected {t.__name__}, got {type(x).__name__}: {x!r}')
    return x


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
    '{ev}\r\n'
    '\r\n'
    '{cm}\r\n'
    '{ba_def}\r\n'
    '{ba_def_rel}'
    '{ba_def_def}\r\n'
    '{ba_def_def_rel}'
    '{ba}\r\n'
    '{ba_rel}'
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

ATTRIBUTE_DEFINITION_LONG_ENVVAR_NAME = AttributeDefinition(
    'SystemEnvVarLongSymbol',
    default_value='',
    kind='EV_',
    type_name='STRING')
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

ATTRIBUTE_DEFINITION_BAUDRATE = AttributeDefinition(
    name='Baudrate',
    default_value=125_000,
    type_name='INT',
    minimum=0,
    maximum=10*1024*1024)

ATTRIBUTE_DEFINITION_VFRAMEFORMAT = AttributeDefinition(
    name='VFrameFormat',
    default_value='StandardCAN',
    kind='BO_',
    type_name='ENUM',
    choices=['StandardCAN', 'ExtendedCAN',
             'reserved', 'J1939PG',
             'reserved', 'reserved',
             'reserved', 'reserved',
             'reserved', 'reserved',
             'reserved', 'reserved',
             'reserved', 'reserved',
             'StandardCAN_FD', 'ExtendedCAN_FD'])

ATTRIBUTE_DEFINITION_CANFD_BRS = AttributeDefinition(
    name='CANFD_BRS',
    default_value='1',
    kind='BO_',
    type_name='ENUM',
    choices=['0', '1'])

ATTRIBUTE_DEFINITION_BUS_TYPE = AttributeDefinition(
    name='BusType',
    default_value='CAN',
    type_name='STRING')

ATTRIBUTE_DEFINITION_GENMSGCYCLETIME = AttributeDefinition(
    name='GenMsgCycleTime',
    default_value=0,
    kind='BO_',
    type_name='INT',
    minimum=0,
    maximum=2**16-1)

ATTRIBUTE_DEFINITION_GENSIGSTARTVALUE = AttributeDefinition(
    name='GenSigStartValue',
    default_value=0.0,
    kind='SG_',
    type_name='FLOAT',
    minimum=0,
    maximum=100000000000)


def to_int(value: DbcAttributeValue) -> int:
    if isinstance(value, str):
        return int(Decimal(value))
    return int(value)

def to_float(value: DbcAttributeValue) -> float:
    if isinstance(value, str):
        return float(Decimal(value))
    return float(value)

class DbcParser(Parser):

    def tokenize(self, string: str) -> list[Token]:
        keywords = {
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
        }

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
            kind: str = mo.lastgroup  # type: ignore[assignment]

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

    def grammar(self) -> Grammar:
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

        relation_attribute_definition = Sequence(
            'BA_DEF_REL_',
            Optional(choice('BU_SG_REL_', 'BU_BO_REL_')),
            'STRING',
            'WORD',
            Optional(choice(DelimitedList('STRING'), OneOrMore('NUMBER'))),
            ';')

        relation_attribute_default_definition = Sequence(
            'BA_DEF_DEF_REL_', 'STRING', choice('NUMBER', 'STRING'), ';')

        signal_relation_attribute = Sequence(
            'BA_REL_', 'STRING', 'BU_SG_REL_', 'WORD', 'SG_', 'NUMBER',
            'WORD', choice('NUMBER', 'STRING'), ';')

        message_relation_attribute = Sequence(
            'BA_REL_', 'STRING', 'BU_BO_REL_', 'WORD', 'NUMBER',
            choice('NUMBER', 'STRING'), ';')

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

        return Grammar(OneOrMoreDict(choice(message,
                                            comment,
                                            attribute_definition,
                                            value_table,
                                            choice_,
                                            attribute,
                                            signal_relation_attribute,
                                            message_relation_attribute,
                                            relation_attribute_definition,
                                            attribute_definition_default,
                                            relation_attribute_default_definition,
                                            signal_group,
                                            signal_type,
                                            signal_multiplexer_values,
                                            message_add_sender,
                                            environment_variable,
                                            nodes,
                                            ns,
                                            bs,
                                            version)))


class LongNamesConverter:
    def __init__(self, long_names: list[str]) -> None:

        self.long_to_short: dict[str, str] = {}
        self.short_to_long: dict[str, str] = {}

        for long_name in sorted(long_names, key=lambda s: (len(s), s)):
            short_name = long_name[:32]
            index = -1
            while short_name in self.short_to_long:
                index += 1
                short_name = f'{short_name[:27]}_{index:04d}'

            self.long_to_short[long_name] = short_name
            self.short_to_long[short_name] = long_name


def get_dbc_frame_id(message: Message) -> int:
    frame_id = message.frame_id

    if message.is_extended_frame:
        frame_id |= 0x80000000

    return frame_id

def get_dbc_name(name: str) -> str:
    # replace special chars with '_'
    name = re.sub(r'\W', '_', name)
    # prepend '_' if it starts with a number
    if name[0].isdigit():
        name = '_' + name

    return name


def _get_node_long_name(attributes: DbcAttributes, node_short_name: str) -> str:
    attrib = attributes.nodes.get(node_short_name, OrderedDict()).get('SystemNodeLongSymbol')
    if attrib is not None:
        return str(attrib.value)
    return node_short_name

def _get_envvar_long_name(attributes: DbcAttributes, envvar_short_name: str) -> str:
    attrib = attributes.envvars.get(envvar_short_name, OrderedDict()).get('SystemEnvVarLongSymbol')
    if attrib is not None:
        return str(attrib.value)
    return envvar_short_name

def _dump_version(database: InternalDatabase) -> str:
    return '' if database.version is None else database.version


def _dump_nodes(database: InternalDatabase) -> list[str]:
    bu: list[str] = []

    for node in database.nodes:
        bu.append(node.name)

    return bu


def _dump_value_tables(database: InternalDatabase) -> list[str]:
    val_table: list[str] = []

    if database.dbc is not None:
        for name, choices in database.dbc.value_tables.items():
            choices_list = [
                f'{number} "{text}"'
                for number, text in sorted(choices.items(), reverse=True)
            ]
            val_table.append('VAL_TABLE_ {} {} ;'.format(name, ' '.join(choices_list)))

    return [*val_table, '']


def _dump_messages(database: InternalDatabase, sort_signals: type_sort_signals) -> list[str]:
    bo: list[str] = []

    def format_mux(signal: Signal) -> str:
        if signal.is_multiplexer:
            return ' M'
        elif signal.multiplexer_ids is not None:
            return f' m{signal.multiplexer_ids[0]}'
        else:
            return ''

    def format_receivers(signal: Signal) -> str:
        if signal.receivers:
            return ' ' + ','.join(signal.receivers)
        else:
            return 'Vector__XXX'

    def format_senders(message: Message) -> str:
        if message.senders:
            return message.senders[0]
        else:
            return 'Vector__XXX'

    for message in database.messages:
        msg: list[str] = []
        msg.append(
            f'BO_ {get_dbc_frame_id(message)} {message.name}: {message.length} {format_senders(message)}')

        if callable(sort_signals):
            signals = sort_signals(message.signals)
        else:
            signals = message.signals
        for signal in signals:
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


def _dump_senders(database: InternalDatabase) -> list[str]:
    bo_tx_bu: list[str] = []

    for message in database.messages:
        if len(message.senders) > 1:
            bo_tx_bu.append(
                'BO_TX_BU_ {frame_id} : {senders};'.format(
                    frame_id=get_dbc_frame_id(message),
                    senders=','.join(message.senders)))

    return bo_tx_bu


def _dump_comments(database: InternalDatabase, sort_signals: type_sort_signals) -> list[str]:
    cm: list[str] = []

    for bus in database.buses:
        if bus.comment is not None:
            cm.append(f'CM_ "{bus.comment}";')

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

        if callable(sort_signals):
            signals = sort_signals(message.signals)
        else:
            signals = message.signals
        for signal in signals:
            if signal.comment is not None:
                cm.append(
                    'CM_ SG_ {frame_id} {name} "{comment}";'.format(
                        frame_id=get_dbc_frame_id(message),
                        name=signal.name,
                        comment=signal.comment.replace('"', '\\"')))

    if database.dbc is not None:
        # Dump environment variable comments (CM_ EV_ <name> "comment";)
        for env in database.dbc.environment_variables.values():
            if env.comment is not None:
                escaped_comment = env.comment.replace('"', '\\"')
                cm.append(f'CM_ EV_ {env.name} "{escaped_comment}";')

    return cm


def _dump_signal_types(database: InternalDatabase) -> list[str]:
    valtype: list[str] = []

    for message in database.messages:
        for signal in message.signals:
            if not signal.is_float:
                continue

            valtype.append(
                f'SIG_VALTYPE_ {get_dbc_frame_id(message)} {signal.name} : {FLOAT_LENGTH_TO_SIGNAL_TYPE[signal.length]};')

    return valtype


def _need_startval_def(database: InternalDatabase) -> bool:
    return any(s.raw_initial is not None
               for m in database.messages
               for s in m.signals)

def _need_cycletime_def(database: InternalDatabase) -> bool:
    # If the user has added cycle times to a database which didn't start with them,
    # we need to add the global attribute definition so the output DBC is valid
    return any(m.cycle_time is not None
               for m in database.messages)

def _bus_is_canfd(database: InternalDatabase) -> bool:
    if database.dbc is None:
        return False

    bus_type = database.dbc.attributes.get('BusType', None)
    if bus_type is None:
        return False
    return bus_type.value == 'CAN FD'

def _dump_attribute_definitions(database: InternalDatabase) -> list[str]:
    ba_def: list[str] = []

    if database.dbc is None:
        return  ba_def

    definitions = database.dbc.attribute_definitions

    # define "GenMsgCycleTime" attribute for specifying the cycle
    # times of messages if it has not been explicitly defined
    if 'GenMsgCycleTime' not in definitions and _need_cycletime_def(database):
        definitions['GenMsgCycleTime'] = ATTRIBUTE_DEFINITION_GENMSGCYCLETIME
    if 'GenSigStartValue' not in definitions and _need_startval_def(database):
        definitions['GenSigStartValue'] = ATTRIBUTE_DEFINITION_GENSIGSTARTVALUE

    # create 'Baudrate' attribute definition
    if len(database.buses) == 1 and database.buses[0].baudrate is not None:
        # add a definition of the `Baudrate` attribute if it is needed
        # and not yet present
        if 'Baudrate' not in definitions:
            definitions['Baudrate'] = ATTRIBUTE_DEFINITION_BAUDRATE
    # remove the definition of the `Baudrate` attribute if it is
    # not needed
    elif 'Baudrate' in definitions:
        del definitions['Baudrate']

    # create 'VFrameFormat' and 'CANFD_BRS' attribute definitions if bus is CAN FD
    if _bus_is_canfd(database):
        if 'VFrameFormat' not in definitions:
            definitions['VFrameFormat'] = ATTRIBUTE_DEFINITION_VFRAMEFORMAT
        if 'CANFD_BRS' not in definitions:
            definitions['CANFD_BRS'] = ATTRIBUTE_DEFINITION_CANFD_BRS

    def get_kind(definition: AttributeDefinitionType) -> str:
        return '' if definition.kind is None else definition.kind + ' '

    for definition in definitions.values():
        if definition.type_name == 'ENUM':
            choices = ','.join([f'"{choice}"'
                                for choice in definition.choices])
            ba_def.append(
                f'BA_DEF_ {get_kind(definition)} "{definition.name}" {definition.type_name}  {choices};')
        elif definition.type_name in ['INT', 'FLOAT', 'HEX']:
            ba_def.append(
                f'BA_DEF_ {get_kind(definition)} "{definition.name}" {definition.type_name}{definition.formatted_minimum}{definition.formatted_maximum};')
        elif definition.type_name == 'STRING':
            ba_def.append(
                f'BA_DEF_ {get_kind(definition)} "{definition.name}" {definition.type_name} ;')

    return ba_def


def _dump_relation_attribute_definitions(database: InternalDatabase) -> list[str]:
    relation_attribute_definitions: list[str] = []

    if database.dbc is None:
        return relation_attribute_definitions

    definitions = database.dbc.relation_attribute_definitions

    for definition in definitions.values():
        if definition.type_name == 'ENUM':
            choices = ','.join([f'"{choice}"'
                                for choice in definition.choices])
            relation_attribute_definitions.append(
                f'BA_DEF_REL_ {definition.kind}  "{definition.name}" {definition.type_name}  {choices};')
        elif definition.type_name in ['INT', 'FLOAT', 'HEX']:
            relation_attribute_definitions.append(
                f'BA_DEF_REL_ {definition.kind}  "{definition.name}" {definition.type_name}{definition.formatted_minimum}{definition.formatted_maximum};')
        elif definition.type_name == 'STRING':
            relation_attribute_definitions.append(
                f'BA_DEF_REL_ {definition.kind}  "{definition.name}" {definition.type_name} ;')

    return relation_attribute_definitions


def _dump_attribute_definition_defaults(database: InternalDatabase) -> list[str]:
    ba_def_def: list[str] = []

    if database.dbc is None:
        return  ba_def_def

    definitions = database.dbc.attribute_definitions

    for definition in definitions.values():
        if definition.default_value is not None:
            if definition.type_name in ['STRING', 'ENUM']:
                fmt = 'BA_DEF_DEF_  "{name}" "{value}";'
            else:
                fmt = 'BA_DEF_DEF_  "{name}" {value};'

            ba_def_def.append(fmt.format(name=definition.name,
                                         value=definition.default_value))

    return ba_def_def


def _dump_relation_attribute_definition_defaults(database: InternalDatabase) -> list[str]:
    relation_attribute_definition_defaults: list[str] = []

    if database.dbc is None:
        return  relation_attribute_definition_defaults

    definitions = database.dbc.relation_attribute_definitions

    for definition in definitions.values():
        if definition.default_value is not None:
            if definition.type_name in ['STRING', 'ENUM']:
                fmt = 'BA_DEF_DEF_REL_ "{name}" "{value}";'
            else:
                fmt = 'BA_DEF_DEF_REL_ "{name}" {value};'

            relation_attribute_definition_defaults.append(
                fmt.format(name=definition.name,
                           value=definition.default_value))

    return relation_attribute_definition_defaults


def _dump_attributes(database: InternalDatabase, sort_signals: type_sort_signals, sort_attributes: type_sort_attributes) -> list[str]:
    ba: list[str] = []

    if database.dbc is None:
        return  ba

    attributes: list[type_sort_attribute] = []

    # remove the old 'Baudrate' attribute if it exists. We synchronize
    # that DBC attribute with the `.baudrate` attribute of the
    # high-level bus description object below.
    if 'Baudrate' in database.dbc.attributes:
        del database.dbc.attributes['Baudrate']

    # add a new 'Baudrate' attribute
    baudrate = None
    if len(database.buses) == 1:
        baudrate = database.buses[0].baudrate
    if baudrate is not None:
        baudrate_attribute_definition = database.dbc.attribute_definitions.get('Baudrate')
        if baudrate_attribute_definition is None:
            raise ParseError('Database defines a baudrate but no definition for '
                             'the corresponding attribute')

    for attribute in database.dbc.attributes.values():
        attributes.append(('dbc', attribute, None, None, None, None))

    for envvar in database.dbc.environment_variables.values():
        if envvar.dbc is not None:
            for attribute in envvar.dbc.attributes.values():
                attributes.append(('envvar', attribute, None, None, None, envvar))

    for node in database.nodes:
        if node.dbc is not None:
            for attribute in node.dbc.attributes.values():
                attributes.append(('node', attribute, node, None, None, None))

    for message in database.messages:
        msg_attributes = OrderedDict[str, AttributeType]()
        if message.dbc is not None:
            msg_attributes = deepcopy(message.dbc.attributes)

        # synchronize the attribute for the message cycle time with
        # the cycle time specified by the message object
        msg_cycle_time = message.cycle_time or 0

        gen_msg_cycle_time_def = database.dbc.attribute_definitions.get('GenMsgCycleTime')

        if gen_msg_cycle_time_def is not None and msg_cycle_time != gen_msg_cycle_time_def.default_value:
            msg_attributes['GenMsgCycleTime'] = Attribute(
                value=msg_cycle_time,
                definition=dbc_assert_type(gen_msg_cycle_time_def, AttributeDefinition),
            )
        elif 'GenMsgCycleTime' in msg_attributes:
            del msg_attributes['GenMsgCycleTime']

        # if bus is CAN FD, set VFrameFormat
        v_frame_format_def = None
        if database.dbc is not None:
            v_frame_format_def = database.dbc.attribute_definitions.get('VFrameFormat')
        if v_frame_format_def is not None:
            if message.protocol == 'j1939':
                v_frame_format_str = 'J1939PG'
            elif message.is_fd and message.is_extended_frame:
                v_frame_format_str = 'ExtendedCAN_FD'
            elif message.is_fd:
                v_frame_format_str = 'StandardCAN_FD'
            elif message.is_extended_frame:
                v_frame_format_str = 'ExtendedCAN'
            else:
                v_frame_format_str = 'StandardCAN'
            v_frame_format_def = _get_enum_vframeformat_definition(v_frame_format_def)
            # only set the VFrameFormat if it is valid according to the attribute definition
            if (
                v_frame_format_str in v_frame_format_def.choices
                and v_frame_format_str != v_frame_format_def.default_value
            ):
                msg_attributes['VFrameFormat'] = Attribute(
                    value=str(v_frame_format_def.choices.index(v_frame_format_str)),
                    definition=v_frame_format_def,
                )


        # output all message attributes
        for attribute in msg_attributes.values():
            attributes.append(('message', attribute, None, message, None, None))

        # handle the signals contained in the message
        if callable(sort_signals):
            signals = sort_signals(message.signals)
        else:
            signals = message.signals
        for signal in signals:
            # retrieve the ordered dictionary of signal attributes
            sig_attributes = signal.dbc.attributes

            # synchronize the attribute for the signal start value with
            # the start value specified by the message object
            if signal.raw_initial is None and 'GenSigStartValue' in sig_attributes:
                del sig_attributes['GenSigStartValue']
            elif signal.raw_initial is not None:
                sig_attributes['GenSigStartValue'] = Attribute(
                    value=signal.raw_initial,
                    definition=ATTRIBUTE_DEFINITION_GENSIGSTARTVALUE)

            # output all signal attributes
            for attribute in sig_attributes.values():
                attributes.append(('signal', attribute, None, message, signal, None))

    if callable(sort_attributes):
        attributes = sort_attributes(attributes)

    for attribute_type, attribute_to_dump, node_to_dump, message_to_dump, signal_to_dump, envvar_to_dump in attributes:
        if attribute_type == 'dbc':
            ba.append(f'BA_ "{attribute_to_dump.definition.name}" '
                      f'{attribute_to_dump.formatted_value};')
        elif attribute_type == 'envvar':
            if envvar_to_dump is None:
                raise ParseError(
                    f'Need to dump environment variable, but none has been specified')

            ba.append(f'BA_ "{attribute_to_dump.definition.name}" '
                      f'{attribute_to_dump.definition.kind} '
                      f'{envvar_to_dump.name} '
                      f'{attribute_to_dump.formatted_value};')
        elif attribute_type == 'node':
            if node_to_dump is None:
                raise ParseError(
                    'Need to dump node, but none has been specified')

            ba.append(f'BA_ "{attribute_to_dump.definition.name}" '
                      f'{attribute_to_dump.definition.kind} '
                      f'{node_to_dump.name} '
                      f'{attribute_to_dump.formatted_value};')
        elif attribute_type == 'message':
            if message_to_dump is None:
                raise ParseError(
                    'Need to dump message, but none has been specified')

            ba.append(f'BA_ "{attribute_to_dump.definition.name}" '
                      f'{attribute_to_dump.definition.kind} '
                      f'{get_dbc_frame_id(message_to_dump)} '
                      f'{attribute_to_dump.formatted_value};')
        elif attribute_type == 'signal':
            if signal_to_dump is None:
                raise ParseError(
                    'Need to dump signal, but none has been specified')
            if message_to_dump is None:
                raise ParseError(
                    'Need to dump message, but none has been specified')

            ba.append(f'BA_ "{attribute_to_dump.definition.name}" '
                      f'{attribute_to_dump.definition.kind} '
                      f'{get_dbc_frame_id(message_to_dump)} '
                      f'{signal_to_dump.name} '
                      f'{attribute_to_dump.formatted_value};')

    return ba


def _dump_relation_attributes(database: InternalDatabase, sort_signals: type_sort_signals) -> str | list[str]:
    relation_attribute_strings: list[str] = []

    if database.dbc is None or database.dbc.relation_attributes is None:
        return relation_attribute_strings

    relation_attributes = database.dbc.relation_attributes
    for frame_id, signal_map in relation_attributes.node_signal_relations.items():
        for signal_name, node_map in signal_map.items():
            for node_name, attribute_map in node_map.items():
                for attribute in attribute_map.values():
                    relation_attribute_strings.append(f'BA_REL_ "{attribute.definition.name}" '
                                                      f'BU_SG_REL_ '
                                                      f'{node_name} '
                                                      f'SG_ '
                                                      f'{frame_id} '
                                                      f'{signal_name} '
                                                      f'{attribute.formatted_value};')
    for frame_id, node_map in relation_attributes.node_message_relations.items():
        for node_name, attribute_map in node_map.items():
            for attribute in attribute_map.values():
                relation_attribute_strings.append(f'BA_REL_ "{attribute.definition.name}" '
                                                  f'BU_BO_REL_ '
                                                  f'{node_name} '
                                                  f'{frame_id} '
                                                  f'{attribute.formatted_value};')

    return relation_attribute_strings


def _dump_choices(database: InternalDatabase, sort_signals: type_sort_signals, sort_choices: type_sort_choices) -> list[str]:
    val: list[str] = []

    for message in database.messages:
        if callable(sort_signals):
            signals = sort_signals(message.signals)
        else:
            signals = message.signals
        for signal in signals:
            if signal.choices is None:
                continue

            if sort_choices:
                choices = sort_choices(signal.choices)
            else:
                choices = signal.choices

            val.append(
                'VAL_ {frame_id} {name} {choices} ;'.format(
                    frame_id=get_dbc_frame_id(message),
                    name=signal.name,
                    choices=' '.join([f'{value} "{text}"' for value, text in choices.items()])))

    return val


def _dump_signal_groups(database: InternalDatabase) -> list[str]:
    sig_group: list[str] = []

    for message in database.messages:
        if message.signal_groups is None:
            continue

        for signal_group in message.signal_groups:
            all_sig_names = [sig.name for sig in message.signals]
            signal_group.signal_names = list(filter(lambda sig_name: sig_name in all_sig_names, signal_group.signal_names))
            sig_group.append(
                'SIG_GROUP_ {frame_id} {signal_group_name} {repetitions} : {signal_names};'.format(
                    frame_id=get_dbc_frame_id(message),
                    signal_group_name=signal_group.name,
                    repetitions=signal_group.repetitions,
                    signal_names=' '.join(signal_group.signal_names)
                ))

    return sig_group


def _is_extended_mux_needed(messages: list[Message]) -> bool:
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


def _create_mux_ranges(multiplexer_ids: list[int]) -> list[list[int]]:
    """Create a list of ranges based on a list of single values.

    Example:
        Input:  [1, 2, 3, 5,      7, 8, 9]
        Output: [[1, 3], [5, 5], [7, 9]]

    """

    ordered = sorted(multiplexer_ids)
    # Anything but ordered[0] - 1
    prev_value = ordered[0]
    ranges: list[list[int]] = []

    for value in ordered:
        if value == prev_value + 1:
            ranges[-1][1] = value
        else:
            ranges.append([value, value])

        prev_value = value

    return ranges


def _dump_signal_mux_values(database: InternalDatabase) -> list[str]:
    """Create multiplex entries ("SG_MUL_VAL_") if extended multiplexing
    is used.

    """

    if not _is_extended_mux_needed(database.messages):
        return []

    sig_mux_values: list[str] = []

    for message in database.messages:
        for signal in message.signals:
            if not signal.multiplexer_ids:
                continue

            ranges = ', '.join([
                f'{minimum}-{maximum}'
                for minimum, maximum in _create_mux_ranges(signal.multiplexer_ids)
            ])

            sig_mux_values.append(
                f'SG_MUL_VAL_ {get_dbc_frame_id(message)} {signal.name} {signal.multiplexer_signal} {ranges};')

    return sig_mux_values


def _dump_environment_variables(database: InternalDatabase) -> list[str]:
    """Dump environment variables (EV_ entries)."""
    ev_lines: list[str] = []

    if database.dbc is None:
        return  ev_lines

    for env in database.dbc.environment_variables.values():
        # escape unit quotes
        unit = env.unit.replace('"', '\\"')
        ev_lines.append(
            f'EV_ {env.name}: {env.env_type} [{env.minimum}|{env.maximum}] "{unit}" {env.initial_value} {env.env_id} {env.access_type} {env.access_node};'
        )

    return ev_lines


def _load_comments(tokens: DbcTokens) -> DbcComments:
    comments = DbcComments()

    for _comment_tokens in tokens.get('CM_', []):
        comment_tokens = dbc_assert_type(_comment_tokens, list)
        if not isinstance(comment_tokens[1], list):
            # the CM_ token is a string, i.e., it is a bus comment. We
            # implement CANdb++ behavior and concatenate all bus
            # comments.
            if comments.bus is None:
                comments.bus = ''
            comments.bus += dbc_assert_type(comment_tokens[1], str)
            continue

        item: TokenList = comment_tokens[1]
        kind = dbc_assert_type(item[0], str)

        if kind == 'SG_':
            # signal comment
            frame_id = int(dbc_assert_type(item[1], str))
            signal_name = dbc_assert_type(item[2], str)
            if frame_id not in comments.signals:
                comments.signals[frame_id] = {}
            message_signal_comments = comments.signals[frame_id]
            message_signal_comments[signal_name] = dbc_assert_type(item[3], str)
        elif kind == 'BO_':
            # message comment
            frame_id = int(dbc_assert_type(item[1], str))
            comments.messages[frame_id] = dbc_assert_type(item[2], str)
        elif kind == 'BU_':
            # node comment
            node_name = dbc_assert_type(item[1], str)
            comments.nodes[node_name] = dbc_assert_type(item[2], str)
        elif kind == 'EV_':
            # environment variable comment
            envvar_name = dbc_assert_type(item[1], str)
            comments.envvars[envvar_name] = dbc_assert_type(item[2], str)

    return comments


def _load_attribute_definitions(tokens: DbcTokens) -> list[MatchObject]:
    return tokens.get('BA_DEF_', [])


def _load_attribute_definition_defaults(tokens: DbcTokens) -> DbcAttributeDefaults:
    defaults: DbcAttributeDefaults = OrderedDict()

    for _default_attr in tokens.get('BA_DEF_DEF_', []):
        default_attr = dbc_assert_type(_default_attr, list)
        defaults[dbc_assert_type(default_attr[1], str)] = typing.cast('DbcAttributeValue', default_attr[2])

    return defaults


def _load_relation_attribute_definitions(tokens: DbcTokens) -> list[MatchObject]:
    return tokens.get('BA_DEF_REL_', [])


def _load_relation_attribute_definition_defaults(tokens: DbcTokens) -> DbcAttributeDefaults:
    defaults: DbcAttributeDefaults = OrderedDict()

    for _default_attr in tokens.get('BA_DEF_DEF_REL_', []):
        default_attr = dbc_assert_type(_default_attr, list)
        defaults[dbc_assert_type(default_attr[1], str)] = typing.cast('DbcAttributeValue', default_attr[2])

    return defaults


def _load_attributes(tokens: DbcTokens, definitions: OrderedDict[str, AttributeDefinitionType]) -> DbcAttributes:
    attributes = DbcAttributes()

    def to_attribute_object(attribute_tokens: TokenList) -> AttributeType:
        raw_value = dbc_assert_type(attribute_tokens[3], str)
        definition = definitions[dbc_assert_type(attribute_tokens[1], str)]

        if definition.type_name in ['INT', 'HEX', 'ENUM']:
            return Attribute[int](value=to_int(raw_value),
                                  definition=typing.cast('AttributeDefinition[int]', definition))
        elif definition.type_name == 'FLOAT':
            return Attribute[float](value=to_float(raw_value),
                                    definition=typing.cast('AttributeDefinition[float]', definition))

        return Attribute[str](value=raw_value,
                              definition=typing.cast('AttributeDefinition[str]', definition))

    for _attribute_tokens in tokens.get('BA_', []):
        attribute_tokens = dbc_assert_type(_attribute_tokens, list)
        attribute_name = dbc_assert_type(attribute_tokens[1], str)

        if not isinstance(attribute_tokens[2], list):
            raise ParseError('Second token of attribute must be a list, '
                             f'got {type(attribute_tokens[2]).__name__}')
        if len(attribute_tokens[2]) > 0:
            attribute_items = dbc_assert_type(dbc_assert_type(attribute_tokens[2], list)[0], list)
            attribute_kind = dbc_assert_type(attribute_items[0], str)

            if attribute_kind == 'SG_':
                frame_id_dbc = int(dbc_assert_type(attribute_items[1], str))

                if frame_id_dbc not in attributes.signals:
                    attributes.signals[frame_id_dbc] = OrderedDict()
                message_signal_attrs = attributes.signals[frame_id_dbc]

                signal_name = dbc_assert_type(attribute_items[2], str)
                if signal_name not in message_signal_attrs:
                    message_signal_attrs[signal_name] = OrderedDict()
                signal_attrs = message_signal_attrs[signal_name]

                signal_attrs[attribute_name] = to_attribute_object(attribute_tokens)
            elif attribute_kind == 'BO_':
                frame_id_dbc = int(dbc_assert_type(attribute_items[1], str))

                if frame_id_dbc not in attributes.messages:
                    attributes.messages[frame_id_dbc] = OrderedDict()
                message_attrs = attributes.messages[frame_id_dbc]

                message_attrs[attribute_name] = to_attribute_object(attribute_tokens)
            elif attribute_kind == 'BU_':
                node_name = dbc_assert_type(attribute_items[1], str)

                if node_name not in attributes.nodes:
                    attributes.nodes[node_name] = OrderedDict()
                attributes.nodes[node_name][attribute_name] = to_attribute_object(attribute_tokens)
            elif attribute_kind == 'EV_':
                envvar_name = dbc_assert_type(attribute_items[1], str)

                if envvar_name not in attributes.envvars:
                    attributes.envvars[envvar_name] = OrderedDict()
                envvar_attribs = attributes.envvars[envvar_name]

                envvar_attribs[attribute_name] = to_attribute_object(attribute_tokens)
        else:
            db_attrs = attributes.database
            db_attrs[attribute_name] = to_attribute_object(attribute_tokens)

    return attributes


def _load_relation_attributes(tokens: DbcTokens, definitions: OrderedDict[str, AttributeDefinitionType]) -> DbcRelationAttributes:
    relation_attributes = DbcRelationAttributes()

    def to_relation_attribute_object(attribute_tokens: TokenList, value: DbcAttributeValue) -> AttributeType:
        definition = definitions[dbc_assert_type(attribute_tokens[1], str)]

        if definition.type_name in ['INT', 'HEX', 'ENUM']:
            return Attribute[int](value=to_int(value),
                                  definition=typing.cast('AttributeDefinition[int]', definition))
        elif definition.type_name == 'FLOAT':
            return Attribute[float](value=to_float(value),
                                    definition=typing.cast('AttributeDefinition[float]', definition))
        else:
            return Attribute[str](str(value),
                                  definition=typing.cast('AttributeDefinition[str]', definition))

    for _relation_attribute_tokens in tokens.get('BA_REL_', []):
        relation_attribute_tokens = dbc_assert_type(_relation_attribute_tokens, list)
        attribute_name = dbc_assert_type(relation_attribute_tokens[1], str)
        relation_type = dbc_assert_type(relation_attribute_tokens[2], str)
        node_name = dbc_assert_type(relation_attribute_tokens[3], str)

        if relation_type == 'BU_SG_REL_':
            frame_id_dbc = int(dbc_assert_type(relation_attribute_tokens[5], str))
            signal_name = dbc_assert_type(relation_attribute_tokens[6], str)

            if frame_id_dbc not in relation_attributes.node_signal_relations:
                relation_attributes.node_signal_relations[frame_id_dbc] = OrderedDict()
            if signal_name not in relation_attributes.node_signal_relations[frame_id_dbc]:
                relation_attributes.node_signal_relations[frame_id_dbc][signal_name] = OrderedDict()
            if node_name not in relation_attributes.node_signal_relations[frame_id_dbc][signal_name]:
                relation_attributes.node_signal_relations[frame_id_dbc][signal_name][node_name] = OrderedDict()

            relation_attributes.node_signal_relations[frame_id_dbc][signal_name][node_name][attribute_name] = \
                to_relation_attribute_object(relation_attribute_tokens, dbc_assert_type(relation_attribute_tokens[7], str))

        elif relation_type == 'BU_BO_REL_':
            frame_id_dbc = int(dbc_assert_type(relation_attribute_tokens[4], str))

            if frame_id_dbc not in relation_attributes.node_message_relations:
                relation_attributes.node_message_relations[frame_id_dbc] = OrderedDict()
            if node_name not in relation_attributes.node_message_relations[frame_id_dbc]:
                relation_attributes.node_message_relations[frame_id_dbc][node_name] = OrderedDict()

            relation_attributes.node_message_relations[frame_id_dbc][node_name][attribute_name] = \
                to_relation_attribute_object(relation_attribute_tokens, dbc_assert_type(relation_attribute_tokens[5], str))

    return relation_attributes


def _load_value_tables(tokens: DbcTokens) -> OrderedDict[str, Choices]:
    """Load value tables, that is, choice definitions.

    """

    value_tables: OrderedDict[str, Choices] = OrderedDict()

    for _value_table in tokens.get('VAL_TABLE_', []):
        value_table = dbc_assert_type(_value_table, list)
        name = dbc_assert_type(value_table[1], str)
        number_text_pairs = dbc_assert_type(value_table[2], list)
        value_tables[name] = OrderedDict((int(number), NamedSignalValue(int(number), text))
                                         for number, text in number_text_pairs)

    return value_tables


def _load_environment_variables(tokens: DbcTokens, comments: DbcComments, attributes: DbcAttributes, attribute_definitions: OrderedDict[str, AttributeDefinitionType]) -> OrderedDict[str, EnvironmentVariable]:
    environment_variables: OrderedDict[str, EnvironmentVariable] = OrderedDict()

    for _env_var in tokens.get('EV_', []):
        env_var = dbc_assert_type(_env_var, list)
        short_name = dbc_assert_type(env_var[1], str)
        long_name = _get_envvar_long_name(attributes, short_name)
        environment_variables[long_name] = EnvironmentVariable(
            name=long_name,
            env_type=int(dbc_assert_type(env_var[3], str)),
            minimum=num(dbc_assert_type(env_var[5], str)),
            maximum=num(dbc_assert_type(env_var[7], str)),
            unit=dbc_assert_type(env_var[9], str),
            initial_value=num(dbc_assert_type(env_var[10], str)),
            env_id=int(dbc_assert_type(env_var[11], str)),
            access_type=dbc_assert_type(env_var[12], str),
            access_node=dbc_assert_type(env_var[13], str),
            comment=comments.envvars.get(short_name),
            dbc_specifics=DbcSpecifics(attributes=attributes.envvars.get(short_name),
                                       attribute_definitions=attribute_definitions))

    return environment_variables

def _load_choices(tokens: DbcTokens) -> ChoicesDict:
    choices: ChoicesDict = defaultdict(dict)

    for _choice_token in tokens.get('VAL_', []):
        choice_token = dbc_assert_type(_choice_token, list)
        frame_id_list = dbc_assert_type(choice_token[1], list)
        if len(frame_id_list) == 0:
            continue

        pairs = dbc_assert_type(choice_token[3], list)
        od: Choices = OrderedDict((int(v[0]), NamedSignalValue(int(v[0]), v[1])) for v in pairs)

        if len(od) == 0:
            continue

        frame_id = int(frame_id_list[0])
        choices[frame_id][dbc_assert_type(choice_token[2], str)] = od

    return choices

def _load_message_senders(tokens: DbcTokens, attributes: DbcAttributes) -> defaultdict[int, list[str]]:
    """Load additional message senders.

    """

    message_senders: defaultdict[int, list[str]] = defaultdict(list)

    for _senders in tokens.get('BO_TX_BU_', []):
        senders = dbc_assert_type(_senders, list)
        frame_id = int(dbc_assert_type(senders[1], str))
        message_senders[frame_id] += [
            _get_node_long_name(attributes, sender) for sender in dbc_assert_type(senders[3], list)
        ]

    return message_senders


def _load_signal_types(tokens: DbcTokens) -> defaultdict[int, dict[str, int]]:
    """Load signal types.

    """

    signal_types: defaultdict[int, dict[str, int]] = defaultdict(dict)

    for _signal_type in tokens.get('SIG_VALTYPE_', []):
        signal_type = dbc_assert_type(_signal_type, list)
        frame_id = int(dbc_assert_type(signal_type[1], str))
        signal_name = dbc_assert_type(signal_type[2], str)
        signal_types[frame_id][signal_name] = int(dbc_assert_type(signal_type[4], str))

    return signal_types


def _load_signal_multiplexer_values(tokens: DbcTokens) -> MuxValues:
    """Load additional signal multiplexer values.

    """

    signal_multiplexer_values: MuxValues = defaultdict(dict)

    for _smv in tokens.get('SG_MUL_VAL_', []):
        smv = dbc_assert_type(_smv, list)
        frame_id = int(dbc_assert_type(smv[1], str))
        signal_name = dbc_assert_type(smv[2], str)
        mux_signal_name = dbc_assert_type(smv[3], str)
        multiplexer_ids: list[int] = []

        for lower_str, upper_str in dbc_assert_type(smv[4], list):
            lower = int(lower_str)
            upper = int(upper_str[1:])
            # TODO: Probably store ranges as tuples to not run out of
            #       memory on huge ranges.
            multiplexer_ids.extend(range(lower, upper + 1))

        mux_signal = signal_multiplexer_values[frame_id].setdefault(mux_signal_name, {})
        mux_signal[signal_name] = multiplexer_ids

    return signal_multiplexer_values


def _load_signal_groups(tokens: DbcTokens, attributes: DbcAttributes) -> defaultdict[int, list[SignalGroup]]:
    """Load signal groups.

    """

    signal_groups: defaultdict[int, list[SignalGroup]] = defaultdict(list)

    def get_signal_attributes(frame_id_dbc: int, signal_short_name: str) -> DbcAttributeMap:
        """Get attributes for given signal.

        """

        frame_signal_attribs: dict[str, DbcAttributeMap] = attributes.signals.get(frame_id_dbc, {})
        return frame_signal_attribs.get(signal_short_name, OrderedDict())

    def get_signal_long_name(frame_id_dbc: int, signal_short_name: str) -> str:
        signal_attributes = get_signal_attributes(frame_id_dbc, signal_short_name)
        if (attrib := signal_attributes.get('SystemSignalLongSymbol')) is not None:
            return str(attrib.value)

        return signal_short_name

    for _sg in tokens.get('SIG_GROUP_',[]):
        sg = dbc_assert_type(_sg, list)
        frame_id = int(dbc_assert_type(sg[1], str))
        signal_names = [get_signal_long_name(frame_id, sn) for sn in dbc_assert_type(sg[5], list)]
        signal_groups[frame_id].append(SignalGroup(name=dbc_assert_type(sg[2], str),
                                                   repetitions=int(dbc_assert_type(sg[3], str)),
                                                   signal_names=signal_names))

    return signal_groups


def _load_signals(tokens: list[MatchObject],
                  comments: DbcComments,
                  attributes: DbcAttributes,
                  definitions: OrderedDict[str, AttributeDefinitionType] | None,
                  choices: ChoicesDict,
                  signal_types: dict[int, dict[str, int]],
                  signal_multiplexer_values: MuxValues,
                  frame_id_dbc: int,
                  multiplexer_signal: str | None) -> list[Signal]:
    signal_to_multiplexer: dict[str, str] = {}

    frame_mux_values: MuxSignalValues = signal_multiplexer_values.get(frame_id_dbc, {})

    for multiplexer_name, items in frame_mux_values.items():
        for name in items:
            signal_to_multiplexer[name] = multiplexer_name

    def get_signal_attributes(frame_id_dbc: int, signal_name: str) -> DbcAttributeMap:
        """Get attributes for given signal.

        """
        if (msg_sig_attribs := attributes.signals.get(frame_id_dbc)) is not None and \
           (sig_attribs := msg_sig_attribs.get(signal_name)) is not None:
            return sig_attribs

        return OrderedDict()

    def get_signal_comment(frame_id_dbc: int, signal_name: str) -> str | None:
        """Get comment for given signal.

        """
        message_signal_comments = comments.signals.get(frame_id_dbc)
        if message_signal_comments is None:
            return None
        return message_signal_comments.get(signal_name)

    def get_signal_choices(frame_id_dbc: int, signal_name: str) -> Choices | None:
        """Get choices for given signal.

        """
        message_choices = choices.get(frame_id_dbc)
        if message_choices is None:
            return None

        return message_choices.get(signal_name)

    def get_signal_is_multiplexer(signal_tokens: TokenList) -> bool:
        mux_field = dbc_assert_type(signal_tokens[1], list)
        if len(mux_field) == 2:
            return dbc_assert_type(mux_field[1], str).endswith('M')
        else:
            return False

    def get_signal_multiplexer_ids(mux_field: list[str], multiplexer_signal: str | None) -> list[int] | None:
        ids: list[int] = []
        signal_name = mux_field[0]

        if multiplexer_signal is not None:
            if len(mux_field) == 2 and not mux_field[1].endswith('M'):
                value = mux_field[1][1:].rstrip('M')
                ids.append(int(value))
        else:
            multiplexer_signal = get_multiplexer_signal(mux_field, multiplexer_signal)

        if multiplexer_signal is not None:
            mux_entry = frame_mux_values.get(multiplexer_signal)
            if mux_entry is not None and signal_name in mux_entry:
                ids.extend(mux_entry[signal_name])

        if ids:
            # make the IDs unique (and sort them)
            return list(set(ids))

        return None

    def get_multiplexer_signal(mux_field: list[str], multiplexer_signal_name: str | None) -> str | None:
        if len(mux_field) != 2:
            return None

        signal_name = mux_field[0]
        if multiplexer_signal is None:
            return signal_to_multiplexer.get(signal_name)
        elif signal_name != multiplexer_signal_name:
            return multiplexer_signal_name

        return None

    def get_signal_receivers(receivers: list[str]) -> list[str]:
        if receivers == ['Vector__XXX']:
            receivers = []

        return [_get_node_long_name(attributes, receiver) for receiver in receivers]

    def get_signal_minimum(minimum: str, maximum: str) -> int | float | None:
        if minimum == maximum == '0':
            return None
        else:
            return num(minimum)

    def get_signal_maximum(minimum: str, maximum: str) -> int | float | None:
        if minimum == maximum == '0':
            return None
        else:
            return num(maximum)

    def get_signal_is_float(frame_id_dbc: int, signal: str) -> bool:
        """Get is_float for given signal.

        """

        frame_signal_types = signal_types.get(frame_id_dbc)
        if frame_signal_types is None:
            return False
        signal_type = frame_signal_types.get(signal)
        return signal_type in FLOAT_SIGNAL_TYPES

    def get_signal_long_name(frame_id_dbc: int, signal_short_name: str) -> str:
        signal_attributes = get_signal_attributes(frame_id_dbc, signal_short_name)
        if signal_attributes is None:
            return signal_short_name

        if (attrib := signal_attributes.get('SystemSignalLongSymbol')) is not None:
            return str(attrib.value)

        return signal_short_name

    def get_signal_initial_value(frame_id_dbc: int, signal_short_name: str) -> int | float | None:
        signal_attributes = get_signal_attributes(frame_id_dbc, signal_short_name)
        if signal_attributes is None:
            return None

        if (attrib := signal_attributes.get('GenSigStartValue')) is not None:
            if not isinstance(attrib.value, (int, float)):
                raise ParseError(
                    f'Expected int or float, got {type(attrib.value).__name__}: {attrib.value!r}')

            return attrib.value

        return None

    def get_signal_spn(frame_id_dbc: int, name: str) -> int | None:
        signal_attributes = get_signal_attributes(frame_id_dbc, name)
        if signal_attributes is None:
            return None
        elif (spn_attrib := signal_attributes.get('SPN')) is not None:
            return dbc_assert_type(spn_attrib.value, int)

        if definitions is not None and 'SPN' in definitions:
            return typing.cast('int | None', definitions['SPN'].default_value)

        return None

    signals = []

    for _signal_tok in tokens:
        signal_tok = dbc_assert_type(_signal_tok, list)
        mux_field = dbc_assert_type(signal_tok[1], list)
        signal_name = mux_field[0]
        signals.append(
            Signal(name=get_signal_long_name(frame_id_dbc, signal_name),
                   start=int(dbc_assert_type(signal_tok[3], str)),
                   length=int(dbc_assert_type(signal_tok[5], str)),
                   receivers=get_signal_receivers(dbc_assert_type(signal_tok[20], list)),
                   byte_order=('big_endian'
                               if signal_tok[7] == '0'
                               else 'little_endian'),
                   is_signed=(signal_tok[8] == '-'),
                   raw_initial=get_signal_initial_value(frame_id_dbc, signal_name),
                   conversion=BaseConversion.factory(
                       scale=num(dbc_assert_type(signal_tok[10], str)),
                       offset=num(dbc_assert_type(signal_tok[12], str)),
                       is_float=get_signal_is_float(frame_id_dbc, signal_name),
                       choices=get_signal_choices(frame_id_dbc, signal_name),
                   ),
                   minimum=get_signal_minimum(dbc_assert_type(signal_tok[15], str), dbc_assert_type(signal_tok[17], str)),
                   maximum=get_signal_maximum(dbc_assert_type(signal_tok[15], str), dbc_assert_type(signal_tok[17], str)),
                   unit=(None if signal_tok[19] == '' else dbc_assert_type(signal_tok[19], str)),
                   spn=get_signal_spn(frame_id_dbc, signal_name),
                   dbc_specifics=DbcSpecifics(attributes=get_signal_attributes(frame_id_dbc, signal_name),
                                              attribute_definitions=definitions),
                   comment=get_signal_comment(frame_id_dbc, signal_name),
                   is_multiplexer=get_signal_is_multiplexer(signal_tok),
                   multiplexer_ids=get_signal_multiplexer_ids(mux_field, multiplexer_signal),
                   multiplexer_signal=get_multiplexer_signal(mux_field, multiplexer_signal)))

    return signals


def _get_enum_vframeformat_definition(attribute_definition: AttributeDefinitionType) -> AttributeDefinition[str]:
    """Get VFrameFormat attribute definition as ENUM.

    VFrameFormat can be defined as either an INT or an ENUM attribute in DBC files. If it is not defined,
    cantools uses :py:data:`ATTRIBUTE_DEFINITION_VFRAMEFORMAT`. If the attribute is defined in the DBC as an ENUM,
    then cantools uses the file's definition directly. If the DBC defines the attribute as an INT, then
    we must convert that to an ENUM using the choices defined in :py:data:`ATTRIBUTE_DEFINITION_VFRAMEFORMAT`.
    """

    if attribute_definition.type_name != 'INT':
        return dbc_assert_type(attribute_definition, AttributeDefinition)

    typed_attribute = dbc_assert_type(attribute_definition, AttributeDefinition)
    default_value = typed_attribute.default_value

    if default_value is None:
        raise ParseError('Default value for VFrameFormat attribute must be defined '
                         'if the attribute is defined as an INT.')
    enum_attribute = deepcopy(ATTRIBUTE_DEFINITION_VFRAMEFORMAT)
    enum_attribute.default_value = enum_attribute.choices[default_value]

    return enum_attribute

def _load_messages(tokens: DbcTokens,
                   comments: DbcComments,
                   attributes: DbcAttributes,
                   definitions: OrderedDict[str, AttributeDefinitionType],
                   choices: ChoicesDict,
                   message_senders: dict[int, list[str]],
                   signal_types: dict[int, dict[str, int]],
                   signal_multiplexer_values: MuxValues,
                   strict: bool,
                   bus_name: str | None,
                   signal_groups: dict[int, list[SignalGroup]],
                   sort_signals: type_sort_signals) -> list[Message]:
    """Load messages.

    """

    def get_message_attributes(frame_id_dbc: int) -> DbcAttributeMap | None:
        """Get attributes for given message.

        """

        return attributes.messages.get(frame_id_dbc)

    def get_message_comment(frame_id_dbc: int) -> str | None:
        """Get comment for given message.

        """

        return comments.messages.get(frame_id_dbc)

    def get_message_send_type(frame_id_dbc: int) -> str | None:
        """Get send type for a given message.

        """

        send_type_def = definitions.get('GenMsgSendType')
        if send_type_def is None:
            return None

        message_attributes = get_message_attributes(frame_id_dbc)
        result: str | None = None
        if message_attributes is not None:
            send_type_attr = message_attributes.get('GenMsgSendType')
            if send_type_attr is not None:
                raw_result = send_type_attr.value
                if send_type_def.choices and raw_result is not None:
                    result = send_type_def.choices[int(raw_result)]
                else:
                    result = dbc_assert_type(raw_result, str)
        if result is None:
            if (tmp := send_type_def.default_value) is not None:
                result = dbc_assert_type(tmp, str)

        if result is None:
            result = dbc_assert_type(send_type_def.default_value, str)

        return result

    def get_message_cycle_time(frame_id_dbc: int) -> int | None:
        """Get cycle time for a given message.

        """
        message_attributes = get_message_attributes(frame_id_dbc)

        gen_msg_cycle_time_def = definitions.get('GenMsgCycleTime')
        if gen_msg_cycle_time_def is None:
            return None

        if message_attributes:
            gen_msg_cycle_time_attr = message_attributes.get('GenMsgCycleTime')
            if gen_msg_cycle_time_attr:
                return typing.cast('int | None', gen_msg_cycle_time_attr.value or None)

        return typing.cast('int | None', gen_msg_cycle_time_def.default_value or None)


    def get_message_frame_format(frame_id_dbc: int) -> str | None:
        """Get frame format for a given message"""

        message_attributes = get_message_attributes(frame_id_dbc)
        ref_definitions = definitions.get('VFrameFormat', None)
        if ref_definitions is None:
            return None

        ref_definitions = _get_enum_vframeformat_definition(ref_definitions)

        frame_format: str | None
        if message_attributes is not None and 'VFrameFormat' in message_attributes:
            frame_format_choice = message_attributes['VFrameFormat'].value
            frame_format = ref_definitions.choices[dbc_assert_type(frame_format_choice, int)]
        else:
            frame_format = ref_definitions.default_value

        return frame_format

    def get_message_protocol(frame_id_dbc: int) -> str | None:
        """Get protocol for a given message.

        """

        frame_format = get_message_frame_format(frame_id_dbc)

        if frame_format == 'J1939PG':
            return 'j1939'
        else:
            return None

    def get_message_name(frame_id_dbc: int, message_short_name: str) -> str:
        message_attributes = get_message_attributes(frame_id_dbc)

        if message_attributes is not None and 'SystemMessageLongSymbol' in message_attributes:
            return str(message_attributes['SystemMessageLongSymbol'].value)

        return message_short_name

    def get_message_signal_groups(frame_id_dbc: int) -> list[SignalGroup] | None:
        return signal_groups.get(frame_id_dbc)

    messages = []

    for _message in tokens.get('BO_', []):
        message = dbc_assert_type(_message, list)
        # Any message named VECTOR__INDEPENDENT_SIG_MSG contains
        # signals not assigned to any message. Cantools does not yet
        # support unassigned signals. Discard them for now.
        if message[2] == 'VECTOR__INDEPENDENT_SIG_MSG':
            continue

        # Frame id.
        frame_id_dbc = int(dbc_assert_type(message[1], str))
        frame_id = frame_id_dbc & 0x7fffffff
        is_extended_frame = bool(frame_id_dbc & 0x80000000)
        frame_format = get_message_frame_format(frame_id_dbc)
        if frame_format is not None:
            is_fd = frame_format.endswith('CAN_FD')
        else:
            is_fd = False

        # Senders.
        senders = [_get_node_long_name(attributes, dbc_assert_type(message[5], str))]

        for node in message_senders.get(frame_id_dbc, []):
            if node not in senders:
                senders.append(node)

        if senders == ['Vector__XXX']:
            senders = []

        # Signal multiplexing.
        multiplexer_signal: str | None = None

        for _sig in dbc_assert_type(message[6], list):
            sig = dbc_assert_type(_sig, list)
            mux_field = dbc_assert_type(sig[1], list)
            if len(mux_field) == 2:
                if mux_field[1].endswith('M'):
                    if multiplexer_signal is None:
                        multiplexer_signal = mux_field[0]
                    else:
                        multiplexer_signal = None
                        break

        signal_tokens = dbc_assert_type(message[6], list)
        signals = _load_signals(signal_tokens,
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
                    name=get_message_name(frame_id_dbc, dbc_assert_type(message[2], str)),
                    length=int(dbc_assert_type(message[4], str), 0),
                    senders=senders,
                    send_type=get_message_send_type(frame_id_dbc),
                    cycle_time=get_message_cycle_time(frame_id_dbc),
                    dbc_specifics=DbcSpecifics(attributes=get_message_attributes(frame_id_dbc),
                                               attribute_definitions=definitions),
                    signals=signals,
                    comment=get_message_comment(frame_id_dbc),
                    strict=strict,
                    unused_bit_pattern=0xff,
                    protocol=get_message_protocol(frame_id_dbc),
                    bus_name=bus_name,
                    signal_groups=get_message_signal_groups(frame_id_dbc),
                    sort_signals=sort_signals,
                    is_fd=is_fd))

    return messages


def _load_version(tokens: DbcTokens) -> str | None:
    version_tokens = tokens.get('VERSION', [])
    if not version_tokens:
        return None
    return typing.cast('str | None', dbc_assert_type(version_tokens[0], list)[1])


def _load_bus(attributes: DbcAttributes, comments: DbcComments) -> Bus | None:
    bus_name_attr = attributes.database.get('DBName')
    bus_name = ''
    if bus_name_attr is not None:
        bus_name = dbc_assert_type(bus_name_attr.value, str)

    bus_baudrate_attr = attributes.database.get('Baudrate')
    bus_baudrate = None
    if bus_baudrate_attr is not None:
        bus_baudrate = dbc_assert_type(bus_baudrate_attr.value, int)

    bus_comment = comments.bus

    if not any([bus_name, bus_baudrate, bus_comment]):
        return None

    return Bus(bus_name, baudrate=bus_baudrate, comment=bus_comment)


def _load_nodes(tokens: DbcTokens, comments: DbcComments, attributes: DbcAttributes, attribute_definitions: OrderedDict[str, AttributeDefinitionType]) -> list[Node] | None:
    nodes = None

    for _token in tokens.get('BU_', []):
        token = dbc_assert_type(_token, list)
        nodes = [Node(name=_get_node_long_name(attributes, node_short_name),
                      comment=comments.nodes.get(node_short_name),
                      dbc_specifics=DbcSpecifics(attributes=attributes.nodes.get(node_short_name, None),
                                                 attribute_definitions=attribute_definitions))
                 for node_short_name in dbc_assert_type(token[2], list)]

    return nodes


def get_attribute_definition(database: InternalDatabase, name: str, default: AttributeDefinitionType) -> AttributeDefinitionType:
    if database.dbc is None:
        database.dbc = DbcSpecifics()

    if name not in database.dbc.attribute_definitions:
        database.dbc.attribute_definitions[name] = default

    return database.dbc.attribute_definitions[name]


def get_long_envvar_name_attribute_definition(database: InternalDatabase) -> AttributeDefinition[str]:
    return dbc_assert_type(get_attribute_definition(database,
                                    'SystemEnvVarLongSymbol',
                                    ATTRIBUTE_DEFINITION_LONG_ENVVAR_NAME), AttributeDefinition)


def get_long_node_name_attribute_definition(database: InternalDatabase) -> AttributeDefinition[str]:
    return dbc_assert_type(get_attribute_definition(database,
                                    'SystemNodeLongSymbol',
                                    ATTRIBUTE_DEFINITION_LONG_NODE_NAME), AttributeDefinition)


def get_long_message_name_attribute_definition(database: InternalDatabase) -> AttributeDefinition[str]:
    return dbc_assert_type(get_attribute_definition(database,
                                    'SystemMessageLongSymbol',
                                    ATTRIBUTE_DEFINITION_LONG_MESSAGE_NAME), AttributeDefinition)


def get_long_signal_name_attribute_definition(database: InternalDatabase) -> AttributeDefinition[str]:
    return dbc_assert_type(get_attribute_definition(database,
                                    'SystemSignalLongSymbol',
                                    ATTRIBUTE_DEFINITION_LONG_SIGNAL_NAME), AttributeDefinition)


def try_remove_attribute(dbc: DbcSpecifics, name: str) -> None:
    if dbc is not None and name in dbc.attributes:
        dbc.attributes.pop(name)


def remove_special_chars(database: InternalDatabase) -> InternalDatabase:
    for node in database.nodes:
        new_node_name = get_dbc_name(node.name)

        for message in database.messages:
            for index, sender in enumerate(message.senders):
                if sender == node.name:
                    message.senders[index] = new_node_name

            for signal in message.signals:
                for index, receiver in enumerate(signal.receivers):
                    if receiver == node.name:
                        signal.receivers[index] = new_node_name

        node.name = new_node_name

    for message in database.messages:
        message.name = get_dbc_name(message.name)

        for signal in message.signals:
            signal.name = get_dbc_name(signal.name)

        if message.signal_groups is not None:
            for signal_group in message.signal_groups:
                signal_group.name = get_dbc_name(signal_group.name)
                signal_group.signal_names = [get_dbc_name(sig_name) for sig_name in signal_group.signal_names]

    return database


def make_node_names_unique(database: InternalDatabase, shorten_long_names: bool) -> None:
    converter = LongNamesConverter([node.name for node in database.nodes])

    for node in database.nodes:
        long_name = node.name
        short_name = converter.long_to_short[long_name]
        try_remove_attribute(node.dbc, 'SystemNodeLongSymbol')

        if (long_name == short_name) or not shorten_long_names:
            continue

        for message in database.messages:
            for index, sender in enumerate(message.senders):
                if sender == node.name:
                    message.senders[index] = short_name

            for signal in message.signals:
                for index, receiver in enumerate(signal.receivers):
                    if receiver == node.name:
                        signal.receivers[index] = short_name

        if node.dbc is None:
            node.dbc = DbcSpecifics()

        node.dbc.attributes['SystemNodeLongSymbol'] = Attribute(
            long_name,
            get_long_node_name_attribute_definition(database))
        node.name = short_name


def make_message_names_unique(database: InternalDatabase, shorten_long_names: bool) -> None:
    converter = LongNamesConverter([message.name for message in database.messages])

    for message in database.messages:
        long_name = message.name
        short_name = converter.long_to_short[long_name]
        if message.dbc is not None:
            try_remove_attribute(message.dbc, 'SystemMessageLongSymbol')

        if (long_name == short_name) or not shorten_long_names:
            continue

        if message.dbc is None:
            message.dbc = DbcSpecifics()

        message.dbc.attributes['SystemMessageLongSymbol'] = Attribute(
            long_name,
            get_long_message_name_attribute_definition(database))
        message.name = short_name


def update_signal_relation_attribute_names(database: InternalDatabase,
                                           message: Message,
                                           converter: LongNamesConverter,
                                           shorten_long_names: bool) -> None:
    if database.dbc is None or database.dbc.relation_attributes is None or not shorten_long_names:
        return

    frame_id = get_dbc_frame_id(message)

    signal_map = database.dbc.relation_attributes.node_signal_relations.get(frame_id)
    if signal_map is None:
        return

    updated_signal_map = OrderedDict()
    for signal_name, value in signal_map.items():
        signal_name = converter.long_to_short.get(signal_name, signal_name)
        updated_signal_map[signal_name] = value

    database.dbc.relation_attributes.node_signal_relations[frame_id] = updated_signal_map


def make_signal_names_unique(database: InternalDatabase, shorten_long_names: bool) -> None:
    for message in database.messages:
        converter = LongNamesConverter([signal.name for signal in message.signals])
        update_signal_relation_attribute_names(
            database,
            message,
            converter,
            shorten_long_names)

        for signal in message.signals:
            long_name = signal.name
            short_name = converter.long_to_short[long_name]
            try_remove_attribute(signal.dbc, 'SystemSignalLongSymbol')

            if (long_name == short_name) or not shorten_long_names:
                continue

            if signal.dbc is None:
                signal.dbc = DbcSpecifics()

            signal.dbc.attributes['SystemSignalLongSymbol'] = Attribute(
                long_name,
                get_long_signal_name_attribute_definition(database))
            signal.name = short_name

        if shorten_long_names and message.signal_groups:
            for signal_group in message.signal_groups:
                signal_group.signal_names = [converter.long_to_short[long_name]
                                             for long_name in signal_group.signal_names]


def make_envvar_names_unique(database: InternalDatabase, shorten_long_names: bool) -> None:
    if database.dbc is None:
        return

    envvars = database.dbc.environment_variables.values()
    converter = LongNamesConverter([envvar.name for envvar in envvars])

    for envvar in envvars:
        long_name = envvar.name
        short_name = converter.long_to_short[long_name]
        try_remove_attribute(envvar.dbc, 'SystemEnvVarLongSymbol')

        if (long_name == short_name) or not shorten_long_names:
            continue

        envvar.dbc.attributes['SystemEnvVarLongSymbol'] = Attribute(
            long_name,
            get_long_envvar_name_attribute_definition(database))
        envvar.name = short_name


def make_names_unique(database: InternalDatabase, shorten_long_names: bool) -> InternalDatabase:
    """Make message, signal and node names unique and add attributes for
    their long names.

    """

    make_node_names_unique(database, shorten_long_names)
    make_message_names_unique(database, shorten_long_names)
    make_signal_names_unique(database, shorten_long_names)
    make_envvar_names_unique(database, shorten_long_names)

    return database


def dump_string(database: InternalDatabase,
                sort_signals:type_sort_signals=SORT_SIGNALS_DEFAULT,
                sort_attribute_signals:type_sort_signals=SORT_SIGNALS_DEFAULT,
                sort_attributes:type_sort_attributes=None,
                sort_choices:type_sort_choices=None,
                shorten_long_names:bool=True) -> str:
    """Format database in DBC file format.
       sort_signals defines how to sort signals in message definitions
       sort_attribute_signals defines how to sort signals in metadata -
          comments, value table definitions and attributes

    """

    if sort_signals == SORT_SIGNALS_DEFAULT:
        sort_signals = sort_signals_by_start_bit_reversed
    if sort_attribute_signals == SORT_SIGNALS_DEFAULT:
        sort_attribute_signals = sort_signals_by_start_bit_reversed

    # Make a deep copy of the database as names and attributes will be
    # modified for items with long names.
    database = deepcopy(database)

    if database.dbc is None:
        database.dbc = DbcSpecifics()

    database = remove_special_chars(database)
    database = make_names_unique(database, shorten_long_names)
    bu = _dump_nodes(database)
    val_table = _dump_value_tables(database)
    bo = _dump_messages(database, sort_signals)
    bo_tx_bu = _dump_senders(database)
    cm = _dump_comments(database, sort_attribute_signals)
    signal_types = _dump_signal_types(database)
    ba_def = _dump_attribute_definitions(database)
    ba_def_rel = _dump_relation_attribute_definitions(database)
    ba_def_def = _dump_attribute_definition_defaults(database)
    ba_def_def_rel = _dump_relation_attribute_definition_defaults(database)
    ba = _dump_attributes(database, sort_attribute_signals, sort_attributes)
    ba_rel = _dump_relation_attributes(database, sort_attribute_signals)
    val = _dump_choices(database, sort_attribute_signals, sort_choices)
    sig_group = _dump_signal_groups(database)
    sig_mux_values = _dump_signal_mux_values(database)
    ev = _dump_environment_variables(database)

    return DBC_FMT.format(version=_dump_version(database),
                          bu=' '.join(bu),
                          val_table='\r\n'.join(val_table),
                          bo='\r\n\r\n'.join(bo),
                          bo_tx_bu='\r\n'.join(bo_tx_bu),
                          ev='\r\n\r\n'.join(ev),
                          cm='\r\n'.join(cm),
                          signal_types='\r\n'.join(signal_types),
                          ba_def='\r\n'.join(ba_def),
                          ba_def_rel=''.join([elem+'\r\n' for elem in ba_def_rel]),
                          ba_def_def='\r\n'.join(ba_def_def),
                          ba_def_def_rel=''.join([elem+'\r\n' for elem in ba_def_def_rel]),
                          ba='\r\n'.join(ba),
                          ba_rel=''.join([elem+'\r\n' for elem in ba_rel]),
                          val='\r\n'.join(val),
                          sig_group='\r\n'.join(sig_group),
                          sig_mux_values='\r\n'.join(sig_mux_values))


def get_attribute_definitions_dict(definitions: list[MatchObject], defaults: DbcAttributeDefaults) -> OrderedDict[str, AttributeDefinitionType]:
    result: OrderedDict[str, AttributeDefinitionType] = OrderedDict()

    def convert_value(definition: AttributeDefinitionType, value: DbcAttributeValue) -> DbcAttributeValue:
        if definition.type_name in ['INT', 'HEX']:
            value = to_int(value)
        elif definition.type_name == 'FLOAT':
            value = to_float(value)

        return value

    for _item in definitions:
        item = dbc_assert_type(_item, list)
        kind_list = dbc_assert_type(item[1], list)
        kind: str | None = kind_list[0] if len(kind_list) > 0 else None

        definition = AttributeDefinition(name=dbc_assert_type(item[2], str),
                                         kind=kind,
                                         type_name=dbc_assert_type(item[3], str))
        values = dbc_assert_type(dbc_assert_type(item[4], list)[0], list)

        if len(values) > 0:
            if definition.type_name == 'ENUM':
                definition.choices = dbc_assert_type(values, list)
            elif definition.type_name in ['INT', 'FLOAT', 'HEX']:
                definition.minimum = typing.cast('int | float', convert_value(definition, typing.cast('DbcAttributeValue', values[0])))
                definition.maximum = typing.cast('int | float', convert_value(definition, typing.cast('DbcAttributeValue', values[1])))

        if definition.name in defaults:
            definition.default_value = convert_value(definition, defaults[definition.name])  # type: ignore[assignment]
        else:
            definition.default_value = None

        result[definition.name] = definition

    return result


def get_relation_definitions_dict(definitions: list[MatchObject], defaults: DbcAttributeDefaults) -> OrderedDict[str, AttributeDefinitionType]:
    result: OrderedDict[str, AttributeDefinitionType] = OrderedDict()

    def convert_value(definition: AttributeDefinitionType, value: DbcAttributeValue) -> DbcAttributeValue:
        if definition.type_name in ['INT', 'HEX']:
            value = to_int(value)
        elif definition.type_name == 'FLOAT':
            value = to_float(value)

        return value

    for _item in definitions:
        item = dbc_assert_type(_item, list)
        kind_list = dbc_assert_type(item[1], list)
        kind: str | None = kind_list[0] if len(kind_list) > 0 else None

        definition = AttributeDefinition(name=dbc_assert_type(item[2], str),
                                         kind=kind,
                                         type_name=dbc_assert_type(item[3], str))
        values = dbc_assert_type(item[4], list)

        if len(values) > 0:
            if definition.type_name == 'ENUM':
                definition.choices = dbc_assert_type(values[0], list)
            elif definition.type_name in ['INT', 'FLOAT', 'HEX']:
                inner = dbc_assert_type(values[0], list)
                definition.minimum = typing.cast('int | float', convert_value(definition, inner[0]))
                definition.maximum = typing.cast('int | float', convert_value(definition, inner[1]))

        if definition.name in defaults:
            definition.default_value = convert_value(definition, defaults[definition.name])  # type: ignore[assignment]
        else:
            definition.default_value = None

        result[definition.name] = definition

    return result


def update_signal_relation_attribute_names_after_load(messages: list[Message],
                                                      attributes: DbcAttributes,
                                                      relation_attributes: DbcRelationAttributes) -> None:
    for message in messages:
        frame_id = get_dbc_frame_id(message)

        signal_map = relation_attributes.node_signal_relations.get(frame_id)
        if signal_map is None:
            continue

        signal_attributes: OrderedDict[str, DbcAttributeMap] = attributes.signals.get(frame_id, OrderedDict())
        short_to_signal_name: dict[str, str] = {}
        for signal_name, value in signal_attributes.items():
            if 'SystemSignalLongSymbol' in value:
                short_to_signal_name[signal_name] = str(value['SystemSignalLongSymbol'].value)

        updated_signal_map: OrderedDict[str, OrderedDict[str, OrderedDict[str, AttributeType]]] = OrderedDict()
        for signal_name, node_map in signal_map.items():
            new_signal_name = short_to_signal_name.get(signal_name, signal_name)
            updated_signal_map[new_signal_name] = node_map

        relation_attributes.node_signal_relations[frame_id] = updated_signal_map


def load_string(string: str, strict: bool = True,
                sort_signals: type_sort_signals = sort_signals_by_start_bit) -> InternalDatabase:
    """Parse given string.

    """

    tokens: DbcTokens = dbc_assert_type(DbcParser().parse(string), dict)

    comments = _load_comments(tokens)
    attribute_definitions = _load_attribute_definitions(tokens)
    defaults = _load_attribute_definition_defaults(tokens)
    relation_definitions = _load_relation_attribute_definitions(tokens)
    relation_defaults = _load_relation_attribute_definition_defaults(tokens)
    attribute_definitions_dict = get_attribute_definitions_dict(attribute_definitions, defaults)
    attributes = _load_attributes(tokens, attribute_definitions_dict)
    relation_attribute_definitions = get_relation_definitions_dict(relation_definitions, relation_defaults)
    relation_attributes = _load_relation_attributes(tokens, relation_attribute_definitions)
    bus = _load_bus(attributes, comments)
    value_tables = _load_value_tables(tokens)
    choices = _load_choices(tokens)
    message_senders = _load_message_senders(tokens, attributes)
    signal_types = _load_signal_types(tokens)
    signal_multiplexer_values = _load_signal_multiplexer_values(tokens)
    signal_groups = _load_signal_groups(tokens, attributes)
    messages = _load_messages(tokens,
                              comments,
                              attributes,
                              attribute_definitions_dict,
                              choices,
                              message_senders,
                              signal_types,
                              signal_multiplexer_values,
                              strict,
                              bus.name if bus else None,
                              signal_groups,
                              sort_signals)
    update_signal_relation_attribute_names_after_load(
        messages,
        attributes,
        relation_attributes)
    nodes = _load_nodes(tokens, comments, attributes, attribute_definitions_dict)
    version = _load_version(tokens)
    environment_variables = _load_environment_variables(tokens, comments, attributes, attribute_definitions_dict)
    dbc_specifics = DbcSpecifics(attributes=attributes.database or None,
                                 attribute_definitions=attribute_definitions_dict,
                                 environment_variables=environment_variables,
                                 value_tables=value_tables,
                                 relation_attributes=relation_attributes,
                                 relation_attribute_definitions=relation_attribute_definitions)

    return InternalDatabase(messages,
                            nodes or [],
                            [bus] if bus else [],
                            version,
                            dbc_specifics)
