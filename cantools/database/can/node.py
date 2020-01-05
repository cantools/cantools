# A CAN bus node (or Board unit)

class Node(object):
    """An NODE on the CAN bus.

    """

    def __init__(self,
                 name,
                 comment,
                 dbc_specifics=None):
        self._name = name
        self._comment = comment
        self._dbc = dbc_specifics

    @property
    def name(self):
        """The node name as a string.

        """

        return self._name

    @name.setter
    def name(self, value):
        self._name = value

    @property
    def comment(self):
        """The node comment, or ``None`` if unavailable.

        """

        return self._comment

    @comment.setter
    def comment(self, value):
        self._comment = value

    @property
    def dbc(self):
        """An object containing dbc specific properties like e.g. attributes.

        """

        return self._dbc

    @dbc.setter
    def dbc(self, value):
        self._dbc = value

    def __repr__(self):
        return "node('{}', {})".format(
            self._name,
            "'" + self._comment + "'" if self._comment is not None else None)
