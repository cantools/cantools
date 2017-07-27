import pyparsing

from .formats import dbc
from .file import File
from .message import Message
from .signal import Signal


def load_file(filename):
    """Open, read and parse given database file and return a
    :class:`~cantools.db.File` object with its contents.

    >>> db = cantools.db.load_file('foo.dbc')
    >>> db.version
    '1.0'

    """

    with open(filename, 'r') as fin:
        return load(fin)


def load(fp):
    """Read and parse given database file-like object and return a
    :class:`~cantools.db.File` object with its contents.

    >>> with open('foo.dbc') as fin:
    ...    db = cantools.db.load(fin)
    >>> db.version
    '1.0'

    """

    return load_string(fp.read())


def load_string(string):
    """Parse given database string and return a :class:`~cantools.db.File`
    object with its contents.

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
    except:
        pass

    raise ValueError('File format not supported.')
