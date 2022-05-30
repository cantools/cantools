import os
import unittest
import logging

import cantools


class CanToolsDiagnosticsDatabaseTest(unittest.TestCase):

    maxDiff = None

    def test_le_example_cdd(self):
        db = cantools.db.load_file('tests/files/cdd/le-example.cdd',
                                   encoding='iso-8859-1')
        self.assertEqual(len(db.dids), 15)
        self.assertEqual([did.name for did in db.dids],
                         [
                             'DEFAULT_SESSION',
                             'ProgrammingSession',
                             'ECU_Identification',
                             'Development_Data',
                             'Serial_Number',
                             'REQUEST_SEED_SERVICE',
                             'SUBMIT_KEY_SERVICE',
                             'A_D_Werte',
                             'SawTooth',
                             'Sine',
                             'FaultMemory_identified',
                             'FaultMemory_supported',
                             'TestData',
                             'Coding',
                             'InputOutput'
                         ])

        # ECU_Identification DID structure.
        did = db.get_did_by_name('ECU_Identification')
        self.assertEqual(did.name, 'ECU_Identification')
        self.assertEqual(did.identifier, 144)
        self.assertEqual(did.length, 10)
        self.assertEqual([data.name for data in did.datas],
                         [
                             'Ident_Number_7_6',
                             'Ident_Number_5_4',
                             'Ident_Number_3_2',
                             'Ident_Number_1_0',
                             'Diagnostic_Identification'
                         ])

        data = did.get_data_by_name('Diagnostic_Identification')
        self.assertEqual(data.name, 'Diagnostic_Identification')
        self.assertEqual(data.start, 64)
        self.assertEqual(data.length, 16)
        self.assertEqual(data.byte_order, 'little_endian')
        self.assertEqual(data.scale, 1)
        self.assertEqual(data.offset, 0)
        self.assertEqual(data.minimum, 0)
        self.assertEqual(data.maximum, 255)
        self.assertEqual(data.unit, None)
        self.assertEqual(data.choices, None)

        decoded_did = {
            'Ident_Number_7_6': 0x1234,
            'Ident_Number_5_4': 0x5678,
            'Ident_Number_3_2': 0x9012,
            'Ident_Number_1_0': 0x3456,
            'Diagnostic_Identification': 0xabcd
        }
        encoded_did = b'\x34\x12\x78\x56\x12\x90\x56\x34\xcd\xab'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

        # SawTooth DID structure.
        did = db.get_did_by_identifier(244)
        self.assertEqual(did.identifier, 244)

        decoded_did = {
            'ampl': 1,
            'period': 40,
            'value': 3
        }
        encoded_did = b'\x01\x02\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

        # Sine DID structure.
        did = db.get_did_by_name('Sine')
        self.assertEqual(len(did.datas), 3)
        self.assertEqual([data.name for data in did.datas],
                         [
                             'ampl',
                             'period',
                             'value'
                         ])
        self.assertEqual(did.identifier, 243)
        self.assertEqual(did.datas[1].name, 'period')
        self.assertEqual(did.datas[1].unit, 'sec')
        self.assertEqual(did.datas[1].scale, 20)
        self.assertEqual(did.datas[1].offset, 0)

        decoded_did = {
            'ampl': 1,
            'period': 40,
            'value': 3
        }
        encoded_did = b'\x01\x02\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

        # Coding DID structure.
        did = db.get_did_by_name('Coding')
        self.assertEqual(len(did.datas), 3)
        self.assertEqual([data.name for data in did.datas],
                         [
                             'Country_variant',
                             'Vehicle_type',
                             'Special_setting'
                         ])
        self.assertEqual(did.identifier, 160)
        self.assertEqual(did.datas[1].name, 'Vehicle_type')
        self.assertEqual(did.datas[1].choices,
                         {
                             0: '(not defined)',
                             1: 'Coupe',
                             2: 'Sedan',
                             3: 'Transporter'
                         })
        self.assertEqual(did.datas[2].name, 'Special_setting')
        self.assertEqual(did.datas[2].choices, None)

        decoded_did = {
            'Country_variant': 'Europe',
            'Vehicle_type': 'Sedan',
            'Special_setting': 3
        }
        encoded_did = b'\x21\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

    def test_be_example_cdd(self):
        db = cantools.db.load_file('tests/files/cdd/example.cdd',
                                   encoding = 'iso-8859-1')
        self.assertEqual(len(db.dids), 15)
        self.assertEqual([did.name for did in db.dids],
                         [
                             'DEFAULT_SESSION',
                             'ProgrammingSession',
                             'ECU_Identification',
                             'Development_Data',
                             'Serial_Number',
                             'REQUEST_SEED_SERVICE',
                             'SUBMIT_KEY_SERVICE',
                             'A_D_Werte',
                             'SawTooth',
                             'Sine',
                             'FaultMemory_identified',
                             'FaultMemory_supported',
                             'TestData',
                             'Coding',
                             'InputOutput'
                         ])

        # ECU_Identification DID structure.
        did = db.get_did_by_name('ECU_Identification')
        self.assertEqual(did.name, 'ECU_Identification')
        self.assertEqual(did.identifier, 144)
        self.assertEqual(did.length, 10)
        self.assertEqual([data.name for data in did.datas],
                         [
                             'Ident_Number_7_6',
                             'Ident_Number_5_4',
                             'Ident_Number_3_2',
                             'Ident_Number_1_0',
                             'Diagnostic_Identification'
                         ])

        data = did.get_data_by_name('Diagnostic_Identification')
        self.assertEqual(data.name, 'Diagnostic_Identification')
        self.assertEqual(data.start, 71)
        self.assertEqual(data.length, 16)
        self.assertEqual(data.byte_order, 'big_endian')
        self.assertEqual(data.scale, 1)
        self.assertEqual(data.offset, 0)
        self.assertEqual(data.minimum, 0)
        self.assertEqual(data.maximum, 255)
        self.assertEqual(data.unit, None)
        self.assertEqual(data.choices, None)

        decoded_did = {
            'Ident_Number_7_6': 0x1234,
            'Ident_Number_5_4': 0x5678,
            'Ident_Number_3_2': 0x9012,
            'Ident_Number_1_0': 0x3456,
            'Diagnostic_Identification': 0xabcd
        }
        encoded_did = b'\x12\x34\x56\x78\x90\x12\x34\x56\xab\xcd'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

        # SawTooth DID structure.
        did = db.get_did_by_identifier(244)
        self.assertEqual(did.identifier, 244)

        decoded_did = {
            'ampl': 1,
            'period': 40,
            'value': 3
        }
        encoded_did = b'\x01\x02\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

        # Sine DID structure.
        did = db.get_did_by_name('Sine')
        self.assertEqual(len(did.datas), 3)
        self.assertEqual([data.name for data in did.datas],
                         [
                             'ampl',
                             'period',
                             'value'
                         ])
        self.assertEqual(did.identifier, 243)
        self.assertEqual(did.datas[1].name, 'period')
        self.assertEqual(did.datas[1].unit, 'sec')
        self.assertEqual(did.datas[1].scale, 20)
        self.assertEqual(did.datas[1].offset, 0)

        decoded_did = {
            'ampl': 1,
            'period': 40,
            'value': 3
        }
        encoded_did = b'\x01\x02\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

        # Coding DID structure.
        did = db.get_did_by_name('Coding')
        self.assertEqual(len(did.datas), 3)
        self.assertEqual([data.name for data in did.datas],
                         [
                             'Country_variant',
                             'Vehicle_type',
                             'Special_setting'
                         ])
        self.assertEqual(did.identifier, 160)
        self.assertEqual(did.datas[1].name, 'Vehicle_type')
        self.assertEqual(did.datas[1].choices,
                         {
                             0: '(not defined)',
                             1: 'Coupe',
                             2: 'Sedan',
                             3: 'Transporter'
                         })
        self.assertEqual(did.datas[2].name, 'Special_setting')
        self.assertEqual(did.datas[2].choices, None)

        decoded_did = {
            'Country_variant': 'Europe',
            'Vehicle_type': 'Sedan',
            'Special_setting': 3
        }
        encoded_did = b'\x21\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded, encoded_did)
        decoded = did.decode(encoded)
        self.assertEqual(decoded, decoded_did)

    def test_be_example_cdd_repr(self):
        db = cantools.db.load_file('tests/files/cdd/example.cdd',
                                   encoding = 'iso-8859-1')
        self.assertEqual(
            repr(db),
            "did('DEFAULT_SESSION', 0x0081)\n"
            "\n"
            "did('ProgrammingSession', 0x0085)\n"
            "\n"
            "did('ECU_Identification', 0x0090)\n"
            "  data('Ident_Number_7_6', 7, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Ident_Number_5_4', 23, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Ident_Number_3_2', 39, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Ident_Number_1_0', 55, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Diagnostic_Identification', 71, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('Development_Data', 0x0091)\n"
            "  data('Operating_System_Version', 7, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('CAN_Driver_Version', 23, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('NM_Version', 39, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Diagnostic_Module_Version', 55, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Transport_Layer_Version', 71, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('Serial_Number', 0x0092)\n"
            "  data('Serial_Number', 7, 32, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('REQUEST_SEED_SERVICE', 0x0001)\n"
            "  data('SEED', 7, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('SUBMIT_KEY_SERVICE', 0x0002)\n"
            "  data('KEY', 7, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('A_D_Werte', 0x0040)\n"
            "  data('Voltage', 7, 8, 'big_endian', 0.1, 0.0, 0, 255, 'V', None)\n"
            "  data('Current', 15, 8, 'big_endian', 0.1, 0.0, 0, 255, 'A', None)\n"
            "  data('Resistance', 23, 16, 'big_endian', 10.0, 0.0, 0, 255, 'Ohm', None)\n"
            "\n"
            "did('SawTooth', 0x00f4)\n"
            "  data('ampl', 7, 8, 'big_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('period', 15, 8, 'big_endian', 20.0, 0.0, 0, 255, 'sec', None)\n"
            "  data('value', 23, 8, 'big_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "\n"
            "did('Sine', 0x00f3)\n"
            "  data('ampl', 7, 8, 'big_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('period', 15, 8, 'big_endian', 20.0, 0.0, 0, 255, 'sec', None)\n"
            "  data('value', 23, 8, 'big_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "\n"
            "did('FaultMemory_identified', 0x00f2)\n"
            "  data('Number', 7, 8, 'big_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('DTC_1', 15, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_1', 31, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_2', 39, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_2', 55, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_3', 63, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_3', 79, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_4', 87, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_4', 103, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_5', 111, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_5', 127, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_6', 135, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_6', 151, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('FaultMemory_supported', 0x00f1)\n"
            "  data('Number', 7, 8, 'big_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('DTC_1', 15, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_1', 31, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_2', 39, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_2', 55, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_3', 63, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_3', 79, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_4', 87, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_4', 103, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_5', 111, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_5', 127, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_6', 135, 16, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_6', 151, 8, 'big_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('TestData', 0x0041)\n"
            "  data('DATA_0', 7, 8, 'big_endian', 1, 0, 1, 1, 'None', None)\n"
            "  data('DATA_1', 15, 8, 'big_endian', 1, 0, 1, 1, 'None', None)\n"
            "\n"
            "did('Coding', 0x00a0)\n"
            "  data('Country_variant', 3, 4, 'big_endian', 1, 0, 1, 1, 'None', {0: '(not defined)', 1: 'Europe', 2: 'USA', 3: 'Japan', 4: '(others)'})\n"
            "  data('Vehicle_type', 7, 4, 'big_endian', 1, 0, 1, 1, 'None', {0: '(not defined)', 1: 'Coupe', 2: 'Sedan', 3: 'Transporter'})\n"
            "  data('Special_setting', 15, 8, 'big_endian', 1, 0, 1, 1, 'None', None)\n"
            "\n"
            "did('InputOutput', 0x0080)\n"
            "  data('Door_contact_front_left', 0, 1, 'big_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('Door_contact_front_right', 1, 1, 'big_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('Door_contact_rear_left', 2, 1, 'big_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('Door_contact_rear_right', 3, 1, 'big_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('_reserved', 7, 4, 'big_endian', 1, 0, 0, 255, 'None', None)\n")

    def test_le_example_cdd_repr(self):
        db = cantools.db.load_file('tests/files/cdd/le-example.cdd',
                                   encoding = 'iso-8859-1')
        self.assertEqual(
            repr(db),
            "did('DEFAULT_SESSION', 0x0081)\n"
            "\n"
            "did('ProgrammingSession', 0x0085)\n"
            "\n"
            "did('ECU_Identification', 0x0090)\n"
            "  data('Ident_Number_7_6', 0, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Ident_Number_5_4', 16, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Ident_Number_3_2', 32, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Ident_Number_1_0', 48, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Diagnostic_Identification', 64, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('Development_Data', 0x0091)\n"
            "  data('Operating_System_Version', 0, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('CAN_Driver_Version', 16, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('NM_Version', 32, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Diagnostic_Module_Version', 48, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('Transport_Layer_Version', 64, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('Serial_Number', 0x0092)\n"
            "  data('Serial_Number', 0, 32, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('REQUEST_SEED_SERVICE', 0x0001)\n"
            "  data('SEED', 0, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('SUBMIT_KEY_SERVICE', 0x0002)\n"
            "  data('KEY', 0, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('A_D_Werte', 0x0040)\n"
            "  data('Voltage', 0, 8, 'little_endian', 0.1, 0.0, 0, 255, 'V', None)\n"
            "  data('Current', 8, 8, 'little_endian', 0.1, 0.0, 0, 255, 'A', None)\n"
            "  data('Resistance', 16, 16, 'little_endian', 10.0, 0.0, 0, 255, 'Ohm', None)\n"
            "\n"
            "did('SawTooth', 0x00f4)\n"
            "  data('ampl', 0, 8, 'little_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('period', 8, 8, 'little_endian', 20.0, 0.0, 0, 255, 'sec', None)\n"
            "  data('value', 16, 8, 'little_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "\n"
            "did('Sine', 0x00f3)\n"
            "  data('ampl', 0, 8, 'little_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('period', 8, 8, 'little_endian', 20.0, 0.0, 0, 255, 'sec', None)\n"
            "  data('value', 16, 8, 'little_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "\n"
            "did('FaultMemory_identified', 0x00f2)\n"
            "  data('Number', 0, 8, 'little_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('DTC_1', 8, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_1', 24, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_2', 32, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_2', 48, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_3', 56, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_3', 72, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_4', 80, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_4', 96, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_5', 104, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_5', 120, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_6', 128, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_6', 144, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('FaultMemory_supported', 0x00f1)\n"
            "  data('Number', 0, 8, 'little_endian', 1.0, 0.0, 0, 255, 'None', None)\n"
            "  data('DTC_1', 8, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_1', 24, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_2', 32, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_2', 48, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_3', 56, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_3', 72, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_4', 80, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_4', 96, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_5', 104, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_5', 120, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('DTC_6', 128, 16, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "  data('StatusOfDTC_6', 144, 8, 'little_endian', 1, 0, 0, 255, 'None', None)\n"
            "\n"
            "did('TestData', 0x0041)\n"
            "  data('DATA_0', 0, 8, 'little_endian', 1, 0, 1, 1, 'None', None)\n"
            "  data('DATA_1', 8, 8, 'little_endian', 1, 0, 1, 1, 'None', None)\n"
            "\n"
            "did('Coding', 0x00a0)\n"
            "  data('Country_variant', 0, 4, 'little_endian', 1, 0, 1, 1, 'None', {0: '(not defined)', 1: 'Europe', 2: 'USA', 3: 'Japan', 4: '(others)'})\n"
            "  data('Vehicle_type', 4, 4, 'little_endian', 1, 0, 1, 1, 'None', {0: '(not defined)', 1: 'Coupe', 2: 'Sedan', 3: 'Transporter'})\n"
            "  data('Special_setting', 8, 8, 'little_endian', 1, 0, 1, 1, 'None', None)\n"
            "\n"
            "did('InputOutput', 0x0080)\n"
            "  data('Door_contact_front_left', 0, 1, 'little_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('Door_contact_front_right', 1, 1, 'little_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('Door_contact_rear_left', 2, 1, 'little_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('Door_contact_rear_right', 3, 1, 'little_endian', 1, 0, 1, 1, 'None', {0: 'closed', 1: 'open'})\n"
            "  data('_reserved', 4, 4, 'little_endian', 1, 0, 0, 255, 'None', None)\n")

    def test_be_cdd_add(self):
        db = cantools.db.diagnostics.Database()
        db.add_cdd_file('tests/files/cdd/example.cdd', encoding = 'iso-8859-1')
        self.assertEqual(len(db.dids), 15)

    def test_le_cdd_add(self):
        db = cantools.db.diagnostics.Database()
        db.add_cdd_file('tests/files/cdd/le-example.cdd', encoding = 'iso-8859-1')
        self.assertEqual(len(db.dids), 15)

    def test_unknown_byteorder(self):
        db = cantools.db.diagnostics.Database()

        with self.assertRaises(cantools.database.ParseError) as pe:
            db.add_cdd_file('tests/files/cdd/invalid-bo-example.cdd', encoding = 'iso-8859-1')

        self.assertEqual(str(pe.exception), "Unknown byte order code: 4321")

    def test_datarefs(self):
        db = cantools.db.load_file('tests/files/cdd/example-diddatarefs.cdd', encoding = 'iso-8859-1')
        self.assertEqual(len(db.dids[-1].datas), 2)


# This file is not '__main__' when executed via 'python setup.py3
# test'.
logging.basicConfig(level=logging.WARNING)

if __name__ == '__main__':
    unittest.main()
