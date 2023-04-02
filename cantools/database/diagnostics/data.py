# DID data.
import logging
from typing import List, Optional, Union

from ...typechecking import ByteOrder, Choices, PiecewiseSegment
from ..dataelement import DataElement

logger = logging.getLogger(__name__)


class Data(DataElement):
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
        is_float = False
        is_signed = False

        super().__init__(
            name,
            start,
            length,
            byte_order,
            is_signed,
            scale,
            offset,
            minimum,
            maximum,
            unit,
            choices,
            is_float,
            segment_boundaries,
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

        if isinstance(self.scale, list):
            scale = self.scale[0]
        else:
            scale = self.scale

        if isinstance(self.offset, list):
            offset = self.offset[0]
        else:
            offset = self.offset

        return "data('{}', {}, {}, '{}', {}, {}, {}, {}, '{}', {})".format(
            self.name,
            self.start,
            self.length,
            self.byte_order,
            scale,
            offset,
            self.minimum,
            self.maximum,
            self.unit,
            choices,
        )
