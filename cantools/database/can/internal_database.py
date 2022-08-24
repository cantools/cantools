# Internal CAN database.

from typing import Optional, List

from .message import Message
from .node import Node
from .bus import Bus
from .formats.dbc_specifics import DbcSpecifics
from .formats.arxml.database_specifics import AutosarDatabaseSpecifics

class InternalDatabase(object):
    """Internal CAN database.

    """

    def __init__(self,
                 messages: List[Message],
                 nodes: List[Node],
                 buses: List[Bus],
                 version : Optional[str],
                 dbc_specifics: Optional[DbcSpecifics] = None,
                 autosar_specifics: Optional[AutosarDatabaseSpecifics] = None):
        self.messages = messages
        self.nodes = nodes
        self.buses = buses
        self.version = version
        self.dbc = dbc_specifics
        self.autosar = autosar_specifics
