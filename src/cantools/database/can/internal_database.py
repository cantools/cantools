# Internal CAN database.

from __future__ import annotations

from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from .bus import Bus
    from .formats.arxml.database_specifics import AutosarDatabaseSpecifics
    from .formats.dbc.dbc_specifics import DbcSpecifics
    from .message import Message
    from .node import Node


class InternalDatabase:
    """Internal CAN database.

    """

    def __init__(self,
                 messages: list[Message],
                 nodes: list[Node],
                 buses: list[Bus],
                 version : str | None,
                 dbc_specifics: DbcSpecifics | None = None,
                 autosar_specifics: AutosarDatabaseSpecifics | None = None):
        self.messages = messages
        self.nodes = nodes
        self.buses = buses
        self.version = version
        self.dbc = dbc_specifics
        self.autosar = autosar_specifics
