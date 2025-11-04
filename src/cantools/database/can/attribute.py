from typing import Generic, Union

from .attribute_definition import AttributeDefinition, AttributeValueType


class Attribute(Generic[AttributeValueType]):
    """An attribute that can be associated with nodes/messages/signals.

    """

    def __init__(self,
                 value: AttributeValueType,
                 definition: AttributeDefinition[AttributeValueType]) -> None:
        self._value: AttributeValueType = value
        self._definition: AttributeDefinition[AttributeValueType] = definition

    @property
    def name(self) -> str:
        """The attribute name as a string.

        """

        return self._definition.name

    @property
    def value(self) -> AttributeValueType:
        """The value that this attribute has.

        """

        return self._value

    @value.setter
    def value(self, value: AttributeValueType) -> None:
        self._value = value

    @property
    def definition(self) -> AttributeDefinition[AttributeValueType]:
        """The attribute definition.

        """

        return self._definition

    def __repr__(self) -> str:
        return f"attribute('{self.name}', {self.value})"


AttributeType = Union[Attribute[str], Attribute[int], Attribute[float]]
