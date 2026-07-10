from typing import Generic

from .dbc_attribute_definition import (
    DbcAttributeDefinition,
    DbcAttributeValueType,
)


class DbcAttribute(Generic[DbcAttributeValueType]):
    """An attribute that can be associated with nodes/messages/signals.

    """

    def __init__(self,
                 value: DbcAttributeValueType,
                 definition: DbcAttributeDefinition[DbcAttributeValueType]) -> None:
        self._value: DbcAttributeValueType = value
        self._definition: DbcAttributeDefinition[DbcAttributeValueType] = definition

    @property
    def name(self) -> str:
        """The attribute name as a string.

        """

        return self._definition.name

    @property
    def value(self) -> DbcAttributeValueType:
        """The value that this attribute has.

        """

        return self._value

    @value.setter
    def value(self, value: DbcAttributeValueType) -> None:
        self._value = value

    @property
    def formatted_value(self) -> str:
        """The value of the attribute as it appears in the DBC file

        """
        if self.definition.type_name == "STRING":
            return f'"{self._value}"'

        return f'{self._value}'

    @property
    def definition(self) -> DbcAttributeDefinition[DbcAttributeValueType]:
        """The attribute definition.

        """

        return self._definition

    def __repr__(self) -> str:
        return f"attribute('{self.name}', {self.value})"


DbcAttributeType = DbcAttribute[str] | DbcAttribute[int] | DbcAttribute[float]
