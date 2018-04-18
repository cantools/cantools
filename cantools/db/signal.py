# A CAN signal.

class Signal(object):
    """A CAN signal with position, size, unit and other information. A
    signal is part of a message.

    Signal bit numbering in a message:

    .. code:: text

       Byte:       0        1        2        3
              +--------+--------+--------+--- - -
              |        |        |        |
              +--------+--------+--------+--- - -
       Bit:    7      0 15     8 23    16 31

    Big endian signal with start bit 2 and length 5 (0=LSB, 4=MSB):

    .. code:: text

       Byte:       0        1        2        3
              +--------+--------+--------+--- - -
              |    |432|10|     |        |
              +--------+--------+--------+--- - -
       Bit:    7      0 15     8 23    16 31

    Little endian signal with start bit 2 and length 9 (0=LSB, 8=MSB):

    .. code:: text

       Byte:       0        1        2        3
              +--------+--------+--------+--- - -
              |543210| |    |876|        |
              +--------+--------+--------+--- - -
       Bit:    7      0 15     8 23    16 31

    """

    def __init__(self,
                 name,
                 start,
                 length,
                 byte_order='little_endian',
                 is_signed=False,
                 scale=1,
                 offset=0,
                 minimum=None,
                 maximum=None,
                 unit=None,
                 choices=None,
                 comment=None,
                 receivers=None,
                 is_multiplexer=False,
                 multiplexer_ids=None,
                 multiplexer_signal=None,
                 is_float=False):
        self._name = name
        self._start = start
        self._length = length
        self._byte_order = byte_order
        self._is_signed = is_signed
        self._scale = scale
        self._offset = offset
        self._minimum = minimum
        self._maximum = maximum
        self._unit = unit
        self._choices = choices
        self._comment = comment
        self._receivers = [] if receivers is None else receivers
        self._is_multiplexer = is_multiplexer
        self._multiplexer_ids = multiplexer_ids
        self._multiplexer_signal = multiplexer_signal
        self._is_float = is_float

    @property
    def name(self):
        """The signal name as a string.

        """

        return self._name

    @property
    def start(self):
        """The start bit position of the signal within its message.

        """

        return self._start

    @property
    def length(self):
        """The length of the signal in bits.

        """

        return self._length

    @property
    def byte_order(self):
        """Signal byte order as ``'little_endian'`` or ``'big_endian'``.

        """

        return self._byte_order

    @property
    def is_signed(self):
        """``True`` if the signal is signed, ``False`` otherwise. Ignore this
           attribute if :data:`~cantools.db.Signal.is_float` is
           ``True``.

        """

        return self._is_signed

    @property
    def is_float(self):
        """``True`` if the signal is a float, ``False`` otherwise.

        """

        return self._is_float

    @property
    def scale(self):
        """The scale factor of the signal value.

        """

        return self._scale

    @property
    def offset(self):
        """The offset of the signal value.

        """

        return self._offset

    @property
    def minimum(self):
        """The minimum value of the signal, or ``None`` if unavailable.

        """

        return self._minimum

    @property
    def maximum(self):
        """The maximum value of the signal, or ``None`` if unavailable.

        """

        return self._maximum

    @property
    def unit(self):
        """The unit of the signal as a string, or ``None`` if unavailable.

        """

        return self._unit

    @property
    def choices(self):
        """A dictionary mapping signal values to enumerated choices, or
        ``None`` if unavailable.

        """

        return self._choices

    @property
    def comment(self):
        """The signal comment, or ``None`` if unavailable.

        """

        return self._comment

    @property
    def receivers(self):
        """A list of all receiver nodes of this signal.

        """

        return self._receivers

    @property
    def is_multiplexer(self):
        """``True`` if this is the multiplexer signal in a message, ``False``
        otherwise.

        """

        return self._is_multiplexer

    @property
    def multiplexer_ids(self):
        """The multiplexer ids list if the signal is part of a multiplexed
        message, ``None`` otherwise.

        """

        return self._multiplexer_ids

    @property
    def multiplexer_signal(self):
        """The multiplexer signal if the signal is part of a multiplexed
        message, ``None`` otherwise.

        """

        return self._multiplexer_signal

    def choice_string_to_number(self, string):
        for choice_number, choice_string in self.choices.items():
            if choice_string == string:
                return choice_number

    def __repr__(self):
        if self._choices is None:
            choices = None
        else:
            choices = '{{{}}}'.format(', '.join(
                ["{}: '{}'".format(value, text)
                 for value, text in self._choices.items()]))

        return "signal('{}', {}, {}, '{}', {}, {}, {}, {}, {}, '{}', {}, {}, {}, {})".format(
            self._name,
            self._start,
            self._length,
            self._byte_order,
            self._is_signed,
            self._scale,
            self._offset,
            self._minimum,
            self._maximum,
            self._unit,
            self._is_multiplexer,
            self._multiplexer_ids,
            choices,
            "'" + self._comment + "'" if self._comment is not None else None)
