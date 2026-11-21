from ...errors import ParseError


def num(number_as_string: str) -> int | float:
    """Convert given string to an integer or a float.

    """

    try:
        return int(number_as_string)
    except ValueError:
        pass

    try:
        return float(number_as_string)
    except ValueError:
        raise ParseError('Expected integer or floating point number.') from None


def parse_int(number_as_string: str, what: str, base: int = 10) -> int:
    """Convert given string to an integer or raise a ParseError naming `what`."""

    try:
        return int(number_as_string, base)
    except ValueError:
        raise ParseError(f"Expected an integer for {what}, but got "
                         f"'{number_as_string}'.") from None


def parse_float(number_as_string: str, what: str) -> float:
    """Convert given string to a float or raise a ParseError naming `what`."""

    try:
        return float(number_as_string)
    except ValueError:
        raise ParseError(f"Expected a number for {what}, but got "
                         f"'{number_as_string}'.") from None
