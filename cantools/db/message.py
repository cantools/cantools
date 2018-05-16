# A CAN message.

import binascii
from collections import namedtuple
from decimal import Decimal
import bitstruct


class EncodeError(Exception):
    pass


class DecodeError(Exception):
    pass


def _format_or(items):
    items = [str(item) for item in items]

    if len(items) == 1:
        return items
    else:
        return '{} or {}'.format(', '.join(items[:-1]),
                                 items[-1])


def _start_bit(signal):
    if signal.byte_order == 'big_endian':
        return (8 * (signal.start // 8) + (7 - (signal.start % 8)))
    else:
        return signal.start


def _encode_signal(signal, data, scaling):
    value = data[signal.name]

    if isinstance(value, str):
        value = signal.choice_string_to_number(value)

    if scaling:
        if signal.is_float:
            return (value - signal.offset) / signal.scale
        else:
            value = (Decimal(value) - Decimal(signal.offset)) / Decimal(signal.scale)

            return value.to_integral()
    else:
        return value


def _decode_signal(signal, value, decode_choices, scaling):
    if scaling:
        value = (signal.scale * value + signal.offset)

    if decode_choices:
        try:
            decoded_signal = signal.choices[value]
        except (KeyError, TypeError):
            decoded_signal = value
    else:
        decoded_signal = value

    return decoded_signal


def _encode_data(data, signals, formats, scaling):
    big_unpacked_data = [
        _encode_signal(signal, data, scaling)
        for signal in signals
        if signal.byte_order == 'big_endian'
    ]
    little_unpacked_data = [
        _encode_signal(signal, data, scaling)
        for signal in signals
        if signal.byte_order == 'little_endian'
    ]
    big_packed = formats.big_endian.pack(*big_unpacked_data)
    little_packed = formats.little_endian.pack(*little_unpacked_data[::-1])[::-1]
    packed_union = int(binascii.hexlify(big_packed), 16)
    packed_union |= int(binascii.hexlify(little_packed), 16)

    return packed_union


def _decode_data(data, signals, formats, decode_choices, scaling):
    big_unpacked = list(formats.big_endian.unpack(data))
    big_signals = [
        signal
        for signal in signals
        if signal.byte_order == 'big_endian'
    ]
    little_unpacked = list(formats.little_endian.unpack(data[::-1])[::-1])
    little_signals = [
        signal
        for signal in signals
        if signal.byte_order == 'little_endian'
    ]
    unpacked = big_unpacked + little_unpacked
    signals = big_signals + little_signals

    return {
        signal.name: _decode_signal(signal,
                                    value,
                                    decode_choices,
                                    scaling)
        for signal, value in zip(signals, unpacked)
    }


class Message(object):
    """A CAN message with frame id, comment, signals and other
    information.

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
                 is_extended_frame=False,
                 bus_name=None):
        self._frame_id = frame_id
        self._is_extended_frame = is_extended_frame
        self._name = name
        self._length = length
        self._signals = signals
        self._signals.sort(key=_start_bit)
        self._comment = comment
        self._senders = senders
        self._send_type = send_type
        self._cycle_time = cycle_time
        self._bus_name = bus_name
        self._codecs = self._create_codec()
        self._signal_tree = self._create_signal_tree(self._codecs)

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
            'formats': self._create_message_encode_decode_formats(signals),
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

    def _create_message_encode_decode_formats(self, signals):
        message_length = (8 * self._length)

        def get_format_string_type(signal):
            if signal.is_float:
                return 'f'
            elif signal.is_signed:
                return 's'
            else:
                return 'u'

        def padding_item(length):
            fmt = 'p{}'.format(length)
            padding_mask = '1' * length

            return fmt, padding_mask

        def signal_item(signal):
            fmt = '{}{}'.format(get_format_string_type(signal),
                                signal.length)
            padding_mask = '0' * signal.length

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

            for signal in signals:
                if signal.byte_order == 'little_endian':
                    continue

                padding_length = (_start_bit(signal) - start)

                if padding_length > 0:
                    items.append(padding_item(padding_length))

                items.append(signal_item(signal))
                start = (_start_bit(signal) + signal.length)

            if start < message_length:
                length = message_length - start
                items.append(padding_item(length))

            return fmt(items), padding_mask(items)

        def create_little():
            items = [('>', '')]
            end = message_length

            for signal in signals[::-1]:
                if signal.byte_order == 'big_endian':
                    continue

                padding_length = end - (signal.start + signal.length)

                if padding_length > 0:
                    items.append(padding_item(padding_length))

                items.append(signal_item(signal))
                end = signal.start

            if end > 0:
                items.append(padding_item(end))

            value = padding_mask(items)

            if message_length > 0:
                value = bitstruct.pack('u{}'.format(message_length), value)
                value = int(binascii.hexlify(value[::-1]), 16)

            return fmt(items), value

        big_fmt, big_padding_mask = create_big()
        little_fmt, little_padding_mask = create_little()

        Formats = namedtuple('Formats',
                             ['big_endian', 'little_endian', 'padding_mask'])

        return Formats(bitstruct.compile(big_fmt),
                       bitstruct.compile(little_fmt),
                       big_padding_mask & little_padding_mask)

    @property
    def frame_id(self):
        """The message frame id.

        """

        return self._frame_id

    @property
    def is_extended_frame(self):
        """``True`` if the message is an extended frame, ``False`` otherwise.

        """

        return self._is_extended_frame

    @property
    def name(self):
        """The message name as a string.

        """

        return self._name

    @property
    def length(self):
        """The message data length in bytes.

        """

        return self._length

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
    def bus_name(self):
        """The message bus name, or ``None`` if unavailable.

        """

        return self._bus_name

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

    def _get_mux_number(self, decoded, signal_name):
        mux = decoded[signal_name]

        if isinstance(mux, str):
            signal = self.get_signal_by_name(signal_name)
            mux = signal.choice_string_to_number(mux)

        return mux

    def _encode(self, node, data, scaling):
        encoded = _encode_data(data,
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
                    _format_or(multiplexers[signal]),
                    mux))

            mux_encoded, mux_padding_mask = self._encode(node, data, scaling)
            encoded |= mux_encoded
            padding_mask &= mux_padding_mask

        return encoded, padding_mask

    def encode(self, data, scaling=True, padding=False):
        """Encode given data as a message of this type.

        If `scaling` is ``False`` no scaling of signals is performed.

        If `padding` is ``True`` unused bits are encoded as 1.

        >>> foo = db.get_message_by_name('Foo')
        >>> foo.encode({'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        encoded, padding_mask = self._encode(self._codecs, data, scaling)

        if padding:
            encoded |= padding_mask

        encoded |= (0x80 << (8 * self._length))
        encoded = hex(encoded)[4:].rstrip('L')

        return binascii.unhexlify(encoded)[:self._length]

    def _decode(self, node, data, decode_choices, scaling):
        decoded = _decode_data(data,
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
                    _format_or(multiplexers[signal]),
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

    def __repr__(self):
        return "message('{}', 0x{:x}, {}, {}, {})".format(
            self._name,
            self._frame_id,
            self._is_extended_frame,
            self._length,
            "'" + self._comment + "'" if self._comment is not None else None)
