# Utility functions.

import binascii
from decimal import Decimal
from collections import namedtuple
import bitstruct


Formats = namedtuple('Formats',
                     [
                         'big_endian',
                         'little_endian',
                         'padding_mask'
                     ])


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
    elif scaling:
        if field.is_float:
            return (value - field.offset) / field.scale
        else:
            value = (Decimal(value) - Decimal(field.offset)) / Decimal(field.scale)

            return value.to_integral()
    else:
        return value


def _decode_field(field, value, decode_choices, scaling):
    if decode_choices:
        try:
            return field.choices[value]
        except (KeyError, TypeError):
            pass

    if scaling:
        return (field.scale * value + field.offset)
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


def decode_data(data, fields, formats, decode_choices, scaling):
    unpacked = formats.big_endian.unpack(data)
    unpacked.update(formats.little_endian.unpack(data[::-1]))

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
        items = [('>', '', None)]
        start = 0

        for data in datas:
            if data.byte_order == 'little_endian':
                continue

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
        items = [('>', '', None)]
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

    return Formats(bitstruct.compile(big_fmt, big_names),
                   bitstruct.compile(little_fmt, little_names),
                   big_padding_mask & little_padding_mask)
