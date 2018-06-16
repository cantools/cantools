# Internal CAN database.

class InternalDatabase(object):
    """Internal CAN database.

    """

    def __init__(self,
                 messages,
                 nodes,
                 buses,
                 version,
                 dbc_specifics=None):
        self.messages = messages
        self.nodes = nodes
        self.buses = buses
        self.version = version
        self.dbc = dbc_specifics
