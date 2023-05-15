# DID data.
from typing import Optional, Union

from ...typechecking import ByteOrder, Choices, SignalValueType
from ..can.signal import NamedSignalValue


class Data:
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

    def raw_to_scaled(
        self, raw: Union[int, float], decode_choices: bool = True
    ) -> SignalValueType:
        """Convert an internal raw value according to the defined scaling or value table.

        :param raw:
            The raw value
        :param decode_choices:
            If `decode_choices` is ``False`` scaled values are not
            converted to choice strings (if available).
        :return:
            The calculated scaled value
        """
        if decode_choices and self.choices and raw in self.choices:
            assert isinstance(raw, int)
            return self.choices[raw]

        if self.offset == 0 and self.scale == 1:
            # treat special case to avoid introduction of unnecessary rounding error
            return raw
        return raw * self.scale + self.offset

    def scaled_to_raw(self, scaled: SignalValueType) -> Union[int, float]:
        """Convert a scaled value to the internal raw value.

        :param scaled:
            The scaled value.
        :return:
            The internal raw value.
        """
        if isinstance(scaled, (float, int)):
            _transform = float if self.is_float else round
            if self.offset == 0 and self.scale == 1:
                # treat special case to avoid introduction of unnecessary rounding error
                return _transform(scaled)  # type: ignore[operator,no-any-return]

            return _transform((scaled - self.offset) / self.scale)  # type: ignore[operator,no-any-return]

        if isinstance(scaled, (str, NamedSignalValue)):
            return self.choice_to_number(str(scaled))

        raise TypeError(f"Conversion of type {type(scaled)} is not supported.")

    def choice_to_number(self, string: Union[str, NamedSignalValue]) -> int:
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
                [f"{value}: '{text}'"
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
