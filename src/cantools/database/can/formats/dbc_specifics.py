# Store the specific DBC format properties of objects

from collections import OrderedDict
from typing import Optional

from cantools.database.can.attribute import Attribute
from cantools.database.can.attribute_definition import AttributeDefinition
from cantools.database.can.environment_variable import EnvironmentVariable
from cantools.typechecking import Choices


class DbcSpecifics:

    def __init__(self,
                 attributes: Optional[OrderedDict[str, Attribute]] = None,
                 attribute_definitions: Optional[OrderedDict[str, AttributeDefinition]] = None,
                 environment_variables: Optional[OrderedDict[str, EnvironmentVariable]] = None,
                 value_tables: Optional[OrderedDict[str, Choices]] = None,
                 attributes_rel: Optional[OrderedDict] = None,
                 attribute_definitions_rel: Optional[OrderedDict] = None):
        if attributes is None:
            attributes = OrderedDict()

        if attribute_definitions is None:
            attribute_definitions = OrderedDict()

        if environment_variables is None:
            environment_variables = OrderedDict()

        if value_tables is None:
            value_tables = OrderedDict()

        if attributes_rel is None:
            attributes_rel = OrderedDict()

        if attribute_definitions_rel is None:
            attribute_definitions_rel = OrderedDict()

        self._attributes = attributes
        self._attribute_definitions = attribute_definitions
        self._environment_variables = environment_variables
        self._value_tables = value_tables
        self._attributes_rel = attributes_rel
        self._attribute_definitions_rel = attribute_definitions_rel

    @property
    def attributes(self) -> OrderedDict[str, Attribute]:
        """The DBC specific attributes of the parent object (database, node,
        message or signal) as a dictionary.

        """

        return self._attributes

    @attributes.setter
    def attributes(self, value: OrderedDict[str, Attribute]) -> None:
        self._attributes = value

    @property
    def attribute_definitions(self) -> OrderedDict[str, AttributeDefinition]:
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
    def attributes_rel(self) -> OrderedDict:
        """The DBC specific attribute rel as dictionary..

        """

        return self._attributes_rel

    @property
    def attribute_definitions_rel(self) -> OrderedDict:
        """The DBC specific attribute definitions rel as dictionary.

        """

        return self._attribute_definitions_rel
