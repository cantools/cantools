from typing import Any

from ..errors import Error as _Error


class Error(_Error):
    pass


class ParseError(Error):
    pass


class EncodeError(Error):
    pass


class DecodeError(Error):
    pass


def format_load_error(database_format: str, e: Exception) -> str:
    """Describe why a string could not be loaded as `database_format`.

    Loaders raise ParseError, or another cantools Error for the checks
    of strict mode, if the string is at fault. Any other exception is a
    bug in cantools.

    """

    if isinstance(e, Error):
        return f'{database_format.upper()}: "{e}"'

    return (f'{database_format.upper()}: "{type(e).__name__}: {e}" '
            f'(this is a bug in cantools that ought to be fixed)')


class UnsupportedDatabaseFormatError(Error):
    """This exception is raised when
    :func:`~cantools.database.load_file()`,
    :func:`~cantools.database.load()` and
    :func:`~cantools.database.load_string()` are unable to parse given
    database file or string.

    """

    def __init__(self,
                 e_arxml: Exception | None,
                 e_dbc: Exception | None,
                 e_kcd: Exception | None,
                 e_sym: Exception | None,
                 e_cdd: Exception | None) -> None:
        message_chunks: list[str] = []

        for database_format, e in [('arxml', e_arxml),
                                   ('dbc', e_dbc),
                                   ('kcd', e_kcd),
                                   ('sym', e_sym),
                                   ('cdd', e_cdd)]:
            if e is not None:
                message_chunks.append(format_load_error(database_format, e))

        message = ', '.join(message_chunks)

        super().__init__(message)

        self.e_arxml = e_arxml
        self.e_dbc = e_dbc
        self.e_kcd = e_kcd
        self.e_sym = e_sym
        self.e_cdd = e_cdd

    def __reduce__(self) -> str | tuple[Any, ...]:
        return type(self), (self.e_arxml, self.e_dbc, self.e_kcd, self.e_sym, self.e_cdd), {}
