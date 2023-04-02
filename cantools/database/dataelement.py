import bisect
import logging
from typing import List, Optional, Tuple, Union

from ..typechecking import ByteOrder, Choices, PiecewiseSegment

logger = logging.getLogger(__name__)


class DataElement:
    def __init__(
        self,
        name: str,
        start: int,
        length: int,
        byte_order: ByteOrder = "little_endian",
        is_signed: bool = False,
        scale: Union[List[float], float] = 1.0,
        offset: Union[List[float], float] = 0.0,
        minimum: Optional[float] = None,
        maximum: Optional[float] = None,
        unit: Optional[str] = None,
        choices: Optional[Choices] = None,
        is_float: bool = False,
        segment_boundaries: Optional[PiecewiseSegment] = None,
    ) -> None:
        # avoid using properties to improve encoding/decoding performance

        #: The signal name as a string.
        self.name: str = name

        #: The scale factor of the data value.
        #: For piecewise linear data each list element
        #: represents the scale for the respective segment
        self._scale: Union[List[float], float] = scale

        #: The offset of the data value.
        #: For piecewise linear data each list element
        #: represents the offset for the respective segment
        self._offset: Union[List[float], float] = offset

        #: ``True`` if the signal is a float, ``False`` otherwise.
        self.is_float: bool = is_float

        #: The minimum value of the signal, or ``None`` if unavailable.
        self.minimum: Optional[float] = minimum

        #: The maximum value of the signal, or ``None`` if unavailable.
        self.maximum: Optional[float] = maximum

        #: "A dictionary mapping signal values to enumerated choices, or
        #: ``None`` if unavailable.
        self.choices: Optional[Choices] = choices

        #: The start bit position of the signal within its message.
        self.start: int = start

        #: The length of the signal in bits.
        self.length: int = length

        #: Signal byte order as ``'little_endian'`` or ``'big_endian'``.
        self.byte_order: ByteOrder = byte_order

        #: ``True`` if the signal is signed, ``False`` otherwise. Ignore this
        #: attribute if :data:`~cantools.db.Signal.is_float` is
        #: ``True``.
        self.is_signed: bool = is_signed

        #: The unit of the signal as a string, or ``None`` if unavailable.
        self.unit: Optional[str] = unit

        #: Stores the raw start and end points of piecewise linear segments
        #: empty for all other types
        self.segment_boundaries_raw: Optional[Tuple[List[float], List[float]]] = None

        #: Stores the scaled start and end points of piecewise linear segments
        #: empty for all other types
        self.segment_boundaries_scaled: Optional[Tuple[List[float], List[float]]] = None
        if segment_boundaries is not None:
            if not isinstance(scale, list) or not isinstance(offset, list):
                raise ValueError(
                    "Params scale and offset need to be of type list "
                    "if segment boundaries are defined."
                )

            self._initialize_segment_limits(segment_boundaries)

    def _initialize_segment_limits(self, segment_boundaries: PiecewiseSegment) -> None:
        def convert(v, o, f):
            return v * f + o

        last_phys_max = None
        self.segment_boundaries_raw = ([], [])
        self.segment_boundaries_scaled = ([], [])
        for i, segment in enumerate(segment_boundaries):
            self.segment_boundaries_raw[0].append(segment[0])
            self.segment_boundaries_raw[1].append(segment[1])
            start, end = segment
            params = [self._offset[i], self._scale[i]]  # type: ignore
            scaled_segment = (convert(start, *params), convert(end, *params))
            self.segment_boundaries_scaled[0].append(scaled_segment[0])
            self.segment_boundaries_scaled[1].append(scaled_segment[1])
            if last_phys_max is None:
                last_phys_max = scaled_segment[1]
                continue

            if last_phys_max >= scaled_segment[0]:
                logger.warning(
                    f"Piecewise linear type: {self.name} has overlapping segments! "
                    f"Segment {i} starts at phys val {scaled_segment[0]} "
                    f"but one of the prev segments ended at {last_phys_max}. "
                    "Encoding might be ambiguous."
                )

            last_phys_max = max(scaled_segment[1], last_phys_max)

        if len(self.segment_boundaries_raw[0]) == 0:
            # no segments found, save lookups later by setting to None
            self.segment_boundaries_raw = None
            self.segment_boundaries_scaled = None

    def choice_string_to_number(self, string: str) -> int:
        if self.choices is None:
            raise ValueError(f"Signal {self.name} has no choices.")

        for choice_number, choice_value in self.choices.items():
            if str(choice_value) == str(string):
                return choice_number

        raise KeyError(f"Choice {string} not found in data element {self.name}.")

    def _get_offset_scaling_from_list(
        self, val: float, segments: Tuple[List[float], List[float]]
    ) -> Tuple[float, float]:
        i = (
            bisect.bisect_right(segments[0], val) - 1
        )  # lower limits: rightmost value less than or equal val
        i2 = bisect.bisect_left(
            segments[1], val
        )  # higher limits: leftmost value greater than or equal val
        if i == i2:
            return self._offset[i], self._scale[i]  # type: ignore
        else:
            err_text = [
                f"{start} <= x <= {end}" for start, end in zip(segments[0], segments[1])
            ]
            raise ValueError(
                f"Value {val} is not in ranges: \n {' OR '.join(err_text)}"
            )

    def get_offset_scaling_from_raw(
        self, raw_val: Optional[float] = None
    ) -> Tuple[float, float]:
        """Get the applicable offset and scaling for the given raw value.

        If data type only defines one set of offset/scaling then
        the `raw_val` param can be omitted
        """
        if raw_val is None or self.segment_boundaries_raw is None:
            try:
                return self._offset[0], self._scale[0]  # type: ignore
            except TypeError:
                return self.offset, self.scale

        return self._get_offset_scaling_from_list(raw_val, self.segment_boundaries_raw)

    def get_offset_scaling_from_scaled(
        self, scaled_val: Optional[float] = None
    ) -> Tuple[float, float]:
        """Get the applicable offset and scaling for the given scaled value.

        If data type only defines one set of offset/scaling then
        the `scaled_val` param can be omitted
        """
        if scaled_val is None or self.segment_boundaries_scaled is None:
            try:
                return self._offset[0], self._scale[0]  # type: ignore
            except TypeError:
                return self.offset, self.scale

        return self._get_offset_scaling_from_list(
            scaled_val, self.segment_boundaries_scaled
        )

    @property
    def offset(self):
        """Return first or only offset element"""
        if isinstance(self._offset, list):
            return self._offset[0]
        else:
            return self._offset

    @offset.setter
    def offset(self, offset: float) -> None:
        """Set offset"""
        self._offset = offset

    @property
    def scale(self):
        """Return first or only scale element"""
        if isinstance(self._scale, list):
            return self._scale[0]
        else:
            return self._scale

    @scale.setter
    def scale(self, scale: float) -> None:
        """Set scale"""
        self._scale = scale
