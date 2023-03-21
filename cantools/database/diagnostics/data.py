# DID data.
import logging
from typing import Optional, List

from ...typechecking import ByteOrder, Choices

logger = logging.getLogger(__name__)


class Data:
    """A data data with position, size, unit and other information. A data
    is part of a DID.

    """

    def __init__(
        self,
        name: str,
        start: int,
        length: int,
        byte_order: ByteOrder = "little_endian",
        scale: Optional[List[float]] = None,
        offset: Optional[List[float]] = None,
        minimum: Optional[List[float]] = None,
        maximum: Optional[List[float]] = None,
        unit: Optional[str] = None,
        choices: Optional[Choices] = None,
    ) -> None:
        #: The data name as a string.
        self.name: str = name

        #: The scale factor of the data value.
        scale = scale or [1.0]
        self.scale: List[float] = scale

        #: The offset of the data value.
        offset = offset or [0.0]
        self.offset: List[float] = offset

        #: The start bit position of the data within its DID.
        self.start: int = start

        #: The length of the data in bits.
        self.length = length

        #: Data byte order as ``'little_endian'`` or ``'big_endian'``.
        self.byte_order: ByteOrder = byte_order

        #: The minimum value of the data, or ``None`` if unavailable.
        self.minimum: Optional[List[float]] = minimum

        #: The maximum value of the data, or ``None`` if unavailable.
        self.maximum: Optional[List[float]] = maximum

        #: The unit of the data as a string, or ``None`` if unavailable.
        self.unit = unit

        #: A dictionary mapping data values to enumerated choices, or ``None``
        #: if unavailable.
        self.choices: Optional[Choices] = choices

        # ToDo: Remove once types are handled properly.
        self.is_float: bool = False
        self.is_signed: bool = False

    def get_offset_scaling(self, raw_val):
        def convert(v, o, f):
            return (v - o) / f

        if not isinstance(self.minimum, list):
            return self.offset, self.scale

        for i in range(len(self.minimum)):
            if self.minimum[i] <= raw_val <= self.maximum[i]:
                return self.offset[i], self.scale[i]
            elif (
                self.minimum[i]
                <= convert(raw_val, self.offset[i], self.scale[i])
                <= self.maximum[i]
            ):
                return self.offset[i], self.scale[i]
        else:
            err_text = [
                f"{self.minimum[i]} <= x <= {self.maximum[i]}"
                for i in range(len(self.minimum))
            ]
            logger.warning(
                f"Value {raw_val} is not in ranges: \n {' OR '.join(err_text)}"
            )
            return self.offset[0], self.scale[0]

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
            choices = "{{{}}}".format(
                ", ".join(
                    [f"{value}: '{text}'" for value, text in self.choices.items()]
                )
            )

        return "data('{}', {}, {}, '{}', {}, {}, {}, {}, '{}', {})".format(
            self.name,
            self.start,
            self.length,
            self.byte_order,
            self.scale[0],
            self.offset[0],
            self.minimum[0] if self.minimum is not None else None,
            self.maximum[0] if self.maximum is not None else None,
            self.unit,
            choices,
        )
