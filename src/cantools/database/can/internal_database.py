# Internal CAN database.

from typing import Optional

from .bus import Bus
from .formats.arxml.database_specifics import AutosarDatabaseSpecifics
from .formats.dbc_specifics import DbcSpecifics
from .message import Message
from .node import Node


class InternalDatabase:
    """Internal CAN database.

    """

    def __init__(self,
                 messages: list[Message],
                 nodes: list[Node],
                 buses: list[Bus],
                 version : Optional[str],
                 dbc_specifics: Optional[DbcSpecifics] = None,
                 autosar_specifics: Optional[AutosarDatabaseSpecifics] = None):
        self.messages = messages
        self.nodes = nodes
        self.buses = buses
        self.version = version
        self.dbc = dbc_specifics
        self.autosar = autosar_specifics
