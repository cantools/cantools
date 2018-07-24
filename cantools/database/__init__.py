from xml.etree import ElementTree
from .errors import ParseError
from .errors import Error
from ..compat import fopen
from . import can
from . import diagnostics
import textparser

# Remove once less users are using the old package structure.
from .can import *


class UnsupportedDatabaseFormatError(Error):
    """This exception is raised when
    :func:`~cantools.database.load_file()`,
    :func:`~cantools.database.load()` and
    :func:`~cantools.database.load_string()` are unable to parse given
    database file or string.

    """

    def __init__(self, e_dbc, e_kcd, e_sym, e_cdd):
        message = []

        if e_dbc is not None:
            message.append('DBC: "{}"'.format(e_dbc))

        if e_kcd is not None:
            message.append('KCD: "{}"'.format(e_kcd))

        if e_sym is not None:
            message.append('SYM: "{}"'.format(e_sym))

        if e_cdd is not None:
            message.append('CDD: "{}"'.format(e_cdd))

        message = ', '.join(message)

        super(UnsupportedDatabaseFormatError, self).__init__(message)

        self.e_dbc = e_dbc
        self.e_kcd = e_kcd
        self.e_sym = e_sym
        self.e_cdd = e_cdd


def load_file(filename,
              database_format=None,
              encoding='utf-8',
              frame_id_mask=None,
              strict=True):
    """Open, read and parse given database file and return a
    :class:`can.Database<.can.Database>` or
    :class:`diagnostics.Database<.diagnostics.Database>` object with
    its contents.

    `encoding` specifies the file encoding.

    See :func:`~cantools.database.load_string()` for descriptions of
    other arguments.

    Raises an
    :class:`~cantools.database.UnsupportedDatabaseFormatError`
    exception if given file does not contain a supported database
    format.

    >>> db = cantools.database.load_file('foo.dbc')
    >>> db.version
    '1.0'

    """

    with fopen(filename, 'r', encoding=encoding) as fin:
        return load(fin,
                    database_format,
                    frame_id_mask,
                    strict)


def load(fp,
         database_format=None,
         frame_id_mask=None,
         strict=True):
    """Read and parse given database file-like object and return a
    :class:`can.Database<.can.Database>` or
    :class:`diagnostics.Database<.diagnostics.Database>` object with
    its contents.

    See :func:`~cantools.database.load_string()` for descriptions of
    other arguments.

    Raises an
    :class:`~cantools.database.UnsupportedDatabaseFormatError`
    exception if given file-like object does not contain a supported
    database format.

    >>> with open('foo.kcd') as fin:
    ...    db = cantools.database.load(fin)
    >>> db.version
    None

    """

    return load_string(fp.read(),
                       database_format,
                       frame_id_mask,
                       strict)


def load_string(string,
                database_format=None,
                frame_id_mask=None,
                strict=True):
    """Parse given database string and return a
    :class:`can.Database<.can.Database>` or
    :class:`diagnostics.Database<.diagnostics.Database>` object with
    its contents.

    `database_format` may be one of ``'dbc'``, ``'kcd'``, ``'sym'``,
    ``'cdd'`` or ``None``, where ``None`` means transparent format.

    See :class:`can.Database<.can.Database>` for a description of
    `strict`.

    Raises an
    :class:`~cantools.database.UnsupportedDatabaseFormatError`
    exception if given string does not contain a supported database
    format.

    >>> with open('foo.dbc') as fin:
    ...    db = cantools.database.load_string(fin.read())
    >>> db.version
    '1.0'

    """

    if database_format not in ['dbc', 'kcd', 'sym', 'cdd', None]:
        raise ValueError(
            "expected database format 'dbc', 'kcd', 'sym', 'cdd' or None, but "
            "got '{}'".format(database_format))

    e_dbc = None
    e_kcd = None
    e_sym = None
    e_cdd = None

    def load_can_database(fmt):
        db = can.Database(frame_id_mask=frame_id_mask,
                          strict=strict)

        if fmt == 'dbc':
            db.add_dbc_string(string)
        elif fmt == 'kcd':
            db.add_kcd_string(string)
        elif fmt == 'sym':
            db.add_sym_string(string)

        return db

    if database_format in ['dbc', None]:
        try:
            return load_can_database('dbc')
        except textparser.ParseError as e:
            e_dbc = e

    if database_format in ['kcd', None]:
        try:
            return load_can_database('kcd')
        except (ElementTree.ParseError, ValueError) as e:
            e_kcd = e

    if database_format in ['sym', None]:
        try:
            return load_can_database('sym')
        except ParseError as e:
            e_sym = e

    if database_format in ['cdd', None]:
        try:
            db = diagnostics.Database()
            db.add_cdd_string(string)
            return db
        except (ElementTree.ParseError, ValueError) as e:
            e_cdd = e

    raise UnsupportedDatabaseFormatError(e_dbc, e_kcd, e_sym, e_cdd)
