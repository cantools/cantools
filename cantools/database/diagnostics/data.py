# DID data.
from typing import Optional

from cantools.typechecking import ByteOrder, Choices


class Data(object):
    """A data data with position, size, unit and other information. A data
    is part of a DID.

    """

    def __init__(self,
                 name: str,
                 start: int,
                 length: int,
                 byte_order: ByteOrder = 'little_endian',
                 scale: float = 1,
                 offset: float = 0,
                 minimum: Optional[float] = None,
                 maximum: Optional[float] = None,
                 unit: Optional[str] = None,
                 choices: Optional[Choices] = None,
                 ) -> None:
        #: The data name as a string.
        self.name: str = name

        #: The scale factor of the data value.
        self.scale: float = scale

        #: The offset of the data value.
        self.offset: float = offset

        #: The start bit position of the data within its DID.
        self.start: int = start

        #: The length of the data in bits.
        self.length = length

        #: Data byte order as ``'little_endian'`` or ``'big_endian'``.
        self.byte_order: ByteOrder = byte_order

        #: The minimum value of the data, or ``None`` if unavailable.
        self.minimum: Optional[float] = minimum

        #: The maximum value of the data, or ``None`` if unavailable.
        self.maximum: Optional[float] = maximum

        #: The unit of the data as a string, or ``None`` if unavailable.
        self.unit = unit

        #: A dictionary mapping data values to enumerated choices, or ``None``
        #: if unavailable.
        self.choices: Optional[Choices] = choices

        # ToDo: Remove once types are handled properly.
        self.is_float: bool = False
        self.is_signed: bool = False

    def choice_string_to_number(self, string: str) -> int:
        if self.choices is None:
            raise ValueError(f"Data {self.name} has no choices.")

        for choice_number, choice_string in self.choices.items():
            if choice_string == string:
                return choice_number

        raise KeyError(f"Choice {string} not found in Data {self.name}.")

    def __repr__(self) -> str:
        if self.choices is None:
            choices = None
        else:
            choices = '{{{}}}'.format(', '.join(
                ["{}: '{}'".format(value, text)
                 for value, text in self.choices.items()]))

        return "data('{}', {}, {}, '{}', {}, {}, {}, {}, '{}', {})".format(
            self.name,
            self.start,
            self.length,
            self.byte_order,
            self.scale,
            self.offset,
            self.minimum,
            self.maximum,
            self.unit,
            choices)
