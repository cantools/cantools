from ..errors import Error as _Error


class Error(_Error):
    pass


class ParseError(Error):
    pass


class EncodeError(Error):
    pass


class DecodeError(Error):
    pass
