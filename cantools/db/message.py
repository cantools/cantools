# CAN database.

import struct
import bitstruct
from collections import namedtuple


def _encode_signal(signal, data, scaling):
    value = data[signal.name]

    if isinstance(value, str):
        for choice_number, choice_string in signal.choices.items():
            if choice_string == value:
                value = choice_number
                break

    if scaling:
        if signal.is_float:
            return (value - signal.offset) / signal.scale
        else:
            return int((value - signal.offset) / signal.scale)
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


def _encode_data(data, signals, formats, scaling, length):
    big_unpacked_data = [
        _encode_signal(signal, data, scaling)
        for signal in signals
        if signal.byte_order == 'big_endian'
    ]
    little_unpacked_data = [
        _encode_signal(signal, data, scaling)
        for signal in signals[::-1]
        if signal.byte_order == 'little_endian'
    ]
    big_packed = bitstruct.pack(formats.big_endian, *big_unpacked_data)
    little_packed = bitstruct.pack(formats.little_endian,
                                   *little_unpacked_data)[::-1]
    packed_union = struct.unpack('>Q', big_packed)[0]
    packed_union |= struct.unpack('>Q', little_packed)[0]

    return struct.pack('>Q', packed_union)[:length]


def _decode_data(data, signals, formats, decode_choices, scaling):
    big_unpacked = list(bitstruct.unpack(formats.big_endian, data))
    big_signals = [
        signal
        for signal in signals
        if signal.byte_order == 'big_endian'
    ]
    little_unpacked = list(bitstruct.unpack(formats.little_endian, data[::-1])[::-1])
    little_signals = [
        signal
        for signal in signals
        if signal.byte_order == 'little_endian'
    ]
    unpacked = big_unpacked + little_unpacked
    signals = big_signals + little_signals

    return {
        signal.name: _decode_signal(signal,
                                    unpacked.pop(0),
                                    decode_choices,
                                    scaling)
        for signal in signals
    }


