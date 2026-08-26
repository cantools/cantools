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
