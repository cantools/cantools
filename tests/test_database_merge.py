import pickle
import unittest
from pathlib import Path
from xml.etree import ElementTree

import textparser  # type: ignore
from parameterized import parameterized  # type: ignore

import cantools.database
from cantools.database.can.bus import Bus
from cantools.database.can.node import Node
from cantools.database.errors import Error


def dbc_source(prefix='Original', frame_id=1793, version='first'):
    return f'''VERSION "{version}"
NS_ :
BS_:
BU_: {prefix}Node
BO_ {frame_id} {prefix}Message: 1 {prefix}Node
 SG_ {prefix}Signal : 0|8@1+ (1,0) [0|255] "" {prefix}Node
EV_ {prefix}Environment: 0 [0|100] "" 0 0 DUMMY_NODE_VECTOR0 {prefix}Node;
VAL_TABLE_ {prefix}Table 0 "zero" 1 "one";
BA_DEF_ "DBName" STRING ;
BA_DEF_ BO_ "{prefix}Attribute" INT 0 100;
BA_DEF_ "{prefix}Project" STRING ;
BA_DEF_DEF_ "DBName" "";
BA_DEF_DEF_ "{prefix}Attribute" 17;
BA_DEF_DEF_ "{prefix}Project" "";
BA_ "DBName" "{prefix}Bus";
BA_ "{prefix}Attribute" BO_ {frame_id} 23;
BA_ "{prefix}Project" "{prefix} project";
BA_DEF_REL_ BU_SG_REL_ "{prefix}SignalRelation" INT 0 100;
BA_DEF_REL_ BU_BO_REL_ "{prefix}MessageRelation" INT 0 100;
BA_DEF_DEF_REL_ "{prefix}SignalRelation" 41;
BA_DEF_DEF_REL_ "{prefix}MessageRelation" 43;
BA_REL_ "{prefix}SignalRelation" BU_SG_REL_ {prefix}Node SG_ {frame_id} {prefix}Signal 31;
BA_REL_ "{prefix}MessageRelation" BU_BO_REL_ {prefix}Node {frame_id} 37;
'''


def added_source(database_format):
    if database_format == 'dbc':
        return ('VERSION "second"\nBU_: AddedNode\n'
                'BO_ 1794 AddedMessage: 1 AddedNode\n'
                ' SG_ AddedSignal : 0|8@1+ (1,0) [0|255] "" AddedNode\n')
    if database_format == 'kcd':
        return '''<NetworkDefinition xmlns="http://kayak.2codeornot2code.org/1.0">
  <Document version="second" />
  <Node id="1" name="AddedNode" />
  <Bus name="AddedBus" baudrate="250000">
    <Message id="0x702" name="AddedMessage" length="1">
      <Producer><NodeRef id="1" /></Producer>
      <Signal name="AddedSignal" offset="0" length="8" />
    </Message>
  </Bus>
</NetworkDefinition>'''
    if database_format == 'sym':
        return '''FormatVersion=6.0 // Do not edit this line!
Title="Added"
{SENDRECEIVE}
[AddedMessage]
ID=702h
Len=1
Var=AddedSignal unsigned 0,8
'''
    return Path('tests/files/arxml/system-4.2.arxml').read_text(encoding='utf-8')


