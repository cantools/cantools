import sys
from xml.etree import ElementTree

from .formats.utils import ParseError
from .database import Database
# ToDo: Remove backwards compatibility File in future release.
from .database import Database as File
from .message import Message
from .message import EncodeError
from .message import DecodeError
from .signal import Signal


class UnsupportedDatabaseFormatError(Exception):
    """This exception is raised when :func:`~cantools.db.load_file()`,
    :func:`~cantools.db.load()` and :func:`~cantools.db.load_string()`
    are unable to parse given database file or string.

    """

    def __init__(self, e_dbc, e_kcd, e_sym):
        message = []

        if e_dbc is not None:
            message.append('DBC: "{}"'.format(e_dbc))

        if e_kcd is not None:
            message.append('KCD: "{}"'.format(e_kcd))

        if e_sym is not None:
            message.append('SYM: "{}"'.format(e_sym))

        message = ', '.join(message)

        super(UnsupportedDatabaseFormatError, self).__init__(message)

        self.e_dbc = e_dbc
        self.e_kcd = e_kcd
        self.e_sym = e_sym


def load_file(filename,
              database_format=None,
              encoding='utf-8',
              frame_id_mask=None):
    """Open, read and parse given database file and return a
    :class:`~cantools.db.Database` object with its
    contents.

    `database_format` may be one of ``'dbc'``, ``'kcd'``, ``'sym'`` or
    ``None``, where ``None`` means transparent format.

    `encoding` specifies the file encoding.

    Raises an :class:`~cantools.db.UnsupportedDatabaseFormatError`
    exception if given file does not contain a supported database
    format.

    >>> db = cantools.db.load_file('foo.dbc')
    >>> db.version
    '1.0'

    """

    if sys.version_info[0] < 3:
        with open(filename, 'r') as fin:
            return load(fin, database_format, frame_id_mask)
    else:
        with open(filename, 'r', encoding=encoding, errors='replace') as fin:
            return load(fin, database_format, frame_id_mask)


def load(fp, database_format=None, frame_id_mask=None):
    """Read and parse given database file-like object and return a
    :class:`~cantools.db.Database` object with its
    contents.

    `database_format` may be one of ``'dbc'``, ``'kcd'``, ``'sym'`` or
    ``None``, where ``None`` means transparent format.

    Raises an :class:`~cantools.db.UnsupportedDatabaseFormatError`
    exception if given file-like object does not contain a supported
    database format.

    >>> with open('foo.kcd') as fin:
    ...    db = cantools.db.load(fin)
    >>> db.version
    None

    """

    return load_string(fp.read(), database_format, frame_id_mask)


def load_string(string, database_format=None, frame_id_mask=None):
    """Parse given database string and return a
    :class:`~cantools.db.Database` object with its
    contents.

    `database_format` may be one of ``'dbc'``, ``'kcd'``, ``'sym'`` or
    ``None``, where ``None`` means transparent format.

    Raises an :class:`~cantools.db.UnsupportedDatabaseFormatError`
    exception if given string does not contain a supported database
    format.

    >>> with open('foo.dbc') as fin:
    ...    db = cantools.db.load_string(fin.read())
    >>> db.version
    '1.0'

    """

    if database_format not in ['dbc', 'kcd', 'sym', None]:
        raise ValueError(
            "expected database format 'dbc', 'kcd', 'sym' or None, but "
            "got '{}'".format(database_format))

    e_dbc = None
    e_kcd = None
    e_sym = None

    if database_format in ['dbc', None]:
        try:
            db = Database(frame_id_mask=frame_id_mask)
            db.add_dbc_string(string)
            return db
        except ParseError as e:
            e_dbc = e

    if database_format in ['kcd', None]:
        try:
            db = Database(frame_id_mask=frame_id_mask)
            db.add_kcd_string(string)
            return db
        except ElementTree.ParseError as e:
            e_kcd = e

    if database_format in ['sym', None]:
        try:
            db = Database(frame_id_mask=frame_id_mask)
            db.add_sym_string(string)
            return db
        except ParseError as e:
            e_sym = e

    raise UnsupportedDatabaseFormatError(e_dbc, e_kcd, e_sym)
