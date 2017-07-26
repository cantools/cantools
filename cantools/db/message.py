# CAN database.

import bitstruct


def _create_message_encode_decode_format(signals):
    fmt = ''
    end = 64

    for signal in signals:
        padding = end - (signal.start + signal.length)

        if padding > 0:
            fmt += 'p{}'.format(padding)

        fmt += '{}{}'.format('s' if signal.is_signed else 'u',
                             signal.length)
        end = signal.start

    return fmt


class Message(object):
    """CAN message.

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
                 is_extended_frame=False):
        self.frame_id = frame_id
        self.is_extended_frame = is_extended_frame
        self.name = name
        self.length = length
        self.signals = signals
        self.signals.sort(key=lambda s: s.start)
        self.signals.reverse()
        self.comment = comment
        self.nodes = nodes
        self.send_type = send_type
        self.cycle_time = cycle_time

        # Message encode/decode format.
        self.fmt = _create_message_encode_decode_format(self.signals)

        # Is it a multiplexed message?
        self.multiplex_selector = None

        for signal in self.signals:
            if signal.is_multiplex_selector:
                self.multiplex_selector = (signal,
                                           _create_message_encode_decode_format(
                                               [signal]))
                break

        # Group signals for each multiplex id.
        self.multiplexed_messages_by_id = {}

        if self.is_multiplexed():
            # Append multiplexed signals.
            for signal in self.signals:
                if signal.multiplex_id is not None:
                    multiplex_id = signal.multiplex_id

                    if multiplex_id not in self.multiplexed_messages_by_id:
                        self.multiplexed_messages_by_id[multiplex_id] = {
                            'signals': [],
                            'fmt': None
                        }

                    self.multiplexed_messages_by_id[multiplex_id]['signals'].append(
                        signal)

            # Append common signals.
            for signal in self.signals:
                if signal.multiplex_id is None:
                    for multiplexed_message in self.multiplexed_messages_by_id.values():
                        multiplexed_message['signals'].append(signal)

            # Sort the signals and create the encode/decode format.
            for message in self.multiplexed_messages_by_id.values():
                message['signals'].sort(key=lambda s: s.start)
                message['signals'].reverse()
                message['fmt'] = _create_message_encode_decode_format(message['signals'])

    def is_multiplexed(self):
        """Returns True if the message is multiplexed, otherwise False.

        """

        return self.multiplex_selector is not None

    def encode(self, data):
        """Encode given data as a message of this type.

        """

        if self.is_multiplexed():
            mux = data[self.multiplex_selector[0].name]

            if mux not in self.multiplexed_messages_by_id:
                raise KeyError('Invalid message multiplex id {}.'.format(mux))

            signals = self.multiplexed_messages_by_id[mux]['signals']
            fmt = self.multiplexed_messages_by_id[mux]['fmt']
        else:
            signals = self.signals
            fmt = self.fmt

        decoded_data = []

        for signal in signals:
            scaled_value = data[signal.name]

            if isinstance(scaled_value, str):
                for choice_number, choice_string in signal.choices.items():
                    if choice_string == scaled_value:
                        scaled_value = choice_number
                        break

            value = int((scaled_value - signal.offset) / signal.scale)
            decoded_data.append(value)

        return bitstruct.pack(fmt, *decoded_data)[::-1]

    def decode(self, data):
        """Decode given data as a message of this type.

        """

        data += b'\x00' * (8 - len(data))

        if self.is_multiplexed():
            mux = bitstruct.unpack(self.multiplex_selector[1],
                                   data[::-1])[0]

            if mux not in self.multiplexed_messages_by_id:
                raise KeyError('Invalid message multiplex id {}.'.format(mux))

            signals = self.multiplexed_messages_by_id[mux]['signals']
            fmt = self.multiplexed_messages_by_id[mux]['fmt']
        else:
            signals = self.signals
            fmt = self.fmt

        unpacked_data = bitstruct.unpack(fmt, data[::-1])
        decoded_signals = {}

        for signal, value in zip(signals, unpacked_data):
            scaled_value = (signal.scale * value + signal.offset)

            try:
                decoded_signals[signal.name] = signal.choices[scaled_value]
            except (KeyError, TypeError):
                decoded_signals[signal.name] = scaled_value

        return decoded_signals

    def get_multiplex_selector_signal_name(self):
        """Returns the message multiplex selector name, or None if the message
        is not multiplexed.

        """

        return self.multiplex_selector[0].name

    def get_multiplexed_message_signals(self, mux):
        """Returns the list of signals for given multiplexed message id.

        """

        return self.multiplexed_messages_by_id[mux]['signals']

    def __repr__(self):
        return "message('{}', 0x{:x}, {}, {}, {})".format(
            self.name,
            self.frame_id,
            self.is_extended_frame,
            self.length,
            "'" + self.comment + "'" if self.comment is not None else None)
