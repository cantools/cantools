# A CAN bus node (or Board unit)

class Node(object):
    """An NODE on the CAN bus.

    """

    def __init__(self,
                 name,
                 comment):
        self.name = name
        self.comment = comment

    def __repr__(self):
        return "node('{}', {})".format(
            self.name,
            "'" + self.comment + "'" if self.comment is not None else None)
