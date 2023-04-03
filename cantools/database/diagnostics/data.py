# DID data.
import logging
from typing import List, Optional, Union

from ...typechecking import ByteOrder, Choices, PiecewiseSegment
from ..signalbase import SignalBase

logger = logging.getLogger(__name__)


class Data(SignalBase):
    """A data data with position, size, unit and other information. A data
    is part of a DID.

    """

    def __init__(
        self,
        name: str,
        start: int,
        length: int,
        byte_order: ByteOrder = "little_endian",
        scale: Union[List[float], float] = 1.0,
        offset: Union[List[float], float] = 0.0,
        minimum: Optional[float] = None,
        maximum: Optional[float] = None,
        segment_boundaries: Optional[PiecewiseSegment] = None,
        unit: Optional[str] = None,
        choices: Optional[Choices] = None,
    ) -> None:
        super().__init__(
            name=name,
            start=start,
            length=length,
            byte_order=byte_order,
            is_signed=False,
            scale=scale,
            offset=offset,
            minimum=minimum,
            maximum=maximum,
            unit=unit,
            choices=choices,
            is_float=False,
            segment_boundaries=segment_boundaries,
        )

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
            self.scale,
            self.offset,
            self.minimum,
            self.maximum,
            self.unit,
            choices,
        )
