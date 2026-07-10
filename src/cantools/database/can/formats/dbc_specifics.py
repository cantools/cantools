# Store the specific DBC format properties of objects

from collections import OrderedDict
from typing import Any

from cantools.database.can.dbc_attribute import DbcAttributeType
from cantools.database.can.dbc_attribute_definition import (
    DbcAttributeDefinitionType,
)
from cantools.database.can.environment_variable import EnvironmentVariable
from cantools.typechecking import Choices


class DbcSpecifics:

    def __init__(self,
                 attributes: OrderedDict[str, DbcAttributeType] | None = None,
                 attribute_definitions: OrderedDict[str, DbcAttributeDefinitionType] | None = None,
                 environment_variables: OrderedDict[str, EnvironmentVariable] | None = None,
                 value_tables: OrderedDict[str, Choices] | None = None,
                 attributes_rel: OrderedDict[int, dict[Any, Any]] | None =None,
                 attribute_definitions_rel: OrderedDict[str, DbcAttributeDefinitionType] | None = None) -> None:
        self._attributes = attributes or OrderedDict()
        self._attribute_definitions = attribute_definitions or OrderedDict()
        self._environment_variables = environment_variables or OrderedDict()
        self._value_tables = value_tables or OrderedDict()
        self._attributes_rel = attributes_rel or OrderedDict()
        self._attribute_definitions_rel = attribute_definitions_rel or OrderedDict()

    @property
    def attributes(self) -> OrderedDict[str, DbcAttributeType]:
        """The DBC specific attributes of the parent object (database, node,
        message or signal) as a dictionary.

        """

        return self._attributes

    @property
    def attribute_definitions(self) -> OrderedDict[str, DbcAttributeDefinitionType]:
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
    def environment_variables(self) -> OrderedDict[str, EnvironmentVariable]:
        """An ordered dictionary of all environment variables. Only valid for
        DBC specifiers on database level.

        """

        return self._environment_variables

    @property
    def attributes_rel(self) -> OrderedDict[int, dict[Any, Any]]:
        """The DBC specific attribute rel as dictionary..

        """

        return self._attributes_rel

    @property
    def attribute_definitions_rel(self) -> OrderedDict[str, DbcAttributeDefinitionType]:
        """The DBC specific attribute definitions rel as dictionary.

        """

        return self._attribute_definitions_rel