class DatabaseMetadataMergeTest(unittest.TestCase):
    def assert_original_metadata(self, database):
        self.assertIn('OriginalNode', [node.name for node in database.nodes])
        self.assertIn('OriginalBus', [bus.name for bus in database.buses])
        self.assertEqual(
            database.dbc.attribute_definitions['OriginalAttribute'].default_value,
            17)
        self.assertEqual(
            database.get_message_by_name('OriginalMessage').dbc.attributes[
                'OriginalAttribute'].value, 23)
        self.assertEqual(database.dbc.attributes['OriginalProject'].value,
                         'Original project')
        self.assertIn('OriginalEnvironment', database.dbc.environment_variables)
        self.assertEqual(database.dbc.relation_attribute_definitions[
            'OriginalSignalRelation'].default_value, 41)
        self.assertEqual(
            {key: str(value) for key, value in
             database.dbc.value_tables['OriginalTable'].items()},
            {0: 'zero', 1: 'one'})
        relations = database.dbc.relation_attributes
        self.assertEqual(
            relations.node_signal_relations[1793]['OriginalSignal'][
                'OriginalNode']['OriginalSignalRelation'].value, 31)
        self.assertEqual(
            relations.node_message_relations[1793]['OriginalNode'][
                'OriginalMessageRelation'].value, 37)

    def test_original_metadata_roundtrips(self):
        database = cantools.database.load_string(dbc_source(), 'dbc')
        self.assert_original_metadata(database)
        reloaded = cantools.database.load_string(database.as_dbc_string(), 'dbc')
        self.assert_original_metadata(reloaded)

    @parameterized.expand(['dbc', 'kcd', 'sym', 'arxml'])
    def test_metadata_survives_format_addition(self, database_format):
        database = cantools.database.load_string(dbc_source(), 'dbc')
        incoming = cantools.database.load_string(
            added_source(database_format), database_format)
        old_message = database.messages[0]

        getattr(database, f'add_{database_format}_string')(
            added_source(database_format))

        self.assertIs(database.messages[0], old_message)
        self.assertEqual(len(database.messages), 1 + len(incoming.messages))
        self.assertEqual(database.version, incoming.version)
        self.assert_original_metadata(database)
        self.assertTrue({node.name for node in incoming.nodes}.issubset(
            {node.name for node in database.nodes}))
        self.assertTrue({bus.name for bus in incoming.buses}.issubset(
            {bus.name for bus in database.buses}))
        reloaded = cantools.database.load_string(database.as_dbc_string(), 'dbc')
        self.assert_original_metadata(reloaded)

    @parameterized.expand(['dbc', 'kcd', 'sym'])
    def test_autosar_metadata_survives_other_format_addition(self, database_format):
        database = cantools.database.load_string(added_source('arxml'), 'arxml')
        original_specifics = database.autosar
        original_arxml_version = original_specifics.arxml_version

        getattr(database, f'add_{database_format}_string')(
            added_source(database_format))

        self.assertIs(database.autosar, original_specifics)
        self.assertEqual(database.autosar.arxml_version, original_arxml_version)

    def test_incoming_arxml_metadata_replaces_previous_specifics(self):
        database = cantools.database.load_string(added_source('arxml'), 'arxml')
        original_specifics = database.autosar
        source = Path('tests/files/arxml/system-3.2.3.arxml').read_text(encoding='utf-8')
        incoming = cantools.database.load_string(source, 'arxml')

        database.add_arxml_string(source)

        self.assertIsNot(database.autosar, original_specifics)
        self.assertEqual(database.autosar.arxml_version,
                         incoming.autosar.arxml_version)

    def test_disjoint_dbc_metadata_and_containers_are_preserved(self):
        database = cantools.database.load_string(dbc_source(), 'dbc')
        original_specifics = database.dbc
        database.add_dbc_string(dbc_source('Added', 1794, 'second'))

        self.assert_original_metadata(database)
        self.assertIn('AddedAttribute', database.dbc.attribute_definitions)
        self.assertIn('AddedEnvironment', database.dbc.environment_variables)
        self.assertIn('AddedTable', database.dbc.value_tables)
        self.assertNotIn('AddedAttribute', original_specifics.attribute_definitions)
        self.assertNotIn('AddedEnvironment', original_specifics.environment_variables)
        self.assertNotIn('AddedTable', original_specifics.value_tables)
        self.assertEqual(database.dbc.attributes['DBName'].value, 'AddedBus')
        self.assertEqual(original_specifics.attributes['DBName'].value,
                         'OriginalBus')

    def test_global_attribute_and_value_table_collisions_use_incoming_values(self):
        original = dbc_source() + '''
BA_DEF_ "Shared" STRING ;
BA_DEF_DEF_ "Shared" "";
BA_ "Shared" "first";
VAL_TABLE_ SharedTable 0 "first";
'''
        incoming = added_source('dbc') + '''
BA_DEF_ "Shared" STRING ;
BA_DEF_DEF_ "Shared" "";
BA_ "Shared" "second";
VAL_TABLE_ SharedTable 0 "second";
'''
        database = cantools.database.load_string(original, 'dbc')
        original_specifics = database.dbc
        database.add_dbc_string(incoming)

        self.assertEqual(database.dbc.attributes['Shared'].value, 'second')
        self.assertEqual(str(database.dbc.value_tables['SharedTable'][0]),
                         'second')
        self.assertEqual(database.dbc.attributes['OriginalProject'].value,
                         'Original project')
        self.assertIn('OriginalTable', database.dbc.value_tables)
        self.assertEqual(original_specifics.attributes['Shared'].value, 'first')
        self.assertEqual(str(original_specifics.value_tables['SharedTable'][0]),
                         'first')

    @parameterized.expand([
        ('kind', 'BO_ "Shared" INT 0 100', 'SG_ "Shared" INT 0 100', '5', '5'),
        ('type', 'BO_ "Shared" INT 0 100', 'BO_ "Shared" HEX 0 100', '5', '5'),
        ('minimum', 'BO_ "Shared" INT 0 100', 'BO_ "Shared" INT 1 100', '5', '5'),
        ('maximum', 'BO_ "Shared" INT 0 100', 'BO_ "Shared" INT 0 99', '5', '5'),
        ('default', 'BO_ "Shared" INT 0 100', 'BO_ "Shared" INT 0 100', '5', '6'),
        ('choices', 'BO_ "Shared" ENUM "a","b"',
         'BO_ "Shared" ENUM "a","c"', '"a"', '"a"'),
    ])
    def test_incompatible_attribute_definition_is_atomic(self, _name, original,
                                                        incoming, old_default,
                                                        new_default):
        database = cantools.database.load_string(
            dbc_source() + f'BA_DEF_ {original};\n'
            f'BA_DEF_DEF_ "Shared" {old_default};\n', 'dbc')
        before = pickle.dumps(database)
        with self.assertRaisesRegex(Error, 'Shared'):
            database.add_dbc_string(
                added_source('dbc') + f'BA_DEF_ {incoming};\n'
                f'BA_DEF_DEF_ "Shared" {new_default};\n')
        self.assertEqual(pickle.dumps(database), before)

    @parameterized.expand([
        ('kind', 'BU_SG_REL_', 'BU_BO_REL_', 'INT 0 100', 'INT 0 100', '5', '5'),
        ('type', 'BU_SG_REL_', 'BU_SG_REL_', 'INT 0 100', 'HEX 0 100', '5', '5'),
        ('minimum', 'BU_SG_REL_', 'BU_SG_REL_', 'INT 0 100', 'INT 1 100', '5', '5'),
        ('maximum', 'BU_SG_REL_', 'BU_SG_REL_', 'INT 0 100', 'INT 0 99', '5', '5'),
        ('default', 'BU_SG_REL_', 'BU_SG_REL_', 'INT 0 100', 'INT 0 100', '5', '6'),
        ('choices', 'BU_SG_REL_', 'BU_SG_REL_', 'ENUM "a","b"',
         'ENUM "a","c"', '"a"', '"a"'),
    ])
    def test_incompatible_relation_definition_is_atomic(self, _name, old_kind,
                                                       new_kind, old_type,
                                                       new_type, old_default,
                                                       new_default):
        database = cantools.database.load_string(
            dbc_source() + f'BA_DEF_REL_ {old_kind} "Shared" {old_type};\n'
            f'BA_DEF_DEF_REL_ "Shared" {old_default};\n', 'dbc')
        before = pickle.dumps(database)
        with self.assertRaisesRegex(Error, 'Shared'):
            database.add_dbc_string(
                added_source('dbc') +
                f'BA_DEF_REL_ {new_kind} "Shared" {new_type};\n'
                f'BA_DEF_DEF_REL_ "Shared" {new_default};\n')
        self.assertEqual(pickle.dumps(database), before)

    def test_nested_relation_siblings_survive_addition(self):
        database = cantools.database.load_string(dbc_source(), 'dbc')
        original_relations = database.dbc.relation_attributes
        incoming = dbc_source('Added', 1793, 'second')
        database.add_dbc_string(incoming)

        relations = database.dbc.relation_attributes
        self.assertEqual(set(relations.node_signal_relations[1793]),
                         {'OriginalSignal', 'AddedSignal'})
        self.assertEqual(set(relations.node_message_relations[1793]),
                         {'OriginalNode', 'AddedNode'})
        self.assertEqual(set(original_relations.node_signal_relations[1793]),
                         {'OriginalSignal'})
        self.assertEqual(set(original_relations.node_message_relations[1793]),
                         {'OriginalNode'})

    def test_relation_leaf_collision_retains_siblings_and_original_containers(self):
        original = dbc_source() + '''
BA_DEF_REL_ BU_SG_REL_ "RetainedSignalRelation" INT 0 100;
BA_DEF_REL_ BU_BO_REL_ "RetainedMessageRelation" INT 0 100;
BA_REL_ "RetainedSignalRelation" BU_SG_REL_ OriginalNode SG_ 1793 OriginalSignal 19;
BA_REL_ "RetainedMessageRelation" BU_BO_REL_ OriginalNode 1793 29;
'''
        database = cantools.database.load_string(original, 'dbc')
        original_relations = database.dbc.relation_attributes
        incoming = dbc_source(version='second').replace(' 31;', ' 47;').replace(
            ' 37;', ' 53;')
        database.add_dbc_string(incoming)

        relations = database.dbc.relation_attributes
        signal_attributes = relations.node_signal_relations[1793][
            'OriginalSignal']['OriginalNode']
        message_attributes = relations.node_message_relations[1793]['OriginalNode']
        self.assertEqual(signal_attributes['OriginalSignalRelation'].value, 47)
        self.assertEqual(signal_attributes['RetainedSignalRelation'].value, 19)
        self.assertEqual(message_attributes['OriginalMessageRelation'].value, 53)
        self.assertEqual(message_attributes['RetainedMessageRelation'].value, 29)
        original_signal_attributes = original_relations.node_signal_relations[
            1793]['OriginalSignal']['OriginalNode']
        original_message_attributes = original_relations.node_message_relations[
            1793]['OriginalNode']
        self.assertEqual(original_signal_attributes['OriginalSignalRelation'].value,
                         31)
        self.assertEqual(original_message_attributes['OriginalMessageRelation'].value,
                         37)
        self.assertIsNot(signal_attributes, original_signal_attributes)
        self.assertIsNot(message_attributes, original_message_attributes)

    def test_duplicate_nodes_and_buses_merge_metadata_in_first_seen_order(self):
        original = dbc_source() + '''
BA_DEF_ BU_ "OldNodeAttribute" INT 0 100;
BA_ "OldNodeAttribute" BU_ OriginalNode 11;
'''
        database = cantools.database.load_string(original, 'dbc')
        old_node = Node('OriginalNode', {None: 'first', 'DE': 'Deutsch'},
                        dbc_specifics=database.nodes[0].dbc)
        old_bus = Bus('OriginalBus', {None: 'first bus', 'DE': 'Bus Deutsch'},
                      baudrate=500000, fd_baudrate=2000000)
        database.nodes[:] = [old_node, Node('ExistingNode')]
        database.buses[:] = [old_bus, Bus('ExistingBus')]
        incoming = dbc_source('Added', 1794, 'second').replace(
            'BU_: AddedNode', 'BU_: OriginalNode AddedNode').replace(
            'BA_ "DBName" "AddedBus";', 'BA_ "DBName" "OriginalBus";') + '''
BA_DEF_ BU_ "NewNodeAttribute" INT 0 100;
BA_ "NewNodeAttribute" BU_ OriginalNode 13;
CM_ BU_ OriginalNode "second";
CM_ "second bus";
'''
        database.add_dbc_string(incoming)

        self.assertEqual([node.name for node in database.nodes],
                         ['OriginalNode', 'ExistingNode', 'AddedNode'])
        self.assertEqual([bus.name for bus in database.buses],
                         ['OriginalBus', 'ExistingBus'])
        node = database.get_node_by_name('OriginalNode')
        self.assertEqual(node.comments, {None: 'second', 'DE': 'Deutsch'})
        self.assertEqual(node.dbc.attributes['OldNodeAttribute'].value, 11)
        self.assertEqual(node.dbc.attributes['NewNodeAttribute'].value, 13)
        bus = database.get_bus_by_name('OriginalBus')
        self.assertEqual(bus.comments,
                         {None: 'second bus', 'DE': 'Bus Deutsch'})
        self.assertEqual((bus.baudrate, bus.fd_baudrate), (500000, 2000000))
        self.assertEqual(old_node.comments[None], 'first')
        self.assertEqual(old_bus.comments[None], 'first bus')

        database.add_kcd_string(added_source('kcd').replace('AddedBus', 'OriginalBus'))
        bus = database.get_bus_by_name('OriginalBus')
        self.assertEqual((bus.baudrate, bus.fd_baudrate), (250000, 2000000))

    def test_environment_variable_collision_replaces_whole_variable(self):
        database = cantools.database.load_string(dbc_source(), 'dbc')
        original_specifics = database.dbc
        database.add_dbc_string(added_source('dbc') + '''
EV_ OriginalEnvironment: 0 [10|200] "rpm" 20 1 DUMMY_NODE_VECTOR0 AddedNode;
EV_ AddedEnvironment: 0 [0|50] "" 0 2 DUMMY_NODE_VECTOR0 AddedNode;
''')
        variable = database.dbc.environment_variables['OriginalEnvironment']
        self.assertEqual((variable.minimum, variable.maximum, variable.unit),
                         (10, 200, 'rpm'))
        self.assertIn('AddedEnvironment', database.dbc.environment_variables)
        self.assertEqual(original_specifics.environment_variables[
            'OriginalEnvironment'].maximum, 100)

    @parameterized.expand([
        ('empty', 'VERSION ""\n', ''),
        ('absent', 'BU_:\n', None),
        ('value', 'VERSION "second"\n', 'second'),
    ])
    def test_incoming_version_policy(self, _name, incoming, expected):
        database = cantools.database.load_string('VERSION "first"\n', 'dbc')
        database.add_dbc_string(incoming)
        self.assertEqual(database.version, expected)

    def test_message_append_and_lookup_policy(self):
        database = cantools.database.load_string(added_source('dbc'), 'dbc')
        original = database.messages[0]
        database.add_dbc_string(added_source('dbc'))
        self.assertEqual(len(database.messages), 2)
        self.assertIs(database.messages[0], original)
        self.assertIs(database.get_message_by_frame_id(1794), database.messages[1])
        self.assertIs(database.get_message_by_name('AddedMessage'),
                      database.messages[1])

    @parameterized.expand([
        ('dbc', textparser.ParseError),
        ('kcd', ElementTree.ParseError),
        ('sym', Error),
        ('arxml', ElementTree.ParseError),
    ])
    def test_parse_failure_leaves_database_unchanged(self, database_format,
                                                    exception):
        database = cantools.database.load_string(dbc_source(), 'dbc')
        before = pickle.dumps(database)
        with self.assertRaises(exception):
            getattr(database, f'add_{database_format}_string')('invalid input')
        self.assertEqual(pickle.dumps(database), before)
