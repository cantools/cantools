# A CAN signal.

class Decimal(object):
    """Holds the same values as
    :attr:`~cantools.database.can.Signal.scale`,
    :attr:`~cantools.database.can.Signal.offset`,
    :attr:`~cantools.database.can.Signal.minimum` and
    :attr:`~cantools.database.can.Signal.maximum`, but as
    ``decimal.Decimal`` instead of ``int`` and ``float`` for higher
    precision (no rounding errors).

    """

    def __init__(self, scale=None, offset=None, minimum=None, maximum=None):
        self._scale = scale
        self._offset = offset
        self._minimum = minimum
        self._maximum = maximum

    @property
    def scale(self):
        """The scale factor of the signal value as ``decimal.Decimal``.

        """

        return self._scale

    @scale.setter
    def scale(self, value):
        self._scale = value

    @property
    def offset(self):
        """The offset of the signal value as ``decimal.Decimal``.

        """

        return self._offset

    @offset.setter
    def offset(self, value):
        self._offset = value

    @property
    def minimum(self):
        """The minimum value of the signal as ``decimal.Decimal``, or ``None``
        if unavailable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value):
        self._minimum = value

    @property
    def maximum(self):
        """The maximum value of the signal as ``decimal.Decimal``, or ``None``
        if unavailable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value):
        self._maximum = value


class Signal(object):
    """A CAN signal with position, size, unit and other information. A
    signal is part of a message.

    Signal bit numbering in a message:

    .. code:: text

       Byte:       0        1        2        3        4        5        6        7
              +--------+--------+--------+--------+--------+--------+--------+--------+--- - -
              |        |        |        |        |        |        |        |        |
              +--------+--------+--------+--------+--------+--------+--------+--------+--- - -
       Bit:    7      0 15     8 23    16 31    24 39    32 47    40 55    48 63    56

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
                 initial=None,
                 scale=1,
                 offset=0,
                 minimum=None,
                 maximum=None,
                 unit=None,
                 choices=None,
                 dbc_specifics=None,
                 comment=None,
                 receivers=None,
                 is_multiplexer=False,
                 multiplexer_ids=None,
                 multiplexer_signal=None,
                 is_float=False,
                 decimal=None):
        self._name = name
        self._start = start
        self._length = length
        self._byte_order = byte_order
        self._is_signed = is_signed
        self._initial = initial
        self._scale = scale
        self._offset = offset
        self._minimum = minimum
        self._maximum = maximum
        self._decimal = Decimal() if decimal is None else decimal
        self._unit = unit
        self._choices = choices
        self._dbc = dbc_specifics
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

    @name.setter
    def name(self, value):
        self._name = value

    @property
    def start(self):
        """The start bit position of the signal within its message.

        """

        return self._start

    @start.setter
    def start(self, value):
        self._start = value

    @property
    def length(self):
        """The length of the signal in bits.

        """

        return self._length

    @length.setter
    def length(self, value):
        self._length = value

    @property
    def byte_order(self):
        """Signal byte order as ``'little_endian'`` or ``'big_endian'``.

        """

        return self._byte_order

    @byte_order.setter
    def byte_order(self, value):
        self._byte_order = value

    @property
    def is_signed(self):
        """``True`` if the signal is signed, ``False`` otherwise. Ignore this
           attribute if :data:`~cantools.db.Signal.is_float` is
           ``True``.

        """

        return self._is_signed

    @is_signed.setter
    def is_signed(self, value):
        self._is_signed = value

    @property
    def is_float(self):
        """``True`` if the signal is a float, ``False`` otherwise.

        """

        return self._is_float

    @is_float.setter
    def is_float(self, value):
        self._is_float = value

    @property
    def initial(self):
        """The initial value of the signal, or ``None`` if unavailable.

        """

        return self._initial

    @initial.setter
    def initial(self, value):
        self._initial = value

    @property
    def scale(self):
        """The scale factor of the signal value.

        """

        return self._scale

    @scale.setter
    def scale(self, value):
        self._scale = value

    @property
    def offset(self):
        """The offset of the signal value.

        """

        return self._offset

    @offset.setter
    def offset(self, value):
        self._offset = value

    @property
    def minimum(self):
        """The minimum value of the signal, or ``None`` if unavailable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value):
        self._minimum = value

    @property
    def maximum(self):
        """The maximum value of the signal, or ``None`` if unavailable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value):
        self._maximum = value

    @property
    def decimal(self):
        """The high precision values of
        :attr:`~cantools.database.can.Signal.scale`,
        :attr:`~cantools.database.can.Signal.offset`,
        :attr:`~cantools.database.can.Signal.minimum` and
        :attr:`~cantools.database.can.Signal.maximum`.

        See :class:`~cantools.database.can.signal.Decimal` for more
        details.

        """

        return self._decimal

    @property
    def unit(self):
        """The unit of the signal as a string, or ``None`` if unavailable.

        """

        return self._unit

    @unit.setter
    def unit(self, value):
        self._unit = value

    @property
    def choices(self):
        """A dictionary mapping signal values to enumerated choices, or
        ``None`` if unavailable.

        """

        return self._choices

    @property
    def dbc(self):
        """An object containing dbc specific properties like e.g. attributes.

        """

        return self._dbc

    @dbc.setter
    def dbc(self, value):
        self._dbc = value

    @property
    def comment(self):
        """The signal comment, or ``None`` if unavailable.

        """

        return self._comment

    @comment.setter
    def comment(self, value):
        self._comment = value

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

    @is_multiplexer.setter
    def is_multiplexer(self, value):
        self._is_multiplexer = value

    @property
    def multiplexer_ids(self):
        """The multiplexer ids list if the signal is part of a multiplexed
        message, ``None`` otherwise.

        """

        return self._multiplexer_ids

    @multiplexer_ids.setter
    def multiplexer_ids(self, value):
        self._multiplexer_ids = value

    @property
    def multiplexer_signal(self):
        """The multiplexer signal if the signal is part of a multiplexed
        message, ``None`` otherwise.

        """

        return self._multiplexer_signal

    @multiplexer_signal.setter
    def multiplexer_signal(self, value):
        self._multiplexer_signal = value

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

        return "signal('{}', {}, {}, '{}', {}, {}, {}, {}, {}, {}, '{}', {}, {}, {}, {})".format(
            self._name,
            self._start,
            self._length,
            self._byte_order,
            self._is_signed,
            self._initial,
            self._scale,
            self._offset,
            self._minimum,
            self._maximum,
            self._unit,
            self._is_multiplexer,
            self._multiplexer_ids,
            choices,
            "'" + self._comment + "'" if self._comment is not None else None)
