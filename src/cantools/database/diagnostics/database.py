import logging
from typing import Optional, TextIO

from cantools.database.diagnostics.did import Did
from cantools.typechecking import StringPathLike

from .formats import cdd

LOGGER = logging.getLogger(__name__)


class Database:
    """This class contains all DIDs.

    The factory functions :func:`load()<cantools.database.load()>`,
    :func:`load_file()<cantools.database.load_file()>` and
    :func:`load_string()<cantools.database.load_string()>` returns
    instances of this class.

    """

    def __init__(self,
                 dids: Optional[list[Did]] = None):
        self._name_to_did: dict[str, Did] = {}
        self._identifier_to_did: dict[int, Did] = {}
        self._dids = dids if dids else []
        self.refresh()

    @property
    def dids(self) -> Optional[list[Did]]:
        """A list of DIDs in the database.

        """

        return self._dids

    def add_cdd(self, fp: TextIO) -> None:
        """Read and parse CDD data from given file-like object and add the
        parsed data to the database.

        """

        self.add_cdd_string(fp.read())

    def add_cdd_file(self, filename: StringPathLike, encoding: str = 'utf-8') -> None:
        """Open, read and parse CDD data from given file and add the parsed
        data to the database.

        `encoding` specifies the file encoding.

        """

        with open(filename, encoding=encoding, errors='replace') as fin:
            self.add_cdd(fin)

    def add_cdd_string(self, string: str) -> None:
        """Parse given CDD data string and add the parsed data to the
        database.

        """

        database = cdd.load_string(string)
        self._dids = database.dids
        self.refresh()

    def _add_did(self, did: Did) -> None:
        """Add given DID to the database.

        """

        if did.name in self._name_to_did:
            LOGGER.warning("Overwriting DID with name '%s' in the "
                           "name to DID dictionary.",
                           did.name)

        if did.identifier in self._identifier_to_did:
            LOGGER.warning(
                "Overwriting DID '%s' with '%s' in the identifier to DID "
                "dictionary because they have identical identifiers 0x%x.",
                self._identifier_to_did[did.identifier].name,
                did.name,
                did.identifier)

        self._name_to_did[did.name] = did
        self._identifier_to_did[did.identifier] = did

    def get_did_by_name(self, name: str) -> Did:
        """Find the DID object for given name `name`.

        """

        return self._name_to_did[name]

    def get_did_by_identifier(self, identifier: int) -> Did:
        """Find the DID object for given identifier `identifier`.

        """

        return self._identifier_to_did[identifier]

    def refresh(self) -> None:
        """Refresh the internal database state.

        This method must be called after modifying any DIDs in the
        database to refresh the internal lookup tables used when
        encoding and decoding DIDs.

        """

        self._name_to_did = {}
        self._identifier_to_did = {}

        for did in self._dids:
            did.refresh()
            self._add_did(did)

    def __repr__(self) -> str:
        lines: list[str] = []

        for did in self._dids:
            lines.append(repr(did))

            for data in did.datas:
                lines.append('  ' + repr(data))

            lines.append('')

        return '\n'.join(lines)
