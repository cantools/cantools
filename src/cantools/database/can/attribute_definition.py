from typing import Generic, TypeVar

AttributeValue = str | int | float
AttributeValueTypeVar = TypeVar('AttributeValueTypeVar', str, int, float)

class AttributeDefinition(Generic[AttributeValueTypeVar]):
    """A definition of an attribute that can be associated with attributes
    in nodes/messages/signals.

    """

    def __init__(self,
                 name: str,
                 default_value: AttributeValueTypeVar | None = None,
                 kind: str | None = None,
                 type_name: str | None = None,
                 minimum: int | float | None = None,
                 maximum: int | float | None = None,
                 choices: list[str] | None = None) -> None:
        self._name = name
        self._default_value: AttributeValueTypeVar | None = default_value
        self._kind = kind
        self._type_name = type_name
        self._minimum = minimum
        self._maximum = maximum
        self._choices = choices or []

    @property
    def name(self) -> str:
        """The attribute name as a string.

        """

        return self._name

    @property
    def default_value(self) -> AttributeValueTypeVar | None:
        """The default value that this attribute has, or ``None`` if
        unavailable.

        """

        return self._default_value

    @default_value.setter
    def default_value(self, value: AttributeValueTypeVar | None) -> None:
        self._default_value = value

    @property
    def kind(self) -> str | None:
        """The attribute kind (BU\\_, BO\\_, SG\\_), or ``None`` if unavailable.

        """

        return self._kind

    @property
    def type_name(self) -> str | None:
        """The attribute type (INT, HEX, FLOAT, STRING, ENUM), or ``None`` if
        unavailable.

        """

        return self._type_name

    @property
    def minimum(self) -> int | float | None:
        """The minimum value of the attribute, or ``None`` if unavailable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value: int | float | None) -> None:
        self._minimum = value

    @property
    def formatted_minimum(self) -> str:
        """The defined minimum value of the attribute definition as it appears in the DBC file

        """
        return '' if self._minimum is None else f' {self._minimum}'

    @property
    def maximum(self) -> int | float | None:
        """The maximum value of the attribute, or ``None`` if unavailable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value: int | float | None) -> None:
        self._maximum = value

    @property
    def formatted_maximum(self) -> str:
        """The defined maximum value of the attribute definition as it appears in the DBC file

        """
        return '' if self._maximum is None else f' {self._maximum}'

    @property
    def choices(self) -> list[str]:
        """A dictionary mapping attribute values to enumerated choices, or
        ``None`` if unavailable.

        """

        return self._choices

    @choices.setter
    def choices(self, value: list[str]) -> None:
        self._choices = value

    def __repr__(self) -> str:
        return f"attribute_definition('{self._name}', {self._default_value})"

AttributeDefinitionType = AttributeDefinition[str] | AttributeDefinition[int] | AttributeDefinition[float]
