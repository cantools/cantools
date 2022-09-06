import os
from typing import Union, Optional, TextIO, MutableMapping, cast
from xml.etree import ElementTree

from .errors import ParseError
from .errors import Error
from ..compat import fopen
from . import can
from . import diagnostics
from . import utils
import textparser
import diskcache

# Remove once less users are using the old package structure.
from .can import *
from ..typechecking import StringPathLike


class UnsupportedDatabaseFormatError(Error):
    """This exception is raised when
    :func:`~cantools.database.load_file()`,
    :func:`~cantools.database.load()` and
    :func:`~cantools.database.load_string()` are unable to parse given
    database file or string.

    """

    def __init__(self, e_arxml, e_dbc, e_kcd, e_sym, e_cdd):
        message = []

        if e_arxml is not None:
            message.append('ARXML: "{}"'.format(e_arxml))

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

        self.e_arxml = e_arxml
        self.e_dbc = e_dbc
        self.e_kcd = e_kcd
        self.e_sym = e_sym
        self.e_cdd = e_cdd


def _resolve_database_format_and_encoding(database_format,
                                          encoding,
                                          filename):
    if database_format is None:
        database_format = os.path.splitext(filename)[1][1:].lower()

    if encoding is None:
        try:
            encoding = {
                'dbc': 'cp1252',
                'sym': 'cp1252'
            }[database_format]
        except KeyError:
            encoding = 'utf-8'

    return database_format, encoding


def _load_file_cache(filename: StringPathLike,
                     database_format: Optional[str],
                     encoding: Optional[str],
                     frame_id_mask: Optional[int],
                     prune_choices: bool,
                     strict: bool,
                     cache_dir: str,
                     sort_signals: utils.type_sort_signals,
                     ) -> Union[can.Database, diagnostics.Database]:
    with open(filename, 'rb') as fin:
        key = fin.read()

    cache: MutableMapping[bytes, Union[can.Database, diagnostics.Database]] = diskcache.Cache(cache_dir)

    try:
        return cache[key]
    except KeyError:
        with fopen(filename, 'r', encoding=encoding) as fin:
            database = load(cast(TextIO, fin),
                            database_format,
                            frame_id_mask,
                            prune_choices,
                            strict,
                            sort_signals)
        cache[key] = database

        return database


