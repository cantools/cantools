# Utility functions.

import binascii
from decimal import Decimal
from collections import namedtuple
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
        value = field.choice_string_to_number(value)

    if scaling:
        if field.is_float:
            return (value - field.offset) / field.scale
        else:
            value = (Decimal(value) - Decimal(field.offset)) / Decimal(field.scale)

            return value.to_integral()

    else:
        return value


def _decode_field(field, value, decode_choices, scaling):
    if scaling:
        value = (field.scale * value + field.offset)

    if decode_choices:
        try:
            decoded_field = field.choices[value]
        except (KeyError, TypeError):
            decoded_field = value
    else:
        decoded_field = value

    return decoded_field


def encode_data(data, fields, formats, scaling):
    big_unpacked_data = [
        _encode_field(field, data, scaling)
        for field in fields
        if field.byte_order == 'big_endian'
    ]
    little_unpacked_data = [
        _encode_field(field, data, scaling)
        for field in fields
        if field.byte_order == 'little_endian'
    ]
    big_packed = formats.big_endian.pack(*big_unpacked_data)
    little_packed = formats.little_endian.pack(*little_unpacked_data[::-1])[::-1]
    packed_union = int(binascii.hexlify(big_packed), 16)
    packed_union |= int(binascii.hexlify(little_packed), 16)

    return packed_union


def decode_data(data, fields, formats, decode_choices, scaling):
    big_unpacked = list(formats.big_endian.unpack(data))
    big_fields = [
        field
        for field in fields
        if field.byte_order == 'big_endian'
    ]
    little_unpacked = list(formats.little_endian.unpack(data[::-1])[::-1])
    little_fields = [
        field
        for field in fields
        if field.byte_order == 'little_endian'
    ]
    unpacked = big_unpacked + little_unpacked
    fields = big_fields + little_fields

    return {
        field.name: _decode_field(field,
                                  value,
                                  decode_choices,
                                  scaling)
        for field, value in zip(fields, unpacked)
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

        return fmt, padding_mask

    def data_item(data):
        fmt = '{}{}'.format(get_format_string_type(data),
                            data.length)
        padding_mask = '0' * data.length

        return fmt, padding_mask

    def fmt(items):
        return ''.join([item[0] for item in items])

    def padding_mask(items):
        try:
            return int(''.join([item[1] for item in items]), 2)
        except ValueError:
            return 0

    def create_big():
        items = [('>', '')]
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

        return fmt(items), padding_mask(items)

    def create_little():
        items = [('>', '')]
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
            value = bitstruct.pack('u{}'.format(format_length), value)
            value = int(binascii.hexlify(value[::-1]), 16)

        return fmt(items), value

    big_fmt, big_padding_mask = create_big()
    little_fmt, little_padding_mask = create_little()

    Formats = namedtuple('Formats',
                         ['big_endian', 'little_endian', 'padding_mask'])

    return Formats(bitstruct.compile(big_fmt),
                   bitstruct.compile(little_fmt),
                   big_padding_mask & little_padding_mask)
