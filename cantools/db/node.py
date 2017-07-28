# A CAN bus node (or Board unit)

class Node(object):
    """An NODE on the CAN bus.

    """

    def __init__(self,
                 name,
                 comment):
        self._name = name
        self._comment = comment

    @property
    def name(self):
        """The node name as a string.

        """

        return self._name

    @property
    def comment(self):
        """The node comment, or ``None`` if unavailable.

        """

        return self._comment

    def __repr__(self):
        return "node('{}', {})".format(
            self._name,
            "'" + self._comment + "'" if self._comment is not None else None)
