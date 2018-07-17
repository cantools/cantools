# A CAN message.

import binascii
from copy import deepcopy

from ..utils import format_or
from ..utils import start_bit
from ..utils import encode_data
from ..utils import decode_data
from ..utils import create_encode_decode_formats
from ..errors import Error


class EncodeError(Error):
    pass


class DecodeError(Error):
    pass


class Message(object):
    """A CAN message with frame id, comment, signals and other
    information.

    If `strict` is ``True`` an exception is raised if any signals are
    overlapping or if they don't fit in the message.

    """

    def __init__(self,
                 frame_id,
                 name,
                 length,
                 signals,
                 comment=None,
                 senders=None,
                 send_type=None,
                 cycle_time=None,
                 dbc_specifics=None,
                 is_extended_frame=False,
                 bus_name=None,
                 strict=True):
        self._frame_id = frame_id
        self._is_extended_frame = is_extended_frame
        self._name = name
        self._length = length
        self._signals = signals
        self._signals.sort(key=start_bit)
        self._comment = comment
        self._senders = senders if senders else []
        self._send_type = send_type
        self._cycle_time = cycle_time
        self._dbc = dbc_specifics
        self._bus_name = bus_name
        self._codecs = None
        self._signal_tree = None
        self._strict = strict
        self.refresh()

    def _create_codec(self, parent_signal=None, multiplexer_id=None):
        """Create a codec of all signals with given parent signal. This is a
        recursive function.

        """

        signals = []
        multiplexers = {}

        # Find all signals matching given parent signal name and given
        # multiplexer id. Root signals' parent and multiplexer id are
        # both None.
        for signal in self._signals:
            if signal.multiplexer_signal != parent_signal:
                continue

            if ((multiplexer_id is not None)
                and (multiplexer_id not in signal.multiplexer_ids)):
                continue

            if signal.is_multiplexer:
                children_ids = set()

                for s in self._signals:
                    if s.multiplexer_signal != signal.name:
                        continue

                    children_ids.update(s.multiplexer_ids)

                # Some CAN messages will have muxes containing only
                # the multiplexer and no additional signals. At Tesla
                # these are indicated in advance by assigning them an
                # enumeration. Here we ensure that any named
                # multiplexer is included, even if it has no child
                # signals.
                if signal.choices:
                    children_ids.update(signal.choices.keys())

                for child_id in children_ids:
                    codec = self._create_codec(signal.name, child_id)

                    if signal.name not in multiplexers:
                        multiplexers[signal.name] = {}

                    multiplexers[signal.name][child_id] = codec

            signals.append(signal)

        return {
            'signals': signals,
            'formats': create_encode_decode_formats(signals,
                                                    self._length),
            'multiplexers': multiplexers
        }

    def _create_signal_tree(self, codec):
        """Create a multiplexing tree node of given codec. This is a recursive
        function.

        """

        nodes = []

        for signal in codec['signals']:
            multiplexers = codec['multiplexers']

            if signal.name in multiplexers:
                node = {
                    signal.name: {
                        mux: self._create_signal_tree(mux_codec)
                        for mux, mux_codec in multiplexers[signal.name].items()
                    }
                }
            else:
                node = signal.name

            nodes.append(node)

        return nodes

    @property
    def frame_id(self):
        """The message frame id.

        """

        return self._frame_id

    @frame_id.setter
    def frame_id(self, value):
        self._frame_id = value

    @property
    def is_extended_frame(self):
        """``True`` if the message is an extended frame, ``False`` otherwise.

        """

        return self._is_extended_frame

    @is_extended_frame.setter
    def is_extended_frame(self, value):
        self._is_extended_frame = value

    @property
    def name(self):
        """The message name as a string.

        """

        return self._name

    @name.setter
    def name(self, value):
        self._name = value

    @property
    def length(self):
        """The message data length in bytes.

        """

        return self._length

    @length.setter
    def length(self, value):
        self._length = value

    @property
    def signals(self):
        """A list of all signals in the message.

        """

        return self._signals

    @property
    def comment(self):
        """The message comment, or ``None`` if unavailable.

        """

        return self._comment

    @comment.setter
    def comment(self, value):
        self._comment = value

    @property
    def senders(self):
        """A list of all sender nodes of this message.

        """

        return self._senders

    @property
    def send_type(self):
        """The message send type, or ``None`` if unavailable.

        """

        return self._send_type

    @property
    def cycle_time(self):
        """The message cycle time, or ``None`` if unavailable.

        """

        return self._cycle_time

    @property
    def dbc(self):
        """An object containing dbc specific properties like e.g. attributes.

        """

        return self._dbc

    @property
    def bus_name(self):
        """The message bus name, or ``None`` if unavailable.

        """

        return self._bus_name

    @bus_name.setter
    def bus_name(self, value):
        self._bus_name = value

    @property
    def signal_tree(self):
        """All signal names and multiplexer ids as a tree. Multiplexer signals
        are dictionaries, while other signals are strings.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.signal_tree
        ['Bar', 'Fum']
        >>> bar = db.get_message_by_name('Bar')
        >>> bar.signal_tree
        [{'A': {0: ['C', 'D'], 1: ['E']}}, 'B']

        """

        return self._signal_tree

    def signal_tree_string(self):
        """Returns the message signal tree as a string.

        """

        def get_prefix(index, length):
            if index < length - 1:
                return '|   '
            else:
                return '    '

        def add_prefix(prefix, lines):
            return [prefix + line for line in lines]

        def format_mux(mux):
            signal_name, multiplexed_signals = list(mux.items())[0]
            multiplexed_signals = sorted(list(multiplexed_signals.items()))
            lines = []

            for index, multiplexed_signal in enumerate(multiplexed_signals):
                multiplexer_id, signal_names = multiplexed_signal
                lines.append('+-- {}'.format(multiplexer_id))
                lines += add_prefix(get_prefix(index, len(multiplexed_signals)),
                                    format_level_lines(signal_names))

            return '+-- {}'.format(signal_name), lines

        def format_level_lines(signal_names):
            lines = []

            for index, signal_name in enumerate(signal_names):
                if isinstance(signal_name, dict):
                    signal_name_line, signal_lines = format_mux(signal_name)
                    signal_lines = add_prefix(get_prefix(index, len(signal_names)),
                                              signal_lines)
                else:
                    signal_name_line = '+-- {}'.format(signal_name)
                    signal_lines = []

                lines.append(signal_name_line)
                lines += signal_lines

            return lines

        lines = format_level_lines(self.signal_tree)
        lines = ['-- {root}'] + add_prefix('   ', lines)

        return '\n'.join(lines)

    def layout_string(self, signal_names=True):
        """Returns the message layout as an ASCII art string. Each signal is
        an arrow from LSB ``x`` to MSB ``<``. Overlapping signal bits
        are set to ``X``.

        Set `signal_names` to ``False`` to hide signal names.

        .. code:: text

                                Bit

                   7   6   5   4   3   2   1   0
                 +---+---+---+---+---+---+---+---+
               0 |   |   |   |   |   |<----------|
                 +---+---+---+---+---+---+---+---+
               1 |------x|   |   |   |   |<-x|   |
                 +---+---+---+---+---+---+---+---+
                       |                   +-- Bar
                       +-- Foo
                 +---+---+---+---+---+---+---+---+
               2 |   |   |   |   |   |   |   |   |
           B     +---+---+---+---+---+---+---+---+
           y   3 |----XXXXXXX---x|   |   |   |   |
           t     +---+---+---+---+---+---+---+---+
           e           +-- Fie
                 +---+---+---+---+---+---+---+---+
               4 |-------------------------------|
                 +---+---+---+---+---+---+---+---+
               5 |   |   |<----------------------|
                 +---+---+---+---+---+---+---+---+
                           +-- Fum
                 +---+---+---+---+---+---+---+---+
               6 |   |   |   |   |   |   |   |   |
                 +---+---+---+---+---+---+---+---+
               7 |   |   |   |   |   |   |   |   |
                 +---+---+---+---+---+---+---+---+

        """

        def format_big():
            signals = []

            for signal in self._signals:
                if signal.byte_order != 'big_endian':
                    continue

                formatted = start_bit(signal) * '   '
                formatted += '<{}x'.format((3 * signal.length - 2) * '-')
                signals.append(formatted)

            return signals

        def format_little():
            signals = []

            for signal in self._signals:
                if signal.byte_order != 'little_endian':
                    continue

                formatted = signal.start * '   '
                formatted += 'x{}<'.format((3 * signal.length - 2) * '-')
                end = signal.start + signal.length

                if end % 8 != 0:
                    formatted += (8 - (end % 8)) * '   '

                formatted = ''.join([
                    formatted[i:i + 24][::-1]
                    for i in range(0, len(formatted), 24)
                ])
                signals.append(formatted)

            return signals

        def format_byte_lines():
            # Signal lines.
            signals = format_big() + format_little()

            if len(signals) > 0:
                length = max([len(signal) for signal in signals])

                if length % 24 != 0:
                    length += (24 - (length % 24))

                signals = [signal + (length - len(signal)) * ' ' for signal in signals]

            # Signals union line.
            signals_union = ''

            for chars in zip(*signals):
                head = chars.count('<')
                dash = chars.count('-')
                tail = chars.count('x')

                if head + dash + tail > 1:
                    signals_union += 'X'
                elif head == 1:
                    signals_union += '<'
                elif dash == 1:
                    signals_union += '-'
                elif tail == 1:
                    signals_union += 'x'
                else:
                    signals_union += ' '

            # Split the signals union line into byte lines, 8 bits per
            # line.
            byte_lines = [
                signals_union[i:i + 24]
                for i in range(0, len(signals_union), 24)
            ]

            unused_byte_lines = (self._length - len(byte_lines))

            if unused_byte_lines > 0:
                byte_lines += unused_byte_lines * [24 * ' ']

            # Insert bits separators into each byte line.
            lines = []

            for byte_line in byte_lines:
                line = ''
                prev_byte = None

                for i in range(0, 24, 3):
                    byte_triple = byte_line[i:i + 3]

                    if i == 0:
                        line += '|'
                    elif byte_triple[0] in ' <>x':
                        line += '|'
                    elif byte_triple[0] == 'X':
                        if prev_byte == 'X':
                            line += 'X'
                        elif prev_byte == '-':
                            line += '-'
                        else:
                            line += '|'
                    else:
                        line += '-'

                    line += byte_triple
                    prev_byte = byte_triple[2]

                line += '|'
                lines.append(line)

            # Add byte numbering.
            number_width = len(str(len(lines))) + 4
            number_fmt = '{{:{}d}} {{}}'.format(number_width - 1)
            a = []

            for number, line in enumerate(lines):
                a.append(number_fmt.format(number, line))

            return a, len(lines), number_width

        def add_header_lines(lines, number_width):
            padding = number_width * ' '

            return [
                padding + '               Bit',
                padding + '',
                padding + '  7   6   5   4   3   2   1   0',
                padding + '+---+---+---+---+---+---+---+---+'
            ] + lines

        def add_horizontal_lines(byte_lines, number_width):
            padding = number_width * ' '
            lines = []

            for byte_line in byte_lines:
                lines.append(byte_line)
                lines.append(padding + '+---+---+---+---+---+---+---+---+')

            return lines

        def name_bit(signal):
            offset = start_bit(signal) + signal.length - 1

            if signal.byte_order == 'big_endian':
                return (8 * (offset // 8) + (7 - (offset % 8)))
            else:
                return offset

        def add_signal_names(input_lines,
                             number_of_bytes,
                             number_width):
            # Find MSB and name of all signals.
            padding = number_width * ' '
            signals_per_byte = [[] for _ in range(number_of_bytes)]

            for signal in self._signals:
                byte, bit = divmod(name_bit(signal), 8)
                signals_per_byte[byte].append((bit, '+-- ' + signal.name))

            # Format signal lines.
            signal_lines_per_byte = []

            for signals in signals_per_byte:
                signals = sorted(signals)
                signals_lines = []

                for signal in signals:
                    line = number_width * ' ' + '  ' + signal[1]
                    line = (7 - signal[0]) * '    ' + line
                    chars = [char for char in line]

                    for other_signal in signals:
                        if other_signal[0] > signal[0]:
                            other_signal_msb = (number_width
                                                + 2
                                                + 4 * (7 - other_signal[0]))
                            chars[other_signal_msb] = '|'

                    signals_lines.append(''.join(chars))

                signal_lines_per_byte.append(signals_lines)

            # Insert the signals names lines among other lines.
            lines = []

            for number in range(number_of_bytes):
                lines += input_lines[2 * number: 2 * number + 2]

                if signal_lines_per_byte[number]:
                    lines += signal_lines_per_byte[number]

                    if number + 1 < number_of_bytes:
                        lines.append(
                            padding + '+---+---+---+---+---+---+---+---+')

            return lines

        def add_y_axis_name(lines):
            number_of_matrix_lines = (len(lines) - 3)

            if number_of_matrix_lines < 5:
                lines += (5 - number_of_matrix_lines) * ['     ']

            start_index = 4 + ((number_of_matrix_lines - 4) // 2 - 1)

            if start_index < 4:
                start_index = 4

            axis_lines = start_index * ['  ']
            axis_lines += [' B', ' y', ' t', ' e']
            axis_lines += (len(lines) - start_index - 4) * ['  ']

            return [
                axis_line + line
                for axis_line, line in zip(axis_lines, lines)
            ]

        lines, number_of_bytes, number_width = format_byte_lines()
        lines = add_horizontal_lines(lines, number_width)

        if signal_names:
            lines = add_signal_names(lines,
                                     number_of_bytes,
                                     number_width)

        lines = add_header_lines(lines, number_width)
        lines = add_y_axis_name(lines)
        lines = [line.rstrip() for line in lines]

        return '\n'.join(lines)

    def _get_mux_number(self, decoded, signal_name):
        mux = decoded[signal_name]

        if isinstance(mux, str):
            signal = self.get_signal_by_name(signal_name)
            mux = signal.choice_string_to_number(mux)

        return mux

    def _check_signals_ranges_scaling(self, signals, data):
        for signal in signals:
            value = data[signal.name]

            # Choices are checked later.
            if isinstance(value, str):
                continue

            if signal.minimum is not None:
                if value < signal.minimum:
                    raise EncodeError(
                        "Expected signal '{}' value greater than or equal to "
                        "{} in message '{}', but got {}.".format(signal.name,
                                                                 signal.minimum,
                                                                 self._name,
                                                                 value))

            if signal.maximum is not None:
                if value > signal.maximum:
                    raise EncodeError(
                        "Expected signal '{}' value less than or equal to "
                        "{} in message '{}', but got {}.".format(signal.name,
                                                                 signal.maximum,
                                                                 self.name,
                                                                 value))

    def _check_signals(self, signals, data, scaling):
        for signal in signals:
            if signal.name not in data:
                raise EncodeError(
                    "Expected signal value for '{}' in data, but got {}.".format(
                        signal.name,
                        data))

        if scaling:
            self._check_signals_ranges_scaling(signals, data)

    def _encode(self, node, data, scaling, strict):
        if strict:
            self._check_signals(node['signals'], data, scaling)

        encoded = encode_data(data,
                              node['signals'],
                              node['formats'],
                              scaling)
        padding_mask = node['formats'].padding_mask
        multiplexers = node['multiplexers']

        for signal in multiplexers:
            mux = self._get_mux_number(data, signal)

            try:
                node = multiplexers[signal][mux]
            except KeyError:
                raise EncodeError('expected multiplexer id {}, but got {}'.format(
                    format_or(multiplexers[signal]),
                    mux))

            mux_encoded, mux_padding_mask = self._encode(node,
                                                         data,
                                                         scaling,
                                                         strict)
            encoded |= mux_encoded
            padding_mask &= mux_padding_mask

        return encoded, padding_mask

    def encode(self, data, scaling=True, padding=False, strict=True):
        """Encode given data as a message of this type.

        If `scaling` is ``False`` no scaling of signals is performed.

        If `padding` is ``True`` unused bits are encoded as 1.

        If `strict` is ``True`` all signal values must be within their
        allowed ranges, or an exception is raised.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.encode({'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        encoded, padding_mask = self._encode(self._codecs,
                                             data,
                                             scaling,
                                             strict)

        if padding:
            encoded |= padding_mask

        encoded |= (0x80 << (8 * self._length))
        encoded = hex(encoded)[4:].rstrip('L')

        return binascii.unhexlify(encoded)[:self._length]

    def _decode(self, node, data, decode_choices, scaling):
        decoded = decode_data(data,
                              node['signals'],
                              node['formats'],
                              decode_choices,
                              scaling)

        multiplexers = node['multiplexers']

        for signal in multiplexers:
            mux = self._get_mux_number(decoded, signal)

            try:
                node = multiplexers[signal][mux]
            except KeyError:
                raise DecodeError('expected multiplexer id {}, but got {}'.format(
                    format_or(multiplexers[signal]),
                    mux))

            decoded.update(self._decode(node,
                                        data,
                                        decode_choices,
                                        scaling))

        return decoded

    def decode(self, data, decode_choices=True, scaling=True):
        """Decode given data as a message of this type.

        If `decode_choices` is ``False`` scaled values are not
        converted to choice strings (if available).

        If `scaling` is ``False`` no scaling of signals is performed.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.decode(b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}

        """

        data = data[:self._length]

        return self._decode(self._codecs, data, decode_choices, scaling)

    def get_signal_by_name(self, name):
        for signal in self._signals:
            if signal.name == name:
                return signal

        raise KeyError(name)

    def is_multiplexed(self):
        """Returns ``True`` if the message is multiplexed, otherwise
        ``False``.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.is_multiplexed()
        False
        >>> bar = db.get_message_by_name('Bar')
        >>> bar.is_multiplexed()
        True

        """

        return bool(self._codecs['multiplexers'])

    def _check_signal(self, message_bits, signal):
        signal_bits = signal.length * [signal.name]

        if signal.byte_order == 'big_endian':
            padding = start_bit(signal) * [None]
            signal_bits = padding + signal_bits
        else:
            signal_bits += signal.start * [None]

            if len(signal_bits) < len(message_bits):
                padding = (len(message_bits) - len(signal_bits)) * [None]
                reversed_signal_bits = padding + signal_bits
            else:
                reversed_signal_bits = signal_bits

            signal_bits = []

            for i in range(0, len(reversed_signal_bits), 8):
                signal_bits = reversed_signal_bits[i:i + 8] + signal_bits

        # Check that the signal fits in the message.
        if len(signal_bits) > len(message_bits):
            raise Error(
                'The signal {} does not fit in message {}.'.format(
                    signal.name,
                    self.name))

        # Check that the signal does not overlap with other
        # signals.
        for offset, signal_bit in enumerate(signal_bits):
            if signal_bit is not None:
                if message_bits[offset] is not None:
                    raise Error(
                        'The signals {} and {} are overlapping in message {}.'.format(
                            signal.name,
                            message_bits[offset],
                            self.name))

                message_bits[offset] = signal.name

    def _check_mux(self, message_bits, mux):
        signal_name, children = list(mux.items())[0]
        self._check_signal(message_bits,
                           self.get_signal_by_name(signal_name))
        children_message_bits = deepcopy(message_bits)

        for multiplexer_id in sorted(children):
            child_tree = children[multiplexer_id]
            child_message_bits = deepcopy(children_message_bits)
            self._check_signal_tree(child_message_bits, child_tree)

            for i, child_bit in enumerate(child_message_bits):
                if child_bit is not None:
                    message_bits[i] = child_bit

    def _check_signal_tree(self, message_bits, signal_tree):
        for signal_name in signal_tree:
            if isinstance(signal_name, dict):
                self._check_mux(message_bits, signal_name)
            else:
                self._check_signal(message_bits,
                                   self.get_signal_by_name(signal_name))

    def refresh(self, strict=None):
        """Refresh the internal message state.

        If `strict` is ``True`` an exception is raised if any signals
        are overlapping or if they don't fit in the message. This
        argument overrides the value of the same argument passed to
        the constructor.

        """

        self._codecs = self._create_codec()
        self._signal_tree = self._create_signal_tree(self._codecs)

        if strict is None:
            strict = self._strict

        if strict:
            message_bits = 8 * self.length * [None]
            self._check_signal_tree(message_bits, self.signal_tree)

    def __repr__(self):
        return "message('{}', 0x{:x}, {}, {}, {})".format(
            self._name,
            self._frame_id,
            self._is_extended_frame,
            self._length,
            "'" + self._comment + "'" if self._comment is not None else None)