def _create_message_encode_decode_formats(signals):
    # Big endian byte order format.
    big_fmt = ''
    start = 0

    def get_format_string_type(signal):
        if signal.is_float:
            return 'f'
        elif signal.is_signed:
            return 's'
        else:
            return 'u'

    for signal in signals:
        if signal.byte_order == 'little_endian':
            continue

        padding = (signal.start - start)

        if padding > 0:
            big_fmt += 'p{}'.format(padding)

        big_fmt += '{}{}'.format(get_format_string_type(signal), signal.length)
        start = (signal.start + signal.length)

    if start < 64:
        big_fmt += 'p{}'.format(64 - start)

    # Little endian byte order format.
    little_fmt = ''
    end = 64

    for signal in signals[::-1]:
        if signal.byte_order == 'big_endian':
            continue

        padding = end - (signal.start + signal.length)

        if padding > 0:
            little_fmt += 'p{}'.format(padding)

        little_fmt += '{}{}'.format(get_format_string_type(signal), signal.length)
        end = signal.start

    if end > 0:
        little_fmt += 'p{}'.format(end)

    Formats = namedtuple('Formats', ['big_endian', 'little_endian'])

    return Formats(big_fmt, little_fmt)


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
                 nodes=None,
                 send_type=None,
                 cycle_time=None,
                 is_extended_frame=False,
                 bus_name=None):
        self._frame_id = frame_id
        self._is_extended_frame = is_extended_frame
        self._name = name
        self._length = length
        self._signals = signals
        self._signals.sort(key=lambda s: s.start)
        self._comment = comment
        self._nodes = nodes
        self._send_type = send_type
        self._cycle_time = cycle_time
        self._bus_name = bus_name

        # Message encode/decode format.
        self._formats = _create_message_encode_decode_formats(self._signals)

        # Is it a multiplexed message?
        self._multiplexer = None

        for signal in self._signals:
            if not signal.is_multiplexer:
                continue

            Multiplexer = namedtuple('Multiplexer', ['signal', 'formats'])
            formats = _create_message_encode_decode_formats([signal])
            self._multiplexer = Multiplexer(signal, formats)
            break

        # Group signals for each multiplex id.
        self._multiplexer_message_by_id = {}

        if self.is_multiplexed():
            # Append multiplexed signals.
            for signal in self._signals:
                if signal.multiplexer_id is None:
                    continue

                multiplexer_id = signal.multiplexer_id

                if multiplexer_id not in self._multiplexer_message_by_id:
                    self._multiplexer_message_by_id[multiplexer_id] = {
                        'signals': [],
                        'formats': None
                    }

                self._multiplexer_message_by_id[multiplexer_id]['signals'].append(
                    signal)

            # Append common signals.
            for signal in self._signals:
                if signal.multiplexer_id is not None:
                    continue

                for message in self._multiplexer_message_by_id.values():
                    message['signals'].append(signal)

            # Sort the signals and create the encode/decode format.
            for message in self._multiplexer_message_by_id.values():
                message['signals'].sort(key=lambda s: s.start)
                formats = _create_message_encode_decode_formats(message['signals'])
                message['formats'] = formats

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
    def nodes(self):
        """A list of all message node names.

        """

        return self._nodes

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

    def encode(self, data, scaling=True):
        """Encode given data as a message of this type.

        If `scaling` is ``False`` no scaling of signals is performed.

        >>> foo = db.messages[0]
        >>> foo.encode({'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        if self.is_multiplexed():
            mux = data[self._multiplexer.signal.name]

            try:
                multiplexer = self._multiplexer_message_by_id[mux]
            except KeyError:
                raise KeyError('Invalid multiplex message id {}.'.format(mux))

            signals = multiplexer['signals']
            formats = multiplexer['formats']
        else:
            signals = self._signals
            formats = self._formats

        return _encode_data(data, signals, formats, scaling, self._length)

    def decode(self, data, decode_choices=True, scaling=True):
        """Decode given data as a message of this type.

        If `decode_choices` is ``False`` scaled values are not
        converted to choice strings (if available).

        If `scaling` is ``False`` no scaling of signals is performed.

        >>> foo = db.messages[0]
        >>> foo.decode(b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}

        """

        data += b'\x00' * (8 - len(data))

        if self.is_multiplexed():
            mux = _decode_data(data,
                               [self._multiplexer.signal],
                               self._multiplexer.formats,
                               False,
                               False)[self._multiplexer.signal.name]

            try:
                multiplexer = self._multiplexer_message_by_id[mux]
            except KeyError:
                raise KeyError('Invalid multiplex message id {}.'.format(mux))

            signals = multiplexer['signals']
            formats = multiplexer['formats']
        else:
            signals = self._signals
            formats = self._formats

        return _decode_data(data, signals, formats, decode_choices, scaling)

    def is_multiplexed(self):
        """Returns ``True`` if the message is multiplexed, otherwise
        ``False``.

        >>> foo = db.messages[0]
        >>> foo.is_multiplexed()
        False
        >>> bar = db.messages[1]
        >>> bar.is_multiplexed()
        True

        """

        return self._multiplexer is not None

    def get_multiplexer_signal_name(self):
        """Returns the message multiplexer signal name, or raises an exception
        if the message is not multiplexed.

        >>> bar = db.messages[1]
        >>> bar.get_multiplexer_signal_name()
        'BarMux'

        """

        return self._multiplexer.signal.name

    def get_signals_by_multiplexer_id(self, multiplexer_id):
        """Returns a list of signals for given multiplexer message id, or
        raises an exception if the message is not multiplexed or an
        invalid multiplexer id is given.

        >>> bar = db.messages[1]
        >>> bar.get_signals_by_multiplexer_id(0)
        [signal(...), signal(...), ...]

        """

        return self._multiplexer_message_by_id[multiplexer_id]['signals']

    def __repr__(self):
        return "message('{}', 0x{:x}, {}, {}, {})".format(
            self._name,
            self._frame_id,
            self._is_extended_frame,
            self._length,
            "'" + self._comment + "'" if self._comment is not None else None)
