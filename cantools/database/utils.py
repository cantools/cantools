# Utility functions.

import binascii
import os.path
import re
from decimal import Decimal
from typing import Dict, Union, List, Callable

from typing_extensions import Literal, Final

from cantools.database.can.signal import NamedSignalValue, Signal
from cantools.typechecking import Formats

try:
    import bitstruct.c
except ImportError:
    import bitstruct


def format_or(items):
    items = [str(item) for item in items]

    if len(items) == 1:
        return items[0]
    else:
        return '{} or {}'.format(', '.join(items[:-1]),
                                 items[-1])


def format_and(items):
    items = [str(item) for item in items]

    if len(items) == 1:
        return items[0]
    else:
        return '{} and {}'.format(', '.join(items[:-1]),
                                  items[-1])


def start_bit(data):
    if data.byte_order == 'big_endian':
        return (8 * (data.start // 8) + (7 - (data.start % 8)))
    else:
        return data.start


def _encode_field(field, data, scaling):
    value = data[field.name]

    if isinstance(value, str):
        return field.choice_string_to_number(value)
    elif isinstance(value, NamedSignalValue):
        return field.choice_string_to_number(str(value))
    elif scaling:
        if field.is_float:
            return (value - field.offset) / field.scale
        else:
            value = (Decimal(value) - Decimal(field.offset)) / Decimal(field.scale)

            return int(value.to_integral())
    else:
        return value


def _decode_field(field, value, decode_choices, scaling):
    if decode_choices:
        try:
            return field.choices[value]
        except (KeyError, TypeError):
            pass

    if scaling:
        is_int = \
            lambda x: isinstance(x, int) or (isinstance(x, float) and x.is_integer())
        if field.is_float \
           or not is_int(field.scale) \
           or not is_int(field.offset):
            return (field.scale * value + field.offset)
        else:
            return int(field.scale * value + field.offset)
    else:
        return value


def encode_data(data, fields, formats, scaling):
    if len(fields) == 0:
        return 0

    unpacked = {
        field.name: _encode_field(field, data, scaling)
        for field in fields
    }
    big_packed = formats.big_endian.pack(unpacked)
    little_packed = formats.little_endian.pack(unpacked)[::-1]
    packed_union = int(binascii.hexlify(big_packed), 16)
    packed_union |= int(binascii.hexlify(little_packed), 16)

    return packed_union


def decode_data(data: bytes,
                fields: List[Signal],
                formats: Formats,
                decode_choices: bool,
                scaling: bool,
                ) -> Dict[str, Union[float, str]]:
    unpacked = formats.big_endian.unpack(bytes(data))
    unpacked.update(formats.little_endian.unpack(bytes(data[::-1])))

    return {
        field.name: _decode_field(field,
                                  unpacked[field.name],
                                  decode_choices,
                                  scaling)
        for field in fields
    }


def create_encode_decode_formats(datas, number_of_bytes):
    format_length = (8 * number_of_bytes)

    def get_format_string_type(data):
        if data.is_float:
            return 'f'
        elif data.is_signed:
            return 's'
        else:
            return 'u'

    def padding_item(length):
        fmt = 'p{}'.format(length)
        padding_mask = '1' * length

        return fmt, padding_mask, None

    def data_item(data):
        fmt = '{}{}'.format(get_format_string_type(data),
                            data.length)
        padding_mask = '0' * data.length

        return fmt, padding_mask, data.name

    def fmt(items):
        return ''.join([item[0] for item in items])

    def names(items):
        return [item[2] for item in items if item[2] is not None]

    def padding_mask(items):
        try:
            return int(''.join([item[1] for item in items]), 2)
        except ValueError:
            return 0

    def create_big():
        items = []
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

    def create_little():
        items = []
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
            value = bitstruct.pack('u{}'.format(length), value)
            value = int(binascii.hexlify(value[::-1]), 16)

        return fmt(items), value, names(items)

    big_fmt, big_padding_mask, big_names = create_big()
    little_fmt, little_padding_mask, little_names = create_little()

    try:
        big_compiled = bitstruct.c.compile(big_fmt, big_names)
    except Exception as e:
        big_compiled = bitstruct.compile(big_fmt, big_names)

    try:
        little_compiled = bitstruct.c.compile(little_fmt, little_names)
    except Exception as e:
        little_compiled = bitstruct.compile(little_fmt, little_names)

    return Formats(big_compiled,
                   little_compiled,
                   big_padding_mask & little_padding_mask)


def sawtooth_to_network_bitnum(sawtooth_bitnum):
    '''Convert SawTooth bit number to Network bit number

    Byte     |   0   |   1   |
    Sawtooth |7 ... 0|15... 8|
    Network  |0 ... 7|8 ...15|
    '''
    return (8 * (sawtooth_bitnum // 8)) + (7 - (sawtooth_bitnum % 8))


def cdd_offset_to_dbc_start_bit(cdd_offset, bit_length, byte_order):
    '''Convert CDD/c-style field bit offset to DBC field start bit convention.

    BigEndian (BE) fields are located by their MSBit's sawtooth index.
    LitteleEndian (LE) fields located by their LSBit's sawtooth index.
    '''
    if byte_order == "big_endian":
        # Note: Allow for BE fields that are smaller or larger than 8 bits.
        return (8 * (cdd_offset // 8)) + min(7, (cdd_offset % 8) + bit_length - 1)
    else:
        return cdd_offset

def prune_signal_choices(signal):
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
        m = re.match(r'^[0-9A-Za-z_]*?_([A-Za-z_]+)$', choice.name)
        val = str(choice)
        if m:
            val = m.group(1)

        if isinstance(choice, str):
            signal.choices[key] = val
        else:
            # assert isinstance(choice, NamedSignalValue)
            signal.choices[key]._name = val
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
            signal.choices[key]._name = choice._name[n:]

def prune_database_choices(database):
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
type_sort_signals = Union[Callable[[List[Signal]], List[Signal]], Literal['default'], None]

def sort_signals_by_start_bit(signals: List[Signal]) -> List[Signal]:
	return list(sorted(signals, key=start_bit))

def sort_signals_by_start_bit_reversed(signals: List[Signal]) -> List[Signal]:
	return list(reversed(sorted(signals, key=start_bit)))
