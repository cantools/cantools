from typing import Generic

from .attribute_definition import AttributeDefinition, AttributeValueTypeVar


class Attribute(Generic[AttributeValueTypeVar]):
    """An attribute that can be associated with nodes/messages/signals.

    """

    def __init__(self,
                 value: AttributeValueTypeVar,
                 definition: AttributeDefinition[AttributeValueTypeVar]) -> None:
        self._value: AttributeValueTypeVar = value
        self._definition: AttributeDefinition[AttributeValueTypeVar] = definition

    @property
    def name(self) -> str:
        """The attribute name as a string.

        """

        return self._definition.name

    @property
    def value(self) -> AttributeValueTypeVar:
        """The value that this attribute has.

        """

        return self._value

    @value.setter
    def value(self, value: AttributeValueTypeVar) -> None:
        self._value = value

    @property
    def formatted_value(self) -> str:
        """The value of the attribute as it appears in the DBC file

        """
        if self.definition.type_name == "STRING":
            return f'"{self._value}"'

        return f'{self._value}'

    @property
    def definition(self) -> AttributeDefinition[AttributeValueTypeVar]:
        """The attribute definition.

        """

        return self._definition

    def __repr__(self) -> str:
        return f"attribute('{self.name}', {self.value})"


AttributeType = Attribute[str] | Attribute[int] | Attribute[float]
