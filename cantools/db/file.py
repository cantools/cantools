import logging

from .formats import dbc, kcd
from .database import Database


LOGGER = logging.getLogger(__name__)


class File(object):
    """This class contains all messages, signals and definitions of a CAN
    network.

    The factory functions :func:`cantools.db.load()`,
    :func:`cantools.db.load_file()` and
    :func:`cantools.db.load_string()` returns instances of this class.

    """

    def __init__(self,
                 messages=None,
                 nodes=None,
                 buses=None,
                 attributes=None,
                 default_attrs=None,
                 version=None):
        self._messages = messages if messages else []
        self._nodes = nodes if nodes else []
        self._buses = buses if buses else []
        self.attributes = attributes if attributes else []
        self.default_attrs = default_attrs if default_attrs else []
        self._frame_id_to_message = {}
        self._version = version

    @property
    def messages(self):
        """A list of messages in the database.

        """

        return self._messages

    @property
    def nodes(self):
        """A list of nodes in the database.

        """

        return self._nodes

    @property
    def buses(self):
        """A list of CAN buses in the database.

        """

        return self._buses

    @property
    def version(self):
        """The database version, or ``None`` if unavailable.

        """

        return self._version

    def add_dbc(self, fp):
        """Read and parse DBC data from given file-like object and add the
        parsed data to the database.

        >>> db = cantools.db.File()
        >>> with open ('foo.dbc', 'r') as fin:
        ...     db.add_dbc(fin)

        """

        self.add_dbc_string(fp.read())

    def add_dbc_file(self, filename):
        """Open, read and parse DBC data from given file and add the parsed
        data to the database.

        >>> db = cantools.db.File()
        >>> db.add_dbc_file('foo.dbc', 'r')

        """

        with open(filename, 'r') as fin:
            self.add_dbc(fin)

    def add_dbc_string(self, string):
        """Parse given DBC data string and add the parsed data to the
        database.

        >>> db = cantools.db.File()
        >>> with open ('foo.dbc', 'r') as fin:
        ...     db.add_dbc_string(fin.read())

        """

        database = dbc.load_string(string)

        for message in database.messages:
            self.add_message(message)
        self._nodes = database.nodes
        self._buses = database.buses
        self.attributes = database.attributes
        self.default_attrs = database.default_attrs
        self._version = database.version

    def add_kcd(self, fp):
        """Read and parse KCD data from given file-like object and add the
        parsed data to the database.

        """

        self.add_kcd_string(fp.read())

    def add_kcd_file(self, filename):
        """Open, read and parse KCD data from given file and add the parsed
        data to the database.

        """

        with open(filename, 'r') as fin:
            self.add_kcd(fin)

    def add_kcd_string(self, string):
        """Parse given KCD data string and add the parsed data to the
        database.

        """

        database = kcd.load_string(string)

        for message in database.messages:
            self.add_message(message)
        self._nodes = database.nodes
        self._buses = database.buses
        self.attributes = database.attributes
        self.default_attrs = database.default_attrs
        self._version = database.version

    def add_message(self, message):
        """Add given message to the database.

        """

        self._messages.append(message)

        if message.frame_id in self._frame_id_to_message:
            LOGGER.warning('Overwriting message with frame id 0x%x in the '
                           'message lookup table.',
                           message.frame_id)

        self._frame_id_to_message[message.frame_id] = message

    def as_dbc_string(self):
        """Return the database as a string formatted as a DBC file.

        """

        return dbc.dump_string(Database(self._messages,
                                        self._nodes,
                                        self._buses,
                                        self.attributes,
                                        self.default_attrs,
                                        self._version))

    def as_kcd_string(self):
        """Return the database as a string formatted as a KCD file.

        """

        return kcd.dump_string(Database(self._messages,
                                        self._nodes,
                                        self._buses,
                                        self.attributes,
                                        self.default_attrs,
                                        self._version))

    def lookup_message(self, frame_id):
        """Find the message object for given frame id `frame_id`.

        """

        return self._frame_id_to_message[frame_id]

    def encode_message(self, frame_id, data):
        """Encode given signal data `data` as a message of given
        `frame_id`. `data` is a dictionary of signal name-value
        entries.

        >>> db.encode_message(158, {'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        message = self._frame_id_to_message[frame_id]

        return message.encode(data)

    def decode_message(self, frame_id, data, decode_choices=True):
        """Decode given signal data `data` as a message of given frame id
        `frame_id`. Returns a dictionary of signal name-value
        entries.

        If `decode_choices` is ``False`` scaled values are not
        converted to choice strings (if available).

        >>> db.decode_message(158, b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}

        """

        message = self._frame_id_to_message[frame_id]

        return message.decode(data, decode_choices)

    def __repr__(self):
        lines = []

        lines.append("version('{}')".format(self._version))
        lines.append('')

        if self._nodes:
            for node in self._nodes:
                lines.append(repr(node))

            lines.append('')

        for message in self._messages:
            lines.append(repr(message))

            for signal in message.signals:
                lines.append('  ' + repr(signal))

            lines.append('')

        return '\n'.join(lines)
