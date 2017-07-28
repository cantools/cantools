from xml.etree import ElementTree

import pyparsing

from .formats import dbc
from .file import File
from .message import Message
from .signal import Signal


class UnsupportedDatabaseFormat(Exception):
    pass


def load_file(filename):
    """Open, read and parse given database file and return a
    :class:`~cantools.db.File` object with its contents. Raises an
    exception if given file does not contain a supported database
    format.

    >>> db = cantools.db.load_file('foo.dbc')
    >>> db.version
    '1.0'

    """

    with open(filename, 'r') as fin:
        return load(fin)


def load(fp):
    """Read and parse given database file-like object and return a
    :class:`~cantools.db.File` object with its contents. Raises an
    exception if given file-like object does not contain a supported
    database format.

    >>> with open('foo.kcd') as fin:
    ...    db = cantools.db.load(fin)
    >>> db.version
    None

    """

    return load_string(fp.read())


def load_string(string):
    """Parse given database string and return a :class:`~cantools.db.File`
    object with its contents. Raises an exception if given string does
    not contain a supported database format.

    >>> with open('foo.dbc') as fin:
    ...    db = cantools.db.load_string(fin.read())
    >>> db.version
    '1.0'

    """

    try:
        db = File()
        db.add_dbc_string(string)
        return db
    except pyparsing.ParseException:
        pass

    try:
        db = File()
        db.add_kcd_string(string)
        return db
    except ElementTree.ParseError:
        pass

    raise UnsupportedDatabaseFormat()
