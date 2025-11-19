# Internal diagnostics database.

from cantools.database.diagnostics.did import Did


class InternalDatabase:
    """Internal diagnostics database.

    """

    def __init__(self, dids: list[Did]):
        self.dids = dids
