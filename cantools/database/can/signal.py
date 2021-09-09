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

class NamedSignalValue(object):
    """Represents a named value of a signal.

    Named values map an integer number to a human-readable
    string. Some file formats like ARXML support specifying
    descriptions for the named value.
    """

    def __init__(self, value, name, comments = {}):
        self._name = name
        self._value = value
        self._comments = comments

    @property
    def name(self):
        """The text intended for human consumption which the specified integer
        is mapped to.
        """

        return self._name

    @property
    def value(self):
        """The integer value that gets mapped
        """

        return self._value

    @property
    def comments(self):
        """The descriptions of the named value

        This is a dictionary containing the descriptions in multiple
        languagues. The dictionary is indexed by the language.

        Example:

        .. code:: text

          # retrieve the English comment of the named value or an empty
          # string if none was specified.
          named_value.comments.get("EN", "")

        """

        return self._comments

    def __str__(self):
        return self._name

    def __repr__(self):
        return f"{self._name}"

    def __eq__(self, x):
        if isinstance(x, NamedSignalValue):
            return \
                x.value == self.value \
                and x.name == self.name \
                and x.comments == self.comments
        elif isinstance(x, str):
            return x == self.name

        return False

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
                 decimal=None,
                 spn=None):
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

        # if the 'comment' argument is a string, we assume that is an
        # english comment. this is slightly hacky because the
        # function's behavior depends on the type of the passed
        # argument, but it is quite convenient...
        if isinstance(comment, str):
            # use the first comment in the dictionary as "The" comment
            self._comments = { None: comment }
        else:
            # assume that we have either no comment at all or a
            # multi-lingual dictionary
            self._comments = comment

        self._receivers = [] if receivers is None else receivers
        self._is_multiplexer = is_multiplexer
        self._multiplexer_ids = multiplexer_ids
        self._multiplexer_signal = multiplexer_signal
        self._is_float = is_float
        self._spn = spn

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
        """The start bit position of the signal within its message as it is saved in the dbc file.

        lsb in case of little_endian,
        msb in case of big_endian.
        """

        return self._start

    @start.setter
    def start(self, value):
        self._start = value

    @property
    def lsb(self):
        """The Startbit as displayed in Vector CANdb++.

        The position of the least significant bit of this signal.
        This is equivalent to start iff byte_order == little_endian.
        """

        if self.byte_order == 'big_endian':
            i = self.start
            for j in range(self.length-1):
                if i % 8 == 0:
                    i += 16
                i -= 1
            return i
        elif self.byte_order == 'little_endian':
            return self.start
        else:
            raise ValueError("invalid value for byte_order: %s" % self.byte_order)


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

        Note that we implicitly try to return the comment's language
        to be English comment if multiple languages were specified.
    
        """
        if self._comments is None:
            return None
        elif self._comments.get(None) is not None:
            return self._comments.get(None)

        return self._comments.get('EN', None)

    @property
    def comments(self):
        """The dictionary with the descriptions of the signal in multiple
        languages. ``None`` if unavailable.

        """
        return self._comments

    @comment.setter
    def comment(self, value):
        self._comments = { None: value }

    @comments.setter
    def comments(self, value):
        self._comments = value

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

    @property
    def spn(self):
        """The J1939 Suspect Parameter Number (SPN) value if the signal
        has this attribute, ``None`` otherwise.
        
        """

        return self._spn

    @spn.setter
    def spn(self, value):
        self._spn = value

    def choice_string_to_number(self, string):
        for choice_number, choice_value in self.choices.items():
            if str(choice_value) == str(string):
                return choice_number

    def __repr__(self):
        if self._choices is None:
            choices = None
        else:
            choices = '{{{}}}'.format(', '.join(
                ["{}: '{}'".format(value, text)
                 for value, text in self._choices.items()]))

        return "signal('{}', {}, {}, '{}', {}, {}, {}, {}, {}, {}, '{}', {}, {}, {}, {}, {})".format(
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
            self._spn,
            self._comments)
