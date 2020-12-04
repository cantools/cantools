import os
import unittest
import logging

import cantools

BASE_PATH = os.path.dirname(__file__)


class CanToolsDiagnosticsDatabaseTest(unittest.TestCase):

    maxDiff = None

    def test_example_cdd(self):
        filename = os.path.join(BASE_PATH, 'files/cdd/example.cdd')

        db = cantools.db.load_file(filename, encoding='iso-8859-1')
        self.assertEqual(15, len(db.dids))
        self.assertEqual([
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
                         ],
                         [did.name for did in db.dids])

        # ECU_Identification DID structure.
        did = db.get_did_by_name('ECU_Identification')
        self.assertEqual('ECU_Identification', did.name)
        self.assertEqual(144, did.identifier)
        self.assertEqual(10, did.length)
        self.assertEqual([
                             'Ident_Number_7_6',
                             'Ident_Number_5_4',
                             'Ident_Number_3_2',
                             'Ident_Number_1_0',
                             'Diagnostic_Identification'
                         ],
                         [data.name for data in did.datas])

        data = did.get_data_by_name('Diagnostic_Identification')
        self.assertEqual('Diagnostic_Identification', data.name)
        self.assertEqual(64, data.start)
        self.assertEqual(16, data.length)
        self.assertEqual('little_endian', data.byte_order)
        self.assertEqual(1, data.scale)
        self.assertEqual(0, data.offset)
        self.assertEqual(0, data.minimum)
        self.assertEqual(255, data.maximum)
        self.assertEqual(None, data.unit)
        self.assertEqual(None, data.choices)

        decoded_did = {
            'Ident_Number_7_6': 0x1234,
            'Ident_Number_5_4': 0x5678,
            'Ident_Number_3_2': 0x9012,
            'Ident_Number_1_0': 0x3456,
            'Diagnostic_Identification': 0xabcd
        }
        encoded_did = b'\x34\x12\x78\x56\x12\x90\x56\x34\xcd\xab'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded_did, encoded)
        decoded = did.decode(encoded)
        self.assertEqual(decoded_did, decoded)

        # SawTooth DID structure.
        did = db.get_did_by_identifier(244)
        self.assertEqual(244, did.identifier)

        decoded_did = {
            'ampl': 1,
            'period': 40,
            'value': 3
        }
        encoded_did = b'\x01\x02\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded_did, encoded)
        decoded = did.decode(encoded)
        self.assertEqual(decoded_did, decoded)

        # Sine DID structure.
        did = db.get_did_by_name('Sine')
        self.assertEqual(3, len(did.datas))
        self.assertEqual([
                             'ampl',
                             'period',
                             'value'
                         ],
                         [data.name for data in did.datas])
        self.assertEqual(243, did.identifier)
        self.assertEqual('period', did.datas[1].name)
        self.assertEqual('sec', did.datas[1].unit)
        self.assertEqual(20, did.datas[1].scale)
        self.assertEqual(0, did.datas[1].offset)

        decoded_did = {
            'ampl': 1,
            'period': 40,
            'value': 3
        }
        encoded_did = b'\x01\x02\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded_did, encoded)
        decoded = did.decode(encoded)
        self.assertEqual(decoded_did, decoded)

        # Coding DID structure.
        did = db.get_did_by_name('Coding')
        self.assertEqual(3, len(did.datas))
        self.assertEqual([
                             'Country_variant',
                             'Vehicle_type',
                             'Special_setting'
                         ],
                         [data.name for data in did.datas])
        self.assertEqual(160, did.identifier)
        self.assertEqual('Vehicle_type', did.datas[1].name)
        self.assertEqual({
                             0: '(not defined)',
                             1: 'Coupe',
                             2: 'Sedan',
                             3: 'Transporter'
                         },
                         did.datas[1].choices)
        self.assertEqual('Special_setting', did.datas[2].name)
        self.assertEqual(None, did.datas[2].choices)

        decoded_did = {
            'Country_variant': 'Europe',
            'Vehicle_type': 'Sedan',
            'Special_setting': 3
        }
        encoded_did = b'\x21\x03'

        encoded = did.encode(decoded_did)
        self.assertEqual(encoded_did, encoded)
        decoded = did.decode(encoded)
        self.assertEqual(decoded_did, decoded)

    def test_example_cdd_repr(self):
        filename = os.path.join(BASE_PATH, 'files/cdd/example.cdd')

        db = cantools.db.load_file(filename, encoding='iso-8859-1')
        self.assertEqual(
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
            "  data('_reserved', 4, 4, 'little_endian', 1, 0, 0, 255, 'None', None)\n",
            repr(db))

    def test_cdd_add(self):
        db = cantools.db.diagnostics.Database()
        filename = os.path.join(BASE_PATH, 'files/cdd/example.cdd')

        db.add_cdd_file(filename, encoding='iso-8859-1')
        self.assertEqual(15, len(db.dids))


# This file is not '__main__' when executed via 'python setup.py3
# test'.
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    unittest.main()
