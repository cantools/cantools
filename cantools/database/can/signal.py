# A CAN signal.
import decimal
from typing import Optional, Dict, TYPE_CHECKING, List, Any, Union

from cantools.typechecking import Comments, ByteOrder, Choices

if TYPE_CHECKING:
    from cantools.database.can.formats.dbc import DbcSpecifics


class Decimal(object):
    """Holds the same values as
    :attr:`~cantools.database.can.Signal.scale`,
    :attr:`~cantools.database.can.Signal.offset`,
    :attr:`~cantools.database.can.Signal.minimum` and
    :attr:`~cantools.database.can.Signal.maximum`, but as
    ``decimal.Decimal`` instead of ``int`` and ``float`` for higher
    precision (no rounding errors).

    """

    def __init__(self,
                 scale: Optional[decimal.Decimal] = None,
                 offset: Optional[decimal.Decimal] = None,
                 minimum: Optional[decimal.Decimal] = None,
                 maximum: Optional[decimal.Decimal] = None) -> None:
        self._scale = scale
        self._offset = offset
        self._minimum = minimum
        self._maximum = maximum

    @property
    def scale(self) -> Optional[decimal.Decimal]:
        """The scale factor of the signal value as ``decimal.Decimal``.

        """

        return self._scale

    @scale.setter
    def scale(self, value: Optional[decimal.Decimal]) -> None:
        self._scale = value

    @property
    def offset(self) -> Optional[decimal.Decimal]:
        """The offset of the signal value as ``decimal.Decimal``.

        """

        return self._offset

    @offset.setter
    def offset(self, value: Optional[decimal.Decimal]) -> None:
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


class NamedSignalValue(object):
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


class Signal(object):
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
                 initial: Optional[int] = None,
                 invalid: Optional[int] = None,
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

        #: The initial value of the signal, or ``None`` if unavailable.
        self.initial: Optional[int] = initial

        #: The value representing that the signal is invalid,
        #: or ``None`` if unavailable.
        self.invalid: Optional[int] = invalid

        #: The high precision values of
        #: :attr:`~cantools.database.can.Signal.scale`,
        #: :attr:`~cantools.database.can.Signal.offset`,
        #: :attr:`~cantools.database.can.Signal.minimum` and
        #: :attr:`~cantools.database.can.Signal.maximum`.
        #:
        #: See :class:`~cantools.database.can.signal.Decimal` for more
        #: details.
        self.decimal: Decimal = Decimal() if decimal is None else decimal

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
                ["{}: '{}'".format(value, text)
                 for value, text in self.choices.items()]))

        return \
            f"signal(" \
            f"'{self.name}', " \
            f"{self.start}, " \
            f"{self.length}, " \
            f"'{self.byte_order}', " \
            f"{self.is_signed}, " \
            f"{self.initial}, " \
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
