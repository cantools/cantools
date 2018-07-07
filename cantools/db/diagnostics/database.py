import logging

from .formats import cdd
from ...compat import fopen


LOGGER = logging.getLogger(__name__)


class Database(object):
    """This class contains all DIDs.

    The factory functions :func:`cantools.db.load()`,
    :func:`cantools.db.load_file()` and
    :func:`cantools.db.load_string()` returns instances of this class.

    """

    def __init__(self,
                 dids=None):
        self._name_to_did = {}
        self._identifier_to_did = {}
        self._dids = dids if dids else []

    @property
    def dids(self):
        """A list of DIDs in the database.

        """

        return self._dids

    def add_cdd(self, fp):
        """Read and parse CDD data from given file-like object and add the
        parsed data to the database.

        """

        self.add_cdd_string(fp.read())

    def add_cdd_file(self, filename, encoding='utf-8'):
        """Open, read and parse CDD data from given file and add the parsed
        data to the database.

        `encoding` specifies the file encoding.

        """

        with fopen(filename, 'r', encoding=encoding) as fin:
            self.add_cdd(fin)

    def add_cdd_string(self, string):
        """Parse given CDD data string and add the parsed data to the
        database.

        """

        database = cdd.load_string(string)

        for did in database.dids:
            self.add_did(did)

    def add_did(self, did):
        """Add given DID to the database.

        """

        self._dids.append(did)

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

    def get_did_by_name(self, name):
        """Find the DID object for given name `name`.

        """

        return self._name_to_did[name]

    def get_did_by_identifier(self, identifier):
        """Find the DID object for given identifier `identifier`.

        """

        return self._identifier_to_did[identifier]

    def __repr__(self):
        lines = []

        for did in self._dids:
            lines.append(repr(did))

            for data in did.datas:
                lines.append('  ' + repr(data))

            lines.append('')

        return '\n'.join(lines)