def load_file(filename: StringPathLike,
              database_format: Optional[str] = None,
              encoding: Optional[str] = None,
              frame_id_mask: Optional[int] = None,
              prune_choices: bool = False,
              strict: bool = True,
              cache_dir: Optional[str] = None,
              sort_signals: utils.type_sort_signals = utils.sort_signals_by_start_bit,
              ) -> Union[can.Database, diagnostics.Database]:
    """Open, read and parse given database file and return a
    :class:`can.Database<.can.Database>` or
    :class:`diagnostics.Database<.diagnostics.Database>` object with
    its contents.

    `database_format` is one of ``'arxml'``, ``'dbc'``, ``'kcd'``,
    ``'sym'``, ``cdd`` and ``None``. If ``None``, the database format
    is selected based on the filename extension as in the table below.
    Filename extensions are case insensitive.

    +-----------+-----------------+
    | Extension | Database format |
    +===========+=================+
    | .arxml    | ``'arxml'``     |
    +-----------+-----------------+
    | .dbc      | ``'dbc'``       |
    +-----------+-----------------+
    | .kcd      | ``'kcd'``       |
    +-----------+-----------------+
    | .sym      | ``'sym'``       |
    +-----------+-----------------+
    | .cdd      | ``'cdd'``       |
    +-----------+-----------------+
    | <unknown> | ``None``        |
    +-----------+-----------------+

    `encoding` specifies the file encoding. If ``None``, the encoding
    is selected based on the database format as in the table
    below. Use ``open()`` and :func:`~cantools.database.load()` if
    platform dependent encoding is desired.

    +-----------------+-------------------+
    | Database format | Default encoding  |
    +=================+===================+
    | ``'arxml'``     | ``'utf-8'``       |
    +-----------------+-------------------+
    | ``'dbc'``       | ``'cp1252'``      |
    +-----------------+-------------------+
    | ``'kcd'``       | ``'utf-8'``       |
    +-----------------+-------------------+
    | ``'sym'``       | ``'cp1252'``      |
    +-----------------+-------------------+
    | ``'cdd'``       | ``'utf-8'``       |
    +-----------------+-------------------+
    | ``None``        | ``'utf-8'``       |
    +-----------------+-------------------+

    `prune_choices` abbreviates the names of choices by removing
    a common prefix ending on an underscore. If you want to have
    the original names you need to pass `prune_choices = False`.

    `cache_dir` specifies the database cache location in the file
    system. Give as ``None`` to disable the cache. By default the
    cache is disabled. The cache key is the contents of given
    file. Using a cache will significantly reduce the load time when
    reloading the same file. The cache directory is automatically
    created if it does not exist. Remove the cache directory
    `cache_dir` to clear the cache.

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

    database_format, encoding = _resolve_database_format_and_encoding(
        database_format,
        encoding,
        filename)

    if cache_dir is None:
        with fopen(filename, 'r', encoding=encoding) as fin:
            return load(fin,
                        database_format,
                        frame_id_mask,
                        prune_choices,
                        strict,
                        sort_signals)
    else:
        return _load_file_cache(filename,
                                database_format,
                                encoding,
                                frame_id_mask,
                                prune_choices,
                                strict,
                                cache_dir,
                                sort_signals)


def dump_file(database,
              filename,
              database_format=None,
              encoding=None,
              sort_signals=utils.SORT_SIGNALS_DEFAULT):
    """Dump given database `database` to given file `filename`.

    Depending on the output file format signals may be sorted by default.
    If you don't want signals to be sorted pass `sort_signals=None`.
    `sort_signals=None` is assumed by default if you have passed `sort_signals=None` to load_file.
    If you want the signals to be sorted in a special way pass something like
    `sort_signals = lambda signals: list(sorted(signals, key=lambda sig: sig.name))`
    For dbc files the default is to sort the signals by their start bit in descending order.
    For kcd files the default is to not sort the signals.

    See :func:`~cantools.database.load_file()` for descriptions of
    other arguments.

    The ``'dbc'`` database format will always have Windows-style line
    endings (``\\r\\n``). For other database formats the line ending
    depends on the operating system.

    >>> db = cantools.database.load_file('foo.dbc')
    >>> cantools.database.dump_file(db, 'bar.dbc')

    Pass `sort_signals=None, prune_choices=False` to load_file
    in order to minimize the differences between foo.dbc and bar.dbc.
    """

    database_format, encoding = _resolve_database_format_and_encoding(
        database_format,
        encoding,
        filename)

    newline = None

    if database_format == 'dbc':
        output = database.as_dbc_string(sort_signals=sort_signals)
        newline = ''
    elif database_format == 'kcd':
        output = database.as_kcd_string(sort_signals=sort_signals)
    elif database_format == 'sym':
        output = database.as_sym_string(sort_signals=sort_signals)
    else:
        raise Error(
            "Unsupported output database format '{}'.".format(database_format))

    with fopen(filename, 'w', encoding=encoding, newline=newline) as fout:
        fout.write(output)


def load(fp: TextIO,
         database_format: Optional[str] = None,
         frame_id_mask: Optional[int] = None,
         prune_choices: bool = False,
         strict: bool = True,
         sort_signals: utils.type_sort_signals = utils.sort_signals_by_start_bit) -> Union[can.Database, diagnostics.Database]:
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
                       prune_choices,
                       strict,
                       sort_signals)


def load_string(string: str,
                database_format: Optional[str] = None,
                frame_id_mask: Optional[int] = None,
                prune_choices: bool = False,
                strict: bool = True,
                sort_signals: utils.type_sort_signals = utils.sort_signals_by_start_bit) -> Union[can.Database, diagnostics.Database]:
    """Parse given database string and return a
    :class:`can.Database<.can.Database>` or
    :class:`diagnostics.Database<.diagnostics.Database>` object with
    its contents.

    `database_format` may be one of ``'arxml'``, ``'dbc'``, ``'kcd'``,
    ``'sym'``, ``'cdd'`` or ``None``, where ``None`` means transparent
    format.

    `prune_choices` is a bool indicating whether signal names are supposed to be abbreviated
    by stripping a common prefix ending on an underscore. This is enabled by default.

    See :class:`can.Database<.can.Database>` for a description of
    `strict`.

    `sort_signals` is a function taking a list of signals as argument and returning a list of signals.
    By default signals are sorted by their start bit when their Message object is created.
    If you don't want them to be sorted pass `sort_signals = None`.
    If you want the signals to be sorted in another way pass something like
    `sort_signals = lambda signals: list(sorted(signals, key=lambda sig: sig.name))`

    Raises an
    :class:`~cantools.database.UnsupportedDatabaseFormatError`
    exception if given string does not contain a supported database
    format.

    >>> with open('foo.dbc') as fin:
    ...    db = cantools.database.load_string(fin.read())
    >>> db.version
    '1.0'

    """

    if database_format not in ['arxml', 'dbc', 'kcd', 'sym', 'cdd', None]:
        raise ValueError(
            "expected database format 'arxml', 'dbc', 'kcd', 'sym', 'cdd' or "
            "None, but got '{}'".format(database_format))

    e_arxml = None
    e_dbc = None
    e_kcd = None
    e_sym = None
    e_cdd = None

    def load_can_database(fmt: str) -> can.Database:
        db = can.Database(frame_id_mask=frame_id_mask,
                          strict=strict,
                          sort_signals=sort_signals)

        if fmt == 'arxml':
            db.add_arxml_string(string)
        elif fmt == 'dbc':
            db.add_dbc_string(string)
        elif fmt == 'kcd':
            db.add_kcd_string(string)
        elif fmt == 'sym':
            db.add_sym_string(string)

        if prune_choices:
            utils.prune_database_choices(db)

        return db

    if database_format in ['arxml', None]:
        try:
            return load_can_database('arxml')
        except (ElementTree.ParseError, ValueError) as e:
            e_arxml = e

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

    raise UnsupportedDatabaseFormatError(e_arxml, e_dbc, e_kcd, e_sym, e_cdd)
