from abc import ABC, abstractmethod
from typing import Dict, Optional, Union

from ...typechecking import Choices, SignalValueType
from ..namedsignalvalue import NamedSignalValue


class BaseConversion(ABC):
    scale: float
    offset: float
    is_float: bool
    choices: Optional[Choices]

    @abstractmethod
    def raw_to_scaled(
        self,
        raw_value: Union[int, float],
        decode_choices: bool = True,
    ) -> SignalValueType:
        """Convert an internal raw value according to the defined scaling or value table.

        :param raw_value:
            The raw value
        :param decode_choices:
            If `decode_choices` is ``False`` scaled values are not
            converted to choice strings (if available).
        :return:
            The calculated scaled value
        """
        raise NotImplementedError

    @abstractmethod
    def scaled_to_raw(self, scaled_value: SignalValueType) -> Union[int, float]:
        """Convert a scaled value to the internal raw value.

        :param scaled_value:
            The scaled value.
        :return:
            The internal raw value.
        """
        raise NotImplementedError


class IdentityConversion(BaseConversion):
    scale = 1
    offset = 0
    choices = None

    def __init__(self, is_float: bool) -> None:
        self.is_float = is_float

    def raw_to_scaled(
        self,
        raw_value: Union[int, float],
        decode_choices: bool = True,
    ) -> Union[int, float]:
        return raw_value

    def scaled_to_raw(self, scaled_value: SignalValueType) -> Union[int, float]:
        if not isinstance(scaled_value, (int, float)):
            raise TypeError(
                f"'scaled_value' must have type 'int' or 'float' (is {type(scaled_value)})"
            )
        return scaled_value


class LinearIntegerConversion(BaseConversion):
    is_float = False
    choices = None

    def __init__(self, scale: int, offset: int) -> None:
        self.scale: int = scale
        self.offset: int = offset

    def raw_to_scaled(
        self,
        raw_value: Union[int, float],
        decode_choices: bool = True,
    ) -> SignalValueType:
        return raw_value * self.scale + self.offset

    def scaled_to_raw(self, scaled_value: SignalValueType) -> Union[int, float]:
        if not isinstance(scaled_value, (int, float)):
            raise TypeError(
                f"'scaled_value' must have type 'int' or 'float' (is {type(scaled_value)})"
            )
        return round((scaled_value - self.offset) / self.scale)


class LinearConversion(BaseConversion):
    choices = None

    def __init__(self, scale: float, offset: float, is_float: bool) -> None:
        self.scale = scale
        self.offset = offset
        self.is_float = is_float

    def raw_to_scaled(
        self,
        raw_value: Union[int, float],
        decode_choices: bool = True,
    ) -> SignalValueType:
        return raw_value * self.scale + self.offset

    def scaled_to_raw(self, scaled_value: SignalValueType) -> Union[int, float]:
        if not isinstance(scaled_value, (int, float)):
            raise TypeError(
                f"'scaled_value' must have type 'int' or 'float' (is {type(scaled_value)})"
            )
        _raw = (scaled_value - self.offset) / self.scale
        if self.is_float:
            return _raw
        return round(_raw)


class NamedSignalConversion(BaseConversion):
    is_float = False

    def __init__(self, scale: float, offset: float, choices: Choices) -> None:
        self.scale = scale
        self.offset = offset
        self._inverse_choices: Optional[Dict[str, int]] = None
        self.choices = choices
        self._update_choices()

        self._conversion = conversion_factory(
            scale=self.scale,
            offset=self.offset,
            choices=None,
            is_float=False,
        )

    def raw_to_scaled(
        self,
        raw_value: Union[int, float],
        decode_choices: bool = True,
    ) -> SignalValueType:
        if decode_choices:
            try:
                return self.choices[raw_value]  # type: ignore[index]
            except (AttributeError, KeyError):
                pass
        return self._conversion.raw_to_scaled(raw_value, False)

    def scaled_to_raw(self, scaled_value: SignalValueType) -> Union[int, float]:
        if isinstance(scaled_value, (int, float)):
            return self._conversion.scaled_to_raw(scaled_value)

        if isinstance(scaled_value, (str, NamedSignalValue)):
            return self.choice_to_number(str(scaled_value))

        raise TypeError

    def set_choices(self, choices: Optional[Choices]) -> None:
        self.choices: Optional[Choices] = choices
        self._update_choices()

    def _update_choices(self) -> None:
        # we simply assume that the choices are invertible
        if self.choices:
            self._inverse_choices = {str(x[1]): x[0] for x in self.choices.items()}
        else:
            self._inverse_choices = None

    def choice_to_number(self, choice: Union[str, "NamedSignalValue"]) -> int:
        if self._inverse_choices is None:
            raise KeyError(f"Signal does not exhibit any choices")
        return self._inverse_choices[str(choice)]


def conversion_factory(
    scale: float = 1,
    offset: float = 0,
    choices: Optional[Choices] = None,
    is_float: bool = False,
) -> BaseConversion:
    if choices is None:
        if scale == 1 and offset == 0:
            return IdentityConversion(is_float=is_float)

        if _is_integer(scale) and _is_integer(offset) and not is_float:
            return LinearIntegerConversion(scale=int(scale), offset=int(offset))

        return LinearConversion(scale=scale, offset=offset, is_float=is_float)

    return NamedSignalConversion(scale=scale, offset=offset, choices=choices)


def _is_integer(value: Union[int, float]) -> bool:
    return isinstance(value, int) or value.is_integer()
