# Store the specific DBC format properties of objects

from __future__ import annotations

from collections import OrderedDict
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from cantools.typechecking import Choices

    from .attribute import AttributeType
    from .attribute_definition import AttributeDefinitionType
    from .dbc_environment_variable import DbcEnvironmentVariable
    from .dbc_loader import DbcRelationAttributes

class DbcSpecifics:

    def __init__(self,
                 *,
                 attributes: OrderedDict[str, AttributeType] | None = None,
                 attribute_definitions: OrderedDict[str, AttributeDefinitionType] | None = None,
                 environment_variables: OrderedDict[str, DbcEnvironmentVariable] | None = None,
                 value_tables: OrderedDict[str, Choices] | None = None,
                 relation_attributes: DbcRelationAttributes | None = None,
                 relation_attribute_definitions: OrderedDict[str, AttributeDefinitionType] | None = None) -> None:
        self._attributes = attributes or OrderedDict()
        self._attribute_definitions = attribute_definitions or OrderedDict()
        self._environment_variables = environment_variables or OrderedDict()
        self._value_tables = value_tables or OrderedDict()
        self._relation_attributes = relation_attributes
        self._relation_attribute_definitions = relation_attribute_definitions or OrderedDict()

    @property
    def attributes(self) -> OrderedDict[str, AttributeType]:
        """The DBC specific attributes of the parent object (database, node,
        message or signal) as a dictionary.

        """

        return self._attributes

    @property
    def attribute_definitions(self) -> OrderedDict[str, AttributeDefinitionType]:
        """The DBC specific attribute definitions as dictionary.

        """

        return self._attribute_definitions

    @property
    def value_tables(self) -> OrderedDict[str, Choices]:
        """An ordered dictionary of all value tables. Only valid for DBC
        specifiers on database level.

        """

        return self._value_tables

    @property
    def environment_variables(self) -> OrderedDict[str, DbcEnvironmentVariable]:
        """An ordered dictionary of all environment variables. Only valid for
        DBC specifiers on database level.

        """

        return self._environment_variables

    @property
    def relation_attributes(self) -> DbcRelationAttributes | None:
        """The DBC specific relation attributes.

        """

        return self._relation_attributes

    @property
    def relation_attribute_definitions(self) -> OrderedDict[str, AttributeDefinitionType]:
        """The DBC specific attribute definitions rel as dictionary.

        """

        return self._relation_attribute_definitions
