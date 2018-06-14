class AttributeDefinition(object):
    """A definition of an attribute that can be associated with attributes in nodes/messages/signals

    """

    def __init__(self,
                 name,
                 defaultValue=None,
                 kind=None,
                 type_=None,
                 minimum=None,
                 maximum=None,
                 choices=None):
        self._name = name
        self._defaultValue = defaultValue
        self._kind=kind
        self._type_ = type_
        self._minimum=minimum
        self._maximum=maximum
        self._choices = choices

    @property
    def name(self):
        """The attribute name as a string.

        """

        return self._name

    @property
    def defaultValue(self):
        """The default value that this attribute has, or ``None`` if unavailable.

        """

        return self._defaultValue

    @defaultValue.setter
    def defaultValue(self, value):
        self._defaultValue = value

    @property
    def kind(self):
        """The attribute kind (BU_, BO_, SG_), or ``None`` if unavailable.

        """

        return self._kind

    @property
    def type_(self):
        """The attribute type (INT, HEX, FLOAT, STRING, ENUM), or ``None`` if unavailable.

        """

        return self._type_

    @property
    def minimum(self):
        """The minimum value of the attribute, or ``None`` if unavailable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value):
        self._minimum = value

    @property
    def maximum(self):
        """The maximum value of the attribute, or ``None`` if unavailable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value):
        self._maximum = value

    @property    
    def choices(self):
        """A dictionary mapping attribute values to enumerated choices, or
        ``None`` if unavailable.

        """

        return self._choices

    @choices.setter
    def choices(self, value):
        self._choices = value

    def __repr__(self):
        return "attribute_definition('{}', {})".format(
            self._name,
            self._defaultValue)
