# A CAN signal.
import contextlib
import decimal
from typing import TYPE_CHECKING, Any, Dict, List, Optional, Union

from ...typechecking import ByteOrder, Choices, Comments, SignalValueType

if TYPE_CHECKING:
    from ...database.can.formats.dbc import DbcSpecifics


class Decimal:
    """Holds the same values as
    :attr:`~cantools.database.can.Signal.scale`,
    :attr:`~cantools.database.can.Signal.offset`,
    :attr:`~cantools.database.can.Signal.minimum` and
    :attr:`~cantools.database.can.Signal.maximum`, but as
    ``decimal.Decimal`` instead of ``int`` and ``float`` for higher
    precision (no rounding errors).

    """

    def __init__(self,
                 scale: decimal.Decimal,
                 offset: decimal.Decimal,
                 minimum: Optional[decimal.Decimal] = None,
                 maximum: Optional[decimal.Decimal] = None) -> None:
        self._scale = scale
        self._offset = offset
        self._minimum = minimum
        self._maximum = maximum

    @property
    def scale(self) -> decimal.Decimal:
        """The scale factor of the signal value as ``decimal.Decimal``.

        """

        return self._scale

    @scale.setter
    def scale(self, value: decimal.Decimal) -> None:
        self._scale = value

    @property
    def offset(self) -> decimal.Decimal:
        """The offset of the signal value as ``decimal.Decimal``.

        """

        return self._offset

    @offset.setter
    def offset(self, value: decimal.Decimal) -> None:
        self._offset = value

    @property
    def minimum(self) -> Optional[decimal.Decimal]:
        """The minimum value of the signal as ``decimal.Decimal``, or ``None``
        if unavailable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value: Optional[decimal.Decimal]) -> None:
        self._minimum = value

    @property
    def maximum(self) -> Optional[decimal.Decimal]:
        """The maximum value of the signal as ``decimal.Decimal``, or ``None``
        if unavailable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value: Optional[decimal.Decimal]) -> None:
        self._maximum = value


class NamedSignalValue:
    """Represents a named value of a signal.

    Named values map an integer number to a human-readable
    string. Some file formats like ARXML support specifying
    descriptions for the named value.
    """

    def __init__(self,
                 value: int,
                 name: str,
                 comments: Optional[Dict[str, str]] = None,
                 ) -> None:
        self._name = name
        self._value = value
        self._comments = comments or {}

    @property
    def name(self) -> str:
        """The text intended for human consumption which the specified integer
        is mapped to.
        """

        return self._name

    @property
    def value(self) -> int:
        """The integer value that gets mapped
        """

        return self._value

    @property
    def comments(self) -> Dict[str, str]:
        """The descriptions of the named value

        This is a dictionary containing the descriptions in multiple
        languages. The dictionary is indexed by the language.

        Example:

        .. code:: text

          # retrieve the English comment of the named value or an empty
          # string if none was specified.
          named_value.comments.get("EN", "")

        """

        return self._comments

    def __str__(self) -> str:
        return f"{self._name}"

    def __repr__(self) -> str:
        return f"'{self._name}'"

    def __eq__(self, x: Any) -> bool:
        if isinstance(x, NamedSignalValue):
            return \
                x.value == self.value \
                and x.name == self.name \
                and x.comments == self.comments
        elif isinstance(x, str):
            return x == self.name

        return False


