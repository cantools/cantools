from __future__ import print_function
import re
import time

from ...version import __version__


class Signal(object):

    def __init__(self, signal):
        self._signal = signal
        self.snake_name = camel_to_snake_case(self.name)

    def __getattr__(self, name):
        return getattr(self._signal, name)

    @property
    def unit(self):
        return self._signal.unit if self._signal.unit is not None else '-'

    @property
    def type_length(self):
        if self.length <= 8:
            return 8
        elif self.length <= 16:
            return 16
        elif self.length <= 32:
            return 32
        else:
            return 64

    @property
    def type_name(self):
        if self.is_float:
            if self.length == 32:
                type_name = 'float'
            else:
                type_name = 'double'
        else:
            type_name = 'int{}_t'.format(self.type_length)

            if not self.is_signed:
                type_name = 'u' + type_name

        return type_name

    @property
    def type_suffix(self):
        try:
            return {
                'uint8_t': 'u',
                'uint16_t': 'u',
                'uint32_t': 'u',
                'int64_t': 'll',
                'uint64_t': 'ull',
                'float': 'f'
            }[self.type_name]
        except KeyError:
            return ''

    @property
    def conversion_type_suffix(self):
        try:
            return {
                8: 'u',
                16: 'u',
                32: 'u',
                64: 'ull'
            }[self.type_length]
        except KeyError:
            return ''

    @property
    def unique_choices(self):
        """Make duplicated choice names unique by first appending its value
        and then underscores until unique.

        """

        items = {
            value: camel_to_snake_case(name).upper()
            for value, name in self.choices.items()
        }
        names = list(items.values())
        duplicated_names = [
            name
            for name in set(names)
            if names.count(name) > 1
        ]
        unique_choices = {
            value: name
            for value, name in items.items()
            if names.count(name) == 1
        }

        for value, name in items.items():
            if name in duplicated_names:
                name += _canonical('_{}'.format(value))

                while name in unique_choices.values():
                    name += '_'

                unique_choices[value] = name

        return unique_choices

    @property
    def minimum_type_value(self):
        if self.type_name == 'int8_t':
            return -128
        elif self.type_name == 'int16_t':
            return -32768
        elif self.type_name == 'int32_t':
            return -2147483648
        elif self.type_name == 'int64_t':
            return -9223372036854775808
        elif self.type_name[0] == 'u':
            return 0
        else:
            return None

    @property
    def maximum_type_value(self):
        if self.type_name == 'int8_t':
            return 127
        elif self.type_name == 'int16_t':
            return 32767
        elif self.type_name == 'int32_t':
            return 2147483647
        elif self.type_name == 'int64_t':
            return 9223372036854775807
        elif self.type_name == 'uint8_t':
            return 255
        elif self.type_name == 'uint16_t':
            return 65535
        elif self.type_name == 'uint32_t':
            return 4294967295
        elif self.type_name == 'uint64_t':
            return 18446744073709551615
        else:
            return None

    @property
    def minimum_value(self):
        if self.is_float:
            return None
        elif self.is_signed:
            return -(2 ** (self.length - 1))
        else:
            return 0

    @property
    def maximum_value(self):
        if self.is_float:
            return None
        elif self.is_signed:
            return ((2 ** (self.length - 1)) - 1)
        else:
            return ((2 ** self.length) - 1)

    def segments(self, invert_shift):
        index, pos = divmod(self.start, 8)
        left = self.length

        while left > 0:
            if self.byte_order == 'big_endian':
                if left >= (pos + 1):
                    length = (pos + 1)
                    pos = 7
                    shift = -(left - length)
                    mask = ((1 << length) - 1)
                else:
                    length = left
                    shift = (pos - length + 1)
                    mask = ((1 << length) - 1)
                    mask <<= (pos - length + 1)
            else:
                shift = (left - self.length) + pos

                if left >= (8 - pos):
                    length = (8 - pos)
                    mask = ((1 << length) - 1)
                    mask <<= pos
                    pos = 0
                else:
                    length = left
                    mask = ((1 << length) - 1)
                    mask <<= pos

            if invert_shift:
                if shift < 0:
                    shift = -shift
                    shift_direction = 'left'
                else:
                    shift_direction = 'right'
            else:
                if shift < 0:
                    shift = -shift
                    shift_direction = 'right'
                else:
                    shift_direction = 'left'

            yield index, shift, shift_direction, mask

            left -= length
            index += 1

    def range_str(self):
        minimum = self.decimal.minimum
        maximum = self.decimal.maximum
        scale = self.decimal.scale
        offset = self.decimal.offset

        if minimum is not None and maximum is not None:
            return '{}..{} ({}..{} {})'.format(
                _format_decimal((minimum - offset) / scale),
                _format_decimal((maximum - offset) / scale),
                minimum,
                maximum,
                self.unit)
        elif minimum is not None:
            return '{}.. ({}.. {})'.format(
                _format_decimal((minimum - offset) / scale),
                minimum,
                self.unit)
        elif maximum is not None:
            return '..{} (..{} {})'.format(
                _format_decimal((maximum - offset) / scale),
                maximum,
                self.unit)
        else:
            return '-'

    def scale_str(self):
        return self.scale if self.scale is not None else '-'

    def offset_str(self):
        return self.offset if self.offset is not None else '-'


class Message(object):

    def __init__(self, database_name, message):
        self._message = message
        self.struct_name = '{}_{}'.format(database_name,
                                          camel_to_snake_case(message.name))
        self.snake_name = camel_to_snake_case(self.name)
        self.signals = [Signal(signal)for signal in message.signals]

    def __getattr__(self, name):
        return getattr(self._message, name)

    def get_signal_by_name(self, name):
        for signal in self.signals:
            if signal.name == name:
                return signal


class Context(object):

    def __init__(self,
                 database,
                 database_name,
             output_basename=None,
                 floating_point_numbers=True,
                 bit_fields=False):
        self.version = __version__
        self.date = time.ctime()

        self.database_name = database_name

        if output_basename is not None:
            self.output_basename = output_basename
        else:
            self.output_basename = database_name

        self.include_guard = '{}_H'.format(self.output_basename.upper())
        self.messages = [Message(database_name, message)
                         for message in database.messages]

        self.bit_fields = bit_fields
        self.floating_point_numbers = floating_point_numbers

    @staticmethod
    def indent_c_comment(comment, indent_spaces=0):
        if comment:
            return '\n'.join([
                '{} * '.format(' '*indent_spaces) + line.rstrip()
                for line in comment.splitlines()
            ])
        else:
            return ''


def _canonical(value):
    """Replace anything but 'a-z', 'A-Z' and '0-9' with '_'.

    """

    return re.sub(r'[^a-zA-Z0-9]', '_', value)


def camel_to_snake_case(value):
    value = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', value)
    value = re.sub(r'(_+)', '_', value)
    value = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', value).lower()
    value = _canonical(value)

    return value


def _format_decimal(value):
    if int(value) == value:
        return str(int(value))
    return str(value)
