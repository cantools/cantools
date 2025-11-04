from typing import Generic, Optional, TypeVar, Union

AttributeValueType = TypeVar('AttributeValueType', str, int, float)

class AttributeDefinition(Generic[AttributeValueType]):
    """A definition of an attribute that can be associated with attributes
    in nodes/messages/signals.

    """

    def __init__(self,
                 name: str,
                 default_value: Optional[AttributeValueType] = None,
                 kind: Optional[str] = None,
                 type_name: Optional[str] = None,
                 minimum: Union[int, float, None] = None,
                 maximum: Union[int, float, None] = None,
                 choices: Optional[list[str]] = None) -> None:
        self._name = name
        self._default_value: Optional[AttributeValueType] = default_value
        self._kind = kind
        self._type_name = type_name
        self._minimum = minimum
        self._maximum = maximum
        self._choices = choices

    @property
    def name(self) -> str:
        """The attribute name as a string.

        """

        return self._name

    @property
    def default_value(self) -> Optional[AttributeValueType]:
        """The default value that this attribute has, or ``None`` if
        unavailable.

        """

        return self._default_value

    @default_value.setter
    def default_value(self, value: Optional[AttributeValueType]) -> None:
        self._default_value = value

    @property
    def kind(self) -> Optional[str]:
        """The attribute kind (BU_, BO_, SG_), or ``None`` if unavailable.

        """

        return self._kind

    @property
    def type_name(self) -> Optional[str]:
        """The attribute type (INT, HEX, FLOAT, STRING, ENUM), or ``None`` if
        unavailable.

        """

        return self._type_name

    @property
    def minimum(self) -> Union[int, float, None]:
        """The minimum value of the attribute, or ``None`` if unavailable.

        """

        return self._minimum

    @minimum.setter
    def minimum(self, value: Union[int, float, None]) -> None:
        self._minimum = value

    @property
    def maximum(self) -> Union[int, float, None]:
        """The maximum value of the attribute, or ``None`` if unavailable.

        """

        return self._maximum

    @maximum.setter
    def maximum(self, value: Union[int, float, None]) -> None:
        self._maximum = value

    @property
    def choices(self) -> Optional[list[str]]:
        """A dictionary mapping attribute values to enumerated choices, or
        ``None`` if unavailable.

        """

        return self._choices

    @choices.setter
    def choices(self, value: list[str]) -> None:
        self._choices = value

    def __repr__(self) -> str:
        return f"attribute_definition('{self._name}', {self._default_value})"

AttributeDefinitionType = Union[AttributeDefinition[str], AttributeDefinition[int], AttributeDefinition[float]]
