# DID data.
import logging
from typing import Dict, List, Literal, Optional, Tuple

from ...typechecking import ByteOrder, Choices

logger = logging.getLogger(__name__)

_SEGMENTDICTDEF = Dict[Literal["raw", "scaled"], Tuple[float, float]]
_SEGMENTLISTLIMITTYPE = Optional[List[_SEGMENTDICTDEF]]


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
        minimum: Optional[float] = None,
        maximum: Optional[float] = None,
        pcw_segments: Optional[List[Tuple[float, float]]] = None,
        unit: Optional[str] = None,
        choices: Optional[Choices] = None,
    ) -> None:
        #: The data name as a string.
        self.name: str = name

        #: The scale factor of the data value.
        #  For piecewise linear data each list element
        #  represents the scale for the respective segment
        scale = scale or [1.0]
        self.scale: List[float] = scale

        #: The offset of the data value.
        #  For piecewise linear data each list element
        #  represents the offset for the respective segment
        offset = offset or [0.0]
        self.offset: List[float] = offset

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

        #: This stores start and end points of piecewise linear segments
        #  None for all other types
        self.pcw_segments: _SEGMENTLISTLIMITTYPE = None
        if pcw_segments is not None:
            self._initialize_segment_limits(pcw_segments)

        # ToDo: Remove once types are handled properly.
        self.is_float: bool = False
        self.is_signed: bool = False

    def _initialize_segment_limits(self, pcw_segments: List[Tuple[float, float]]) -> None:
        def convert(v, o, f):
            return v * f + o

        self.pcw_segments = []
        last_phys_max = None
        for i, segment in enumerate(pcw_segments):
            start, end = segment
            params = [self.offset[i], self.scale[i]]
            parsed_segment: _SEGMENTDICTDEF = {
                "raw": (start, end),
                "scaled": (convert(start, *params), convert(end, *params))
            }
            self.pcw_segments.append(parsed_segment)
            if last_phys_max is None:
                last_phys_max = parsed_segment["scaled"][1]
                continue

            if last_phys_max >= parsed_segment["scaled"][0]:
                logger.warning(f"Piecewise linear type: {self.name} has overlapping segments! "
                               f"Segment {i} starts at phys val {parsed_segment['scaled'][0]} "
                               f"but one of the prev segments ended at {last_phys_max}. "
                               "Encoding might be ambiguous.")

            last_phys_max = max(parsed_segment["scaled"][1], last_phys_max)

    def get_offset_scaling(self, raw_val: Optional[float] = None, scaled: Optional[float] = None) -> Tuple[float, float]:
        if raw_val is None and scaled is None:
            raise ValueError("Either raw or scaled value needs to be given!")

        if self.pcw_segments is None or len(self.pcw_segments) == 0:
            return self.offset[0], self.scale[0]

        type_specifer: Literal["raw", "scaled"] = "raw"
        val = raw_val
        if raw_val is None:
            val = scaled
            type_specifer = "scaled"

        relevant_segments = []  # only for err text
        for i, segment in enumerate(self.pcw_segments):
            start, end = segment[type_specifer]
            relevant_segments.append((start, end))
            if start <= val <= end:  # type: ignore
                return self.offset[i], self.scale[i]
        else:
            err_text = [
                f"{start} <= x <= {end}"
                for start, end in relevant_segments
            ]
            raise ValueError(
                f"Value {val} is not in ranges: \n {' OR '.join(err_text)}"
            )

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
            self.minimum,
            self.maximum,
            self.unit,
            choices,
        )
