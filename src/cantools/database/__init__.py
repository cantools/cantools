__all__ = ["Bus", "Database", "DecodeError", "EncodeError", "Message",
           "Node", "Signal", "dump_file", "load", "load_file", "load_string"]

import logging
import os
import warnings
from contextlib import nullcontext
from typing import Any, TextIO

try:
    import diskcache  # type: ignore
    _DISKCACHE_AVAILABLE = True
except ImportError:
    _DISKCACHE_AVAILABLE = False

from ..typechecking import StringPathLike
from . import can, diagnostics, utils
from .can.bus import Bus
from .can.database import Database
from .can.message import Message
from .can.node import Node
from .can.signal import Signal
from .errors import (
    DecodeError,
    EncodeError,
    Error,
    UnsupportedDatabaseFormatError,
    format_load_error,
)

LOGGER = logging.getLogger(__name__)


def _resolve_database_format_and_encoding(database_format: str | None,
                                          encoding: str | None,
                                          filename: StringPathLike) -> tuple[str, str]:
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

def load_file(filename: StringPathLike,
              database_format: str | None = None,
              encoding: str | None = None,
              frame_id_mask: int | None = None,
              prune_choices: bool = False,
              strict: bool = True,
              cache_dir: str | None = None,
              sort_signals: utils.type_sort_signals = utils.sort_signals_by_start_bit,
              ) -> can.Database | diagnostics.Database:
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
    cache is disabled, but can be enabled with environment variable
    `CANTOOLS_CACHE_DIR`. The cache key is db path with modification
    time and all arguments that may influence the result. Using a
    cache will significantly reduce the load time when reloading the
    same file. The cache directory is automatically created if it does
    not exist. Remove the cache directory `cache_dir` to clear the cache.
    Requires the optional ``diskcache`` package
    (``pip install cantools[cache]``); if it is not installed, a
    warning is issued and caching is disabled.

    See :func:`~cantools.database.load_string()` for descriptions of
    other arguments.

    Raises an
    :class:`~cantools.database.UnsupportedDatabaseFormatError`
    exception if given file does not contain a supported database
    format and a :class:`NotImplementedError` if the file uses a
    variant of a supported format which is not supported.

    >>> db = cantools.database.load_file('foo.dbc')
    >>> db.version
    '1.0'

    """

    database_format, encoding = _resolve_database_format_and_encoding(
        database_format,
        encoding,
        filename)

    cache_dir = cache_dir or os.getenv("CANTOOLS_CACHE_DIR", None)
    if cache_dir and not _DISKCACHE_AVAILABLE:
        warnings.warn(
            "diskcache is not installed; caching is disabled. "
            "Install it with: pip install cantools[cache]",
            UserWarning,
            stacklevel=2,
        )
        cache_dir = None
    cache_key: tuple[Any, ...] | None = None
    db: can.Database | diagnostics.Database

    with diskcache.Cache(cache_dir) if cache_dir else nullcontext() as cache:
        if cache is not None:
            # do not cache if user-defined sort_signals function is provided
            # the key cannot be created if function is local or depends on context
            # pickle serializer will fail anyway
            if not callable(sort_signals) or sort_signals.__module__ == 'cantools.database.utils':
                cache_key = (
                    database_format,
                    encoding,
                    frame_id_mask,
                    prune_choices,
                    strict,
                    sort_signals,
                    filename,
                    os.path.getmtime(filename),
                )

            db = cache.get(cache_key)
            if isinstance(db, (can.Database, diagnostics.Database)):
                return db

        with open(filename, encoding=encoding, errors='replace') as fin:
            db = load(fin,
                    database_format,
                    frame_id_mask,
                    prune_choices,
                    strict,
                    sort_signals)

        if cache is not None:
            cache[cache_key] = db

        return db

def dump_file(database: can.Database,
              filename: StringPathLike,
              database_format: str | None = None,
              encoding: str | None = None,
              sort_signals: utils.type_sort_signals = utils.SORT_SIGNALS_DEFAULT) -> None:
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
            f"Unsupported output database format '{database_format}'.")

    with open(filename, 'w', encoding=encoding, newline=newline, errors='replace') as fout:
        fout.write(output)


def load(fp: TextIO,
         database_format: str | None = None,
         frame_id_mask: int | None = None,
         prune_choices: bool = False,
         strict: bool = True,
         sort_signals: utils.type_sort_signals = utils.sort_signals_by_start_bit) -> can.Database | diagnostics.Database:
    """Read and parse given database file-like object and return a
    :class:`can.Database<.can.Database>` or
    :class:`diagnostics.Database<.diagnostics.Database>` object with
    its contents.

    See :func:`~cantools.database.load_string()` for descriptions of
    other arguments.

    Raises an
    :class:`~cantools.database.UnsupportedDatabaseFormatError`
    exception if given file-like object does not contain a supported
    database format and a :class:`NotImplementedError` if it uses a
    variant of a supported format which is not supported.

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
                database_format: str | None = None,
                frame_id_mask: int | None = None,
                prune_choices: bool = False,
                strict: bool = True,
                sort_signals: utils.type_sort_signals = utils.sort_signals_by_start_bit) \
        -> can.Database | diagnostics.Database:
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
    format and a :class:`NotImplementedError` if the string uses a
    variant of a supported format which is not supported.

    >>> with open('foo.dbc') as fin:
    ...    db = cantools.database.load_string(fin.read())
    >>> db.version
    '1.0'

    """

    if database_format not in ['arxml', 'dbc', 'kcd', 'sym', 'cdd', None]:
        raise ValueError(
            f"expected database format 'arxml', 'dbc', 'kcd', 'sym', 'cdd' or "
            f"None, but got '{database_format}'")

    def load_database(fmt: str) -> can.Database | diagnostics.Database:
        if fmt == 'cdd':
            diagnostics_db = diagnostics.Database()
            diagnostics_db.add_cdd_string(string)
            return diagnostics_db

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

    if database_format is None:
        formats = ['arxml', 'dbc', 'kcd', 'sym', 'cdd']
    else:
        formats = [database_format]

    errors: dict[str, Exception] = {}

    for fmt in formats:
        try:
            return load_database(fmt)
        except Error as e:
            # the string cannot be loaded as this format. try the next
            # one
            errors[fmt] = e
        except NotImplementedError:
            # the string is in this format, but the loader does not
            # support this variant of it. trying further formats is
            # pointless
            raise
        except Exception as e:
            # loaders are supposed to raise ParseError for everything
            # that is wrong with the input, so this is a bug. report
            # it, but do not let it get in the way of the other formats
            LOGGER.warning(format_load_error(fmt, e))
            errors[fmt] = e

    exc = UnsupportedDatabaseFormatError(errors.get('arxml'),
                                         errors.get('dbc'),
                                         errors.get('kcd'),
                                         errors.get('sym'),
                                         errors.get('cdd'))

    if database_format is None:
        raise exc

    # keep the traceback of the loader's exception usable. this is not
    # possible when probing because then there is one exception per
    # format
    raise exc from errors[database_format]
