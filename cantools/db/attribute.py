

class Attribute(object):
    """An atttribute that can be associated with nodes/messages/signals

    """

    def __init__(self,
                 name,
                 value,
                 type_=None,
                 owner=None,
                 signal_name=None):
        self._name = name
        self._value = value
        self._type_ = type_
        self._owner = owner
        self._signal_name = signal_name

    @property
    def name(self):
        """The attribute name as a string.

        """

        return self._name

    @property
    def value(self):
        """The value that this attribute has.

        """

        return self._value

    @property
    def type_(self):
        """The attribute type (either BU_ or BO_ or SG_), or ``None`` if unavailable.

        """

        return self._type_

    @property
    def owner(self):
        """The owner owning this attribute (either node name or frame id) or ``None`` if unavailable.

        """

        return self._owner

    @property
    def signal_name(self):
        """The signal name that this attribute is associated with or ``None`` if unavailable.

        """

        return self._signal_name

    def __repr__(self):
        return "attribute('{}', {})".format(
            self._name,
            self._value)
