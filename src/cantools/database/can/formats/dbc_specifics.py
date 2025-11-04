# Store the specific DBC format properties of objects

from collections import OrderedDict
from typing import Optional

from cantools.database.can.attribute import AttributeType
from cantools.database.can.attribute_definition import AttributeDefinitionType
from cantools.database.can.environment_variable import EnvironmentVariable
from cantools.typechecking import Choices


class DbcSpecifics:

    def __init__(self,
                 attributes: Optional[OrderedDict[str, AttributeType]] = None,
                 attribute_definitions: Optional[OrderedDict[str, AttributeDefinitionType]] = None,
                 environment_variables: Optional[OrderedDict[str, EnvironmentVariable]] = None,
                 value_tables: Optional[OrderedDict[str, Choices]] = None,
                 attributes_rel: Optional[OrderedDict[str, AttributeType]] = None,
                 attribute_definitions_rel: Optional[OrderedDict[str, AttributeDefinitionType]] = None):
        self._attributes = attributes or OrderedDict()
        self._attribute_definitions = attribute_definitions or OrderedDict()
        self._environment_variables = environment_variables or OrderedDict()
        self._value_tables = value_tables or OrderedDict()
        self._attributes_rel = attributes_rel or OrderedDict()
        self._attribute_definitions_rel = attribute_definitions_rel or OrderedDict()

    @property
    def attributes(self) -> OrderedDict[str, AttributeType]:
        """The DBC specific attributes of the parent object (database, node,
        message or signal) as a dictionary.

        """

        return self._attributes

    @attributes.setter
    def attributes(self, value: OrderedDict[str, AttributeType]) -> None:
        self._attributes = value

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
    def environment_variables(self) -> OrderedDict[str, EnvironmentVariable]:
        """An ordered dictionary of all environment variables. Only valid for
        DBC specifiers on database level.

        """

        return self._environment_variables

    @property
    def attributes_rel(self) -> OrderedDict[str, AttributeType]:
        """The DBC specific attribute rel as dictionary..

        """

        return self._attributes_rel

    @property
    def attribute_definitions_rel(self) -> OrderedDict[str, AttributeDefinitionType]:
        """The DBC specific attribute definitions rel as dictionary.

        """

        return self._attribute_definitions_rel
