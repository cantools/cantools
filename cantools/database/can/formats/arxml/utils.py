# utility functions that are helpful when dealing with ARXML files
from xml.etree import ElementTree
from typing import Union, List

def parse_number_string(in_string : str, allow_float : bool=False) \
    -> Union[int, float]:
    """Convert a string representing numeric value that is specified
    within an ARXML file to either an integer or a floating point object

    This is surprisingly complicated:

    - Some ARXML files use "true" and "false" synonymous to 1 and 0
    - ARXML uses the C notation (leading 0) to specify octal numbers
      whereas python only accepts the "0o" prefix
    - Some ARXML editors seem to sometimes include a dot in integer
      numbers (e.g., they produce "123.0" instead of "123")
    """

    # the string literals "true" and "false" are interpreted as 1 and 0
    if in_string == 'true':
        return 1
    elif in_string == 'false':
        return 0
    # the input string contains a dot -> floating point value
    elif '.' in in_string:
        tmp = float(in_string)

        if not allow_float and tmp != int(tmp):
            raise ValueError('Floating point value specified where integer '
                             'is required')

        # if an integer is required but a .0 floating point value is
        # specified, we accept the input anyway. (this seems to be an
        # ambiguity in the AUTOSAR specification.)
        return tmp if allow_float else int(tmp)

    in_string = in_string.strip()
    if not in_string:
        return 0
    elif in_string[0] == '0' and in_string[1:2].isdigit():
        # interpret strings starting with a 0 as octal because
        # python's int(*, 0) does not for some reason.
        return int(in_string, 8)

    return int(in_string, 0) # autodetect the base
