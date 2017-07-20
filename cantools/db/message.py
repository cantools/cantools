# CAN database.

import bitstruct


class Message(object):
    """CAN message.

    """

    class Signals(object):

        def __init__(self, message, *args, **kwargs):
            self.message = message

            if args:
                for signal, value in zip(message.signals, args):
                    setattr(self, signal.name, value)

            self.__dict__.update(kwargs)

        def __str__(self):
            signals = []

            for signal in self.message.signals:
                unit = ' ' + signal.unit if signal.unit else ''
                value = getattr(self, signal.name)
                signals.append(
                    '{name}: {value}{unit}'.format(name=signal.name,
                                                   value=value,
                                                   unit=unit))

            return '{}({})'.format(self.message.name, ', '.join(signals))

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
        self.fmt = ''
        end = 64

        for signal in signals:
            padding = end - (signal.start + signal.length)

            if padding > 0:
                self.fmt += 'p{}'.format(padding)

            self.fmt += '{}{}'.format('s' if signal.is_signed else 'u',
                                      signal.length)
            end = signal.start

    def encode(self, data):
        """Encode given data as a message of this type.

        """

        if isinstance(data, dict):
            data = self.Signals(self, **data)

        return bitstruct.pack(self.fmt,
                              *[int((getattr(data, signal.name)
                                     - signal.offset) / signal.scale)
                                for signal in self.signals])[::-1]

    def decode(self, data):
        """Decode given data as a message of this type.

        """

        data += b'\x00' * (8 - len(data))
        unpacked_data = bitstruct.unpack(self.fmt, data[::-1])
        signals = []

        for signal, value in zip(self.signals, unpacked_data):
            scaled_value = signal.scale * value + signal.offset

            try:
                signals.append(signal.choices[scaled_value])
            except (KeyError, TypeError):
                signals.append(scaled_value)

        return self.Signals(self, *signals)

    def __repr__(self):
        return "message('{}', 0x{:x}, {}, {}, {})".format(
            self.name,
            self.frame_id,
            self.is_extended_frame,
            self.length,
            "'" + self.comment + "'" if self.comment is not None else None)
