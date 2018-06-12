# Internal CAN database.

class InternalDatabase(object):
    """Internal CAN database.

    """

    def __init__(self,
                 messages,
                 nodes,
                 buses,
                 version,
                 attributes=None,
                 attribute_definitions=None,
                 attribute_definition_defaults=None):
        self.messages = messages
        self.nodes = nodes
        self.buses = buses
        self.version = version

        if attributes is None:
            attributes = []

        self.attributes = attributes

        if attribute_definitions is None:
            attribute_definitions = []

        self.attribute_definitions = attribute_definitions

        if attribute_definition_defaults is None:
            attribute_definition_defaults = {}

        self.attribute_definition_defaults = attribute_definition_defaults
