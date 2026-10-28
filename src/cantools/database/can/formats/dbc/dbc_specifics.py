# Store the specific DBC format properties of objects

from __future__ import annotations

from collections import OrderedDict
from typing import TYPE_CHECKING

from ....errors import Error

if TYPE_CHECKING:
    from cantools.typechecking import Choices

    from .dbc_attribute import DbcAttributeType
    from .dbc_attribute_definition import DbcAttributeDefinitionType
    from .dbc_environment_variable import DbcEnvironmentVariable
    from .dbc_loader import DbcRelationAttributes

class DbcSpecifics:

    def __init__(self,
                 *,
                 attributes: OrderedDict[str, DbcAttributeType] | None = None,
                 attribute_definitions: OrderedDict[str, DbcAttributeDefinitionType] | None = None,
                 environment_variables: OrderedDict[str, DbcEnvironmentVariable] | None = None,
                 value_tables: OrderedDict[str, Choices] | None = None,
                 relation_attributes: DbcRelationAttributes | None = None,
                 relation_attribute_definitions: OrderedDict[str, DbcAttributeDefinitionType] | None = None) -> None:
        self._attributes = attributes or OrderedDict()
        self._attribute_definitions = attribute_definitions or OrderedDict()
        self._environment_variables = environment_variables or OrderedDict()
        self._value_tables = value_tables or OrderedDict()
        self._relation_attributes = relation_attributes
        self._relation_attribute_definitions = relation_attribute_definitions or OrderedDict()

    def _merge(self, other: DbcSpecifics) -> DbcSpecifics:
        """Combine metadata without modifying either input's mappings.

        Attribute definitions must agree, including their defaults. Otherwise
        retained assignments or implicit defaults would change meaning.
        Incoming values win for other duplicate keys.
        """
        from .dbc_loader import DbcRelationAttributes  # noqa: PLC0415

        definitions = _merge_definitions(self.attribute_definitions,
                                         other.attribute_definitions)
        relation_definitions = _merge_definitions(
            self.relation_attribute_definitions,
            other.relation_attribute_definitions)

        relations = None
        if self.relation_attributes is not None or other.relation_attributes is not None:
            relations = DbcRelationAttributes()
            for source in (self.relation_attributes, other.relation_attributes):
                if source is None:
                    continue
                for frame_id, signals in source.node_signal_relations.items():
                    target_signals = relations.node_signal_relations.setdefault(
                        frame_id, OrderedDict())
                    for signal_name, nodes in signals.items():
                        target_nodes = target_signals.setdefault(signal_name, OrderedDict())
                        for node_name, attributes in nodes.items():
                            target_nodes.setdefault(node_name, OrderedDict()).update(attributes)
                for frame_id, nodes in source.node_message_relations.items():
                    target_nodes = relations.node_message_relations.setdefault(
                        frame_id, OrderedDict())
                    for node_name, attributes in nodes.items():
                        target_nodes.setdefault(node_name, OrderedDict()).update(attributes)

        return DbcSpecifics(
            attributes=OrderedDict([*self.attributes.items(), *other.attributes.items()]),
            attribute_definitions=definitions,
            environment_variables=OrderedDict([
                *self.environment_variables.items(), *other.environment_variables.items()]),
            value_tables=OrderedDict([*self.value_tables.items(), *other.value_tables.items()]),
            relation_attributes=relations,
            relation_attribute_definitions=relation_definitions)

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
    def relation_attribute_definitions(self) -> OrderedDict[str, DbcAttributeDefinitionType]:
        """The DBC specific attribute definitions rel as dictionary.

        """

        return self._relation_attribute_definitions


def _merge_definitions(
        original: OrderedDict[str, DbcAttributeDefinitionType],
        incoming: OrderedDict[str, DbcAttributeDefinitionType],
) -> OrderedDict[str, DbcAttributeDefinitionType]:
    definitions = original.copy()
    for name, definition in incoming.items():
        if name in definitions:
            previous = definitions[name]
            if definition != previous:
                raise Error(f'Incompatible DBC attribute definition {name!r}.')
        else:
            definitions[name] = definition
    return definitions
