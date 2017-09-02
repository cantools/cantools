# CAN database.

import struct
import bitstruct


def _unscale_value(signal, data):
    scaled_value = data[signal.name]

    if isinstance(scaled_value, str):
        for choice_number, choice_string in signal.choices.items():
            if choice_string == scaled_value:
                scaled_value = choice_number
                break

    return int((scaled_value - signal.offset) / signal.scale)


def _decode_signal(signal, value, decode_choices):
    scaled_value = (signal.scale * value + signal.offset)

    if decode_choices:
        try:
            decoded_signal = signal.choices[scaled_value]
        except (KeyError, TypeError):
            decoded_signal = scaled_value
    else:
        decoded_signal = scaled_value

    return decoded_signal


def _encode_data(data, signals, formats):
    big_decoded_data = []
    little_decoded_data = []

    for signal in signals:
        if signal.byte_order == 'little_endian':
            continue

        big_decoded_data.append(_unscale_value(signal, data))

    for signal in signals[::-1]:
        if signal.byte_order == 'big_endian':
            continue

        little_decoded_data.append(_unscale_value(signal, data))

    big_packed = bitstruct.pack(formats[0], *big_decoded_data)
    little_packed = bitstruct.pack(formats[1], *little_decoded_data)[::-1]
    packed_union = struct.unpack('>Q', big_packed)[0]
    packed_union |= struct.unpack('>Q', little_packed)[0]

    return struct.pack('>Q', packed_union)


def _decode_data(data, signals, formats, decode_choices):
    decoded_signals = {}

    # Big endian signals.
    big_unpacked = list(bitstruct.unpack(formats[0], data))

    for signal in signals:
        if signal.byte_order == 'little_endian':
            continue

        decoded_signals[signal.name] = _decode_signal(signal,
                                                      big_unpacked.pop(0),
                                                      decode_choices)

    # Little endian signals.
    little_unpacked = list(bitstruct.unpack(formats[1], data[::-1])[::-1])

    for signal in signals:
        if signal.byte_order == 'big_endian':
            continue

        decoded_signals[signal.name] = _decode_signal(signal,
                                                      little_unpacked.pop(0),
                                                      decode_choices)

    return decoded_signals


def _create_message_encode_decode_formats(signals):
    # Big endian byte order format.
    big_fmt = ''
    start = 0

    for signal in signals:
        if signal.byte_order == 'little_endian':
            continue

        padding = (signal.start - start)

        if padding > 0:
            big_fmt += 'p{}'.format(padding)

        big_fmt += '{}{}'.format('s' if signal.is_signed else 'u',
                                 signal.length)
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

        little_fmt += '{}{}'.format('s' if signal.is_signed else 'u',
                                    signal.length)
        end = signal.start

    if end > 0:
        little_fmt += 'p{}'.format(end)

    return big_fmt, little_fmt


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
            if signal.is_multiplexer:
                self._multiplexer = (signal,
                                     _create_message_encode_decode_formats(
                                         [signal]))
                break

        # Group signals for each multiplex id.
        self._multiplexer_message_by_id = {}

        if self.is_multiplexed():
            # Append multiplexed signals.
            for signal in self._signals:
                if signal.multiplexer_id is not None:
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
                if signal.multiplexer_id is None:
                    for message in self._multiplexer_message_by_id.values():
                        message['signals'].append(signal)

            # Sort the signals and create the encode/decode format.
            for message in self._multiplexer_message_by_id.values():
                message['signals'].sort(key=lambda s: s.start)
                message['formats'] = _create_message_encode_decode_formats(message['signals'])

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

    def encode(self, data):
        """Encode given data as a message of this type.

        >>> foo = db.messages[0]
        >>> foo.encode({'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        if self.is_multiplexed():
            mux = data[self._multiplexer[0].name]

            if mux not in self._multiplexer_message_by_id:
                raise KeyError('Invalid multiplex message id {}.'.format(mux))

            signals = self._multiplexer_message_by_id[mux]['signals']
            formats = self._multiplexer_message_by_id[mux]['formats']
        else:
            signals = self._signals
            formats = self._formats

        return _encode_data(data, signals, formats)

    def decode(self, data, decode_choices=True):
        """Decode given data as a message of this type.

        If `decode_choices` is ``False`` scaled values are not
        converted to choice strings (if available).

        >>> foo = db.messages[0]
        >>> foo.decode(b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}

        """

        data += b'\x00' * (8 - len(data))

        if self.is_multiplexed():
            mux = _decode_data(data,
                               [self._multiplexer[0]],
                               self._multiplexer[1],
                               False)[self._multiplexer[0].name]

            if mux not in self._multiplexer_message_by_id:
                raise KeyError('Invalid multiplex message id {}.'.format(mux))

            signals = self._multiplexer_message_by_id[mux]['signals']
            formats = self._multiplexer_message_by_id[mux]['formats']
        else:
            signals = self._signals
            formats = self._formats

        return _decode_data(data, signals, formats, decode_choices)

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

        return self._multiplexer[0].name

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
