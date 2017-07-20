from .formats import dbc
from .database import Database


class File(object):
    """CAN database file.

    """

    def __init__(self,
                 messages=None,
                 nodes=None,
                 attributes=None,
                 default_attrs=None,
                 version=None):
        self.messages = messages if messages else []
        self.nodes = nodes if nodes else []
        self.attributes = attributes if attributes else []
        self.default_attrs = default_attrs if default_attrs else []
        self._frame_id_to_message = {}
        self.version = version

    def add_dbc(self, iostream):
        """Add information from given DBC iostream.

        """

        f = dbc.loads(iostream.read())

        for message in f.messages:
            self.add_message(message)
        self.nodes = f.nodes
        self.attributes = f.attributes
        self.default_attrs = f.default_attrs
        self.version = f.version

    def add_dbc_file(self, filename):
        """Add information from given DBC-file.

        """

        with open(filename, 'r') as fin:
            self.add_dbc(fin)

    def add_message(self, message):
        """Add given message to the database.

        """

        self.messages.append(message)
        self._frame_id_to_message[message.frame_id] = message

    def as_dbc(self):
        """Return a string of the database in DBC-file format.

        """

        return dbc.dumps(Database(self.messages,
                                  self.nodes,
                                  self.attributes,
                                  self.default_attrs,
                                  self.version))

    def lookup_message(self, frame_id):
        """Find the message object for given frame id `frame_id`.

        """

        return self._frame_id_to_message[frame_id]

    def encode_message(self, frame_id, data):
        """Encode given signal data `data` as a message of given
        `frame_id`. `data` can be a dictionary or a named tuple with
        signal values.

        """

        message = self._frame_id_to_message[frame_id]

        return message.encode(data)

    def decode_message(self, frame_id, data):
        """Decode given signal data `data` as a message of given frame id
        `frame_id`.

        """

        message = self._frame_id_to_message[frame_id]

        return message.decode(data)

    def __repr__(self):
        lines = []

        lines.append("version('{}')".format(self.version))
        lines.append('')

        if self.nodes:
            for node in self.nodes:
                lines.append(repr(node))

            lines.append('')

        for message in self.messages:
            lines.append(repr(message))

            for signal in message.signals:
                lines.append('  ' + repr(signal))

            lines.append('')

        return '\n'.join(lines)
