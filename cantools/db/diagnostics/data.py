# DID data.

class Data(object):
    """A data data with position, size, unit and other information. A data
    is part of a DID.

    """

    def __init__(self,
                 name,
                 start,
                 length,
                 byte_order='little_endian',
                 scale=1,
                 offset=0,
                 minimum=None,
                 maximum=None,
                 unit=None,
                 choices=None):
        self._name = name
        self._start = start
        self._length = length
        self._byte_order = byte_order
        self._scale = scale
        self._offset = offset
        self._minimum = minimum
        self._maximum = maximum
        self._unit = unit
        self._choices = choices
        # ToDo: Remove once types are handled properly.
        self.is_float = False
        self.is_signed = False

    @property
    def name(self):
        """The data name as a string.

        """

        return self._name

    @name.setter
    def name(self, value):
        self._name = value

    @property
    def start(self):
        """The start bit position of the data within its DID.

        """

        return self._start

    @start.setter
    def start(self, value):
        self._start = value

    @property
    def length(self):
        """The length of the data in bits.

        """

        return self._length

    @length.setter
    def length(self, value):
        self._length = value

    @property
    def byte_order(self):
        """Data byte order as ``'little_endian'`` or ``'big_endian'``.

        """

        return self._byte_order

    @byte_order.setter
    def byte_order(self, value):
        self._byte_order = value

    @property
    def scale(self):
        """The scale factor of the data value.

        """

        return self._scale

    @scale.setter
    def scale(self, value):
        self._scale = value

    @property
    def offset(self):
        """The offset of the data value.

        """

        return self._offset

    @offset.setter
    def offset(self, value):
        self._offset = value

    @property
    def minimum(self):
        """The minimum value of the data, or ``None`` if unavailable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value):
        self._minimum = value

    @property
    def maximum(self):
        """The maximum value of the data, or ``None`` if unavailable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value):
        self._maximum = value

    @property
    def unit(self):
        """The unit of the data as a string, or ``None`` if unavailable.

        """

        return self._unit

    @unit.setter
    def unit(self, value):
        self._unit = value

    @property
    def choices(self):
        """A dictionary mapping data values to enumerated choices, or ``None``
        if unavailable.

        """

        return self._choices

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

        return "data('{}', {}, {}, '{}', {}, {}, {}, {}, '{}', {})".format(
            self._name,
            self._start,
            self._length,
            self._byte_order,
            self._scale,
            self._offset,
            self._minimum,
            self._maximum,
            self._unit,
            choices)
