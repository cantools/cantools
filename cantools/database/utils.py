# Utility functions.

from collections import OrderedDict
import os.path
import re
from typing import Union, List, Callable, Tuple, Optional, Dict, Sequence, TYPE_CHECKING

from typing_extensions import Literal, Final

from cantools.typechecking import (
    Formats,
    Choices,
    SignalDictType,
    ByteOrder,
)

if TYPE_CHECKING:
    from cantools.database import Database
    from cantools.database.can.attribute import Attribute
    from cantools.database.can.message import Message
    from cantools.database.can.node import Node
    from cantools.database.can.signal import Signal
    from cantools.database.diagnostics import Data

try:
    import bitstruct.c
except ImportError:
    import bitstruct


def format_or(items: List[Union[int, str]]) -> str:
    string_items = [str(item) for item in items]

    if len(string_items) == 1:
        return string_items[0]
    else:
        return '{} or {}'.format(', '.join(string_items[:-1]),
                                 string_items[-1])


def format_and(items: List[Union[int, str]]) -> str:
    string_items = [str(item) for item in items]

    if len(string_items) == 1:
        return str(string_items[0])
    else:
        return '{} and {}'.format(', '.join(string_items[:-1]),
                                  string_items[-1])


def start_bit(data: Union["Data", "Signal"]) -> int:
    if data.byte_order == 'big_endian':
        return 8 * (data.start // 8) + (7 - (data.start % 8))
    else:
        return data.start


def _encode_fields(fields: Sequence[Union["Signal", "Data"]],
                   data: SignalDictType,
                   scaling: bool,
                   ) -> Dict[str, Union[int, float]]:
    unpacked = {}
    for field in fields:
        value = data[field.name]

        if isinstance(value, (float, int)):
            _transform = float if field.is_float else round
            if scaling:
                if field.offset == 0 and field.scale == 1:
                    # treat special case to avoid introduction of unnecessary rounding error
                    unpacked[field.name] = _transform(value)  # type: ignore[operator]
                    continue

                unpacked[field.name] = _transform((value - field.offset) / field.scale)  # type: ignore[operator]
                continue

            unpacked[field.name] = _transform(value)  # type: ignore[operator]
            continue

        unpacked[field.name] = field.choice_string_to_number(str(value))

    return unpacked


def encode_data(data: SignalDictType,
                fields: Sequence[Union["Signal", "Data"]],
                formats: Formats,
                scaling: bool
                ) -> int:
    if len(fields) == 0:
        return 0

    unpacked = _encode_fields(fields, data, scaling)
    big_packed = formats.big_endian.pack(unpacked)
    little_packed = formats.little_endian.pack(unpacked)
    packed_union = int.from_bytes(big_packed, "big") | int.from_bytes(little_packed, "little")

    return packed_union


def decode_data(data: bytes,
                expected_length: int,
                fields: Sequence[Union["Signal", "Data"]],
                formats: Formats,
                decode_choices: bool,
                scaling: bool,
                allow_truncated: bool,
                ) -> SignalDictType:

    actual_length = len(data)
    if allow_truncated and actual_length < expected_length:
        data = data.ljust(expected_length, b"\xFF")

    unpacked = {
        **formats.big_endian.unpack(bytes(data)),
        **formats.little_endian.unpack(bytes(data[::-1])),
    }

    if allow_truncated and actual_length < expected_length:
        # remove fields that are outside available data bytes
        valid_bit_count = actual_length * 8
        for field in fields:
            if field.byte_order == "little_endian":
                sequential_startbit = field.start
            else:
                # Calculate startbit with inverted indices.
                # Function body of ``sawtooth_to_network_bitnum()``
                # is inlined for improved performance.
                sequential_startbit = (8 * (field.start // 8)) + (7 - (field.start % 8))

            if sequential_startbit + field.length > valid_bit_count:
                del unpacked[field.name]

    decoded = {}
    for field in fields:
        try:
            value = unpacked[field.name]

            if decode_choices:
                try:
                    decoded[field.name] = field.choices[value]  # type: ignore[index]
                    continue
                except (KeyError, TypeError):
                    pass

            if scaling:
                decoded[field.name] = field.scale * value + field.offset
                continue
            else:
                decoded[field.name] = value

        except KeyError:
            if not allow_truncated:
                raise

    return decoded


def create_encode_decode_formats(datas: Sequence[Union["Data", "Signal"]], number_of_bytes: int) -> Formats:
    format_length = (8 * number_of_bytes)

    def get_format_string_type(data: Union["Data", "Signal"]) -> str:
        if data.is_float:
            return 'f'
        elif data.is_signed:
            return 's'
        else:
            return 'u'

    def padding_item(length: int) -> Tuple[str, str, None]:
        fmt = 'p{}'.format(length)
        padding_mask = '1' * length

        return fmt, padding_mask, None

    def data_item(data: Union["Data", "Signal"]) -> Tuple[str, str, str]:
        fmt = '{}{}'.format(get_format_string_type(data),
                            data.length)
        padding_mask = '0' * data.length

        return fmt, padding_mask, data.name

    def fmt(items: List[Tuple[str, str, Optional[str]]]) -> str:
        return ''.join([item[0] for item in items])

    def names(items:  List[Tuple[str, str, Optional[str]]]) -> List[str]:
        return [item[2] for item in items if item[2] is not None]

    def padding_mask(items: List[Tuple[str, str, Optional[str]]]) -> int:
        try:
            return int(''.join([item[1] for item in items]), 2)
        except ValueError:
            return 0

    def create_big() -> Tuple[str, int, List[str]]:
        items: List[Tuple[str, str, Optional[str]]] = []
        start = 0

        # Select BE fields
        be_datas = [data for data in datas if data.byte_order == "big_endian"]

        # Ensure BE fields are sorted in network order
        sorted_datas = sorted(be_datas, key = lambda data: sawtooth_to_network_bitnum(data.start))

        for data in sorted_datas:

            padding_length = (start_bit(data) - start)

            if padding_length > 0:
                items.append(padding_item(padding_length))

            items.append(data_item(data))
            start = (start_bit(data) + data.length)

        if start < format_length:
            length = format_length - start
            items.append(padding_item(length))

        return fmt(items), padding_mask(items), names(items)

    def create_little() -> Tuple[str, int, List[str]]:
        items: List[Tuple[str, str, Optional[str]]] = []
        end = format_length

        for data in datas[::-1]:
            if data.byte_order == 'big_endian':
                continue

            padding_length = end - (data.start + data.length)

            if padding_length > 0:
                items.append(padding_item(padding_length))

            items.append(data_item(data))
            end = data.start

        if end > 0:
            items.append(padding_item(end))

        value = padding_mask(items)

        if format_length > 0:
            length = len(''.join([item[1] for item in items]))
            _packed = bitstruct.pack('u{}'.format(length), value)
            value = int.from_bytes(_packed, "little")

        return fmt(items), value, names(items)

    big_fmt, big_padding_mask, big_names = create_big()
    little_fmt, little_padding_mask, little_names = create_little()

    try:
        big_compiled = bitstruct.compile(big_fmt, big_names)
    except Exception as e:
        big_compiled = bitstruct.compile(big_fmt, big_names)

    try:
        little_compiled = bitstruct.compile(little_fmt, little_names)
    except Exception as e:
        little_compiled = bitstruct.compile(little_fmt, little_names)

    return Formats(big_compiled,
                   little_compiled,
                   big_padding_mask & little_padding_mask)


def sawtooth_to_network_bitnum(sawtooth_bitnum: int) -> int:
    '''Convert SawTooth bit number to Network bit number

    Byte     |   0   |   1   |
    Sawtooth |7 ... 0|15... 8|
    Network  |0 ... 7|8 ...15|
    '''
    return (8 * (sawtooth_bitnum // 8)) + (7 - (sawtooth_bitnum % 8))


def cdd_offset_to_dbc_start_bit(cdd_offset: int, bit_length: int, byte_order: ByteOrder) -> int:
    '''Convert CDD/c-style field bit offset to DBC field start bit convention.

    BigEndian (BE) fields are located by their MSBit's sawtooth index.
    LitteleEndian (LE) fields located by their LSBit's sawtooth index.
    '''
    if byte_order == "big_endian":
        # Note: Allow for BE fields that are smaller or larger than 8 bits.
        return (8 * (cdd_offset // 8)) + min(7, (cdd_offset % 8) + bit_length - 1)
    else:
        return cdd_offset


def prune_signal_choices(signal: "Signal") -> None:
    '''Shorten the names of the signal choices of a single signal

    For signals with multiple named values this means removing the
    longest common prefix that ends with an underscore and for which
    the removal still result the named signal values to be valid
    python identifiers. For signals with a single named choice, this
    means removing all leading segments between underscores which
    occur before a segment that contains a digit.

    Examples:

    ..code:: text

       MyMessage_MySignal_Uint32_Choice1, MyMessage_MySignal_Uint32_Choice2
       -> Choice1, Choice2
       MyMessage_MySignal_Uint32_NotAvailable
       -> NotAvailable

    '''

    if signal.choices is None:
        # no named choices
        return

    if len(signal.choices) == 1:
        # signal exhibits only a single named value: Use the longest
        # postfix starting with an underscore that does not contain
        # digits as the new name. If no such suffix exists, leave the
        # choice alone...
        key = next(iter(signal.choices.keys()))
        choice = next(iter(signal.choices.values()))
        m = re.match(r'^[0-9A-Za-z_]*?_([A-Za-z_]+)$', str(choice))
        val = str(choice)
        if m:
            val = m.group(1)

        if isinstance(choice, str):
            signal.choices[key] = val
        else:
            # assert isinstance(choice, NamedSignalValue)
            choice._name = val
        return

    # if there are multiple choices, remove the longest common prefix
    # that ends with an underscore from all of them provided that the
    # names of the choices stay valid identifiers
    choice_values = [ str(x) for x in signal.choices.values() ]
    full_prefix = os.path.commonprefix(choice_values)
    i = full_prefix.rfind('_')

    if i >= 0:
        full_prefix = full_prefix[0:i]
    else:
        # full_prefix does not contain an underscore
        # but the following algorithm assumes it does
        # and would strip too much
        return

    if not full_prefix:
        # the longest possible prefix is empty, i.e., there is nothing
        # to strip from the names of the signal choices
        return

    full_prefix_segments = full_prefix.split('_')

    # find the longest prefix of the choices which keeps all
    # names valid python identifiers
    prefix = ''
    n = 0
    valid_name_re = re.compile('^[a-zA-Z_][a-zA-Z0-9_]*$')
    for i in range(len(full_prefix_segments), -1, -1):
        if i == 0:
            # there is no such non-empty prefix
            return

        prefix = '_'.join(full_prefix_segments[:i]) + '_'
        n = len(prefix)

        if all([valid_name_re.match(x[n:]) for x in choice_values]):
            break

    # remove the prefix from the choice names
    for key, choice in signal.choices.items():
        if isinstance(choice, str):
            signal.choices[key] = choice[n:]
        else:
            # assert isinstance(choice, NamedSignalValue)
            choice._name = choice._name[n:]


def prune_database_choices(database: "Database") -> None:
    '''
    Prune names of all named signal values of all signals of a database
    '''
    for message in database.messages:

        for signal in message.signals:
            prune_signal_choices(signal)

        if message.contained_messages is not None:
            for cm in message.contained_messages:
                for cs in cm.signals:
                    prune_signal_choices(cs)


SORT_SIGNALS_DEFAULT: Final = 'default'
type_sort_signals = Union[Callable[[List["Signal"]], List["Signal"]], Literal['default'], None]

type_sort_attribute = Union[
    Tuple[Literal['dbc'],     "Attribute", None,   None,      None],
    Tuple[Literal['node'],    "Attribute", "Node", None,      None],
    Tuple[Literal['message'], "Attribute", None,   "Message", None],
    Tuple[Literal['signal'],  "Attribute", None,   "Message", "Signal"],
]

type_sort_attributes = Union[Callable[[List[type_sort_attribute]], List[type_sort_attribute]], Literal['default'], None]

type_sort_choices = Union[Callable[[Choices], Choices], None]

def sort_signals_by_start_bit(signals: List["Signal"]) -> List["Signal"]:
    return list(sorted(signals, key=start_bit))


def sort_signals_by_start_bit_reversed(signals: List["Signal"]) -> List["Signal"]:
    return list(reversed(sorted(signals, key=start_bit)))


def sort_signals_by_name(signals: List["Signal"]) -> List["Signal"]:
    return list(sorted(signals, key=lambda s: s.name))


def sort_signals_by_start_bit_and_mux(signals: List["Signal"]) -> List["Signal"]:
    # sort by start bit
    signals = sorted(signals, key=start_bit)
    # but unmuxed values come first
    signals = sorted(signals, key=lambda s: bool(s.multiplexer_ids))
    # and group by mux... -1 is fine as the "no mux" case because even negative
    # multiplexors get cast to unsigned in the .dbc
    signals = sorted(
        signals, key=lambda s: s.multiplexer_ids[0] if s.multiplexer_ids else -1
    )

    return signals


def sort_choices_by_value(choices: Choices) -> Choices:
    return OrderedDict(sorted(choices.items(), key=lambda x: x[0]))


def sort_choices_by_value_descending(choices: Choices) -> Choices:
    return OrderedDict(sorted(choices.items(), key=lambda x: x[0], reverse=True))
