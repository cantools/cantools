from ..errors import Error as _Error


class Error(_Error):
    pass


class ParseError(Error):
    pass
