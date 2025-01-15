# Store the specific DBC format properties of objects

from collections import OrderedDict


class DbcSpecifics:

    def __init__(self,
                 attributes=None,
                 attribute_definitions=None,
                 environment_variables=None,
                 value_tables=None,
                 attributes_rel=None,
                 attribute_definitions_rel=None):
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
    def attributes(self):
        """The DBC specific attributes of the parent object (database, node,
        message or signal) as a dictionary.

        """

        return self._attributes

    @attributes.setter
    def attributes(self, value):
        self._attributes = value

    @property
    def attribute_definitions(self):
        """The DBC specific attribute definitions as dictionary.

        """

        return self._attribute_definitions

    @property
    def value_tables(self):
        """An ordered dictionary of all value tables. Only valid for DBC
        specifiers on database level.

        """

        return self._value_tables

    @property
    def environment_variables(self):
        """An ordered dictionary of all environment variables. Only valid for
        DBC specifiers on database level.

        """

        return self._environment_variables

    @property
    def attributes_rel(self):
        """The DBC specific attribute rel as dictionary..

        """

        return self._attributes_rel

    @property
    def attribute_definitions_rel(self):
        """The DBC specific attribute definitions rel as dictionary.

        """

        return self._attribute_definitions_rel

    def __add__(self, other):
        new = self.__class__(attributes=self._attributes,
                             attribute_definitions=self._attribute_definitions,
                             environment_variables=self._environment_variables,
                             value_tables=self._value_tables,
                             attributes_rel=self._attributes_rel,
                             attribute_definitions_rel=self._attribute_definitions_rel)

        new._attributes.update(other._attributes)
        new._attribute_definitions.update(other._attribute_definitions)
        new._environment_variables.update(other._environment_variables)
        new._value_tables.update(other._value_tables)
        new._attributes_rel.update(other._attributes_rel)
        new._attribute_definitions_rel.update(other._attribute_definitions_rel)

        return new
