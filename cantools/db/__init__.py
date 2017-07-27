from .formats import dbc
from .file import File
from .message import Message
from .signal import Signal


def load(fp):
    """Read and parse given database file-like object and return a
    :class:`~cantools.db.File` object with its contents.

    >>> with open('foo.dbc') as fin:
    ...    db = cantools.db.load(fin.read())
    >>> db.version
    '1.0'

    """

    db = File()
    db.add_dbc(fp)

    return db


def load_file(filename):
    """Open, read and parse given database file and return a
    :class:`~cantools.db.File` object with its contents.

    >>> db = cantools.db.load_file('foo.dbc')
    >>> db.version
    '1.0'

    """

    db = File()
    db.add_dbc_file(filename)

    return db
