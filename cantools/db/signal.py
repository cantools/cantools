# A CAN signal.

class Signal(object):
    """A CAN signal.

    """

    def __init__(self,
                 name,
                 start,
                 length,
                 byte_order,
                 is_signed,
                 scale,
                 offset,
                 minimum,
                 maximum,
                 unit,
                 choices,
                 comment,
                 nodes=None):
        self.name = name
        self.start = start
        self.length = length
        self.byte_order = byte_order
        self.is_signed = is_signed
        self.scale = scale
        self.offset = offset
        self.minimum = minimum
        self.maximum = maximum
        self.unit = unit
        self.choices = choices
        self.comment = comment
        self.nodes = nodes

    def __repr__(self):
        if self.choices is None:
            choices = None
        else:
            choices = '{{{}}}'.format(', '.join(
                ["{}: '{}'".format(value, text)
                 for value, text in self.choices.items()]))

        return "signal('{}', {}, {}, '{}', {}, {}, {}, {}, {}, '{}', {}, {})".format(
            self.name,
            self.start,
            self.length,
            self.byte_order,
            self.is_signed,
            self.scale,
            self.offset,
            self.minimum,
            self.maximum,
            self.unit,
            choices,
            "'" + self.comment + "'" if self.comment is not None else None)
