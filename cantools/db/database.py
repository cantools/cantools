import logging

from .formats import dbc
from .formats import kcd
from .formats import sym
from .internal_database import InternalDatabase
from ..compat import fopen


LOGGER = logging.getLogger(__name__)


class Database(object):
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
                 version=None,
                 dbc_specifics=None,
                 frame_id_mask=None):
        self._messages = messages if messages else []
        self._nodes = nodes if nodes else []
        self._buses = buses if buses else []
        self._name_to_message = {}
        self._frame_id_to_message = {}
        self._version = version
        self._dbc = dbc_specifics

        if frame_id_mask is None:
            frame_id_mask = 0xffffffff

        self._frame_id_mask = frame_id_mask

    @property
    def messages(self):
        """A list of messages in the database.

        Use :meth:`.get_message_by_frame_id()` or
        :meth:`.get_message_by_name()` to find a message by its frame
        id or name.

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

    @property
    def dbc(self):
        """An object containing dbc specific properties like e.g. attributes.

        """

        return self._dbc

    def add_dbc(self, fp):
        """Read and parse DBC data from given file-like object and add the
        parsed data to the database.

        >>> db = cantools.db.Database()
        >>> with open ('foo.dbc', 'r') as fin:
        ...     db.add_dbc(fin)

        """

        self.add_dbc_string(fp.read())

    def add_dbc_file(self, filename, encoding='utf-8'):
        """Open, read and parse DBC data from given file and add the parsed
        data to the database.

        `encoding` specifies the file encoding.

        >>> db = cantools.db.Database()
        >>> db.add_dbc_file('foo.dbc', 'r')

        """

        with fopen(filename, 'r', encoding=encoding) as fin:
            self.add_dbc(fin)

    def add_dbc_string(self, string):
        """Parse given DBC data string and add the parsed data to the
        database.

        >>> db = cantools.db.Database()
        >>> with open ('foo.dbc', 'r') as fin:
        ...     db.add_dbc_string(fin.read())

        """

        database = dbc.load_string(string)

        for message in database.messages:
            self.add_message(message)
            
        self._nodes = database.nodes
        self._buses = database.buses
        self._version = database.version
        self._dbc=database.dbc

    def add_kcd(self, fp):
        """Read and parse KCD data from given file-like object and add the
        parsed data to the database.

        """

        self.add_kcd_string(fp.read())

    def add_kcd_file(self, filename, encoding='utf-8'):
        """Open, read and parse KCD data from given file and add the parsed
        data to the database.

        `encoding` specifies the file encoding.

        """

        with fopen(filename, 'r', encoding=encoding) as fin:
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
        self._version = database.version
        self._dbc=database.dbc

    def add_sym(self, fp):
        """Read and parse SYM data from given file-like object and add the
        parsed data to the database.

        """

        self.add_sym_string(fp.read())

    def add_sym_file(self, filename, encoding='utf-8'):
        """Open, read and parse SYM data from given file and add the parsed
        data to the database.

        `encoding` specifies the file encoding.

        """

        with fopen(filename, 'r', encoding=encoding) as fin:
            self.add_sym(fin)

    def add_sym_string(self, string):
        """Parse given SYM data string and add the parsed data to the
        database.

        """

        database = sym.load_string(string)

        for message in database.messages:
            self.add_message(message)
        self._nodes = database.nodes
        self._buses = database.buses
        self._version = database.version
        self._dbc=database.dbc

    def add_message(self, message):
        """Add given message to the database.

        """

        self._messages.append(message)

        if message.name in self._name_to_message:
            LOGGER.warning("Overwriting message with name '%s' in the "
                           "name to message dictionary.",
                           message.name)

        masked_frame_id = (message.frame_id & self._frame_id_mask)

        if masked_frame_id in self._frame_id_to_message:
            LOGGER.warning(
                "Overwriting message '%s' with '%s' in the frame id to message "
                "dictionary because they have identical masked frame ids 0x%x.",
                self._frame_id_to_message[masked_frame_id].name,
                message.name,
                masked_frame_id)

        self._name_to_message[message.name] = message
        self._frame_id_to_message[masked_frame_id] = message

    def as_dbc_string(self):
        """Return the database as a string formatted as a DBC file.

        """

        return dbc.dump_string(InternalDatabase(self._messages,
                                                self._nodes,
                                                self._buses,
                                                self._version,
                                                self._dbc))

    def as_kcd_string(self):
        """Return the database as a string formatted as a KCD file.

        """

        return kcd.dump_string(InternalDatabase(self._messages,
                                                self._nodes,
                                                self._buses,
                                                self._version,
                                                self._dbc))

    def get_message_by_name(self, name):
        """Find the message object for given name `name`.

        """
        
        try:
            return self._name_to_message[name]
        except KeyError:
            # Maybe the dict is outdated. Try finding the message and update the dict
            message = self._find_message_by_name(name)

            if message != None:
                self._name_to_message[name] = message
                return message
                
        raise KeyError(name)

    def get_message_by_frame_id(self, frame_id):
        """Find the message object for given frame id `frame_id`.

        """

        try:
            return self._frame_id_to_message[frame_id & self._frame_id_mask]
        except KeyError:
            # Maybe the dict is outdated. Try finding the message and update the dict
            message = self._find_message_by_frame_id(frame_id)

            if message != None:
                self._frame_id_to_message[frame_id & self._frame_id_mask] = message
                return message

        raise KeyError(frame_id & self._frame_id_mask)

    def _find_message_by_name(self, name):
        """Find the message object for given name `name`. This is just private to 
        encourage usage of the dict based get_message_by_name. It is used as a 
        backup mechanism to cope with outdated dicts due to name changes.

        """

        for message in self._messages:

            if message.name == name:
                return message

        return None

    def _find_message_by_frame_id(self, frame_id):
        """Find the message object for given frame id `frame_id`. This is just
        private to encourage usage of the dict based get_message_by_name. It 
        is used as a backup mechanism to cope with outdated dicts due to name
        changes.

        """

        mask = self._frame_id_mask

        for message in self._messages:

            if message.frame_id & mask == frame_id & mask:
                return message

        return None

    def get_node_by_name(self, name):
        """Find the node object for given name `name`.

        """

        for node in self._nodes:

            if node.name == name:
                return node

        raise KeyError(name)

    def get_bus_by_name(self, name):
        """Find the bus object for given name `name`.

        """

        for bus in self._buses:

            if bus.name == name:
                return bus

        raise KeyError(name)

    def encode_message(self,
                       frame_id_or_name,
                       data,
                       scaling=True,
                       padding=False):
        """Encode given signal data `data` as a message of given frame id or
        name `frame_id_or_name`. `data` is a dictionary of signal
        name-value entries.

        If `scaling` is ``False`` no scaling of signals is performed.

        If `padding` is ``True`` unused bits are encoded as 1.

        >>> db.encode_message(158, {'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'
        >>> db.encode_message('Foo', {'Bar': 1, 'Fum': 5.0})
        b'\\x01\\x45\\x23\\x00\\x11'

        """

        try:
            message = self._frame_id_to_message[frame_id_or_name]
        except KeyError:
            message = self._name_to_message[frame_id_or_name]

        return message.encode(data, scaling, padding)

    def decode_message(self,
                       frame_id_or_name,
                       data,
                       decode_choices=True,
                       scaling=True):
        """Decode given signal data `data` as a message of given frame id or
        name `frame_id_or_name`. Returns a dictionary of signal
        name-value entries.

        If `decode_choices` is ``False`` scaled values are not
        converted to choice strings (if available).

        If `scaling` is ``False`` no scaling of signals is performed.

        >>> db.decode_message(158, b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}
        >>> db.decode_message('Foo', b'\\x01\\x45\\x23\\x00\\x11')
        {'Bar': 1, 'Fum': 5.0}

        """

        try:
            message = self._frame_id_to_message[frame_id_or_name]
        except KeyError:
            message = self._name_to_message[frame_id_or_name]

        return message.decode(data, decode_choices, scaling)

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