class Signal:
    """A CAN signal with position, size, unit and other information. A
    signal is part of a message.

    Signal bit numbering in a message:

    .. code:: text

       Byte:       0        1        2        3        4        5        6        7
              +--------+--------+--------+--------+--------+--------+--------+--------+--- - -
              |        |        |        |        |        |        |        |        |
              +--------+--------+--------+--------+--------+--------+--------+--------+--- - -
       Bit:    7      0 15     8 23    16 31    24 39    32 47    40 55    48 63    56

    Big endian signal with start bit 2 and length 5 (0=LSB, 4=MSB):

    .. code:: text

       Byte:       0        1        2        3
              +--------+--------+--------+--- - -
              |    |432|10|     |        |
              +--------+--------+--------+--- - -
       Bit:    7      0 15     8 23    16 31

    Little endian signal with start bit 2 and length 9 (0=LSB, 8=MSB):

    .. code:: text

       Byte:       0        1        2        3
              +--------+--------+--------+--- - -
              |543210| |    |876|        |
              +--------+--------+--------+--- - -
       Bit:    7      0 15     8 23    16 31

    """

    def __init__(self,
                 name: str,
                 start: int,
                 length: int,
                 byte_order: ByteOrder = 'little_endian',
                 is_signed: bool = False,
                 raw_initial: Optional[Union[int, float]] = None,
                 raw_invalid: Optional[Union[int, float]] = None,
                 scale: float = 1,
                 offset: float = 0,
                 minimum: Optional[float] = None,
                 maximum: Optional[float] = None,
                 unit: Optional[str] = None,
                 choices: Optional[Choices] = None,
                 dbc_specifics: Optional["DbcSpecifics"] = None,
                 comment: Optional[Union[str, Comments]] = None,
                 receivers: Optional[List[str]] = None,
                 is_multiplexer: bool = False,
                 multiplexer_ids: Optional[List[int]] = None,
                 multiplexer_signal: Optional[str] = None,
                 is_float: bool = False,
                 decimal: Optional[Decimal] = None,
                 spn: Optional[int] = None
                 ) -> None:
        # avoid using properties to improve encoding/decoding performance

        #: The signal name as a string.
        self.name: str = name

        #: The scale factor of the signal value.
        self.scale: float = scale

        #: The offset of the signal value.
        self.offset: float = offset

        #: ``True`` if the signal is a float, ``False`` otherwise.
        self.is_float: bool = is_float

        #: The scaled minimum value of the signal, or ``None`` if unavailable.
        self.minimum: Optional[float] = minimum

        #: The scaled maximum value of the signal, or ``None`` if unavailable.
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

        #: The raw initial value of the signal, or ``None`` if unavailable.
        self.raw_initial: Optional[Union[int, float]] = raw_initial

        #: The scaled initial value of the signal, or ``None`` if unavailable.
        self.initial: Optional[SignalValueType] = (
            self.raw_to_scaled(raw_initial) if raw_initial is not None else None
        )

        #: The raw value representing that the signal is invalid,
        #: or ``None`` if unavailable.
        self.raw_invalid: Optional[Union[int, float]] = raw_invalid

        #: The scaled value representing that the signal is invalid,
        #: or ``None`` if unavailable.
        self.invalid: Optional[SignalValueType] = (
            self.raw_to_scaled(raw_invalid) if raw_invalid is not None else None
        )

        #: The high precision values of
        #: :attr:`~cantools.database.can.Signal.scale`,
        #: :attr:`~cantools.database.can.Signal.offset`,
        #: :attr:`~cantools.database.can.Signal.minimum` and
        #: :attr:`~cantools.database.can.Signal.maximum`.
        #:
        #: See :class:`~cantools.database.can.signal.Decimal` for more
        #: details.
        self.decimal: Optional[Decimal] = decimal

        #: The unit of the signal as a string, or ``None`` if unavailable.
        self.unit: Optional[str] = unit

        #: An object containing dbc specific properties like e.g. attributes.
        self.dbc: Optional["DbcSpecifics"] = dbc_specifics

        #: A list of all receiver nodes of this signal.
        self.receivers: List[str] = receivers or []

        #: ``True`` if this is the multiplexer signal in a message, ``False``
        #: otherwise.
        self.is_multiplexer: bool = is_multiplexer

        #: The multiplexer ids list if the signal is part of a multiplexed
        #: message, ``None`` otherwise.
        self.multiplexer_ids: Optional[List[int]] = multiplexer_ids

        #: The multiplexer signal if the signal is part of a multiplexed
        #: message, ``None`` otherwise.
        self.multiplexer_signal: Optional[str] = multiplexer_signal

        #: The J1939 Suspect Parameter Number (SPN) value if the signal
        #: has this attribute, ``None`` otherwise.
        self.spn: Optional[int] = spn

        #: The dictionary with the descriptions of the signal in multiple
        #: languages. ``None`` if unavailable.
        self.comments: Optional[Comments]

        # if the 'comment' argument is a string, we assume that is an
        # english comment. this is slightly hacky because the
        # function's behavior depends on the type of the passed
        # argument, but it is quite convenient...
        if isinstance(comment, str):
            # use the first comment in the dictionary as "The" comment
            self.comments = {None: comment}
        else:
            # assume that we have either no comment at all or a
            # multi-lingual dictionary
            self.comments = comment

    def raw_to_scaled(self, raw: Union[int, float], decode_choices: bool = True) -> SignalValueType:
        """Convert an internal raw value according to the defined scaling or value table.

        :param raw:
            The raw value
        :param decode_choices:
            If `decode_choices` is ``False`` scaled values are not
            converted to choice strings (if available).
        :return:
            The calculated scaled value
        """
        if decode_choices:
            with contextlib.suppress(KeyError, TypeError):
                return self.choices[raw]  # type: ignore[index]

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

        return self.choice_string_to_number(str(scaled))

    @property
    def comment(self) -> Optional[str]:
        """The signal comment, or ``None`` if unavailable.

        Note that we implicitly try to return the English comment if
        multiple languages were specified.

        """
        if self.comments is None:
            return None
        elif self.comments.get(None) is not None:
            return self.comments.get(None)
        elif self.comments.get("FOR-ALL") is not None:
            return self.comments.get("FOR-ALL")

        return self.comments.get('EN')

    @comment.setter
    def comment(self, value: Optional[str]) -> None:
        if value is None:
            self.comments = None
        else:
            self.comments = {None: value}

    def choice_string_to_number(self, string: str) -> int:
        if self.choices is None:
            raise ValueError(f"Signal {self.name} has no choices.")

        for choice_number, choice_value in self.choices.items():
            if str(choice_value) == str(string):
                return choice_number

        raise KeyError(f"Choice {string} not found in Signal {self.name}.")

    def __repr__(self) -> str:
        if self.choices is None:
            choices = None
        else:
            choices = '{{{}}}'.format(', '.join(
                [f"{value}: '{text}'"
                 for value, text in self.choices.items()]))

        return \
            f"signal(" \
            f"'{self.name}', " \
            f"{self.start}, " \
            f"{self.length}, " \
            f"'{self.byte_order}', " \
            f"{self.is_signed}, " \
            f"{self.raw_initial}, " \
            f"{self.scale}, " \
            f"{self.offset}, " \
            f"{self.minimum}, " \
            f"{self.maximum}, " \
            f"'{self.unit}', " \
            f"{self.is_multiplexer}, " \
            f"{self.multiplexer_ids}, " \
            f"{choices}, " \
            f"{self.spn}, " \
            f"{self.comments})"
