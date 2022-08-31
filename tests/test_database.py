# -*- coding: utf-8 -*-

import sys
import math
import unittest
from decimal import Decimal
from collections import namedtuple
import textparser
import os
import re
import shutil

import logging
from xml.etree import ElementTree
import timeit

import cantools.autosar
from cantools.database.utils import prune_signal_choices, sort_choices_by_value, sort_signals_by_name

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

import cantools
from cantools.database.can.formats import dbc
from cantools.database import UnsupportedDatabaseFormatError
from cantools.database.can.signal import NamedSignalValue

class CanToolsDatabaseTest(unittest.TestCase):

    maxDiff = None

    cache_dir = '__cache_dir'

    def assertEqualChoicesDictHelper_(self, have, expect):
        if have.keys() != expect.keys():
            raise AssertationError(f'keys differ: {have} != {expect}')

        for key in have.keys():
            self.assertEqual(str(have[key]), str(expect[key]))

    def assertEqualChoicesDict(self, have, expect):
        if have.keys() != expect.keys():
            raise AssertationError(f'keys differ: {have.keys()} != {expect.keys()}')

        for key in have.keys():
            self.assertEqualChoicesDictHelper_(have[key], expect[key])

    def assert_dbc_dump(self, db, filename):
        actual = db.as_dbc_string()
        # open(filename, 'wb').write(actual.encode('cp1252'))

        with open(filename, 'rb') as fin:
            expected = fin.read().decode('cp1252')

        self.assertEqual(actual, expected)


    def tearDown(self):
        if os.path.exists(self.cache_dir):
            shutil.rmtree(self.cache_dir)


    def test_vehicle(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        db = cantools.database.load_file(filename)
        self.assertEqual(len(db.nodes), 1)
        self.assertEqual(db.nodes[0].name, 'UnusedNode')
        self.assertEqual(len(db.messages), 217)
        self.assertEqual(db.messages[216].protocol, None)
        self.assertEqual(db.messages[216].name, 'RT_SB_Gyro_Rates')
        self.assertEqual(db.messages[216].frame_id, 155872546)
        self.assertEqual(db.messages[216].senders, [])
        self.assertEqual(str(db.messages[0]),
                         "message('RT_SB_INS_Vel_Body_Axes', 0x9588322, True, 8, None)")
        self.assertEqual(repr(db.messages[0].signals[0]),
                         "signal('Validity_INS_Vel_Forwards', 0, 1, 'little_endian', "
                         "False, 0, 1, 0, 0, 1, 'None', False, None, None, None, {None: 'Valid when "
                         "bit is set, invalid when bit is clear.'})")
        self.assertEqual(db.messages[0].signals[0].initial, 0)
        self.assertEqual(db.messages[0].signals[0].receivers, [])
        self.assertEqual(db.messages[0].cycle_time, None)
        self.assertEqual(db.messages[0].send_type, None)
        self.assertEqual(repr(db.nodes[0]), "node('UnusedNode', None)")
        i = 0

        for message in db.messages:
            for signal in message.signals:
                if signal.choices is not None:
                    i += 1

        self.assertEqual(i, 15)
        self.assert_dbc_dump(db, filename)

    def test_dbc_signal_initial_value(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        db = cantools.database.load_file(filename)

        message = db.get_message_by_name('RT_IMU06_Accel')

        signal = message.get_signal_by_name('Validity_Accel_Longitudinal')
        self.assertEqual(signal.initial, None)
        self.assertEqual(
            repr(signal),
            "signal('Validity_Accel_Longitudinal', 0, 1, 'little_endian', False, "
            "None, 1, 0, None, None, 'None', False, None, None, None, {None: 'Valid when bit is "
            "set, invalid when bit is clear.'})")

        signal = message.get_signal_by_name('Validity_Accel_Lateral')
        self.assertEqual(signal.initial, 1)

        signal = message.get_signal_by_name('Validity_Accel_Vertical')
        self.assertEqual(signal.initial, 0)

        signal = message.get_signal_by_name('Accuracy_Accel')
        self.assertEqual(signal.initial, 127)

        signal = message.get_signal_by_name('Accel_Longitudinal')
        self.assertEqual(signal.initial, 32767)
        self.assertEqual(
            repr(signal),
            "signal('Accel_Longitudinal', 16, 16, 'little_endian', True, 32767, "
            "0.001, 0, -65, 65, 'g', False, None, None, None, {None: 'Longitudinal "
            "acceleration.  This is positive when the vehicle accelerates in a "
            "forwards direction.'})")

        signal = message.get_signal_by_name('Accel_Lateral')
        self.assertEqual(signal.initial, -30000)

        signal = message.get_signal_by_name('Accel_Vertical')
        self.assertEqual(signal.initial, 16120)

    def test_motohawk(self):
        filename = 'tests/files/dbc/motohawk.dbc'

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        self.assertEqual(db.buses, [])
        self.assertEqual(len(db.nodes), 2)
        self.assertEqual(db.nodes[0].name, 'PCM1')
        self.assertEqual(db.nodes[1].name, 'FOO')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(db.messages[0].bus_name, None)
        self.assertEqual(len(db.messages[0].signals[2].receivers), 2)
        self.assertEqual(db.messages[0].signals[2].receivers[0], 'PCM1')
        self.assertEqual(db.messages[0].signals[2].receivers[1], 'FOO')
        self.assertEqual(db.messages[0].signals[1].receivers, [])

        self.assert_dbc_dump(db, filename)

    def test_emc32(self):
        db = cantools.db.Database()

        with open('tests/files/dbc/emc32.dbc', 'r') as fin:
            db.add_dbc(fin)

        self.assertEqual(len(db.nodes), 1)
        self.assertEqual(db.nodes[0].name, 'EMV_Statusmeldungen')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(db.messages[0].signals[0].name, 'EMV_Aktion_Status_3')
        self.assertEqual(len(db.messages[0].signals[0].receivers), 1)
        self.assertEqual(db.messages[0].signals[1].name, 'EMV_Aktion_Status_4')
        self.assertEqual(len(db.messages[0].signals[1].receivers), 0)

        self.assertEqual(len(db.dbc.environment_variables), 17)
        env_var_1 = db.dbc.environment_variables['EMC_Azimuth']
        self.assertEqual(env_var_1.name, 'EMC_Azimuth')
        self.assertEqual(env_var_1.env_type, 1)
        self.assertEqual(env_var_1.minimum, -180)
        self.assertEqual(env_var_1.maximum, 400)
        self.assertEqual(env_var_1.unit, 'deg')
        self.assertEqual(env_var_1.initial_value, 0)
        self.assertEqual(env_var_1.env_id, 12)
        self.assertEqual(env_var_1.access_type, 'DUMMY_NODE_VECTOR0')
        self.assertEqual(env_var_1.access_node, 'Vector__XXX')
        self.assertEqual(env_var_1.comment, 'Elevation Head')
        self.assertEqual(
            repr(env_var_1),
            "environment_variable('EMC_Azimuth', 1, -180, 400, 'deg', 0, 12,"
            " 'DUMMY_NODE_VECTOR0', 'Vector__XXX', 'Elevation Head')")

        env_var_2 = db.dbc.environment_variables['EMC_TrdPower']
        self.assertEqual(env_var_2.name, 'EMC_TrdPower')
        self.assertEqual(env_var_2.comment, None)
        #
        # Test setters
        env_var_2.env_type = 1
        env_var_2.minimum = -180
        env_var_2.maximum = 400
        env_var_2.unit = 'deg'
        env_var_2.initial_value = 0
        env_var_2.env_id = 12
        env_var_2.access_type = 'DUMMY_NODE_VECTOR0'
        env_var_2.access_node = 'Vector__XXX'
        env_var_2.comment = 'Elevation Head'
        self.assertEqual(env_var_2.env_type, 1)
        self.assertEqual(env_var_2.minimum, -180)
        self.assertEqual(env_var_2.maximum, 400)
        self.assertEqual(env_var_2.unit, 'deg')
        self.assertEqual(env_var_2.initial_value, 0)
        self.assertEqual(env_var_2.env_id, 12)
        self.assertEqual(env_var_2.access_type, 'DUMMY_NODE_VECTOR0')
        self.assertEqual(env_var_2.access_node, 'Vector__XXX')
        self.assertEqual(env_var_2.comment, 'Elevation Head')
        self.assertEqual(
            repr(env_var_2),
            "environment_variable('EMC_TrdPower', 1, -180, 400, 'deg', 0, 12,"
            " 'DUMMY_NODE_VECTOR0', 'Vector__XXX', 'Elevation Head')")

    def test_foobar(self):
        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/foobar.dbc')

        self.assertEqual(len(db.nodes), 4)
        self.assertEqual(db.version, '2.0')
        self.assertEqual(
            repr(db),
            "version('2.0')\n"
            "\n"
            "node('FOO', None)\n"
            "node('BAR', 'fam \"1\"')\n"
            "node('FIE', None)\n"
            "node('FUM', None)\n"
            "\n"
            "message('Foo', 0x12330, True, 8, {None: 'Foo.'})\n"
            "  signal('Foo', 0, 12, 'big_endian', True, None, 0.01, "
            "250, 229.53, 270.47, 'degK', False, None, None, None, None)\n"
            "  signal('Bar', 24, 32, 'big_endian', True, None, 0.1, "
            "0, 0, 5, 'm', False, None, None, None, {None: 'Bar.'})\n"
            "\n"
            "message('Fum', 0x12331, True, 5, None)\n"
            "  signal('Fum', 0, 12, 'little_endian', True, None, 1, 0, 0, 10, "
            "'None', False, None, None, None, None)\n"
            "  signal('Fam', 12, 12, 'little_endian', True, None, 1, 0, "
            "0, 8, 'None', False, None, {1: \'Enabled\', 0: \'Disabled\'}, None, None)\n"
            "\n"
            "message('Bar', 0x12332, True, 4, None)\n"
            "  signal('Binary32', 0, 32, 'little_endian', True, None, 1, 0, None, "
            "None, 'None', False, None, None, None, None)\n"
            "\n"
            "message('CanFd', 0x12333, True, 64, None)\n"
            "  signal('Fie', 0, 64, 'little_endian', False, None, 1, 0, None, None, "
            "'None', False, None, None, None, None)\n"
            "  signal('Fas', 64, 64, 'little_endian', False, None, 1, 0, None, None, "
            "'None', False, None, None, None, None)\n"
            "\n"
            "message('FOOBAR', 0x30c, False, 8, None)\n"
            "  signal('ACC_02_CRC', 0, 12, 'little_endian', True, None, 1, 0, 0, 1, "
            "'None', False, None, None, None, None)\n")

        self.assertEqual(len(db.buses), 1)
        self.assertEqual(db.buses[0].name, 'TheBusName')
        self.assertEqual(db.buses[0].comment, None)
        self.assertEqual(db.buses[0].baudrate, 125000)

        message = db.get_message_by_frame_id(0x12331)
        self.assertEqual(message.name, 'Fum')
        self.assertEqual(message.bus_name, 'TheBusName')
        self.assertEqual(message.senders, ['FOO'])
        self.assertEqual(message.signals[0].is_float, False)

        message = db.get_message_by_frame_id(0x12332)
        self.assertEqual(message.name, 'Bar')
        self.assertEqual(message.bus_name, 'TheBusName')
        self.assertEqual(message.senders, ['FOO', 'BAR'])
        self.assertEqual(message.signals[0].receivers, ['FUM'])
        self.assertEqual(message.signals[0].is_float, True)
        self.assertEqual(message.signals[0].length, 32)

        message = db.get_message_by_frame_id(0x12333)
        self.assertEqual(message.name, 'CanFd')
        self.assertEqual(message.bus_name, 'TheBusName')
        self.assertEqual(message.senders, ['FOO'])
        self.assertEqual(message.signals[0].receivers, ['FUM'])
        self.assertEqual(message.signals[0].is_float, False)
        self.assertEqual(message.signals[0].length, 64)

    def test_foobar_encode_decode(self):
        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/foobar.dbc')

        messages = [
            (
                'Foo',
                {'Foo': 250, 'Bar': 0.0},
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
            ),
            (
                'Fum',
                {'Fum': 9, 'Fam': 5},
                b'\x09\x50\x00\x00\x00'
            ),
            (
                'Bar',
                {'Binary32': 1.0},
                b'\x00\x00\x80\x3f'
            ),
            (
                'CanFd',
                {'Fie': 0x123456789abcdef, 'Fas': 0xdeadbeefdeadbeef},
                b'\xef\xcd\xab\x89\x67\x45\x23\x01'
                b'\xef\xbe\xad\xde\xef\xbe\xad\xde'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
            )
        ]

        for name, decoded_message, encoded_message in messages:
            encoded = db.encode_message(name, decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message(name, encoded)
            self.assertEqual(decoded, decoded_message)

    def test_foobar_encode_decode_frame_ids(self):
        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/foobar.dbc')

        messages = [
            (
                0x12330,
                {'Foo': 250, 'Bar': 0.0},
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
            ),
            (
                0x12331,
                {'Fum': 9, 'Fam': 5},
                b'\x09\x50\x00\x00\x00'
            ),
            (
                0x12332,
                {'Binary32': 1.0},
                b'\x00\x00\x80\x3f'
            ),
            (
                0x12333,
                {'Fie': 0x123456789abcdef, 'Fas': 0xdeadbeefdeadbeef},
                b'\xef\xcd\xab\x89\x67\x45\x23\x01'
                b'\xef\xbe\xad\xde\xef\xbe\xad\xde'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00'
            )
        ]

        for frame_id, decoded_message, encoded_message in messages:
            encoded = db.encode_message(frame_id, decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message(frame_id, encoded)
            self.assertEqual(decoded, decoded_message)

    def test_foobar_decode_masked_frame_id(self):
        db = cantools.db.Database(frame_id_mask=0xff)
        db.add_dbc_file('tests/files/dbc/foobar.dbc')

        frame_ids = [
            0x12331,
            0xfff31,
            0x00031
        ]

        for frame_id in frame_ids:
            db.get_message_by_frame_id(frame_id)

    def test_dbc_dump_val_table(self):
        filename = 'tests/files/dbc/val_table.dbc'
        db = cantools.database.load_file(filename)

        self.assertEqual(
            db.dbc.value_tables,
            {
                'Table3': {16: '16', 0: '0', 2: '2', 7: '7'},
                'Table2': {},
                'Table1': {0: 'Zero', 1: 'One'}
            })
        self.assert_dbc_dump(db, filename)

    def test_dbc_load_empty_choice(self):
        filename = 'tests/files/dbc/empty_choice.dbc'
        db = cantools.database.load_file(filename)

        message = db.get_message_by_frame_id(10)
        self.assertEqual(message.signals[0].name, 'non_empty_choice')
        self.assertEqual(message.signals[0].choices,
                         {254: 'Error', 255: 'Not available'})
        self.assertEqual(message.signals[1].name, 'empty_choice')
        self.assertEqual(message.signals[1].choices, None)
        self.assertEqual(message.signals[2].name, 'no_choice')
        self.assertEqual(message.signals[2].choices, None)

    def test_dbc_load_choices(self):
        filename = 'tests/files/dbc/choices.dbc'
        db = cantools.database.load_file(filename)
        msg = db.messages[0]
        sig = msg.signals[0]

        self.assertEqual(sig.choices[0].value, 0)
        self.assertEqual(sig.choices[0].name, 'With space')

    def test_dbc_load_choices_issue_with_name(self):
        filename = 'tests/files/dbc/choices_issue_with_name.dbc'

        db = cantools.database.load_file(filename, prune_choices=False)
        msg = db.messages[0]
        sig = msg.signals[0]

        self.assertEqual(sig.choices[0].value, 0)
        self.assertEqual(sig.choices[0].name, 'SignalWithChoices_CmdRespErr')
        self.assertEqual(sig.choices[1].value, 1)
        self.assertEqual(sig.choices[1].name, 'SignalWithChoices_CmdRespOK')

        db = cantools.database.load_file(filename, prune_choices=True)
        msg = db.messages[0]
        sig = msg.signals[0]

        self.assertEqual(sig.choices[0].value, 0)
        self.assertEqual(sig.choices[0].name, 'CmdRespErr')
        self.assertEqual(sig.choices[1].value, 1)
        self.assertEqual(sig.choices[1].name, 'CmdRespOK')

        # make sure that the default for pruning signal choices is False
        db = cantools.database.load_file(filename)
        msg = db.messages[0]
        sig = msg.signals[0]

        self.assertEqual(sig.choices[0].value, 0)
        self.assertEqual(sig.choices[0].name, 'SignalWithChoices_CmdRespErr')
        self.assertEqual(sig.choices[1].value, 1)
        self.assertEqual(sig.choices[1].name, 'SignalWithChoices_CmdRespOK')



    def test_padding_bit_order(self):
        """Encode and decode signals with reversed bit order.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/padding_bit_order.dbc')

        # Message 0.
        msg0_frame_id = 1

        decoded_message = {
            'B': 1,      # should set byte[0]bit[7]=1
            'A': 0x2c9,  # should set byte[0]bit[1]=1 and byte[1]=c9
            'D': 0,      # should set byte[5]bit[7]=0
            'C': 0x2c9   # should set byte[4]bit[1]=1 and byte [5]=c9
        }
        encoded_message = b'\x82\xc9\x00\x00\x02\xc9\x00\x00'

        encoded = db.encode_message(msg0_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(msg0_frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # Message 1.
        msg1_frame_id = 2

        decoded_message = {
            'E': 1,      # should set byte[0]bit[0]=1
            'F': 0x2c9,  # should set byte[0]bit[7:1]=92 and byte[1]=05
            'G': 0,      # should set byte[4]bit[0]=0
            'H': 0x2c9   # should set byte[4]bit[7:1]=92 and byte[5]=05
        }
        encoded_message = b'\x93\x05\x00\x00\x92\x05\x00\x00'

        encoded = db.encode_message(msg1_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(msg1_frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # Message 2.
        msg2_frame_id = 3

        decoded_message = {
            'I': 1,  # should set byte[0]bit[3:0]=1
            'J': 2,  # should set byte[0]bit[7:4]=2
            'K': 3   # should set byte[1]bit[3:0]=3
        }
        encoded_message = b'\x21\x03\x00\x00\x00\x00\x00\x00'

        encoded = db.encode_message(msg2_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(msg2_frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # Message 3.
        msg3_frame_id = 4

        decoded_message = {
            'L': 0x0123456789abcdef
        }
        encoded_message = b'\x01\x23\x45\x67\x89\xab\xcd\xef'

        encoded = db.encode_message(msg3_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(msg3_frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # Message 4.
        msg4_frame_id = 5

        decoded_message = {
            'M': 0x0123456789abcdef
        }
        encoded_message = b'\xef\xcd\xab\x89\x67\x45\x23\x01'

        encoded = db.encode_message(msg4_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(msg4_frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_motohawk_encode_decode(self):
        """Encode and decode the signals in a ExampleMessage frame.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/motohawk.dbc')

        example_message_name = 'ExampleMessage'
        example_message_frame_id = 496

        # Encode and decode with non-enumerated values. Temperature
        # 229.53 is encoded as a negative value.
        datas = [
            (
                {
                    'Temperature': 250.55,
                    'AverageRadius': 3.2,
                    'Enable': 1
                },
                b'\xc0\x06\xe0\x00\x00\x00\x00\x00'
            ),
            (
                {
                    'Temperature': 229.53,
                    'AverageRadius': 0,
                    'Enable': 0
                },
                b'\x01\x00\x20\x00\x00\x00\x00\x00'
            )
        ]

        for decoded_message, encoded_message in datas:
            encoded = db.encode_message(example_message_frame_id,
                                        decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message(example_message_frame_id,
                                        encoded,
                                        decode_choices=False)
            self.assertEqual(decoded, decoded_message)

            # check that encode(decode(encode(decoded))) == encode(decoded)
            encoded2 = db.encode_message(example_message_frame_id, decoded)
            self.assertEqual(encoded2, encoded)

        # Make sure that the decoded message dictionary does not
        # contain any unknown entries
        decoded_message = datas[0][0]
        decoded_message["UnspecifiedSignal"] = "pointless"
        with self.assertRaises(cantools.database.EncodeError):
            db.encode_message(example_message_frame_id,
                              decoded_message)

        # make sure that we don't get this error if we use non-strict
        # encoding
        db.encode_message(example_message_frame_id,
                          decoded_message,
                          strict=False)
        # Encode with enumerated values.
        decoded_message = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 'Enabled'
        }
        encoded_message = b'\xc0\x06\xe0\x00\x00\x00\x00\x00'

        # By frame id.
        encoded = db.encode_message(example_message_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(example_message_frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # check that encode(decode(data)) == data
        encoded2 = db.encode_message(example_message_frame_id, decoded)
        self.assertEqual(encoded2, encoded)

        # By name.
        encoded = db.encode_message(example_message_name, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(example_message_name, encoded)
        self.assertEqual(decoded, decoded_message)

        # make sure that unknown signals are not accepted in
        # strict mode
        filename = 'tests/files/dbc/multiplex_choices.dbc'
        db = cantools.database.load_file(filename)

        decoded_message = {
            # the following signals are required by MULTIPLEXOR_8
            'Multiplexor': 'MULTIPLEXOR_8',
            'BIT_J': 0,
            'BIT_C': 0,
            'BIT_G': 0,
            'BIT_L': 0,

            # BIT_A is only featured by MULTIPLEXOR_24
            'BIT_A': 0,
        }

        with self.assertRaises(cantools.database.EncodeError):
            db.encode_message("Message2",
                              decoded_message)

    def test_motohawk_decode_truncated(self):
        """Decoding truncated frames.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/motohawk.dbc')

        msgname = 'ExampleMessage'
        with self.assertRaises(Exception):
            db.decode_message(msgname, b'\x00\xff')

        decoded = db.decode_message(msgname, b'\x00\x11', allow_truncated=True)
        self.assertEqual(decoded, {'AverageRadius': 0.0, 'Enable': 'Disabled'})

        msg = db.get_message_by_name(msgname)
        with self.assertRaises(Exception):
            msg.decode(b'\x00\xff')

        decoded = msg.decode(b'\x00\xff', allow_truncated=True)
        self.assertEqual(decoded, {'AverageRadius': 0.0, 'Enable': 'Disabled'})

    def test_decode_truncated_multiplexed(self):
        db = cantools.database.load_file('tests/files/dbc/multiplex.dbc')
        msg = db.get_message_by_name('Message1')

        encoded = bytes.fromhex('60008c35c3000000')
        decoded_full = msg.decode(encoded)

        # the last byte of the message does not encode any signals,
        # but the specified frame length must still be observed!
        with self.assertRaises(Exception):
            msg.decode(encoded[:-1])

        # partial message without omitted signals
        self.assertEqual(msg.decode(encoded[:-1], allow_truncated=True),
                         decoded_full)

        # partial message with omitted signals
        self.assertEqual(msg.decode(encoded[:-4], allow_truncated=True),
                         {
                             'Multiplexor': 24,
                             'BIT_J': 1,
                             'BIT_C': 1,
                             'BIT_G': 1,
                             'BIT_L': 1,
                             'BIT_A': 1,
                             'BIT_K': 1,
                             'BIT_E': 1
                         })

        # partial message with omitted multiplexor signal
        self.assertEqual(msg.decode(b'', allow_truncated=True), {})

    def test_big_endian_no_decode_choices(self):
        """Decode a big endian signal with `decode_choices` set to False.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/motohawk.dbc')

        decoded_message = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 1
        }
        encoded_message = b'\xc0\x06\xe0\x00\x00\x00\x00\x00'

        decoded = db.decode_message(496,
                                    encoded_message,
                                    decode_choices=False)
        self.assertEqual(decoded, decoded_message)

    def test_little_endian_no_decode_choices(self):
        """Decode a little endian signal with `decode_choices` set to False.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/socialledge.dbc')

        decoded_message = {
            'DRIVER_HEARTBEAT_cmd': 1
        }
        encoded_message = b'\x01\x00\x00\x00\x00\x00\x00\x00'

        decoded = db.decode_message(100,
                                    encoded_message,
                                    decode_choices=False)
        self.assertEqual(decoded, decoded_message)

        decoded_message = {
            'DRIVER_HEARTBEAT_cmd': 'DRIVER_HEARTBEAT_cmd_SYNC'
        }

        decoded = db.decode_message(100,
                                    encoded_message)
        self.assertEqual(decoded, decoded_message)

    def test_encode_decode_no_scaling(self):
        """Encode and decode a message without scaling the signal values.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/motohawk.dbc')

        decoded_message = {
            'Temperature': 55,
            'AverageRadius': 32,
            'Enable': 'Enabled'
        }
        encoded_message = b'\xc0\x06\xe0\x00\x00\x00\x00\x00'

        encoded = db.encode_message(496,
                                    decoded_message,
                                    scaling=False)
        self.assertEqual(encoded, encoded_message)

        decoded = db.decode_message(496,
                                    encoded,
                                    scaling=False)
        self.assertEqual(decoded, decoded_message)

    def test_encode_signal_strict(self):
        """Test signal out of range errors.

        """

        db = cantools.database.load_file('tests/files/kcd/signal_range.kcd')

        # Values in range.
        datas = [
            ('Message2', 3),
            ('Message3', 0)
        ]

        for message_name, signal_value in datas:
            db.encode_message(message_name, {'Signal1': signal_value})

        # Values out of range.
        datas = [
            (
                'Message1',
                0,
                'Expected signal "Signal1" value greater than or equal to 1 in '
                'message "Message1", but got 0.'
            ),
            (
                'Message1',
                3,
                'Expected signal "Signal1" value less than or equal to 2 in '
                'message "Message1", but got 3.'
            ),
            (
                'Message2',
                0,
                'Expected signal "Signal1" value greater than or equal to 1 in '
                'message "Message2", but got 0.'
            ),
            (
                'Message3',
                3,
                'Expected signal "Signal1" value less than or equal to 2 in '
                'message "Message3", but got 3.'
            ),
            (
                'Message4',
                1.9,
                'Expected signal "Signal1" value greater than or equal to 2 in '
                'message "Message4", but got 1.9.'
            ),
            (
                'Message4',
                8.1,
                'Expected signal "Signal1" value less than or equal to 8 in '
                'message "Message4", but got 8.1.'
            )
        ]

        for message_name, signal_value, error_message in datas:
            with self.assertRaises(cantools.database.EncodeError) as cm:
                db.encode_message(message_name, {'Signal1': signal_value})

            self.assertEqual(str(cm.exception), error_message)

        # Values out of range, but range checks disabled.
        datas = [
            ('Message1', 0),
            ('Message1', 3)
        ]

        for message_name, signal_value in datas:
            db.encode_message(message_name,
                              {'Signal1': signal_value},
                              strict=False)

        # Missing value.
        with self.assertRaises(cantools.database.EncodeError) as cm:
            db.encode_message('Message1', {'Foo': 1})

        self.assertEqual(
            str(cm.exception),
            'The signal "Signal1" is required for encoding.')

        # Missing value, but checks disabled.
        with self.assertRaises(KeyError):
            db.encode_message('Message1', {'Foo': 1}, strict=False)

    def test_encode_decode_no_scaling_no_decode_choices(self):
        """Encode and decode a message without scaling the signal values, not
        decoding choices.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/motohawk.dbc')

        decoded_message = {
            'Temperature': 3,
            'AverageRadius': 2,
            'Enable': 1
        }
        encoded_message = b'\x84\x00\x60\x00\x00\x00\x00\x00'

        encoded = db.encode_message(496,
                                    decoded_message,
                                    scaling=False)
        self.assertEqual(encoded, encoded_message)

        decoded = db.decode_message(496,
                                    encoded,
                                    decode_choices=False,
                                    scaling=False)
        self.assertEqual(decoded, decoded_message)

    def test_socialledge(self):
        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/socialledge.dbc')

        # Verify nodes.
        self.assertEqual(len(db.nodes), 5)
        self.assertEqual(db.nodes[0].name, 'DBG')
        self.assertEqual(db.nodes[0].comment, None)
        self.assertEqual(db.nodes[1].name, 'DRIVER')
        self.assertEqual(db.nodes[1].comment,
                         '// The driver controller driving the car //')
        self.assertEqual(db.nodes[2].name, 'IO')
        self.assertEqual(db.nodes[2].comment, None)
        self.assertEqual(db.nodes[3].name, 'MOTOR')
        self.assertEqual(db.nodes[3].comment,
                         'The motor controller of the car')
        self.assertEqual(db.nodes[4].name, 'SENSOR')
        self.assertEqual(db.nodes[4].comment,
                         'The sensor controller of the car')

        # Verify messages and their signals.
        self.assertEqual(len(db.messages), 5)
        self.assertEqual(db.messages[0].name, 'DRIVER_HEARTBEAT')
        self.assertEqual(db.messages[0].comment,
                         'Sync message used to synchronize the controllers')
        self.assertEqual(db.messages[0].signals[0].choices[0],
                         'DRIVER_HEARTBEAT_cmd_NOOP')
        self.assertEqual(db.messages[0].signals[0].choices[1],
                         'DRIVER_HEARTBEAT_cmd_SYNC')
        self.assertEqual(db.messages[0].signals[0].choices[2],
                         'DRIVER_HEARTBEAT_cmd_REBOOT')
        self.assertEqual(db.messages[1].name, 'IO_DEBUG')
        self.assertEqual(db.messages[2].name, 'MOTOR_CMD')
        self.assertEqual(db.messages[3].name, 'MOTOR_STATUS')
        self.assertEqual(db.messages[4].name, 'SENSOR_SONARS')

        sensor_sonars = db.messages[-1]

        self.assertFalse(db.messages[0].is_multiplexed())
        self.assertTrue(sensor_sonars.is_multiplexed())
        self.assertEqual(sensor_sonars.signals[-1].name, 'SENSOR_SONARS_no_filt_rear')
        self.assertEqual(sensor_sonars.signals[-1].multiplexer_ids, [1])
        self.assertEqual(sensor_sonars.signals[2].name, 'SENSOR_SONARS_left')
        self.assertEqual(sensor_sonars.signals[2].multiplexer_ids, [0])
        self.assertEqual(sensor_sonars.signals[0].name, 'SENSOR_SONARS_mux')
        self.assertEqual(sensor_sonars.signals[0].is_multiplexer, True)

        sensor_sonars.assert_signals_encodable({
            'SENSOR_SONARS_mux': 0,
            'SENSOR_SONARS_err_count': 1,
            'SENSOR_SONARS_left': 2,
            'SENSOR_SONARS_middle': 3,
            'SENSOR_SONARS_right': 4,
            'SENSOR_SONARS_rear': 5,
            #'foo':'bar',
        }, scaling=True)

        with self.assertRaises(cantools.database.EncodeError):
            sensor_sonars.assert_signals_encodable(123, scaling=True)

        self.assertEqual(sensor_sonars.signal_tree,
                         [
                             {
                                 'SENSOR_SONARS_mux': {
                                     0: [
                                         'SENSOR_SONARS_left',
                                         'SENSOR_SONARS_middle',
                                         'SENSOR_SONARS_right',
                                         'SENSOR_SONARS_rear'
                                     ],
                                     1: [
                                         'SENSOR_SONARS_no_filt_left',
                                         'SENSOR_SONARS_no_filt_middle',
                                         'SENSOR_SONARS_no_filt_right',
                                         'SENSOR_SONARS_no_filt_rear'
                                     ]
                                 }
                             },
                             'SENSOR_SONARS_err_count'
                         ])

        self.assertEqual(db.version, '')

    def test_socialledge_encode_decode_mux_0(self):
        """Encode and decode the signals in a SENSOR_SONARS frame with mux 0.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/socialledge.dbc')

        frame_id = 200

        decoded_message = {
            'SENSOR_SONARS_mux': 0,
            'SENSOR_SONARS_err_count': 1,
            'SENSOR_SONARS_left': 2,
            'SENSOR_SONARS_middle': 3,
            'SENSOR_SONARS_right': 4,
            'SENSOR_SONARS_rear': 5
        }
        encoded_message = b'\x10\x00\x14\xe0\x01( \x03'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_socialledge_encode_decode_mux_1(self):
        """Encode and decode the signals in a SENSOR_SONARS frame with mux 1.

        """

        db = cantools.db.Database()
        db.add_dbc_file('tests/files/dbc/socialledge.dbc')

        frame_id = 200

        decoded_message = {
            'SENSOR_SONARS_mux': 1,
            'SENSOR_SONARS_err_count': 2,
            'SENSOR_SONARS_no_filt_left': 3,
            'SENSOR_SONARS_no_filt_middle': 4,
            'SENSOR_SONARS_no_filt_right': 5,
            'SENSOR_SONARS_no_filt_rear': 6
        }
        encoded_message = b'!\x00\x1e\x80\x022\xc0\x03'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_encode_decode_containers(self):
        """Encode and decode container messages

        """

        db = cantools.db.load_file('tests/files/arxml/system-4.2.arxml')

        db_msg = db.get_message_by_name('OneToContainThemAll')

        orig_msg = [
            (
                'message1',
                {
                    'message1_SeqCounter': 123,
                    'message1_CRC': 456,
                    'signal6': 'zero',
                    'signal1': 5.2,
                    'signal5': 3.1415
                }
            ),
        ]

        encoded = db_msg.encode(orig_msg)
        encoded2 = db.encode_message(db_msg.frame_id, orig_msg)
        encoded3 = db.encode_message(db_msg.name, orig_msg)

        self.assertEqual(encoded, b'\n\x0b\x0c\t{\x00\xc8\x01\x04V\x0eI@')
        self.assertEqual(encoded, encoded2)
        self.assertEqual(encoded, encoded3)

        # make sure that we do not accept signal name -> value
        # dictionaries when encoding container frames
        with self.assertRaises(ValueError):
            db.encode_message(db_msg.frame_id,
                              {'signal': 'value'},
                              strict=False)
        with self.assertRaises(ValueError):
            db_msg.encode({'signal': 'value'},
                          strict=False)

        # cannot decode a container without explicitly specifying that
        # containers ought to be decoded
        with self.assertRaises(cantools.database.DecodeError):
            decoded = db_msg.decode(encoded)
        with self.assertRaises(cantools.database.DecodeError):
            decoded = db.decode_message(db_msg.frame_id, encoded)

        decoded = db_msg.decode(encoded, decode_containers=True)
        decoded2 = db.decode_message(db_msg.frame_id,
                                     encoded,
                                     decode_containers=True)
        decoded3 = db.decode_message(db_msg.name,
                                     encoded,
                                     decode_containers=True)

        self.assertEqual(decoded[0][0].name, orig_msg[0][0])
        self.assertEqual(str(decoded[0][1]['signal6']),
                         orig_msg[0][1]['signal6'])
        self.assertEqual(decoded[0][1]['signal1'], 5)
        self.assertAlmostEqual(decoded[0][1]['signal5'], 3.1415)

        self.assertEqual(decoded, decoded2)
        self.assertEqual(decoded, decoded3)

        # make sure that we won't let ourselves disturb by trailing
        # garbage
        encoded += b'\x00\x00\x00\x00'
        decoded2 = db.decode_message(db_msg.frame_id,
                                     encoded,
                                     decode_containers=True)
        self.assertEqual(len(decoded2), 2)
        self.assertEqual(decoded2[0], decoded[0])
        self.assertEqual(decoded2[1], (0, b''))

        # specify a contained message as raw data
        orig_msg.append( (0xddeeff, b'\xa0\xa1\xa2\xa3\xa4') )
        encoded = db_msg.encode(orig_msg)
        self.assertEqual(encoded,
                         b'\n\x0b\x0c\t{\x00\xc8\x01\x04V\x0e'
                         b'I@\xdd\xee\xff\x05\xa0\xa1\xa2\xa3\xa4')


        decoded = db_msg.decode(encoded, decode_containers=True)
        self.assertEqual(decoded[0], decoded2[0])
        self.assertEqual(decoded[1], (0xddeeff, b'\xa0\xa1\xa2\xa3\xa4') )

        # specify a contained message using its name but the payload
        # as raw bytes
        orig_msg[-1] = ('message1', b'\xa0\xa1\xa2\xa3\xa4')
        with self.assertRaises(cantools.database.EncodeError):
            # fail if the raw payload's length is unequal to the size
            # specified of the contained message
            encoded2 = db_msg.encode(orig_msg)

        # succeed if the sizes match
        orig_msg[-1] = ('message1', b'\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8')
        encoded2 = db_msg.encode(orig_msg)

        self.assertEqual(encoded2,
                         b'\n\x0b\x0c\t{\x00\xc8\x01\x04V\x0eI@\n\x0b\x0c\t'
                         b'\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8')

    def test_gather_signals(self):
        db = cantools.db.load_file('tests/files/arxml/system-4.2.arxml')

        db_msg = db.get_message_by_name('MultiplexedMessage')

        global_signal_dict = {
            'MultiplexedStatic': 4,
            'MultiplexedMessage_selector1': 'SELECT_HELLO',
            'OneToContainThemAll_selector1': 'SELECT_WORLD',
            'Hello': 5,
            'World2': 1,
            'World1': 0,
            'MultiplexedStatic2': 123,
            'signal6' : 'zero',
            'signal1': 15,
            'signal5': 3.141529,

            'message1_SeqCounter': 123,
            'message1_CRC': 456,
        }

        msg_signal_dict = db_msg.gather_signals(global_signal_dict)

        self.assertEqual(list(msg_signal_dict),
                         ['MultiplexedStatic',
                          'MultiplexedMessage_selector1',
                          'MultiplexedStatic2',
                          'Hello'])

        # a missing signal should raise an exception
        del global_signal_dict['Hello']
        with self.assertRaises(cantools.database.EncodeError):
            msg_signal_dict = db_msg.gather_signals(global_signal_dict)

        global_signal_dict['MultiplexedMessage_selector1'] = 'SELECT_non_existant'
        with self.assertRaises(cantools.database.EncodeError):
            msg_signal_dict = db_msg.gather_signals(global_signal_dict)

        global_signal_dict['MultiplexedMessage_selector1'] = 'SELECT_WORLD'
        msg_signal_dict = db_msg.gather_signals(global_signal_dict)
        self.assertEqual(list(msg_signal_dict),
                         ['MultiplexedStatic',
                          'MultiplexedMessage_selector1',
                          'MultiplexedStatic2',
                          'World2',
                          'World1',
                          ])

        # test the gather method for container messages
        cmsg = db.get_message_by_name('OneToContainThemAll')

        cmplexer = cmsg.get_contained_message_by_name('multiplexed_message')
        ccontent = cmsg.gather_container([0xa0b0c, 'message1', cmplexer],
                                         global_signal_dict)

        cnames = [ x[0].name for x in ccontent ]
        self.assertEqual(cnames,
                         ['message1', 'message1', 'multiplexed_message'])
        csignals = [ list(x[1]) for x in ccontent ]
        self.assertEqual(csignals,
                         [['message1_SeqCounter', 'message1_CRC', 'signal6',
                           'signal1', 'signal5'],
                          ['message1_SeqCounter', 'message1_CRC', 'signal6',
                           'signal1', 'signal5'],
                          ['MultiplexedStatic',
                           'OneToContainThemAll_selector1',
                           'MultiplexedStatic2',
                           'World2',
                           'World1']
                          ])

        # unknown contained message
        with self.assertRaises(cantools.database.EncodeError):
            cmsg.gather_container([0x00733d], global_signal_dict)

        # make sure that what we've collected is encodable as a
        # container
        ccontent[0] = (0xa0b0c, ccontent[0][1])
        ccontent[1] = ('message1', ccontent[1][1])
        ccontent.append((0x010203, b'\x00\x11\x22'))
        cmsg.assert_container_encodable(ccontent, scaling=True)

        # dictionary as input type is not encodable for containers
        with self.assertRaises(cantools.database.EncodeError):
            cmsg.assert_container_encodable(msg_signal_dict, scaling=True)

        # normal messages don't support assert_container_encodable()
        with self.assertRaises(cantools.database.EncodeError):
            db_msg.assert_container_encodable(ccontent, scaling=True)

        # container messages don't support assert_signals_encodable()
        with self.assertRaises(cantools.database.EncodeError):
            cmsg.assert_signals_encodable(msg_signal_dict, scaling=True)

        # make sure that the content specification for containers
        # cannot be used for normal messages
        with self.assertRaises(cantools.database.EncodeError):
            cmsg.assert_signals_encodable(ccontent, scaling=True)

        # make sure that container messages messages cannot be
        # nested
        ccontent.append((cmsg, b'\x00\x11\x22'))
        with self.assertRaises(cantools.database.EncodeError):
            cmsg.assert_container_encodable(ccontent, scaling=True)
        del ccontent[-1]

        # make sure that messages without a header id cannot be contained
        db_msg.header_id = None
        ccontent = [ (db_msg, b'') ]
        with self.assertRaises(cantools.database.EncodeError):
            cmsg.assert_container_encodable(ccontent, scaling=True)

        # make sure that if payload is specified as raw data must
        # exhibit the correct length
        ccontent = [ ('message1', b'\x00\x11\x22\x33\x44\x55\x66\x77\x88') ]
        cmsg.assert_container_encodable(ccontent, scaling=True)
        ccontent = [ ('message1', b'\x00\x11\x22\x33\x44') ]
        with self.assertRaises(cantools.database.EncodeError):
            cmsg.assert_container_encodable(ccontent, scaling=True)

    def test_get_message_by_frame_id_and_name(self):
        with open('tests/files/dbc/motohawk.dbc', 'r') as fin:
            db = cantools.db.load(fin)

        message = db.get_message_by_name('ExampleMessage')
        self.assertEqual(message.name, 'ExampleMessage')

        message = db.get_message_by_frame_id(496)
        self.assertEqual(message.frame_id, 496)

    def test_get_signal_by_name(self):
        db = cantools.db.load_file('tests/files/dbc/foobar.dbc')

        message = db.get_message_by_name('Foo')

        signal = message.get_signal_by_name('Foo')
        self.assertEqual(signal.name, 'Foo')

        signal = message.get_signal_by_name('Bar')
        self.assertEqual(signal.name, 'Bar')

        with self.assertRaises(KeyError) as cm:
            message.get_signal_by_name('Fum')

        self.assertEqual(str(cm.exception), "'Fum'")

    def test_cp1252_dbc(self):
        db = cantools.database.load_file('tests/files/dbc/cp1252.dbc')

        self.assertEqual(
            db.nodes[0].comment,
            bytearray([
                v for v in range(256)
                if v not in [34, 129, 141, 143, 144, 157]
            ])[32:].decode('cp1252'))

    def test_the_homer(self):
        db = cantools.db.load_file('tests/files/kcd/the_homer.kcd')

        self.assertEqual(db.version, '1.23')
        self.assertEqual(len(db.nodes), 18)
        self.assertEqual(db.nodes[0].name, 'Motor ACME')
        self.assertEqual(db.nodes[1].name, 'Motor alternative supplier')
        self.assertEqual(len(db.buses), 3)
        self.assertEqual(db.buses[0].name, 'Motor')
        self.assertEqual(db.buses[1].name, 'Instrumentation')
        self.assertEqual(db.buses[2].name, 'Comfort')
        self.assertEqual(db.buses[0].comment, None)
        self.assertEqual(db.buses[0].baudrate, 500000)
        self.assertEqual(db.buses[1].baudrate, 125000)

        self.assertEqual(len(db.messages), 33)
        self.assertEqual(db.messages[0].frame_id, 0xa)
        self.assertEqual(db.messages[0].is_extended_frame, False)
        self.assertEqual(db.messages[0].is_fd, False)
        self.assertEqual(db.messages[0].name, 'Airbag')
        self.assertEqual(db.messages[0].length, 3)
        self.assertEqual(db.messages[0].senders, ['Brake ACME'])
        self.assertEqual(len(db.messages[0].signals), 8)
        self.assertEqual(db.messages[0].comment, None)
        self.assertEqual(db.messages[0].send_type, None)
        self.assertEqual(db.messages[0].cycle_time, None)
        self.assertEqual(db.messages[0].bus_name, 'Motor')

        self.assertEqual(db.messages[1].frame_id, 0x0B2)
        self.assertEqual(db.messages[1].name, 'ABS')
        self.assertEqual(db.messages[1].cycle_time, 100)
        self.assertEqual(db.messages[1].senders,
                         ['Brake ACME', 'Brake alternative supplier'])

        self.assertEqual(db.messages[3].frame_id, 0x400)
        self.assertEqual(db.messages[3].name, 'Emission')
        self.assertEqual(db.messages[3].length, 5)
        self.assertEqual(db.messages[3].senders, [])

        self.assertEqual(db.messages[-1].bus_name, 'Comfort')

        seat_configuration = db.messages[0].signals[-1]

        self.assertEqual(seat_configuration.name, 'SeatConfiguration')
        self.assertEqual(seat_configuration.start, 16)
        self.assertEqual(seat_configuration.length, 8)
        self.assertEqual(seat_configuration.receivers, [])
        self.assertEqual(seat_configuration.byte_order, 'little_endian')
        self.assertEqual(seat_configuration.is_signed, False)
        self.assertEqual(seat_configuration.is_float, False)
        self.assertEqual(seat_configuration.scale, 1)
        self.assertEqual(seat_configuration.offset, 0)
        self.assertEqual(seat_configuration.minimum, None)
        self.assertEqual(seat_configuration.maximum, None)
        self.assertEqual(seat_configuration.decimal.scale, 1)
        self.assertEqual(seat_configuration.decimal.offset, 0)
        self.assertEqual(seat_configuration.decimal.minimum, None)
        self.assertEqual(seat_configuration.decimal.maximum, None)
        self.assertEqual(seat_configuration.unit, None)
        self.assertEqual(seat_configuration.choices, None)
        self.assertEqual(seat_configuration.comment, None)

        tank_temperature = db.messages[10].signals[1]

        self.assertEqual(tank_temperature.name, 'TankTemperature')
        self.assertEqual(tank_temperature.start, 16)
        self.assertEqual(tank_temperature.length, 16)
        self.assertEqual(tank_temperature.receivers, [])
        self.assertEqual(tank_temperature.byte_order, 'little_endian')
        self.assertEqual(tank_temperature.is_signed, True)
        self.assertEqual(tank_temperature.is_float, False)
        self.assertEqual(tank_temperature.scale, 1)
        self.assertEqual(tank_temperature.offset, 0)
        self.assertEqual(tank_temperature.minimum, None)
        self.assertEqual(tank_temperature.maximum, None)
        self.assertEqual(tank_temperature.decimal.scale, 1)
        self.assertEqual(tank_temperature.decimal.offset, 0)
        self.assertEqual(tank_temperature.decimal.minimum, None)
        self.assertEqual(tank_temperature.decimal.maximum, None)
        self.assertEqual(tank_temperature.unit, 'Cel')
        self.assertEqual(tank_temperature.choices, None)
        self.assertEqual(tank_temperature.comment, None)

        info_0 = db.messages[1].signals[0]

        self.assertEqual(info_0.name, 'Info0')
        self.assertEqual(info_0.start, 0)
        self.assertEqual(info_0.length, 8)
        self.assertEqual(info_0.receivers, [])
        self.assertEqual(info_0.byte_order, 'little_endian')
        self.assertEqual(info_0.is_signed, False)
        self.assertEqual(info_0.is_float, False)
        self.assertEqual(info_0.is_multiplexer, False)
        self.assertEqual(info_0.multiplexer_ids, [0])
        self.assertEqual(info_0.multiplexer_signal, 'ABS_InfoMux')

        info_2 = db.messages[1].signals[1]

        self.assertEqual(info_2.name, 'Info2')
        self.assertEqual(info_2.start, 0)
        self.assertEqual(info_2.length, 8)
        self.assertEqual(info_2.receivers, [])
        self.assertEqual(info_2.byte_order, 'little_endian')
        self.assertEqual(info_2.is_signed, False)
        self.assertEqual(info_2.is_float, False)
        self.assertEqual(info_2.is_multiplexer, False)
        self.assertEqual(info_2.multiplexer_ids, [1])
        self.assertEqual(info_2.multiplexer_signal, 'ABS_InfoMux')

        info_4 = db.messages[1].signals[2]

        self.assertEqual(info_4.name, 'Info4')
        self.assertEqual(info_4.start, 0)
        self.assertEqual(info_4.length, 8)
        self.assertEqual(info_4.receivers, [])
        self.assertEqual(info_4.byte_order, 'little_endian')
        self.assertEqual(info_4.is_signed, False)
        self.assertEqual(info_4.is_float, False)
        self.assertEqual(info_4.is_multiplexer, False)
        self.assertEqual(info_4.multiplexer_ids, [2])
        self.assertEqual(info_4.multiplexer_signal, 'ABS_InfoMux')

        info_6 = db.messages[1].signals[3]

        self.assertEqual(info_6.name, 'Info6')
        self.assertEqual(info_6.start, 0)
        self.assertEqual(info_6.length, 8)
        self.assertEqual(info_6.receivers, [])
        self.assertEqual(info_6.byte_order, 'little_endian')
        self.assertEqual(info_6.is_signed, False)
        self.assertEqual(info_6.is_float, False)
        self.assertEqual(info_6.is_multiplexer, False)
        self.assertEqual(info_6.multiplexer_ids, [3])
        self.assertEqual(info_6.multiplexer_signal, 'ABS_InfoMux')

        abs_info_mux = db.messages[1].signals[8]
        self.assertEqual(abs_info_mux.name, 'ABS_InfoMux')
        self.assertEqual(abs_info_mux.start, 16)
        self.assertEqual(abs_info_mux.length, 2)
        self.assertEqual(abs_info_mux.receivers, [])
        self.assertEqual(abs_info_mux.byte_order, 'little_endian')
        self.assertEqual(abs_info_mux.is_signed, False)
        self.assertEqual(abs_info_mux.is_float, False)
        self.assertEqual(abs_info_mux.is_multiplexer, True)
        self.assertEqual(abs_info_mux.multiplexer_ids, None)
        self.assertEqual(abs_info_mux.multiplexer_signal, None)

        outside_temp = db.messages[1].signals[9]

        self.assertEqual(outside_temp.name, 'OutsideTemp')
        self.assertEqual(outside_temp.start, 18)
        self.assertEqual(outside_temp.length, 12)
        self.assertEqual(outside_temp.receivers, ['BodyComputer'])
        self.assertEqual(outside_temp.byte_order, 'little_endian')
        self.assertEqual(outside_temp.is_signed, False)
        self.assertEqual(outside_temp.is_float, False)
        self.assertEqual(outside_temp.scale, 0.05)
        self.assertEqual(outside_temp.offset, -40)
        self.assertEqual(outside_temp.minimum, 0)
        self.assertEqual(outside_temp.maximum, 100)
        self.assertEqual(outside_temp.decimal.scale, Decimal('0.05'))
        self.assertEqual(outside_temp.decimal.offset, -40)
        self.assertEqual(outside_temp.decimal.minimum, 0)
        self.assertEqual(outside_temp.decimal.maximum, 100)
        self.assertEqual(outside_temp.unit, 'Cel')
        self.assertEqual(outside_temp.choices, {0: 'init'})
        self.assertEqual(outside_temp.comment, 'Outside temperature.')

        speed_km = db.messages[1].signals[10]

        self.assertEqual(speed_km.name, 'SpeedKm')
        self.assertEqual(speed_km.start, 30)
        self.assertEqual(speed_km.length, 24)
        self.assertEqual(speed_km.receivers, ['BodyComputer'])
        self.assertEqual(speed_km.byte_order, 'little_endian')
        self.assertEqual(speed_km.is_signed, False)
        self.assertEqual(speed_km.is_float, False)
        self.assertEqual(speed_km.scale, 0.2)
        self.assertEqual(speed_km.offset, 0)
        self.assertEqual(speed_km.minimum, None)
        self.assertEqual(speed_km.maximum, None)
        self.assertEqual(speed_km.decimal.scale, Decimal('0.2'))
        self.assertEqual(speed_km.decimal.offset, 0)
        self.assertEqual(speed_km.decimal.minimum, None)
        self.assertEqual(speed_km.decimal.maximum, None)
        self.assertEqual(speed_km.unit, 'km/h')
        self.assertEqual(speed_km.choices, {16777215: 'invalid'})
        self.assertEqual(speed_km.comment,
                         'Middle speed of front wheels in kilometers per hour.')

        self.assertEqual(db.messages[1].signal_tree,
                         [
                             {
                                 'ABS_InfoMux': {
                                     0: ['Info0', 'Info1'],
                                     1: ['Info2', 'Info3'],
                                     2: ['Info4', 'Info5'],
                                     3: ['Info6', 'Info7']
                                 }
                             },
                             'OutsideTemp',
                             'SpeedKm',
                             'Handbrake'
        ])

        ambient_lux = db.messages[24].signals[0]

        self.assertEqual(ambient_lux.name, 'AmbientLux')
        self.assertEqual(ambient_lux.start, 0)
        self.assertEqual(ambient_lux.length, 64)
        self.assertEqual(ambient_lux.receivers, [])
        self.assertEqual(ambient_lux.byte_order, 'little_endian')
        self.assertEqual(ambient_lux.is_signed, False)
        self.assertEqual(ambient_lux.is_float, True)
        self.assertEqual(ambient_lux.scale, 1)
        self.assertEqual(ambient_lux.offset, 0)
        self.assertEqual(ambient_lux.minimum, None)
        self.assertEqual(ambient_lux.maximum, None)
        self.assertEqual(ambient_lux.decimal.scale, 1)
        self.assertEqual(ambient_lux.decimal.offset, 0)
        self.assertEqual(ambient_lux.decimal.minimum, None)
        self.assertEqual(ambient_lux.decimal.maximum, None)
        self.assertEqual(ambient_lux.unit, 'Lux')
        self.assertEqual(ambient_lux.choices, None)
        self.assertEqual(ambient_lux.comment, None)

        windshield_humidity = db.messages[25].signals[0]

        self.assertEqual(windshield_humidity.name, 'Windshield')
        self.assertEqual(windshield_humidity.start, 0)
        self.assertEqual(windshield_humidity.length, 32)
        self.assertEqual(windshield_humidity.receivers, [])
        self.assertEqual(windshield_humidity.byte_order, 'little_endian')
        self.assertEqual(windshield_humidity.is_signed, False)
        self.assertEqual(windshield_humidity.is_float, True)
        self.assertEqual(windshield_humidity.scale, 1)
        self.assertEqual(windshield_humidity.offset, 0)
        self.assertEqual(windshield_humidity.minimum, None)
        self.assertEqual(windshield_humidity.maximum, None)
        self.assertEqual(windshield_humidity.decimal.scale, 1)
        self.assertEqual(windshield_humidity.decimal.offset, 0)
        self.assertEqual(windshield_humidity.decimal.minimum, None)
        self.assertEqual(windshield_humidity.decimal.maximum, None)
        self.assertEqual(windshield_humidity.unit, '% RH')
        self.assertEqual(windshield_humidity.choices, None)
        self.assertEqual(windshield_humidity.comment, None)

        wheel_angle = db.messages[4].signals[1]

        self.assertEqual(wheel_angle.name, 'WheelAngle')
        self.assertEqual(wheel_angle.start, 1)
        self.assertEqual(wheel_angle.length, 14)
        self.assertEqual(wheel_angle.receivers, [])
        self.assertEqual(wheel_angle.byte_order, 'little_endian')
        self.assertEqual(wheel_angle.is_signed, False)
        self.assertEqual(wheel_angle.is_float, False)
        self.assertEqual(wheel_angle.scale, 0.1)
        self.assertEqual(wheel_angle.offset, -800)
        self.assertEqual(wheel_angle.minimum, None)
        self.assertEqual(wheel_angle.maximum, None)
        self.assertEqual(wheel_angle.decimal.scale, Decimal('0.1'))
        self.assertEqual(wheel_angle.decimal.offset, -800)
        self.assertEqual(wheel_angle.decimal.minimum, None)
        self.assertEqual(wheel_angle.decimal.maximum, None)
        self.assertEqual(wheel_angle.unit, 'deg')
        self.assertEqual(wheel_angle.choices,
                         {
                             0: 'left',
                             8000: 'straight',
                             16000: 'right',
                             16382: 'init',
                             16383: 'sensor '
                         })
        self.assertEqual(wheel_angle.comment, None)

        big_endian_a = db.get_message_by_name('BigEndian').signals[0]

        self.assertEqual(big_endian_a.name, 'A')
        self.assertEqual(big_endian_a.start, 7)
        self.assertEqual(big_endian_a.length, 17)
        self.assertEqual(big_endian_a.receivers, [])
        self.assertEqual(big_endian_a.byte_order, 'big_endian')
        self.assertEqual(big_endian_a.is_signed, False)
        self.assertEqual(big_endian_a.is_float, False)
        self.assertEqual(big_endian_a.scale, 1)
        self.assertEqual(big_endian_a.offset, 0)
        self.assertEqual(big_endian_a.minimum, None)
        self.assertEqual(big_endian_a.maximum, None)
        self.assertEqual(big_endian_a.decimal.scale, 1)
        self.assertEqual(big_endian_a.decimal.offset, 0)
        self.assertEqual(big_endian_a.decimal.minimum, None)
        self.assertEqual(big_endian_a.decimal.maximum, None)
        self.assertEqual(big_endian_a.unit, None)
        self.assertEqual(big_endian_a.choices, None)
        self.assertEqual(big_endian_a.comment, None)

        message = db.get_message_by_name('LittleEndianAuto')
        self.assertEqual(message.length, 1)

        message = db.get_message_by_name('BigEndianAuto')
        self.assertEqual(message.length, 1)

        message = db.get_message_by_name('LittleBigEndianAuto')
        self.assertEqual(message.length, 1)

    def test_the_homer_encode_length(self):
        db = cantools.db.Database()
        db.add_kcd_file('tests/files/kcd/the_homer.kcd')

        frame_id = 0x400
        decoded_message = {
            'MIL': 0,
            'Enginespeed': 127,
            'NoxSensor': 127,
        }
        encoded_message = b'\xfe\x00\xfe\x00\x00'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(len(encoded), 5)
        self.assertEqual(encoded, encoded_message)

    def test_the_homer_float(self):
        db = cantools.db.Database()
        db.add_kcd_file('tests/files/kcd/the_homer.kcd')

        # Message 1 (binary64).
        frame_id = 0x732

        decoded_message = {'AmbientLux': math.pi}
        encoded_message = b'\x18\x2d\x44\x54\xfb\x21\x09\x40'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(len(encoded), 8)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # Message 2 (binary32).
        frame_id = 0x745

        decoded_message = {'Windshield': 3.1415927410125732}
        encoded_message = b'\xdb\x0f\x49\x40'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(len(encoded), 4)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_the_homer_encode_decode_choices(self):
        db = cantools.db.load_file('tests/files/kcd/the_homer.kcd')

        messages = [
            ({'EngagedGear': 'disengaged'}, b'\x00\x00'),
            ({'EngagedGear': '1'}, b'\x00\x10'),
            ({'EngagedGear': '2'}, b'\x00\x20'),
            ({'EngagedGear': '3'}, b'\x00\x30'),
            ({'EngagedGear': '4'}, b'\x00\x40'),
            ({'EngagedGear': '5'}, b'\x00\x50'),
            ({'EngagedGear': '6'}, b'\x00\x60'),
            ({'EngagedGear': 7}, b'\x00\x70'),
            ({'EngagedGear': 8}, b'\x00\x80'),
            ({'EngagedGear': 9}, b'\x00\x90'),
            ({'EngagedGear': 'reverse'}, b'\x00\xa0'),
            ({'EngagedGear': 'Unspecific error'}, b'\x00\xf0')
        ]

        for decoded_message, encoded_message in messages:
            encoded = db.encode_message('Gear', decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message('Gear', encoded)
            self.assertEqual(decoded, decoded_message)

    def test_the_homer_encode_decode_choice_scaling(self):
        """Verify a label/enum matches the raw value, and is not affected by
        scaling.

        """

        db = cantools.database.load_file('tests/files/kcd/the_homer.kcd')

        messages = [
            (0x700, {'EnumTest': 'one'}, b'\x80\x00\x00\x00\x00\x00\x00\x00'),
            (0x700, {'EnumTest': 'two'}, b'\xff\x00\x00\x00\x00\x00\x00\x00'),
            (0x701, {'EnumTestFloat': 'one'}, b'\x00\x00\x00\x43\x00\x00\x00\x00'),
            (0x701, {'EnumTestFloat': 'two'}, b'\x00\x00\x7f\x43\x00\x00\x00\x00'),

            # Verify encode/decode using int/float to verify scaling
            # still works.
            (0x700, {'EnumTest': 4}, b'\x02\x00\x00\x00\x00\x00\x00\x00'),
            (0x701, {'EnumTestFloat': 4}, b'\x00\x00\x00\x40\x00\x00\x00\x00'),
            (0x700, {'EnumTest': 4.0}, b'\x02\x00\x00\x00\x00\x00\x00\x00'),
            (0x701, {'EnumTestFloat': 4.0}, b'\x00\x00\x00\x40\x00\x00\x00\x00')
        ]

        for message_id, decoded_message, encoded_message in messages:
            encoded = db.encode_message(message_id, decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message(message_id, encoded)
            self.assertEqual(decoded, decoded_message)

    def test_the_homer_encode_decode_big_endian(self):
        db = cantools.db.load_file('tests/files/kcd/the_homer.kcd')

        decoded_message = {
            'A': 0x140fa,
            'B': 1,
            'C': 18
        }
        encoded_message = b'\xa0\x7d\x64'

        encoded = db.encode_message('BigEndian', decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message('BigEndian', encoded)
        self.assertEqual(decoded, decoded_message)

    def test_the_homer_encode_decode_signed(self):
        db = cantools.db.load_file('tests/files/kcd/the_homer.kcd')

        datas = [
            (
                {
                    'TankLevel': 0,
                    'TankTemperature': -32768,
                    'FillingStatus': 0
                },
                b'\x00\x00\x00\x80\x00'
            ),
            (
                {
                    'TankLevel': 65535,
                    'TankTemperature': 32767,
                    'FillingStatus': 15
                },
                b'\xff\xff\xff\x7f\x0f'
            )
        ]

        for decoded_message, encoded_message in datas:
            encoded = db.encode_message('TankController', decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message('TankController', encoded)
            self.assertEqual(decoded, decoded_message)

    def test_empty_kcd(self):
        db = cantools.db.load_file('tests/files/kcd/empty.kcd')

        self.assertEqual(db.version, None)
        self.assertEqual(db.nodes, [])

    def test_invalid_kcd(self):
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_string('<WrongRootElement/>',
                                    database_format='kcd')

        self.assertEqual(
            str(cm.exception),
            'KCD: "Expected root element tag '
            '{http://kayak.2codeornot2code.org/1.0}NetworkDefinition, but '
            'got WrongRootElement."')

    def test_jopp_5_0_sym(self):
        db = cantools.db.Database()

        with self.assertRaises(cantools.db.ParseError) as cm:
            db.add_sym_file('tests/files/sym/jopp-5.0.sym')

        self.assertEqual(str(cm.exception), 'Only SYM version 6.0 is supported.')

    def internal_test_jopp_6_0_sym(self, test_sym_string):
        db = cantools.db.Database()
        db.add_sym_file('tests/files/sym/jopp-6.0.sym')
        if test_sym_string:
            db = cantools.db.load_string(db.as_sym_string())

        self.assertEqual(len(db.messages), 7)
        self.assertEqual(len(db.messages[0].signals), 0)

        # Message1.
        message_1 = db.messages[3]
        self.assertEqual(message_1.frame_id, 0)
        self.assertEqual(message_1.is_extended_frame, False)
        self.assertEqual(message_1.is_fd, False)
        self.assertEqual(message_1.name, 'Message1')
        self.assertEqual(message_1.length, 8)
        self.assertSequenceEqual(message_1.senders, ['ECU', 'Peripherals'])
        self.assertEqual(message_1.send_type, None)
        self.assertEqual(message_1.cycle_time, 30)
        self.assertEqual(len(message_1.signals), 2)
        self.assertEqual(message_1.comment, 'apa')
        self.assertEqual(message_1.bus_name, None)

        signal_1 = message_1.signals[0]
        self.assertEqual(signal_1.name, 'Signal1')
        self.assertEqual(signal_1.start, 0)
        self.assertEqual(signal_1.length, 11)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, 255)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, 255)
        self.assertEqual(signal_1.unit, 'A')
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)
        self.assertEqual(signal_1.spn, 1234)

        signal_2 = message_1.signals[1]
        self.assertEqual(signal_2.name, 'Signal2')
        self.assertEqual(signal_2.start, 32)
        self.assertEqual(signal_2.length, 32)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, True)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 48)
        self.assertEqual(signal_2.minimum, 16)
        self.assertEqual(signal_2.maximum, 130)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 48)
        self.assertEqual(signal_2.decimal.minimum, 16)
        self.assertEqual(signal_2.decimal.maximum, 130)
        self.assertEqual(signal_2.unit, 'V')
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comment, 'bbb')
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)
        self.assertEqual(signal_2.spn, None)

        # Message2.
        message_2 = db.messages[1]
        self.assertEqual(message_2.frame_id, 0x22)
        self.assertEqual(message_2.is_extended_frame, True)
        self.assertEqual(message_2.is_fd, False)
        self.assertEqual(message_2.name, 'Message2')
        self.assertEqual(message_2.length, 8)
        self.assertSequenceEqual(message_2.senders, ['Peripherals'])
        self.assertEqual(message_2.send_type, None)
        self.assertEqual(message_2.cycle_time, None)
        self.assertEqual(len(message_2.signals), 1)
        self.assertEqual(message_2.comment, None)
        self.assertEqual(message_2.bus_name, None)
        self.assertEqual(message_2.is_multiplexed(), False)

        signal_3 = message_2.signals[0]
        self.assertEqual(signal_3.name, 'Signal3')
        self.assertEqual(signal_3.start, 5)
        self.assertEqual(signal_3.length, 11)
        self.assertEqual(signal_3.receivers, [])
        self.assertEqual(signal_3.byte_order, 'big_endian')
        self.assertEqual(signal_3.is_signed, True)
        self.assertEqual(signal_3.is_float, False)
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 0)
        self.assertEqual(signal_3.minimum, 0)
        self.assertEqual(signal_3.maximum, 1)
        self.assertEqual(signal_3.decimal.scale, 1)
        self.assertEqual(signal_3.decimal.offset, 0)
        self.assertEqual(signal_3.decimal.minimum, 0)
        self.assertEqual(signal_3.decimal.maximum, 1)
        self.assertEqual(signal_3.unit, None)
        self.assertEqual(signal_3.choices, {0: 'foo', 1: 'bar'})
        self.assertEqual(signal_3.comment, None)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)
        self.assertEqual(signal_3.spn, None)

        # Symbol2.
        signal_4 = db.messages[5].signals[0]
        self.assertEqual(signal_4.name, 'Signal4')
        self.assertEqual(signal_4.start, 0)
        self.assertEqual(signal_4.length, 64)
        self.assertEqual(signal_4.receivers, [])
        self.assertEqual(signal_4.byte_order, 'little_endian')
        self.assertEqual(signal_4.is_signed, False)
        self.assertEqual(signal_4.is_float, True)
        self.assertEqual(signal_4.scale, 6)
        self.assertEqual(signal_4.offset, 5)
        self.assertEqual(signal_4.minimum, -1.7e+308)
        self.assertEqual(signal_4.maximum, 1.7e+308)
        self.assertEqual(signal_4.decimal.scale, 6)
        self.assertEqual(signal_4.decimal.offset, 5)
        self.assertEqual(signal_4.decimal.minimum, Decimal('-1.7e+308'))
        self.assertEqual(signal_4.decimal.maximum, Decimal('1.7e+308'))
        self.assertEqual(signal_4.unit, '*UU')
        self.assertEqual(signal_4.choices, None)
        self.assertEqual(signal_4.comment, None)
        self.assertEqual(signal_4.is_multiplexer, False)
        self.assertEqual(signal_4.multiplexer_ids, None)
        self.assertEqual(signal_4.spn, None)

        # Symbol3.
        symbol_3 = db.messages[6]
        self.assertEqual(symbol_3.frame_id, 0x33)
        self.assertEqual(symbol_3.length, 8)
        self.assertEqual(symbol_3.is_multiplexed(), True)
        self.assertEqual(len(symbol_3.signals), 4)
        self.assertSequenceEqual(symbol_3.senders, ['ECU', 'Peripherals'])
        multiplexer = symbol_3.signals[0]
        if test_sym_string:
            self.assertEqual(multiplexer.name, '0')
        else:
            self.assertEqual(multiplexer.name, 'Multiplexer1')
        self.assertEqual(multiplexer.start, 0)
        self.assertEqual(multiplexer.length, 3)
        self.assertEqual(multiplexer.is_multiplexer, True)
        self.assertEqual(multiplexer.multiplexer_ids, None)
        signal_1 = symbol_3.signals[1]
        self.assertEqual(signal_1.name, 'Signal1')
        self.assertEqual(signal_1.start, 3)
        self.assertEqual(signal_1.length, 11)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, [0])
        signal_2 = symbol_3.signals[2]
        self.assertEqual(signal_2.name, 'Signal2')
        self.assertEqual(signal_2.start, 6)
        self.assertEqual(signal_2.length, 32)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, [1])
        signal_3 = symbol_3.signals[3]
        self.assertEqual(signal_3.name, 'Signal3')
        self.assertEqual(signal_3.start, 14)
        self.assertEqual(signal_3.length, 11)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, [2])
        
        # Message3.
        message_3 = db.messages[4]
        self.assertEqual(message_3.frame_id, 0xA)
        self.assertEqual(message_3.length, 8)
        signal_3 = message_3.signals[0]
        self.assertEqual(signal_3.name, 'Signal3')
        self.assertEqual(signal_3.start, 7)
        self.assertEqual(signal_3.length, 11)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        # Encode and decode.
        frame_id = 0x009
        encoded = db.encode_message(frame_id, {})
        self.assertEqual(len(encoded), 8)
        self.assertEqual(encoded, 8 * b'\x00')
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, {})

        frame_id = 0x022
        encoded = db.encode_message(frame_id, {'Signal3': 'bar'})
        self.assertEqual(len(encoded), 8)
        self.assertEqual(encoded, b'\x00\x08\x00\x00\x00\x00\x00\x00')
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded['Signal3'], 'bar')

    def test_jopp_6_0_sym(self):
        self.internal_test_jopp_6_0_sym(False)

    def test_jopp_6_0_sym_re_read(self):
        self.internal_test_jopp_6_0_sym(True)

    def test_empty_6_0_sym(self):
        db = cantools.database.load_file('tests/files/sym/empty-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 0)

    def test_send_6_0_sym(self):
        db = cantools.database.load_file('tests/files/sym/send-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(db.messages[0].name, 'Symbol1')

    def test_receive_6_0_sym(self):
        db = cantools.database.load_file('tests/files/sym/receive-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(db.messages[0].name, 'Symbol1')

    def test_sendreceive_6_0_sym(self):
        db = cantools.database.load_file('tests/files/sym/sendreceive-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(db.messages[0].name, 'Symbol1')

    def test_signal_types_6_0_sym(self):
        db = cantools.database.load_file('tests/files/sym/signal-types-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(len(db.messages[0].signals), 8)

        signal_0 = db.messages[0].signals[0]
        self.assertEqual(signal_0.name, 'Bit')
        self.assertEqual(signal_0.start, 0)
        self.assertEqual(signal_0.length, 1)
        self.assertEqual(signal_0.receivers, [])
        self.assertEqual(signal_0.byte_order, 'little_endian')
        self.assertEqual(signal_0.is_signed, False)
        self.assertEqual(signal_0.is_float, False)
        self.assertEqual(signal_0.scale, 1)
        self.assertEqual(signal_0.offset, 0)
        self.assertEqual(signal_0.minimum, 0)
        self.assertEqual(signal_0.maximum, 1)
        self.assertEqual(signal_0.decimal.scale, 1)
        self.assertEqual(signal_0.decimal.offset, 0)
        self.assertEqual(signal_0.decimal.minimum, 0)
        self.assertEqual(signal_0.decimal.maximum, 1)
        self.assertEqual(signal_0.unit, None)
        self.assertEqual(signal_0.choices, None)
        self.assertEqual(signal_0.comment, None)
        self.assertEqual(signal_0.is_multiplexer, False)
        self.assertEqual(signal_0.multiplexer_ids, None)

        signal_1 = db.messages[0].signals[1]
        self.assertEqual(signal_1.name, 'Char')
        self.assertEqual(signal_1.start, 1)
        self.assertEqual(signal_1.length, 8)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = db.messages[0].signals[2]
        self.assertEqual(signal_2.name, 'Enum')
        self.assertEqual(signal_2.start, 9)
        self.assertEqual(signal_2.length, 4)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, {})
        self.assertEqual(signal_2.comment, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = db.messages[0].signals[3]
        self.assertEqual(signal_3.name, 'Signed')
        self.assertEqual(signal_3.start, 13)
        self.assertEqual(signal_3.length, 3)
        self.assertEqual(signal_3.receivers, [])
        self.assertEqual(signal_3.byte_order, 'little_endian')
        self.assertEqual(signal_3.is_signed, True)
        self.assertEqual(signal_3.is_float, False)
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 0)
        self.assertEqual(signal_3.minimum, None)
        self.assertEqual(signal_3.maximum, None)
        self.assertEqual(signal_3.decimal.scale, 1)
        self.assertEqual(signal_3.decimal.offset, 0)
        self.assertEqual(signal_3.decimal.minimum, None)
        self.assertEqual(signal_3.decimal.maximum, None)
        self.assertEqual(signal_3.unit, None)
        self.assertEqual(signal_3.choices, None)
        self.assertEqual(signal_3.comment, None)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        signal_4 = db.messages[0].signals[4]
        self.assertEqual(signal_4.name, 'String')
        self.assertEqual(signal_4.start, 16)
        self.assertEqual(signal_4.length, 16)
        self.assertEqual(signal_4.receivers, [])
        self.assertEqual(signal_4.byte_order, 'little_endian')
        self.assertEqual(signal_4.is_signed, False)
        self.assertEqual(signal_4.is_float, False)
        self.assertEqual(signal_4.scale, 1)
        self.assertEqual(signal_4.offset, 0)
        self.assertEqual(signal_4.minimum, None)
        self.assertEqual(signal_4.maximum, None)
        self.assertEqual(signal_4.decimal.scale, 1)
        self.assertEqual(signal_4.decimal.offset, 0)
        self.assertEqual(signal_4.decimal.minimum, None)
        self.assertEqual(signal_4.decimal.maximum, None)
        self.assertEqual(signal_4.unit, None)
        self.assertEqual(signal_4.choices, None)
        self.assertEqual(signal_4.comment, None)
        self.assertEqual(signal_4.is_multiplexer, False)
        self.assertEqual(signal_4.multiplexer_ids, None)

        signal_5 = db.messages[0].signals[5]
        self.assertEqual(signal_5.name, 'Raw')
        self.assertEqual(signal_5.start, 32)
        self.assertEqual(signal_5.length, 16)
        self.assertEqual(signal_5.receivers, [])
        self.assertEqual(signal_5.byte_order, 'little_endian')
        self.assertEqual(signal_5.is_signed, False)
        self.assertEqual(signal_5.is_float, False)
        self.assertEqual(signal_5.scale, 1)
        self.assertEqual(signal_5.offset, 0)
        self.assertEqual(signal_5.minimum, None)
        self.assertEqual(signal_5.maximum, None)
        self.assertEqual(signal_5.decimal.scale, 1)
        self.assertEqual(signal_5.decimal.offset, 0)
        self.assertEqual(signal_5.decimal.minimum, None)
        self.assertEqual(signal_5.decimal.maximum, None)
        self.assertEqual(signal_5.unit, None)
        self.assertEqual(signal_5.choices, None)
        self.assertEqual(signal_5.comment, None)
        self.assertEqual(signal_5.is_multiplexer, False)
        self.assertEqual(signal_5.multiplexer_ids, None)

        signal_6 = db.messages[0].signals[6]
        self.assertEqual(signal_6.name, 'Unsigned')
        self.assertEqual(signal_6.start, 48)
        self.assertEqual(signal_6.length, 2)
        self.assertEqual(signal_6.receivers, [])
        self.assertEqual(signal_6.byte_order, 'little_endian')
        self.assertEqual(signal_6.is_signed, False)
        self.assertEqual(signal_6.is_float, False)
        self.assertEqual(signal_6.scale, 1)
        self.assertEqual(signal_6.offset, 0)
        self.assertEqual(signal_6.minimum, None)
        self.assertEqual(signal_6.maximum, None)
        self.assertEqual(signal_6.decimal.scale, 1)
        self.assertEqual(signal_6.decimal.offset, 0)
        self.assertEqual(signal_6.decimal.minimum, None)
        self.assertEqual(signal_6.decimal.maximum, None)
        self.assertEqual(signal_6.unit, None)
        self.assertEqual(signal_6.choices, None)
        self.assertEqual(signal_6.comment, None)
        self.assertEqual(signal_6.is_multiplexer, False)
        self.assertEqual(signal_6.multiplexer_ids, None)

        signal_7 = db.messages[0].signals[7]
        self.assertEqual(signal_7.name, 'Enum2')
        self.assertEqual(signal_7.start, 50)
        self.assertEqual(signal_7.length, 3)
        self.assertEqual(signal_7.receivers, [])
        self.assertEqual(signal_7.byte_order, 'little_endian')
        self.assertEqual(signal_7.is_signed, False)
        self.assertEqual(signal_7.is_float, False)
        self.assertEqual(signal_7.scale, 1)
        self.assertEqual(signal_7.offset, 0)
        self.assertEqual(signal_7.minimum, None)
        self.assertEqual(signal_7.maximum, None)
        self.assertEqual(signal_7.decimal.scale, 1)
        self.assertEqual(signal_7.decimal.offset, 0)
        self.assertEqual(signal_7.decimal.minimum, None)
        self.assertEqual(signal_7.decimal.maximum, None)
        self.assertEqual(signal_7.unit, None)
        self.assertEqual(signal_7.choices, None)
        self.assertEqual(signal_7.comment, None)
        self.assertEqual(signal_7.is_multiplexer, False)
        self.assertEqual(signal_7.multiplexer_ids, None)

    def test_variables_color_enum_6_0_sym(self):
        db = cantools.database.load_file(
            'tests/files/sym/variables-color-enum-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 1)

        signal_0 = db.messages[0].signals[0]
        self.assertEqual(signal_0.name, 'Enum')
        self.assertEqual(signal_0.start, 5)
        self.assertEqual(signal_0.length, 8)
        self.assertEqual(signal_0.receivers, [])
        self.assertEqual(signal_0.byte_order, 'big_endian')
        self.assertEqual(signal_0.is_signed, False)
        self.assertEqual(signal_0.is_float, False)
        self.assertEqual(signal_0.scale, 1)
        self.assertEqual(signal_0.offset, 0)
        self.assertEqual(signal_0.minimum, None)
        self.assertEqual(signal_0.maximum, None)
        self.assertEqual(signal_0.decimal.scale, 1)
        self.assertEqual(signal_0.decimal.offset, 0)
        self.assertEqual(signal_0.decimal.minimum, None)
        self.assertEqual(signal_0.decimal.maximum, None)
        self.assertEqual(signal_0.unit, None)
        self.assertEqual(signal_0.choices, {0: 'Foo'})
        self.assertEqual(signal_0.comment, None)
        self.assertEqual(signal_0.is_multiplexer, False)
        self.assertEqual(signal_0.multiplexer_ids, None)

        signal_1 = db.messages[0].signals[1]
        self.assertEqual(signal_1.name, 'Variable2')
        self.assertEqual(signal_1.start, 11)
        self.assertEqual(signal_1.length, 10)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'big_endian')
        self.assertEqual(signal_1.is_signed, True)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, {0: 'Foo'})
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = db.messages[0].signals[2]
        self.assertEqual(signal_2.name, 'Variable1')
        self.assertEqual(signal_2.start, 12)
        self.assertEqual(signal_2.length, 1)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, {})
        self.assertEqual(signal_2.comment, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = db.messages[0].signals[3]
        self.assertEqual(signal_3.name, 'Color')
        self.assertEqual(signal_3.start, 17)
        self.assertEqual(signal_3.length, 32)
        self.assertEqual(signal_3.receivers, [])
        self.assertEqual(signal_3.byte_order, 'big_endian')
        self.assertEqual(signal_3.is_signed, False)
        self.assertEqual(signal_3.is_float, True)
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 2)
        self.assertEqual(signal_3.minimum, None)
        self.assertEqual(signal_3.maximum, None)
        self.assertEqual(signal_3.decimal.scale, 1)
        self.assertEqual(signal_3.decimal.offset, 2)
        self.assertEqual(signal_3.decimal.minimum, None)
        self.assertEqual(signal_3.decimal.maximum, None)
        self.assertEqual(signal_3.unit, 'A')
        self.assertEqual(signal_3.choices, None)
        self.assertEqual(signal_3.comment, None)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

    def test_empty_enum_6_0_sym(self):
        db = cantools.database.load_file('tests/files/sym/empty-enum-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 1)

        signal_0 = db.messages[0].signals[0]
        self.assertEqual(signal_0.name, 'Signal1')
        self.assertEqual(signal_0.choices, {})

    def test_special_chars_6_0_sym(self):
        db = cantools.database.load_file('tests/files/sym/special-chars-6.0.sym')

        self.assertEqual(db.version, '6.0')
        self.assertEqual(len(db.messages), 1)

        message = db.messages[0]
        self.assertEqual(message.frame_id, 1)
        self.assertEqual(message.is_extended_frame, False)
        self.assertEqual(message.is_fd, False)
        self.assertEqual(message.name, 'A/=*')
        self.assertEqual(message.length, 8)
        self.assertSequenceEqual(message.senders, ['ECU', 'Peripherals'])
        self.assertEqual(message.send_type, None)
        self.assertEqual(message.cycle_time, 5)
        self.assertEqual(len(message.signals), 6)
        self.assertEqual(message.comment, 'dd')
        self.assertEqual(message.bus_name, None)
        self.assertEqual(message.is_multiplexed(), False)

        signal_0 = message.signals[0]
        self.assertEqual(signal_0.name, 'A B')
        self.assertEqual(signal_0.start, 0)
        self.assertEqual(signal_0.length, 8)
        self.assertEqual(signal_0.receivers, [])
        self.assertEqual(signal_0.byte_order, 'little_endian')
        self.assertEqual(signal_0.is_signed, False)
        self.assertEqual(signal_0.is_float, False)
        self.assertEqual(signal_0.scale, 1)
        self.assertEqual(signal_0.offset, 0)
        self.assertEqual(signal_0.minimum, None)
        self.assertEqual(signal_0.maximum, None)
        self.assertEqual(signal_0.decimal.scale, 1)
        self.assertEqual(signal_0.decimal.offset, 0)
        self.assertEqual(signal_0.decimal.minimum, None)
        self.assertEqual(signal_0.decimal.maximum, None)
        self.assertEqual(signal_0.unit, 'A B')
        self.assertEqual(signal_0.choices, None)
        self.assertEqual(signal_0.comment, 'A B')
        self.assertEqual(signal_0.is_multiplexer, False)
        self.assertEqual(signal_0.multiplexer_ids, None)

        signal_1 = message.signals[1]
        self.assertEqual(signal_1.name, 'S/')
        self.assertEqual(signal_1.start, 15)
        self.assertEqual(signal_1.length, 8)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'big_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, '/')
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, '/')
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = message.signals[2]
        self.assertEqual(signal_2.name, 'S=')
        self.assertEqual(signal_2.start, 23)
        self.assertEqual(signal_2.length, 8)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'big_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, -55)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, -55)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, '=')
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comment, '=')
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = message.signals[3]
        self.assertEqual(signal_3.name, 'S{SEND}')
        self.assertEqual(signal_3.start, 24)
        self.assertEqual(signal_3.length, 8)
        self.assertEqual(signal_3.receivers, [])
        self.assertEqual(signal_3.byte_order, 'little_endian')
        self.assertEqual(signal_3.is_signed, False)
        self.assertEqual(signal_3.is_float, False)
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 0)
        self.assertEqual(signal_3.minimum, None)
        self.assertEqual(signal_3.maximum, None)
        self.assertEqual(signal_3.decimal.scale, 1)
        self.assertEqual(signal_3.decimal.offset, 0)
        self.assertEqual(signal_3.decimal.minimum, None)
        self.assertEqual(signal_3.decimal.maximum, None)
        self.assertEqual(signal_3.unit, '{SEND}')
        self.assertEqual(signal_3.choices, None)
        self.assertEqual(signal_3.comment, ']')
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        signal_4 = message.signals[4]
        self.assertEqual(signal_4.name, 'a/b')
        self.assertEqual(signal_4.start, 32)
        self.assertEqual(signal_4.length, 8)
        self.assertEqual(signal_4.receivers, [])
        self.assertEqual(signal_4.byte_order, 'little_endian')
        self.assertEqual(signal_4.is_signed, False)
        self.assertEqual(signal_4.is_float, False)
        self.assertEqual(signal_4.scale, 1)
        self.assertEqual(signal_4.offset, 0)
        self.assertEqual(signal_4.minimum, None)
        self.assertEqual(signal_4.maximum, None)
        self.assertEqual(signal_4.decimal.scale, 1)
        self.assertEqual(signal_4.decimal.offset, 0)
        self.assertEqual(signal_4.decimal.minimum, None)
        self.assertEqual(signal_4.decimal.maximum, None)
        self.assertEqual(signal_4.unit, '][')
        self.assertEqual(signal_4.choices, None)
        self.assertEqual(signal_4.comment, 'ö')

        self.assertEqual(signal_4.is_multiplexer, False)
        self.assertEqual(signal_4.multiplexer_ids, None)

        signal_5 = message.signals[5]
        self.assertEqual(signal_5.name, 'Variable1')
        self.assertEqual(signal_5.start, 40)
        self.assertEqual(signal_5.length, 1)
        self.assertEqual(signal_5.receivers, [])
        self.assertEqual(signal_5.byte_order, 'little_endian')
        self.assertEqual(signal_5.is_signed, False)
        self.assertEqual(signal_5.is_float, False)
        self.assertEqual(signal_5.scale, 1)
        self.assertEqual(signal_5.offset, 0)
        self.assertEqual(signal_5.minimum, 0)
        self.assertEqual(signal_5.maximum, 1)
        self.assertEqual(signal_5.decimal.scale, 1)
        self.assertEqual(signal_5.decimal.offset, 0)
        self.assertEqual(signal_5.decimal.minimum, 0)
        self.assertEqual(signal_5.decimal.maximum, 1)
        self.assertEqual(signal_5.unit, 'm/s')
        self.assertEqual(signal_5.choices, None)
        self.assertEqual(signal_5.comment, 'comment')
        self.assertEqual(signal_5.is_multiplexer, False)
        self.assertEqual(signal_5.multiplexer_ids, None)

    def test_add_bad_sym_string(self):
        db = cantools.db.Database()

        with self.assertRaises(textparser.ParseError) as cm:
            db.add_sym_string('FormatVersion=6.0\n'
                              'Foo="Jopp"')

        self.assertEqual(
            str(cm.exception),
            'Invalid syntax at line 2, column 1: ">>!<<Foo="Jopp""')

    def test_multiplexed_variables_sym(self):
        db = cantools.db.Database()
        db.add_sym_file('tests/files/sym/multiplexed_variables.sym')
        message = db.get_message_by_name('TestAlert')
        self.assertEqual(message.signal_tree,
                         [
                             {
                                 'FSM': {
                                     1: [
                                         'alert1', 'alert2', 'alert3'
                                     ],
                                     2: [
                                         'alert4', 'alert5', 'alert6'
                                     ]
                                 }
                             }
                         ])

    def test_type_parameter_overrides_is_extended_sym(self):
        db = cantools.db.Database()
        db.add_sym_file('tests/files/sym/type-extended-cycle-dash-p.sym')
        message = db.get_message_by_name('CAN-Tx Query')
        self.assertTrue(message.is_extended_frame)
        self.assertEqual(message.is_fd, False)
        encoded_msg = message.encode({'Switch Outputs': 984, 'Switch Ch 02': 1, 'Switch Ch 01': 1}).hex()
        # Ensure proper endianness
        self.assertEqual(encoded_msg[:4], '03d8')

    def test_comments_hex_and_motorola_sym(self):
        db = cantools.database.load_file('tests/files/sym/comments_hex_and_motorola.sym')
        self.assertEqual(len(db.messages), 2)

        msg1 = db.get_message_by_name('Msg1')
        self.assertEqual(msg1.frame_id, 0x620)
        self.assertEqual(msg1.length, 2)
        self.assertEqual(len(msg1.signals), 3)
        self.assertEqual(msg1.comment, None)

        sig1 = msg1.get_signal_by_name('sig1')
        self.assertEqual(sig1.start, 7)
        self.assertEqual(sig1.length, 8)
        self.assertEqual(sig1.receivers, [])
        self.assertEqual(sig1.byte_order, 'big_endian')
        self.assertEqual(sig1.is_signed, False)
        self.assertEqual(sig1.is_float, False)
        self.assertEqual(sig1.scale, 1)
        self.assertEqual(sig1.offset, 0)
        self.assertEqual(sig1.minimum, None)
        self.assertEqual(sig1.maximum, None)
        self.assertEqual(sig1.decimal.scale, 1)
        self.assertEqual(sig1.decimal.offset, 0)
        self.assertEqual(sig1.decimal.minimum, None)
        self.assertEqual(sig1.decimal.maximum, None)
        self.assertEqual(sig1.unit, None)
        self.assertEqual(sig1.choices, None)
        self.assertEqual(sig1.comment, 'a comment')
        self.assertEqual(sig1.is_multiplexer, True)
        self.assertEqual(sig1.multiplexer_ids, None)
        self.assertEqual(sig1.multiplexer_signal, None)

        sig12 = msg1.get_signal_by_name('sig12')
        self.assertEqual(sig12.start, 15)
        self.assertEqual(sig12.length, 8)
        self.assertEqual(sig12.receivers, [])
        self.assertEqual(sig12.byte_order, 'big_endian')
        self.assertEqual(sig12.is_signed, False)
        self.assertEqual(sig12.is_float, False)
        self.assertEqual(sig12.scale, 1)
        self.assertEqual(sig12.offset, 0)
        self.assertEqual(sig12.minimum, None)
        self.assertEqual(sig12.maximum, 1)
        self.assertEqual(sig12.decimal.scale, 1)
        self.assertEqual(sig12.decimal.offset, 0)
        self.assertEqual(sig12.decimal.minimum, None)
        self.assertEqual(sig12.decimal.maximum, 1)
        self.assertEqual(sig12.unit, None)
        self.assertEqual(sig12.choices, None)
        self.assertEqual(sig12.comment, 'another comment for sig1=1')
        self.assertEqual(sig12.is_multiplexer, False)
        self.assertEqual(sig12.multiplexer_ids, [1])
        self.assertEqual(sig12.multiplexer_signal, 'sig1')

        sig22 = msg1.get_signal_by_name('sig22')
        self.assertEqual(sig22.start, 15)
        self.assertEqual(sig22.length, 8)
        self.assertEqual(sig22.receivers, [])
        self.assertEqual(sig22.byte_order, 'big_endian')
        self.assertEqual(sig22.is_signed, False)
        self.assertEqual(sig22.is_float, False)
        self.assertEqual(sig22.scale, 1)
        self.assertEqual(sig22.offset, 0)
        self.assertEqual(sig22.minimum, None)
        self.assertEqual(sig22.maximum, 1)
        self.assertEqual(sig22.decimal.scale, 1)
        self.assertEqual(sig22.decimal.offset, 0)
        self.assertEqual(sig22.decimal.minimum, None)
        self.assertEqual(sig22.decimal.maximum, 1)
        self.assertEqual(sig22.unit, None)
        self.assertEqual(sig22.choices, None)
        self.assertEqual(sig22.comment, 'another comment for sig1=2')
        self.assertEqual(sig22.is_multiplexer, False)
        self.assertEqual(sig22.multiplexer_ids, [2])
        self.assertEqual(sig22.multiplexer_signal, 'sig1')

        msg2 = db.get_message_by_name('Msg2')
        self.assertEqual(msg2.frame_id, 0x555)
        self.assertEqual(msg2.length, 8)
        self.assertEqual(len(msg2.signals), 8)
        self.assertEqual(msg2.comment, 'test')

        for i in range(8):
            sig = msg2.signals[i]
            self.assertEqual(sig.name, 'Test%s' % i)
            self.assertEqual(sig.start, i*8 + 7)
            self.assertEqual(sig.length, 8)
            self.assertEqual(sig.receivers, [])
            self.assertEqual(sig.byte_order, 'big_endian')
            self.assertEqual(sig.is_signed, False)
            self.assertEqual(sig.is_float, False)
            self.assertEqual(sig.scale, 1)
            self.assertEqual(sig.offset, 0)
            self.assertEqual(sig.minimum, None)
            self.assertEqual(sig.maximum, None)
            self.assertEqual(sig.decimal.scale, 1)
            self.assertEqual(sig.decimal.offset, 0)
            self.assertEqual(sig.decimal.minimum, None)
            self.assertEqual(sig.decimal.maximum, None)
            self.assertEqual(sig.unit, None)
            if i != 7:
                self.assertEqual(sig.choices, None)
            self.assertEqual(sig.comment, None)
            self.assertEqual(sig.is_multiplexer, False)
            self.assertEqual(sig.multiplexer_ids, None)
            self.assertEqual(sig.multiplexer_signal, None)

        expected_choices = [(1, 'A'), (2, 'B'), (3, 'C'), (4, 'D')]
        for choice, expected_choice in zip(msg2.signals[7].choices.items(), expected_choices):
            self.assertEqual(choice, expected_choice)

    def test_big_endian(self):
        db = cantools.database.load_file('tests/files/sym/big-endian.sym')
        self.assertEqual(len(db.messages), 1)

        msg1 = db.get_message_by_name('Msg1')
        self.assertEqual(msg1.frame_id, 0x1)
        self.assertEqual(msg1.length, 8)
        self.assertEqual(len(msg1.signals), 6)
        self.assertEqual(msg1.comment, None)

        m1h = msg1.get_signal_by_name('m1h')
        self.assertEqual(m1h.byte_order, 'big_endian')
        self.assertEqual(m1h.start, 7)
        self.assertEqual(m1h.length, 1)
        self.assertEqual(m1h.receivers, [])
        self.assertEqual(m1h.is_signed, False)
        self.assertEqual(m1h.is_float, False)
        self.assertEqual(m1h.scale, 1)
        self.assertEqual(m1h.offset, 0)
        self.assertEqual(m1h.minimum, None)
        self.assertEqual(m1h.maximum, None)
        self.assertEqual(m1h.decimal.scale, 1)
        self.assertEqual(m1h.decimal.offset, 0)
        self.assertEqual(m1h.decimal.minimum, None)
        self.assertEqual(m1h.decimal.maximum, None)
        self.assertEqual(m1h.unit, None)
        self.assertEqual(m1h.choices, None)
        self.assertEqual(m1h.comment, None)
        self.assertEqual(m1h.is_multiplexer, False)
        self.assertEqual(m1h.multiplexer_ids, None)
        self.assertEqual(m1h.multiplexer_signal, None)

        m1d = msg1.get_signal_by_name('m1d')
        self.assertEqual(m1d.byte_order, 'big_endian')
        self.assertEqual(m1d.start, 6)
        self.assertEqual(m1d.length, 1)
        self.assertEqual(m1d.receivers, [])
        self.assertEqual(m1d.is_signed, False)
        self.assertEqual(m1d.is_float, False)
        self.assertEqual(m1d.scale, 1)
        self.assertEqual(m1d.offset, 0)
        self.assertEqual(m1d.minimum, None)
        self.assertEqual(m1d.maximum, None)
        self.assertEqual(m1d.decimal.scale, 1)
        self.assertEqual(m1d.decimal.offset, 0)
        self.assertEqual(m1d.decimal.minimum, None)
        self.assertEqual(m1d.decimal.maximum, None)
        self.assertEqual(m1d.unit, None)
        self.assertEqual(m1d.choices, None)
        self.assertEqual(m1d.comment, None)
        self.assertEqual(m1d.is_multiplexer, False)
        self.assertEqual(m1d.multiplexer_ids, None)
        self.assertEqual(m1d.multiplexer_signal, None)

        m12d = msg1.get_signal_by_name('m12d')
        self.assertEqual(m12d.byte_order, 'big_endian')
        self.assertEqual(m12d.start, 5)
        self.assertEqual(m12d.length, 12)
        self.assertEqual(m12d.receivers, [])
        self.assertEqual(m12d.is_signed, False)
        self.assertEqual(m12d.is_float, False)
        self.assertEqual(m12d.scale, 1)
        self.assertEqual(m12d.offset, 0)
        self.assertEqual(m12d.minimum, None)
        self.assertEqual(m12d.maximum, 1)
        self.assertEqual(m12d.decimal.scale, 1)
        self.assertEqual(m12d.decimal.offset, 0)
        self.assertEqual(m12d.decimal.minimum, None)
        self.assertEqual(m12d.decimal.maximum, 1)
        self.assertEqual(m12d.unit, None)
        self.assertEqual(m12d.choices, None)
        self.assertEqual(m12d.comment, None)
        self.assertEqual(m12d.is_multiplexer, False)
        self.assertEqual(m12d.multiplexer_ids, None)
        self.assertEqual(m12d.multiplexer_signal, None)

        m7d = msg1.get_signal_by_name('m7d')
        self.assertEqual(m7d.byte_order, 'big_endian')
        self.assertEqual(m7d.start, 9)
        self.assertEqual(m7d.length, 7)
        self.assertEqual(m7d.receivers, [])
        self.assertEqual(m7d.is_signed, False)
        self.assertEqual(m7d.is_float, False)
        self.assertEqual(m7d.scale, 1)
        self.assertEqual(m7d.offset, 0)
        self.assertEqual(m7d.minimum, None)
        self.assertEqual(m7d.maximum, 1)
        self.assertEqual(m7d.decimal.scale, 1)
        self.assertEqual(m7d.decimal.offset, 0)
        self.assertEqual(m7d.decimal.minimum, None)
        self.assertEqual(m7d.decimal.maximum, 1)
        self.assertEqual(m7d.unit, None)
        self.assertEqual(m7d.choices, None)
        self.assertEqual(m7d.comment, None)
        self.assertEqual(m7d.is_multiplexer, False)
        self.assertEqual(m7d.multiplexer_ids, None)
        self.assertEqual(m7d.multiplexer_signal, None)

        m42d = msg1.get_signal_by_name('m42d')
        self.assertEqual(m42d.byte_order, 'big_endian')
        self.assertEqual(m42d.start, 18)
        self.assertEqual(m42d.length, 42)
        self.assertEqual(m42d.receivers, [])
        self.assertEqual(m42d.is_signed, False)
        self.assertEqual(m42d.is_float, False)
        self.assertEqual(m42d.scale, 1)
        self.assertEqual(m42d.offset, 0)
        self.assertEqual(m42d.minimum, None)
        self.assertEqual(m42d.maximum, 1)
        self.assertEqual(m42d.decimal.scale, 1)
        self.assertEqual(m42d.decimal.offset, 0)
        self.assertEqual(m42d.decimal.minimum, None)
        self.assertEqual(m42d.decimal.maximum, 1)
        self.assertEqual(m42d.unit, None)
        self.assertEqual(m42d.choices, None)
        self.assertEqual(m42d.comment, None)
        self.assertEqual(m42d.is_multiplexer, False)
        self.assertEqual(m42d.multiplexer_ids, None)
        self.assertEqual(m42d.multiplexer_signal, None)

        i1h = msg1.get_signal_by_name('i1h')
        self.assertEqual(i1h.byte_order, 'little_endian')
        self.assertEqual(i1h.start, 56)
        self.assertEqual(i1h.length, 1)
        self.assertEqual(i1h.receivers, [])
        self.assertEqual(i1h.is_signed, False)
        self.assertEqual(i1h.is_float, False)
        self.assertEqual(i1h.scale, 1)
        self.assertEqual(i1h.offset, 0)
        self.assertEqual(i1h.minimum, None)
        self.assertEqual(i1h.maximum, None)
        self.assertEqual(i1h.decimal.scale, 1)
        self.assertEqual(i1h.decimal.offset, 0)
        self.assertEqual(i1h.decimal.minimum, None)
        self.assertEqual(i1h.decimal.maximum, None)
        self.assertEqual(i1h.unit, None)
        self.assertEqual(i1h.choices, None)
        self.assertEqual(i1h.comment, None)
        self.assertEqual(i1h.is_multiplexer, False)
        self.assertEqual(i1h.multiplexer_ids, None)
        self.assertEqual(i1h.multiplexer_signal, None)


    def test_load_bad_format(self):
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError):
            cantools.db.load(StringIO(''))

    def test_add_bad_kcd_string(self):
        db = cantools.db.Database()

        with self.assertRaises(ElementTree.ParseError) as cm:
            db.add_kcd_string('not xml')

        self.assertEqual(str(cm.exception), 'syntax error: line 1, column 0')

    def test_bus(self):
        bus = cantools.db.bus.Bus('foo')
        self.assertEqual(repr(bus), "bus('foo', None)")

        bus = cantools.db.bus.Bus('foo', 'bar')
        self.assertEqual(repr(bus), "bus('foo', 'bar')")

    def test_num(self):
        self.assertEqual(cantools.database.can.formats.utils.num('1'), 1)
        self.assertEqual(cantools.database.can.formats.utils.num('1.0'), 1.0)

        with self.assertRaises(ValueError):
            cantools.database.can.formats.utils.num('x')

    def test_timing(self):
        filename = 'tests/files/dbc/timing.dbc'
        db = cantools.db.load_file(filename)

        # Message cycle time is 200, as given by BA_.
        message = db.get_message_by_frame_id(1)
        self.assertEqual(message.cycle_time, 200)
        self.assertEqual(message.send_type, 'Cyclic')

        # Default message cycle time is None which is defined in the
        # DBC as 0 (as given by BA_DEF_DEF_)
        message = db.get_message_by_frame_id(2)
        self.assertEqual(message.cycle_time, None)
        self.assertEqual(message.send_type, 'NoMsgSendType')

        self.assert_dbc_dump(db, filename)

    def test_multiplex(self):
        db = cantools.db.load_file('tests/files/dbc/multiplex.dbc')

        message_1 = db.messages[0]
        self.assertTrue(message_1.is_multiplexed())

        self.assertEqual(message_1.signal_tree,
                         [
                             {
                                 'Multiplexor': {
                                     8: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L'
                                     ],
                                     16: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L'
                                     ],
                                     24: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L',
                                         'BIT_A', 'BIT_K', 'BIT_E', 'BIT_D',
                                         'BIT_B', 'BIT_H', 'BIT_F'
                                     ]
                                 }
                             }
                         ])

        signal_multiplexor = message_1.signals[0]
        self.assertEqual(signal_multiplexor.is_multiplexer, True)

        signal_bit_j = message_1.signals[1]
        self.assertEqual(signal_bit_j.is_multiplexer, False)
        self.assertEqual(signal_bit_j.multiplexer_ids, [8, 16, 24])

        signal_bit_a = message_1.signals[5]
        self.assertEqual(signal_bit_a.is_multiplexer, False)
        self.assertEqual(signal_bit_a.multiplexer_ids, [24])

        # Encoding and decoding.
        messages = [
            (
                {
                    'Multiplexor': 8,
                    'BIT_C': 1, 'BIT_G': 1, 'BIT_J': 1, 'BIT_L': 1
                },
                b'\x20\x00\x8c\x01\x00\x00\x00\x00'
            ),
            (
                {
                    'Multiplexor': 16,
                    'BIT_C': 1, 'BIT_G': 1, 'BIT_J': 1, 'BIT_L': 1
                },
                b'\x40\x00\x8c\x01\x00\x00\x00\x00'
            ),
            (
                {
                    'Multiplexor': 24,
                    'BIT_A': 1, 'BIT_B': 1, 'BIT_C': 1, 'BIT_D': 1,
                    'BIT_E': 1, 'BIT_F': 1, 'BIT_G': 1, 'BIT_H': 1,
                    'BIT_J': 1, 'BIT_K': 1, 'BIT_L': 1
                },
                b'\x60\x00\x8c\x35\xc3\x00\x00\x00'
            )
        ]

        for decoded_message, encoded_message in messages:
            encoded = message_1.encode(decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = message_1.decode(encoded)
            self.assertEqual(decoded, decoded_message)

        self.assert_dbc_dump(db, 'tests/files/dbc/multiplex_dumped.dbc')

    def test_multiplex_choices(self):
        db = cantools.db.load_file('tests/files/dbc/multiplex_choices.dbc',
                                   prune_choices=False)

        # With Multiplexor and BIT_L as strings.
        decoded_message = {
            'Multiplexor': 'MULTIPLEXOR_8',
            'BIT_C': 1, 'BIT_G': 1, 'BIT_J': 1, 'BIT_L': 'On'
        }
        encoded_message = b'\x20\x00\x8c\x01\x00\x00\x00\x00'

        message_1 = db.messages[0]

        encoded = message_1.encode(decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = message_1.decode(encoded)
        self.assertEqual(decoded, decoded_message)

        # With Multiplexor and BIT_L as numbers.
        decoded_message = {
            'Multiplexor': 8,
            'BIT_C': 1, 'BIT_G': 1, 'BIT_J': 1, 'BIT_L': 1
        }
        encoded_message = b'\x20\x00\x8c\x01\x00\x00\x00\x00'

        message_1 = db.messages[0]

        encoded = message_1.encode(decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = message_1.decode(encoded, decode_choices=False)
        self.assertEqual(decoded, decoded_message)

        # With Multiplexor as the only signal.
        decoded_message = {
            'Multiplexor': 4
        }
        encoded_message = b'\x10\x00\x00\x00\x00\x00\x00\x00'

        message_2 = db.messages[1]

        encoded = message_2.encode(decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = message_2.decode(encoded, decode_choices=False)
        self.assertEqual(decoded, decoded_message)

        self.assertEqual(message_2.signal_tree,
                         [
                             {
                                 'Multiplexor': {
                                     4: [],
                                     8: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L'
                                     ],
                                     16: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L'
                                     ],
                                     24: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L',
                                         'BIT_A', 'BIT_K', 'BIT_E', 'BIT_D',
                                         'BIT_B', 'BIT_H', 'BIT_F'
                                     ]
                                 }
                             }
                         ])

        self.assert_dbc_dump(db, 'tests/files/dbc/multiplex_choices_dumped.dbc')

    def test_multiplex_2(self):
        db = cantools.db.load_file('tests/files/dbc/multiplex_2.dbc')

        # Shared.
        message = db.messages[0]
        self.assertEqual(message.signal_tree,
                         [
                             {
                                 'S0': {
                                     1: ['S1'],
                                     2: ['S2'],
                                     3: ['S1', 'S2'],
                                     4: ['S2'],
                                     5: ['S2']
                                 }
                             }
                         ])

        # Normal.
        message = db.messages[1]
        self.assertEqual(message.signal_tree,
                         [
                             {
                                 'S0': {
                                     0: ['S1'],
                                     1: ['S2']
                                 }
                             }
                         ])

        # Extended.
        message = db.messages[2]
        self.assertEqual(message.signal_tree,
                         [
                             {
                                 'S0': {
                                     0: [
                                         {
                                             'S1': {
                                                 0: ['S2', 'S3'],
                                                 2: ['S4']
                                             }
                                         }
                                     ],
                                     1: ['S5']
                                 }
                             },
                             {
                                 'S6': {
                                     1: ['S7'],
                                     2: ['S8']
                                 }
                             }
                         ])

        # Extended types.
        message = db.messages[3]
        self.assertEqual(message.signal_tree,
                         [
                             {
                                 'S11': {
                                     3: [
                                         {
                                             'S0': {
                                                 0: [
                                                     'S10'
                                                 ]
                                             }
                                         }
                                     ],
                                     5: [
                                         'S9'
                                     ]
                                 }
                             }
                         ])

        self.assert_dbc_dump(db, 'tests/files/dbc/multiplex_2_dumped.dbc')

    def test_multiplex_extended(self):
        #            tree              |  bits
        # =============================+========
        # -- {root}                    |
        #    +-- S0                    |  0..4
        #    |   +-- 0                 |
        #    |   |   +-- S1            |  4..8
        #    |   |       +-- 0         |
        #    |   |       |   +-- S2    |  8..16
        #    |   |       |   +-- S3    | 16..32
        #    |   |       +-- 2         |
        #    |   |           +-- S4    |  8..32
        #    |   +-- 1                 |
        #    |       +-- S5            |  4..32
        #    +-- S6                    | 32..40
        #        +-- 1                 |
        #        |   +-- S7            | 40..64
        #        +-- 2                 |
        #            +-- S8            | 40..48
        #
        signals = [
            cantools.db.Signal(name='S0',
                               start=0,
                               length=4,
                               is_multiplexer=True),
            cantools.db.Signal(name='S1',
                               start=4,
                               length=4,
                               is_multiplexer=True,
                               multiplexer_ids=[0],
                               multiplexer_signal='S0'),
            cantools.db.Signal(name='S2',
                               start=8,
                               length=8,
                               multiplexer_ids=[0],
                               multiplexer_signal='S1'),
            cantools.db.Signal(name='S3',
                               start=16,
                               length=16,
                               multiplexer_ids=[0],
                               multiplexer_signal='S1'),
            cantools.db.Signal(name='S4',
                               start=8,
                               length=24,
                               multiplexer_ids=[2],
                               multiplexer_signal='S1'),
            cantools.db.Signal(name='S5',
                               start=4,
                               length=28,
                               multiplexer_ids=[1],
                               multiplexer_signal='S0'),
            cantools.db.Signal(name='S6',
                               start=32,
                               length=8,
                               is_multiplexer=True),
            cantools.db.Signal(name='S7',
                               start=40,
                               length=24,
                               multiplexer_ids=[1],
                               multiplexer_signal='S6'),
            cantools.db.Signal(name='S8',
                               start=40,
                               length=8,
                               multiplexer_ids=[2],
                               multiplexer_signal='S6')
        ]

        message = cantools.db.Message(frame_id=1,
                                      name='M0',
                                      length=8,
                                      signals=signals)

        self.assertEqual(message.signal_tree,
                         [
                             {
                                 'S0': {
                                     0: [
                                         {
                                             'S1': {
                                                 0: ['S2', 'S3'],
                                                 2: ['S4']
                                             }
                                         }
                                     ],
                                     1: ['S5']
                                 }
                             },
                             {
                                 'S6': {
                                     1: ['S7'],
                                     2: ['S8']
                                 }
                             }
                         ])

        # Encode and decode a few messages with different
        # multiplexing.
        messages = [
            (
                {'S0': 0, 'S1': 2, 'S4': 10000, 'S6': 1, 'S7': 33},
                b'\x20\x10\x27\x00\x01\x21\x00\x00'
            ),
            (
                {'S0': 0, 'S1': 0, 'S2': 100, 'S3': 5000, 'S6': 2, 'S8': 22},
                b'\x00\x64\x88\x13\x02\x16\x00\x00'
            ),
            (
                {'S0': 1, 'S5': 3, 'S6': 1, 'S7': 772},
                b'\x31\x00\x00\x00\x01\x04\x03\x00'
            )
        ]

        for decoded_message, encoded_message in messages:
            encoded = message.encode(decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = message.decode(encoded)
            self.assertEqual(decoded, decoded_message)

            # make sure that encoding is idempotent for multiplexed messages
            encoded2 = message.encode(decoded_message)
            self.assertEqual(encoded2, encoded)

            # strict encoding ought to fail when specifying unknown signals, non-strict encoding ought to succeed
            decoded['UndefinedMutiplexerSignal'] = 'Boo!'

            encoded2 = message.encode(decoded, strict=False)
            self.assertEqual(encoded2, encoded)

            with self.assertRaises(cantools.database.EncodeError):
                encoded2 = message.encode(decoded)

    def test_dbc_parse_error_messages(self):
        # No valid entry.
        with self.assertRaises(textparser.ParseError) as cm:

            dbc.load_string('abc')

        self.assertEqual(
            str(cm.exception),
            'Invalid syntax at line 1, column 1: ">>!<<abc"')

        # Bad message frame id.
        with self.assertRaises(textparser.ParseError) as cm:
            dbc.load_string('VERSION "1.0"\n'
                            'BO_ dssd\n')

        self.assertEqual(
            str(cm.exception),
            'Invalid syntax at line 2, column 5: "BO_ >>!<<dssd"')

        # Bad entry key.
        with self.assertRaises(textparser.ParseError) as cm:
            dbc.load_string('VERSION "1.0"\n'
                            'dd\n')

        self.assertEqual(
            str(cm.exception),
            'Invalid syntax at line 2, column 1: ">>!<<dd"')

        # Missing colon in message.
        with self.assertRaises(textparser.ParseError) as cm:
            dbc.load_string('VERSION "1.0"\n'
                            'BO_ 546 EMV_Stati 8 EMV_Statusmeldungen\n')

        self.assertEqual(
            str(cm.exception),
            'Invalid syntax at line 2, column 19: "BO_ 546 EMV_Stati '
            '>>!<<8 EMV_Statusmeldungen"')

        # Missing frame id in message comment.
        with self.assertRaises(textparser.ParseError) as cm:
            dbc.load_string('CM_ BO_ "Foo.";')

        self.assertEqual(
            str(cm.exception),
            'Invalid syntax at line 1, column 9: "CM_ BO_ >>!<<"Foo.";"')

        # Missing frame id in message comment, using load_string().
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_string('CM_ BO_ "Foo.";')

        self.assertEqual(
            str(cm.exception),
            "ARXML: \"syntax error: line 1, column 0\", DBC: \"Invalid syntax "
            "at line 1, column 9: \"CM_ BO_ >>!<<\"Foo.\";\"\", KCD: \"syntax "
            "error: line 1, column 0\", SYM: \"Only SYM version 6.0 is "
            "supported.\", CDD: \"syntax error: line 1, column 0\"")

    def test_get_node_by_name(self):
        db = cantools.db.load_file('tests/files/kcd/the_homer.kcd')

        self.assertIs(db.get_node_by_name('Motor alternative supplier'),
                      db.nodes[1])

        with self.assertRaises(KeyError) as cm:
            db.get_node_by_name('Missing')

        self.assertEqual(str(cm.exception), "'Missing'")

    def test_get_bus_by_name(self):
        db = cantools.db.load_file('tests/files/kcd/the_homer.kcd')

        self.assertIs(db.get_bus_by_name('Comfort'), db.buses[2])

        with self.assertRaises(KeyError) as cm:
            db.get_bus_by_name('Missing')

        self.assertEqual(str(cm.exception), "'Missing'")

    def test_load_file_with_database_format(self):
        filename_dbc = 'tests/files/dbc/foobar.dbc'
        filename_kcd = 'tests/files/kcd/the_homer.kcd'
        filename_sym = 'tests/files/sym/jopp-6.0.sym'

        # Matching file contents and database format.
        cantools.db.load_file(filename_dbc, database_format=None)
        cantools.db.load_file(filename_dbc, database_format='dbc')
        cantools.db.load_file(filename_kcd, database_format='kcd')
        cantools.db.load_file(filename_sym, database_format='sym')

        # KCD database format, but file is DBC.
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename_dbc, database_format='kcd')

        self.assertEqual(
            str(cm.exception),
            "KCD: \"not well-formed (invalid token): line 1, column 0\"")

        # DBC database format, but file is KCD.
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename_kcd, database_format='dbc')

        self.assertEqual(
            str(cm.exception),
            "DBC: \"Invalid syntax at line 1, column 1: \">>!<<<!--\"\"")

        # SYM database format, but file is KCD.
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename_kcd, database_format='sym')

        self.assertEqual(
            str(cm.exception),
            "SYM: \"Only SYM version 6.0 is supported.\"")

        # Unsupported database format.
        with self.assertRaises(ValueError) as cm:
            cantools.db.load_file(filename_kcd, database_format='bad')

        self.assertEqual(
            str(cm.exception),
            "expected database format 'arxml', 'dbc', 'kcd', 'sym', 'cdd' or "
            "None, but got 'bad'")

    def test_load_file_encoding(self):
        # Override default encoding.
        #
        # Does not fail to load using UTF-8 encoding because encoding
        # errors are replaced (fopen(errors='replace')).
        db = cantools.database.load_file('tests/files/dbc/cp1252.dbc',
                                         encoding='utf-8')

        replaced = 123 * b'\xef\xbf\xbd'.decode('utf-8')

        self.assertEqual(
            db.nodes[0].comment,
            r" !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTU"
            r"VWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
            + replaced)

    def test_performance_big_endian_signals(self):
        """Test encode/decode performance of a frame with big endian signals.

        """

        iterations = 10000

        signals = [
            cantools.db.Signal('S0',  7, 4,  'big_endian'),
            cantools.db.Signal('S1',  3, 4,  'big_endian'),
            cantools.db.Signal('S2', 15, 4,  'big_endian'),
            cantools.db.Signal('S3', 11, 8,  'big_endian'),
            cantools.db.Signal('S4', 19, 1,  'big_endian'),
            cantools.db.Signal('S5', 17, 17, 'big_endian'),
            cantools.db.Signal('S6', 47, 15, 'big_endian')
        ]

        message = cantools.db.Message(frame_id=1,
                                      name='M0',
                                      length=8,
                                      signals=signals)

        # Encode.
        def encode():
            message.encode({
                'S0': 3,
                'S1': 0,
                'S2': 2,
                'S3': 55,
                'S4': 1,
                'S5': 2323,
                'S6': 3224
            })

        time = timeit.timeit(encode, number=iterations)

        print()
        print("Encode time: {} s ({} s/encode)".format(time, time / iterations))

        # Decode.
        def decode():
            message.decode(b'\x30\x23\x78\x12\x26\x19\x30\x00')

        time = timeit.timeit(decode, number=iterations)

        print("Decode time: {} s ({} s/decode)".format(time, time / iterations))

    def test_padding_one(self):
        """Test to encode a message with padding as one.

        """

        signals = [
            cantools.db.Signal('S1',  3, 4,  'big_endian'),
            cantools.db.Signal('S2', 15, 4,  'big_endian'),
            cantools.db.Signal('S3', 11, 8,  'big_endian'),
            cantools.db.Signal('S4', 19, 1,  'big_endian'),
            cantools.db.Signal('S5', 17, 17, 'big_endian'),
            cantools.db.Signal('S6', 47, 15, 'big_endian')
        ]

        message = cantools.db.Message(frame_id=1,
                                      name='M0',
                                      length=8,
                                      signals=signals,
                                      unused_bit_pattern=0xff)

        decoded_message = {
            'S1': 0,
            'S2': 2,
            'S3': 55,
            'S4': 1,
            'S5': 2323,
            'S6': 3224
        }
        encoded_message = b'\xf0\x23\x7c\x12\x27\x19\x31\xff'

        encoded = message.encode(decoded_message, padding=True)
        self.assertEqual(encoded, encoded_message)
        decoded = message.decode(encoded)
        self.assertEqual(decoded, decoded_message)

    def test_multiplex_choices_padding_one(self):
        """Test to encode a multiplexed message with padding as one.

        """

        db = cantools.db.load_file('tests/files/dbc/multiplex_choices.dbc',
                                   prune_choices=True)

        decoded_message = {
            'Multiplexor': 'MULTIPLEXOR_8',
            'BIT_C': 0, 'BIT_G': 0, 'BIT_J': 0, 'BIT_L': 'Off'
        }
        encoded_message = b'\x23\xff\x73\xfe\xff\xff\xff\xff'

        message_1 = db.messages[0]

        encoded = message_1.encode(decoded_message, padding=True)
        self.assertEqual(encoded, encoded_message)
        encoded = db.encode_message(message_1.name,
                                    decoded_message,
                                    padding=True)
        self.assertEqual(encoded, encoded_message)
        decoded = message_1.decode(encoded)
        self.assertEqual(decoded, decoded_message)

    def test_multiplex_bad_multiplexer(self):
        """Test to encode and decode a multiplexer not part of the
        multiplexer.

        """

        db = cantools.db.load_file('tests/files/dbc/multiplex_choices.dbc',
                                   prune_choices=False)

        message_1 = db.messages[0]

        # Encode.
        with self.assertRaises(cantools.db.EncodeError) as cm:
            message_1.encode({'Multiplexor': 7})

        self.assertEqual(str(cm.exception),
                         'A valid value for the multiplexer selector signal '
                         '"Multiplexor" is required: Expected one of '
                         '{8, 16 or 24}, but got 7')

        # Decode.
        with self.assertRaises(cantools.db.DecodeError) as cm:
            message_1.decode(b'\x1f\xff\x73\xfe\xff\xff\xff\xff')

        self.assertEqual(str(cm.exception),
                         'expected multiplexer id 8, 16 or 24, but got 7')

        message_3 = db.messages[2]

        # Encode with single multiplexer id 8.
        with self.assertRaises(cantools.db.EncodeError) as cm:
            message_3.encode({'Multiplexor': 7})

        self.assertEqual(str(cm.exception),
                         'A valid value for the multiplexer selector '
                         'signal "Multiplexor" is required: Expected '
                         'one of {8}, but got 7')

        # Decode with single multiplexer id 8.
        with self.assertRaises(cantools.db.DecodeError) as cm:
            message_3.decode(b'\x1f\xff\x73\xfe\xff\xff\xff\xff')

        self.assertEqual(str(cm.exception),
                         'expected multiplexer id 8, but got 7')

    def test_multiplex_dump(self):
        db = cantools.db.load_file('tests/files/dbc/test_multiplex_dump.dbc')
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        dumped_msg = dumped_db.get_message_by_frame_id(0x100)

        self.assertEqual(dumped_msg.signals[0].name, "MultiplexorSig")
        self.assertEqual(dumped_msg.signals[0].is_multiplexer, True)
        self.assertEqual(dumped_msg.signals[0].multiplexer_ids, None)
        self.assertEqual(dumped_msg.signals[1].name, "MultiplexedSig")
        self.assertEqual(dumped_msg.signals[1].is_multiplexer, False)
        self.assertEqual(dumped_msg.signals[1].multiplexer_ids[0], 0x2a)
        self.assertEqual(dumped_msg.signals[2].name, "UnmultiplexedSig")
        self.assertEqual(dumped_msg.signals[2].multiplexer_ids, None)
        self.assertEqual(dumped_msg.signals[2].is_multiplexer, False)

    def test_multiplex_sym_dump(self):
        db = cantools.db.load_file('tests/files/sym/test_multiplex_dump.sym')
        dumped_db = cantools.db.load_string(db.as_sym_string())
        dumped_msg = dumped_db.get_message_by_frame_id(0x100)

        # Note: cantools database cannot support multiple multiplexer signal names, so SYM file names the multiplexer
        # signal after the multiplexer id (Hence, 2A, not MultiplexerSig)
        self.assertEqual(dumped_msg.signals[0].name, "2A")
        self.assertEqual(dumped_msg.signals[0].is_multiplexer, True)
        self.assertEqual(dumped_msg.signals[0].multiplexer_ids, None)
        self.assertEqual(dumped_msg.signals[1].name, "MultiplexedSig")
        self.assertEqual(dumped_msg.signals[1].is_multiplexer, False)
        self.assertEqual(dumped_msg.signals[1].multiplexer_ids[0], 0x2a)

    def test_string_attribute_definition_dump(self):
        db = cantools.db.load_file('tests/files/dbc/test_multiplex_dump.dbc')
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        attribute = dumped_db.dbc.attribute_definitions

        self.assertEqual(attribute['BusType'].type_name, "STRING")

    def test_extended_id_dump(self):
        db = cantools.db.load_file('tests/files/dbc/test_extended_id_dump.dbc')
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        reg_id_msg = dumped_db.get_message_by_frame_id(0x100)
        ext_id_msg = dumped_db.get_message_by_frame_id(0x1c2a2a2a)

        self.assertEqual(reg_id_msg.is_extended_frame, False)
        self.assertEqual(ext_id_msg.is_extended_frame, True)

    def test_extended_id_sym_dump(self):
        db = cantools.db.load_file('tests/files/sym/test_extended_id_dump.sym')
        dumped_db = cantools.db.load_string(db.as_sym_string())
        reg_id_msg = dumped_db.get_message_by_frame_id(0x100)
        ext_id_msg = dumped_db.get_message_by_frame_id(0x1c2a2a2a)

        self.assertEqual(reg_id_msg.is_extended_frame, False)
        self.assertEqual(ext_id_msg.is_extended_frame, True)

    def test_event_attributes(self):
        db = cantools.db.load_file('tests/files/dbc/attribute_Event.dbc')

        self.assertEqual(db.messages[0].send_type, 'Event')
        self.assertEqual(db.messages[0].frame_id, 1234)
        self.assertEqual( db.messages[0].name, 'INV2EventMsg1')

    def test_attributes(self):
        filename = 'tests/files/dbc/attributes.dbc'

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        # Signal attributes.
        attributes = db.messages[0].signals[0].dbc.attributes

        attribute = attributes['TheSignalStringAttribute']
        self.assertEqual(attribute.name, 'TheSignalStringAttribute')
        self.assertEqual(attribute.value, 'TestString')
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['TheSignalStringAttribute'])
        self.assertEqual(attribute.definition.default_value, '100')
        self.assertEqual(attribute.definition.kind, 'SG_')
        self.assertEqual(attribute.definition.type_name, 'STRING')
        self.assertEqual(attribute.definition.minimum, None)
        self.assertEqual(attribute.definition.maximum, None)
        self.assertEqual(attribute.definition.choices, None)

        attribute = attributes['GenSigSendType']
        self.assertEqual(attribute.name, 'GenSigSendType')
        self.assertEqual(attribute.value, 1)
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['GenSigSendType'])
        self.assertEqual(attribute.definition.default_value, 'Cyclic')
        self.assertEqual(attribute.definition.kind, 'SG_')
        self.assertEqual(attribute.definition.type_name, 'ENUM')
        self.assertEqual(attribute.definition.minimum, None)
        self.assertEqual(attribute.definition.maximum, None)
        self.assertEqual(attribute.definition.choices,
                         [
                             'Cyclic',
                             'OnWrite',
                             'OnWriteWithRepetition',
                             'OnChange',
                             'OnChangeWithRepetition',
                             'IfActive',
                             'IfActiveWithRepetition',
                             'NoSigSendType',
                             'NotUsed',
                             'NotUsed',
                             'NotUsed',
                             'NotUsed',
                             'NotUsed'
                         ])

        # Message attribute.
        attributes = db.messages[0].dbc.attributes
        self.assertEqual(len(attributes), 4)

        attribute = attributes['TheHexAttribute']
        self.assertEqual(attribute.name, 'TheHexAttribute')
        self.assertEqual(attribute.value, 5)
        self.assertEqual(repr(attribute), "attribute('TheHexAttribute', 5)")
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['TheHexAttribute'])
        self.assertEqual(repr(attribute.definition),
                         "attribute_definition('TheHexAttribute', 4)")
        self.assertEqual(attribute.definition.default_value, 4)
        self.assertEqual(attribute.definition.kind, 'BO_')
        self.assertEqual(attribute.definition.type_name, 'HEX')
        self.assertEqual(attribute.definition.minimum, 0)
        self.assertEqual(attribute.definition.maximum, 8)
        self.assertEqual(attribute.definition.choices, None)

        attribute = attributes['TheFloatAttribute']
        self.assertEqual(attribute.name, 'TheFloatAttribute')
        self.assertEqual(attribute.value, Decimal('58.7'))
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['TheFloatAttribute'])
        self.assertEqual(attribute.definition.default_value, 55.0)
        self.assertEqual(attribute.definition.kind, 'BO_')
        self.assertEqual(attribute.definition.type_name, 'FLOAT')
        self.assertEqual(attribute.definition.minimum, 5.0)
        self.assertEqual(attribute.definition.maximum, 87.0)
        self.assertEqual(attribute.definition.choices, None)

        # Node attributes.
        node = db.nodes[0]
        self.assertEqual(node.name, 'TheNode')
        self.assertEqual(node.comment, 'TheNodeComment')

        attribute = node.dbc.attributes['TheNodeAttribute']
        self.assertEqual(attribute.name, 'TheNodeAttribute')
        self.assertEqual(attribute.value, 99)
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['TheNodeAttribute'])
        self.assertEqual(attribute.definition.default_value, 100)
        self.assertEqual(attribute.definition.kind, 'BU_')
        self.assertEqual(attribute.definition.type_name, 'INT')
        self.assertEqual(attribute.definition.minimum, 50)
        self.assertEqual(attribute.definition.maximum, 150)
        self.assertEqual(attribute.definition.choices, None)

        # Database attributes.
        attribute = db.dbc.attributes['BusType']
        self.assertEqual(attribute.name, 'BusType')
        self.assertEqual(attribute.value, 'CAN')
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['BusType'])
        self.assertEqual(attribute.definition.default_value, '')
        self.assertEqual(attribute.definition.kind, None)
        self.assertEqual(attribute.definition.type_name, 'STRING')
        self.assertEqual(attribute.definition.minimum, None)
        self.assertEqual(attribute.definition.maximum, None)
        self.assertEqual(attribute.definition.choices, None)

        attribute = db.dbc.attributes['TheNetworkAttribute']
        self.assertEqual(attribute.name, 'TheNetworkAttribute')
        self.assertEqual(attribute.value, 51)
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['TheNetworkAttribute'])
        self.assertEqual(attribute.definition.default_value, 50)
        self.assertEqual(attribute.definition.kind, None)
        self.assertEqual(attribute.definition.type_name, 'INT')
        self.assertEqual(attribute.definition.minimum, 0)
        self.assertEqual(attribute.definition.maximum, 100)
        self.assertEqual(attribute.definition.choices, None)

        # Message send type.
        message = db.get_message_by_frame_id(0x39)
        self.assertEqual(message.cycle_time, 1000)
        self.assertEqual(message.send_type, 'Cyclic')

        self.assert_dbc_dump(db, filename)

    def test_big_numbers(self):
        filename = 'tests/files/dbc/big_numbers.dbc'

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        # Node attributes.
        node = db.nodes[0]
        attribute = node.dbc.attributes['TheNodeAttribute']
        self.assertEqual(attribute.name, 'TheNodeAttribute')
        self.assertEqual(attribute.value, 99)
        self.assertEqual(attribute.definition,
                         db.dbc.attribute_definitions['TheNodeAttribute'])
        self.assertEqual(attribute.definition.default_value, 100)
        self.assertEqual(attribute.definition.kind, 'BU_')
        self.assertEqual(attribute.definition.type_name, 'INT')
        self.assertEqual(attribute.definition.minimum, -9223372036854780000)
        self.assertEqual(attribute.definition.maximum, 18446744073709600000)
        self.assertEqual(attribute.definition.choices, None)

    def test_setters(self):
        with open('tests/files/dbc/attributes.dbc', 'r') as fin:
            db = cantools.db.load(fin)

        # Calling the setters for coverage. Assertions are not
        # necessary here since functionality is trivial.
        db.nodes[0].name = 'SetterName'
        db.nodes[0].comment = 'SetterComment'
        db.dbc.attributes['BusType'].value = 'LIN'
        db.messages[0].name = 'SetterName'
        db.messages[0].frame_id = 0x12121212
        db.messages[0].is_extended_frame = True
        db.messages[0].is_fd = True
        db.messages[0].length = 6
        db.messages[0].comment = 'TheNewComment'
        db.messages[0].bus_name = 'TheNewBusName'
        db.messages[0].signals[0].name = 'SetterName'
        db.messages[0].signals[0].start = 8
        db.messages[0].signals[0].length = 8
        db.messages[0].signals[0].byte_order = 'big_endian'
        db.messages[0].signals[0].is_signed = True
        db.messages[0].signals[0].is_float = True
        db.messages[0].signals[0].scale = 10
        db.messages[0].signals[0].offset = 1
        db.messages[0].signals[0].minimum = 0
        db.messages[0].signals[0].maximum = 100
        db.messages[0].signals[0].decimal.scale = Decimal(10)
        db.messages[0].signals[0].decimal.offset = Decimal(1)
        db.messages[0].signals[0].decimal.minimum = Decimal(0)
        db.messages[0].signals[0].decimal.maximum = Decimal(100)
        db.messages[0].signals[0].unit = 'TheNewUnit'
        db.messages[0].signals[0].is_multiplexer = True
        db.messages[0].signals[0].multiplexer_signal = db.messages[0].signals[0]
        db.messages[0].signals[0].comment = 'TheNewComment'
        db.messages[0].signals[0].spn = 500

    def test_refresh(self):
        with open('tests/files/dbc/attributes.dbc', 'r') as fin:
            db = cantools.db.load(fin)

        message = db.get_message_by_frame_id(0x39)
        self.assertEqual(message.name, 'TheMessage')
        message.frame_id = 0x40
        db.refresh()
        message = db.get_message_by_frame_id(0x40)
        self.assertEqual(message.name, 'TheMessage')
        self.assertEqual(message.frame_id, 0x40)

        message.name = 'TheNewMessage'
        db.refresh()
        message = db.get_message_by_name('TheNewMessage')
        self.assertEqual(message.name, 'TheNewMessage')
        self.assertEqual(message.frame_id, 0x40)

        with self.assertRaises(KeyError) as cm:
            db.get_message_by_name('TheMissingMessage')

        self.assertEqual(str(cm.exception), "'TheMissingMessage'")

        with self.assertRaises(KeyError) as cm:
            db.get_message_by_frame_id(0x41)

        self.assertEqual(cm.exception.args[0], 0x41)

    def test_missing_dbc_specifics(self):
        db = cantools.db.Database()

        node = cantools.db.Node('FakeNode', 'Comment')
        db.nodes.append(node)

        signals = [cantools.db.Signal('C', 0, 8)]
        message = cantools.db.Message(0x20, 'D', 8, signals)
        db.messages.append(message)

        # Test that dump executes without raising.
        db.as_dbc_string()

    def test_strict_no_multiplexer(self):
        Data = namedtuple('Data', ['start', 'length', 'byte_order'])

        # Signals not overlapping.
        datas = [
            (
                Data(start=7, length=1, byte_order='big_endian'),
                Data(start=6, length=1, byte_order='big_endian')
            ),
            (
                Data(start=7, length=8, byte_order='big_endian'),
                Data(start=15, length=8, byte_order='big_endian')
            ),
            (
                Data(start=7, length=7, byte_order='big_endian'),
                Data(start=0, length=2, byte_order='big_endian')
            ),
            (
                Data(start=2, length=6, byte_order='little_endian'),
                Data(start=0, length=2, byte_order='little_endian')
            ),
            (
                Data(start=2, length=7, byte_order='little_endian'),
                Data(start=1, length=9, byte_order='big_endian')
            )
        ]

        for data0, data1 in datas:
            signal_0 = cantools.db.Signal('S0',
                                          data0.start,
                                          data0.length,
                                          data0.byte_order)
            signal_1 = cantools.db.Signal('S1',
                                          data1.start,
                                          data1.length,
                                          data1.byte_order)
            cantools.db.Message(1,
                                'M',
                                7,
                                [signal_0, signal_1],
                                strict=True)

        # Signals overlapping.
        datas = [
            (
                Data(start=7, length=1, byte_order='big_endian'),
                Data(start=7, length=1, byte_order='big_endian')
            ),
            (
                Data(start=7, length=8, byte_order='big_endian'),
                Data(start=5, length=10, byte_order='big_endian')
            ),
            (
                Data(start=2, length=7, byte_order='little_endian'),
                Data(start=1, length=10, byte_order='big_endian')
            )
        ]

        for data0, data1 in datas:
            signal_0 = cantools.db.Signal('S0',
                                          data0.start,
                                          data0.length,
                                          data0.byte_order)
            signal_1 = cantools.db.Signal('S1',
                                          data1.start,
                                          data1.length,
                                          data1.byte_order)

            with self.assertRaises(cantools.db.Error) as cm:
                cantools.db.Message(1,
                                    'M',
                                    7,
                                    [signal_0, signal_1],
                                    strict=True)

            self.assertEqual(str(cm.exception),
                             'The signals S1 and S0 are overlapping in message M.')

        # Signal outside the message.
        datas = [
            Data(start=56, length=2, byte_order='big_endian'),
            Data(start=63, length=2, byte_order='little_endian'),
            Data(start=64, length=1, byte_order='big_endian'),
            Data(start=64, length=1, byte_order='little_endian'),
            Data(start=7, length=65, byte_order='big_endian'),
            Data(start=0, length=65, byte_order='little_endian')
        ]

        for data in datas:
            signal = cantools.db.Signal('S',
                                        data.start,
                                        data.length,
                                        data.byte_order)

            with self.assertRaises(cantools.db.Error) as cm:
                cantools.db.Message(1,
                                    'M',
                                    8,
                                    [signal],
                                    strict=True)

            self.assertEqual(str(cm.exception),
                             'The signal S does not fit in message M.')

    def test_strict_multiplexer(self):
        # Signals not overlapping.
        signals = [
            cantools.db.Signal('S0',
                               7,
                               2,
                               'big_endian',
                               is_multiplexer=True),
            cantools.db.Signal('S1',
                               5,
                               2,
                               'big_endian',
                               multiplexer_ids=[0],
                               multiplexer_signal='S0'),
            cantools.db.Signal('S2',
                               5,
                               1,
                               'big_endian',
                               multiplexer_ids=[1],
                               multiplexer_signal='S0'),
            cantools.db.Signal('S3',
                               3,
                               1,
                               'big_endian'),
            cantools.db.Signal('S4',
                               2,
                               2,
                               'big_endian',
                               is_multiplexer=True),
            cantools.db.Signal('S5',
                               0,
                               2,
                               'big_endian',
                               multiplexer_ids=[0],
                               multiplexer_signal='S4'),
            cantools.db.Signal('S6',
                               0,
                               2,
                               'big_endian',
                               is_multiplexer=True,
                               multiplexer_ids=[1],
                               multiplexer_signal='S4'),
            cantools.db.Signal('S7',
                               14,
                               1,
                               'big_endian',
                               multiplexer_ids=[0],
                               multiplexer_signal='S6')
        ]

        cantools.db.Message(1,
                            'M',
                            7,
                            signals,
                            strict=True)

        # Signals overlapping.
        datas = [
            (
                [
                    cantools.db.Signal('S0',
                                       7,
                                       2,
                                       'big_endian',
                                       is_multiplexer=True,
                                       multiplexer_ids=None,
                                       multiplexer_signal=None),
                    cantools.db.Signal('S1',
                                       5,
                                       2,
                                       'big_endian',
                                       is_multiplexer=False,
                                       multiplexer_ids=[0],
                                       multiplexer_signal='S0'),
                    cantools.db.Signal('S2',
                                       5,
                                       1,
                                       'big_endian',
                                       is_multiplexer=False,
                                       multiplexer_ids=[1],
                                       multiplexer_signal='S0'),
                    cantools.db.Signal('S3',
                                       4,
                                       1,
                                       'big_endian',
                                       is_multiplexer=False,
                                       multiplexer_ids=None,
                                       multiplexer_signal=None)
                ],
                'The signals S3 and S1 are overlapping in message M.'
            ),
            (
                [
                    cantools.db.Signal('S0',
                                       7,
                                       2,
                                       'big_endian',
                                       is_multiplexer=True),
                    cantools.db.Signal('S1',
                                       5,
                                       2,
                                       'big_endian',
                                       multiplexer_ids=[0],
                                       multiplexer_signal='S0'),
                    cantools.db.Signal('S2',
                                       5,
                                       1,
                                       'big_endian',
                                       multiplexer_ids=[1],
                                       multiplexer_signal='S0'),
                    cantools.db.Signal('S3',
                                       3,
                                       1,
                                       'big_endian'),
                    cantools.db.Signal('S4',
                                       2,
                                       2,
                                       'big_endian',
                                       is_multiplexer=True),
                    cantools.db.Signal('S5',
                                       0,
                                       2,
                                       'big_endian',
                                       multiplexer_ids=[0],
                                       multiplexer_signal='S4'),
                    cantools.db.Signal('S6',
                                       0,
                                       2,
                                       'big_endian',
                                       is_multiplexer=True,
                                       multiplexer_ids=[1],
                                       multiplexer_signal='S4'),
                    cantools.db.Signal('S7',
                                       7,
                                       1,
                                       'big_endian',
                                       multiplexer_ids=[0],
                                       multiplexer_signal='S6')
                ],
                'The signals S7 and S0 are overlapping in message M.'
            ),
            (
                [
                    cantools.db.Signal('S0',
                                       7,
                                       2,
                                       'big_endian',
                                       is_multiplexer=True),
                    cantools.db.Signal('S1',
                                       5,
                                       2,
                                       'big_endian',
                                       multiplexer_ids=[0],
                                       multiplexer_signal='S0'),
                    cantools.db.Signal('S2',
                                       5,
                                       1,
                                       'big_endian',
                                       multiplexer_ids=[1],
                                       multiplexer_signal='S0'),
                    cantools.db.Signal('S3',
                                       3,
                                       1,
                                       'big_endian'),
                    cantools.db.Signal('S4',
                                       2,
                                       2,
                                       'big_endian',
                                       is_multiplexer=True),
                    cantools.db.Signal('S5',
                                       0,
                                       2,
                                       'big_endian',
                                       multiplexer_ids=[0],
                                       multiplexer_signal='S4'),
                    cantools.db.Signal('S6',
                                       1,
                                       2,
                                       'big_endian',
                                       is_multiplexer=True,
                                       multiplexer_ids=[1],
                                       multiplexer_signal='S4'),
                    cantools.db.Signal('S7',
                                       14,
                                       1,
                                       'big_endian',
                                       multiplexer_ids=[0],
                                       multiplexer_signal='S6')
                ],
                'The signals S6 and S4 are overlapping in message M.'
            )
        ]

        for signals, expected_overlpping in datas:
            with self.assertRaises(cantools.db.Error) as cm:
                cantools.db.Message(1, 'M', 7, signals, strict=True)

            self.assertEqual(str(cm.exception), expected_overlpping)

    def test_strict_load(self):
        filenames = [
            'tests/files/kcd/bad_message_length.kcd',
            'tests/files/dbc/bad_message_length.dbc',
            'tests/files/sym/bad_message_length.sym'
        ]

        for filename in filenames:
            # Strict true.
            with self.assertRaises(cantools.database.Error) as cm:
                cantools.database.load_file(filename, strict=True)

            self.assertEqual(str(cm.exception),
                             'The signal Signal1 does not fit in message Message1.')

            # Strict false.
            db = cantools.database.load_file(filename, strict=False)

            # Sanity checks.
            message_1 = db.get_message_by_frame_id(1)
            self.assertEqual(message_1.length, 1)
            self.assertEqual(message_1.signals[0].start, 8)
            self.assertEqual(message_1.signals[0].length, 1)

    def test_database_signals_check_failure(self):
        signal = cantools.database.can.Signal('S',
                                              7,
                                              33,
                                              'big_endian')

        message = cantools.database.can.Message(37,
                                                'M',
                                                4,
                                                [signal],
                                                strict=False)

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.can.Database([message])

        self.assertEqual(str(cm.exception),
                         'The signal S does not fit in message M.')

    def test_add_two_dbc_files(self):
        """Test adding two DBC-files to the same database.

        """

        db = cantools.database.Database()

        db.add_dbc_file('tests/files/dbc/add_two_dbc_files_1.dbc')
        self.assertEqual(len(db.messages), 2)
        self.assertEqual(db.get_message_by_name('M1').frame_id, 1)
        self.assertEqual(db.get_message_by_frame_id(2).name, 'M2')

        db.add_dbc_file('tests/files/dbc/add_two_dbc_files_2.dbc')
        self.assertEqual(len(db.messages), 3)
        self.assertEqual(db.get_message_by_name('M1').frame_id, 2)
        self.assertEqual(db.get_message_by_frame_id(2).name, 'M1')

    def test_empty_ns_dbc(self):
        """Test loading a DBC-file with empty NS_.

        """

        db = cantools.database.Database()

        db.add_dbc_file('tests/files/dbc/empty_ns.dbc')
        self.assertEqual(len(db.nodes), 0)

    def test_as_kcd_string(self):
        """Test the KCD dump functionality.

        """

        if sys.version_info < (3, 8):
            return

        filename = 'tests/files/kcd/dump.kcd'
        db = cantools.database.load_file(filename)

        with open(filename, 'r') as fin:
            self.assertEqual(db.as_kcd_string(), fin.read())

    def test_issue_62(self):
        """Test issue 62.

        """

        db = cantools.database.load_file('tests/files/dbc/issue_62.dbc')
        self.assertEqual(len(db.messages), 0)

    def test_issue_63(self):
        """Test issue 63.

        """

        filename = 'tests/files/dbc/issue_63.dbc'

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            str(cm.exception),
            'The signals HtrRes and MaxRes are overlapping in message '
            'AFT1PSI2.')

    def test_j1939_dbc(self):
        db = cantools.database.load_file('tests/files/dbc/j1939.dbc')

        self.assertEqual(db.messages[0].name, 'Message1')
        self.assertEqual(db.messages[0].frame_id, 0x15340201)
        self.assertEqual(db.messages[0].protocol, 'j1939')

        signal = db.messages[0].signals[0]
        self.assertEqual(signal.spn, 500)

        signal = db.messages[1].signals[0]
        self.assertEqual(signal.spn, None)

    def test_j1939_frame_id_pack_unpack(self):
        Data = namedtuple('Data',
                          [
                              'priority',
                              'reserved',
                              'data_page',
                              'pdu_format',
                              'pdu_specific',
                              'source_address',
                              'packed'
                          ])

        datas = [
            Data(priority=0x7,
                 reserved=0x1,
                 data_page=0x1,
                 pdu_format=0xff,
                 pdu_specific=0xff,
                 source_address=0xff,
                 packed=0x1fffffff),
            Data(priority=0x5,
                 reserved=0x0,
                 data_page=0x0,
                 pdu_format=0x12,
                 pdu_specific=0x34,
                 source_address=0x56,
                 packed=0x14123456)
        ]

        for data in datas:
            packed = cantools.j1939.frame_id_pack(*data[:-1])
            self.assertEqual(packed, data.packed)
            unpacked = cantools.j1939.frame_id_unpack(packed)
            self.assertEqual(unpacked, data[:-1])

    def test_j1939_frame_id_pack_bad_data(self):
        Data = namedtuple('Data',
                          [
                              'priority',
                              'reserved',
                              'data_page',
                              'pdu_format',
                              'pdu_specific',
                              'source_address',
                              'message'
                          ])

        datas = [
            Data(priority=8,
                 reserved=0,
                 data_page=0,
                 pdu_format=0,
                 pdu_specific=0,
                 source_address=0,
                 message='Expected priority 0..7, but got 8.'),
            Data(priority=0,
                 reserved=2,
                 data_page=0,
                 pdu_format=0,
                 pdu_specific=0,
                 source_address=0,
                 message='Expected reserved 0..1, but got 2.'),
            Data(priority=0,
                 reserved=0,
                 data_page=2,
                 pdu_format=0,
                 pdu_specific=0,
                 source_address=0,
                 message='Expected data page 0..1, but got 2.'),
            Data(priority=0,
                 reserved=0,
                 data_page=0,
                 pdu_format=0x100,
                 pdu_specific=0,
                 source_address=0,
                 message='Expected PDU format 0..255, but got 256.'),
            Data(priority=0,
                 reserved=0,
                 data_page=0,
                 pdu_format=0,
                 pdu_specific=0x100,
                 source_address=0,
                 message='Expected PDU specific 0..255, but got 256.'),
            Data(priority=0,
                 reserved=0,
                 data_page=0,
                 pdu_format=0,
                 pdu_specific=0,
                 source_address=256,
                 message='Expected source address 0..255, but got 256.')
        ]

        for data in datas:
            with self.assertRaises(cantools.Error) as cm:
                cantools.j1939.frame_id_pack(*data[:-1])

            self.assertEqual(str(cm.exception), data.message)

    def test_j1939_frame_id_unpack_bad_data(self):
        Data = namedtuple('Data', ['data', 'message'])

        datas = [
            Data(data=0x100000000,
                 message=('Expected a frame id 0..0x1fffffff, but got '
                          '0x100000000.'))
        ]

        for data in datas:
            with self.assertRaises(cantools.Error) as cm:
                cantools.j1939.frame_id_unpack(data.data)

            self.assertEqual(str(cm.exception), data.message)

    def test_j1939_pgn_pack_unpack(self):
        Data = namedtuple('Data',
                          [
                              'reserved',
                              'data_page',
                              'pdu_format',
                              'pdu_specific',
                              'packed'
                          ])

        datas = [
            Data(reserved=1,
                 data_page=1,
                 pdu_format=0xff,
                 pdu_specific=0xff,
                 packed=0x3ffff),
            Data(reserved=0,
                 data_page=1,
                 pdu_format=0xef,
                 pdu_specific=0,
                 packed=0x1ef00),
            Data(reserved=0,
                 data_page=0,
                 pdu_format=0xf0,
                 pdu_specific=0x34,
                 packed=0xf034)
        ]

        for data in datas:
            packed = cantools.j1939.pgn_pack(*data[:4])
            self.assertEqual(packed, data.packed)
            unpacked = cantools.j1939.pgn_unpack(packed)
            self.assertEqual(unpacked, data[:4])

    def test_j1939_pgn_pack_bad_data(self):
        Data = namedtuple('Data',
                          [
                              'reserved',
                              'data_page',
                              'pdu_format',
                              'pdu_specific',
                              'message'
                          ])

        datas = [
            Data(reserved=2,
                 data_page=0,
                 pdu_format=0,
                 pdu_specific=0,
                 message='Expected reserved 0..1, but got 2.'),
            Data(reserved=0,
                 data_page=2,
                 pdu_format=0,
                 pdu_specific=0,
                 message='Expected data page 0..1, but got 2.'),
            Data(reserved=0,
                 data_page=0,
                 pdu_format=0x100,
                 pdu_specific=0,
                 message='Expected PDU format 0..255, but got 256.'),
            Data(reserved=0,
                 data_page=0,
                 pdu_format=0xf0,
                 pdu_specific=0x100,
                 message='Expected PDU specific 0..255, but got 256.'),
            Data(reserved=0,
                 data_page=0,
                 pdu_format=0xef,
                 pdu_specific=1,
                 message=('Expected PDU specific 0 when PDU format is 0..239, '
                          'but got 1.'))
        ]

        for data in datas:
            with self.assertRaises(cantools.Error) as cm:
                cantools.j1939.pgn_pack(*data[:4])

            self.assertEqual(str(cm.exception), data.message)

    def test_j1939_pgn_unpack_bad_data(self):
        Data = namedtuple('Data', ['data', 'message'])

        datas = [
            Data(data=0x40000,
                 message=('Expected a parameter group number 0..0x3ffff, '
                          'but got 0x40000.'))
        ]

        for data in datas:
            with self.assertRaises(cantools.Error) as cm:
                cantools.j1939.pgn_unpack(data.data)

            self.assertEqual(str(cm.exception), data.message)

    def test_float_dbc(self):
        filename = 'tests/files/dbc/floating_point.dbc'
        db = cantools.database.load_file(filename)

        # Double.
        message = db.get_message_by_frame_id(1024)
        signal = message.get_signal_by_name('Signal1')
        self.assertEqual(signal.is_float, True)
        self.assertEqual(signal.length, 64)

        decoded_message = {'Signal1': -129.448}
        encoded_message = b'\x75\x93\x18\x04\x56\x2e\x60\xc0'

        encoded = message.encode(decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = message.decode(encoded)
        self.assertEqual(decoded, decoded_message)

        # Float.
        message = db.get_message_by_frame_id(1025)
        signal = message.get_signal_by_name('Signal1')
        self.assertEqual(signal.is_float, True)
        self.assertEqual(signal.length, 32)

        decoded_message = {
            'Signal1': 129.5,
            'Signal2': 1234500.5
        }
        encoded_message = b'\x00\x80\x01\x43\x24\xb2\x96\x49'

        encoded = message.encode(decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = message.decode(encoded)
        self.assertEqual(decoded, decoded_message)

        self.assert_dbc_dump(db, filename)

    def test_signed_dbc(self):
        db = cantools.database.load_file('tests/files/dbc/signed.dbc')

        datas = [
            (
                'Message64',
                {'s64': -5},
                b'\xfb\xff\xff\xff\xff\xff\xff\xff'
            ),
            (
                'Message33',
                {'s33': -5},
                b'\xfb\xff\xff\xff\x01\x00\x00\x00'
            ),
            (
                'Message32',
                {'s32': -5},
                b'\xfb\xff\xff\xff\x00\x00\x00\x00'
            ),
            (
                'Message64big',
                {'s64big': -5},
                b'\xff\xff\xff\xff\xff\xff\xff\xfb'
            ),
            (
                'Message33big',
                {'s33big': -5},
                b'\xff\xff\xff\xfd\x80\x00\x00\x00'
            ),
            (
                'Message32big',
                {'s32big': -5},
                b'\xff\xff\xff\xfb\x00\x00\x00\x00'
            ),
            (
                'Message378910',
                {
                    's7': -40,
                    's8big': 0x5a,
                    's9': 0xa5,
                    's8': -43,
                    's3big': -4,
                    's3': 1,
                    's10big': -253,
                    's7big': -9
                },
                b'\xb0\xb4\x4a\x55\x87\x01\x81\xf7'
            )
        ]

        for name, decoded_message, encoded_message in datas:
            encoded = db.encode_message(name, decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message(name, encoded)
            self.assertEqual(decoded, decoded_message)

    def test_long_names_dbc(self):
        db = cantools.database.load_file('tests/files/dbc/long_names.dbc')

        # Nodes.
        self.assertEqual(db.nodes[0].name, 'NN123456789012345678901234567890123')
        self.assertEqual(db.nodes[1].name, 'N123456789012345678901234567890123')
        self.assertEqual(db.nodes[2].name, 'N1234567890123456789012345678901')
        self.assertEqual(db.nodes[3].name, 'N12345678901234567890123456789012')

        # Messages.
        self.assertEqual(db.messages[0].name, 'SS12345678901234567890123458789012345')
        self.assertEqual(db.messages[1].name, 'SS1234567890123456789012345778901')
        self.assertEqual(db.messages[2].name, 'SS1234567890123456789012345878901234')
        self.assertEqual(db.messages[3].name, 'SS123456789012345678901234577890')
        self.assertEqual(db.messages[4].name, 'SS12345678901234567890123456789012')
        self.assertEqual(db.messages[5].name, 'S12345678901234567890123456789012')
        self.assertEqual(db.messages[6].name, 'M123456789012345678901234567890123')
        self.assertEqual(db.messages[7].name, 'M1234567890123456789012345678901')
        self.assertEqual(db.messages[8].name, 'M12345678901234567890123456789012')
        self.assertEqual(db.messages[9].name, 'MM12345678901234567890123456789012')

        self.assertEqual(db.messages[7].senders, ['N1234567890123456789012345678901'])
        self.assertEqual(db.messages[8].senders, ['N12345678901234567890123456789012'])

        # Signals.
        self.assertEqual(db.messages[5].signals[0].name,
                         'SS12345678901234567890123456789012')
        self.assertEqual(db.messages[6].signals[0].name,
                         'SSS12345678901234567890123456789012')
        self.assertEqual(db.messages[7].signals[0].name,
                         'S1234567890123456789012345678901')
        self.assertEqual(db.messages[7].signals[1].name,
                         'S123456789012345678901234567890123')
        self.assertEqual(db.messages[7].signals[2].name,
                         'SS12345678901234567890123456789012')
        self.assertEqual(db.messages[7].signals[3].name,
                         'SS1234567890123456789012345678901233')
        self.assertEqual(db.messages[7].signals[4].name,
                         'SS12345678901234567890123456789012332')
        self.assertEqual(db.messages[8].signals[0].name,
                         'S123456789012345678901234567890123')
        self.assertEqual(db.messages[8].signals[1].name,
                         'S12345678901234567890123456789012')
        self.assertEqual(db.messages[8].signals[2].name,
                         'SS12345678901234567890123456789012')
        self.assertEqual(db.messages[8].signals[3].name,
                         'SS12345678901234567890123456789012dw')
        self.assertEqual(db.messages[9].signals[0].name,
                         'SS1234567890123456789012345678901233')
        self.assertEqual(db.messages[9].signals[1].name,
                         'SSS12345678901234567890123456789012')

        self.assertEqual(db.messages[7].signals[2].receivers,
                         ['N123456789012345678901234567890123'])

        # environment variables
        envvar_names = db.dbc.environment_variables
        self.assertTrue('E1234567890123456789012345678901' in envvar_names)
        self.assertFalse('E12345678901234567890123456_0000' in envvar_names)
        self.assertTrue('E12345678901234567890123456789012' in envvar_names)

    def test_illegal_namespace(self):
        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            db = cantools.db.load_file('tests/files/arxml/system-illegal-namespace-4.2.arxml')

        self.assertEqual(
            str(cm.exception),
            'ARXML: "Unrecognized XML namespace \'http://autosar.org/schema/argh4.0\'"')

        root = ElementTree.parse('tests/files/arxml/system-illegal-namespace-4.2.arxml').getroot()
        with self.assertRaises(ValueError) as cm:
            loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=False)

        self.assertEqual(
            str(cm.exception),
            'Unrecognized AUTOSAR XML namespace \'http://autosar.org/schema/argh4.0\'')

    def test_illegal_root(self):
        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            db = cantools.db.load_file('tests/files/arxml/system-illegal-root-4.2.arxml')

        self.assertEqual(
            str(cm.exception),
            'ARXML: "No XML namespace specified or illegal root tag name \'{http://autosar.org/schema/r4.0}AUTOSARGH\'"')

        root = ElementTree.parse('tests/files/arxml/system-illegal-root-4.2.arxml').getroot()
        with self.assertRaises(ValueError) as cm:
            loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=False)

        self.assertEqual(
            str(cm.exception),
            'No XML namespace specified or illegal root tag name \'{http://autosar.org/schema/r4.0}AUTOSARGH\'')

    def test_illegal_version(self):
        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            db = cantools.db.load_file('tests/files/arxml/system-illegal-version-4.2.2.1.0.arxml')

        self.assertEqual(
            str(cm.exception),
            'ARXML: "Could not parse AUTOSAR version \'4.2.2.1.0\'"')

    def test_arxml_version(self):
        root = ElementTree.parse('tests/files/arxml/system-4.2.arxml').getroot()
        loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=False)

        self.assertEqual(loader.autosar_version_newer(3), True)
        self.assertEqual(loader.autosar_version_newer(4), True)
        self.assertEqual(loader.autosar_version_newer(5), False)

        # for AUTOSAR 4, we always pretend to be version 4.0
        self.assertEqual(loader.autosar_version_newer(4, 0), True)
        self.assertEqual(loader.autosar_version_newer(4, 1), False)
        self.assertEqual(loader.autosar_version_newer(4, 2), False)
        self.assertEqual(loader.autosar_version_newer(4, 3), False)

    def test_DAI_namespace(self):
        db = cantools.db.load_file('tests/files/arxml/system-DAI-3.1.2.arxml')

    def test_system_3_arxml(self):
        db = cantools.db.load_file('tests/files/arxml/system-3.2.3.arxml')

        self.assertEqual(len(db.buses), 1)
        bus = db.buses[0]
        self.assertEqual(bus.name, 'Network')
        self.assertEqual(bus.comment, None)
        self.assertEqual(bus.comments, None)
        self.assertEqual(bus.baudrate, 250000)
        self.assertEqual(bus.fd_baudrate, None)

        self.assertEqual(len(db.nodes), 2)
        self.assertEqual(len(db.messages), 2)
        self.assertTrue(db.autosar is not None)
        self.assertTrue(db.dbc is None)
        self.assertEqual(db.autosar.arxml_version, "3.2.3")

        node1 = db.nodes[0]
        self.assertEqual(node1.name, 'Driver')
        self.assertEqual(node1.comments, {'DE': 'Der rätselhafte Fahrer', 'EN': 'The enigmatic driver'})

        node2 = db.nodes[1]
        self.assertEqual(node2.name, 'Passenger')
        self.assertEqual(node2.comments, {'FOR-ALL': 'A boring passenger', 'DE': 'Ein langweiliger Passagier'})

        mux_message = db.messages[0]
        self.assertEqual(mux_message.frame_id, 4)
        self.assertEqual(mux_message.is_extended_frame, False)
        self.assertEqual(mux_message.is_fd, False)
        self.assertEqual(mux_message.name, 'Multiplexed')
        self.assertEqual(mux_message.length, 2)
        self.assertEqual(mux_message.senders, [])
        self.assertEqual(mux_message.send_type, None)
        self.assertEqual(mux_message.cycle_time, None)
        self.assertEqual(len(mux_message.signals), 6)
        self.assertEqual(mux_message.comments, None)
        self.assertEqual(mux_message.bus_name, 'Network')
        self.assertTrue(mux_message.dbc is None)
        self.assertTrue(mux_message.autosar is not None)
        self.assertEqual(mux_message.autosar.pdu_paths, [
            '/Network/CanCluster/CAN/PDUs/Multiplexed',
            '/Network/CanCluster/CAN/PDUs/Multiplexed_0',
            '/Network/CanCluster/CAN/PDUs/Multiplexed_1',
            '/Network/CanCluster/CAN/PDUs/Multiplexed_Static',
        ])

        mux_signal_selector = mux_message.signals[0]
        self.assertEqual(mux_signal_selector.name, 'Multiplexed_selector1')
        self.assertEqual(mux_signal_selector.start, 7)
        self.assertEqual(mux_signal_selector.length, 2)
        self.assertEqual(mux_signal_selector.receivers, [])
        self.assertEqual(mux_signal_selector.byte_order, 'big_endian')
        self.assertEqual(mux_signal_selector.initial, 0)
        self.assertEqual(mux_signal_selector.is_signed, False)
        self.assertEqual(mux_signal_selector.is_float, False)
        self.assertEqual(mux_signal_selector.scale, 1.0)
        self.assertEqual(mux_signal_selector.offset, 0.0)
        self.assertEqual(mux_signal_selector.minimum, None)
        self.assertEqual(mux_signal_selector.maximum, None)
        self.assertEqual(mux_signal_selector.decimal.scale, 1)
        self.assertEqual(mux_signal_selector.decimal.offset, 0)
        self.assertEqual(mux_signal_selector.decimal.minimum, None)
        self.assertEqual(mux_signal_selector.decimal.maximum, None)
        self.assertEqual(mux_signal_selector.unit, None)
        self.assertEqual(mux_signal_selector.choices, {})
        self.assertEqual(mux_signal_selector.comments, None)
        self.assertEqual(mux_signal_selector.is_multiplexer, True)
        self.assertEqual(mux_signal_selector.multiplexer_signal , None)
        self.assertEqual(mux_signal_selector.multiplexer_ids, None)

        mux_signal_static = mux_message.signals[1]
        self.assertEqual(mux_signal_static.name, 'Static')
        self.assertEqual(mux_signal_static.start, 0)
        self.assertEqual(mux_signal_static.length, 3)
        self.assertEqual(mux_signal_static.receivers, [])
        self.assertEqual(mux_signal_static.byte_order, 'little_endian')
        self.assertEqual(mux_signal_static.initial, False)
        self.assertEqual(mux_signal_static.is_signed, False)
        self.assertEqual(mux_signal_static.is_float, False)
        self.assertEqual(mux_signal_static.scale, 1.0)
        self.assertEqual(mux_signal_static.offset, 0.0)
        self.assertEqual(mux_signal_static.minimum, None)
        self.assertEqual(mux_signal_static.maximum, None)
        self.assertEqual(mux_signal_static.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_static.decimal.offset, 0.0)
        self.assertEqual(mux_signal_static.decimal.minimum, None)
        self.assertEqual(mux_signal_static.decimal.maximum, None)
        self.assertEqual(mux_signal_static.unit, None)
        self.assertEqual(mux_signal_static.choices, None)
        self.assertEqual(mux_signal_static.comments, None)
        self.assertEqual(mux_signal_static.is_multiplexer, False)
        self.assertEqual(mux_signal_static.multiplexer_signal , None)
        self.assertEqual(mux_signal_static.multiplexer_ids, None)

        mux_signal_hello = mux_message.signals[2]
        self.assertEqual(mux_signal_hello.name, 'Hello')
        self.assertEqual(mux_signal_hello.start, 3)
        self.assertEqual(mux_signal_hello.length, 3)
        self.assertEqual(mux_signal_hello.receivers, [])
        self.assertEqual(mux_signal_hello.byte_order, 'little_endian')
        self.assertEqual(mux_signal_hello.initial, 7)
        self.assertEqual(mux_signal_hello.is_signed, False)
        self.assertEqual(mux_signal_hello.is_float, False)
        self.assertEqual(mux_signal_hello.scale, 1.0)
        self.assertEqual(mux_signal_hello.offset, 0.0)
        self.assertEqual(mux_signal_hello.minimum, None)
        self.assertEqual(mux_signal_hello.maximum, None)
        self.assertEqual(mux_signal_hello.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_hello.decimal.offset, 0.0)
        self.assertEqual(mux_signal_hello.decimal.minimum, None)
        self.assertEqual(mux_signal_hello.decimal.maximum, None)
        self.assertEqual(mux_signal_hello.unit, None)
        self.assertEqual(mux_signal_hello.choices, None)
        self.assertEqual(mux_signal_hello.comments, None)
        self.assertEqual(mux_signal_hello.is_multiplexer, False)
        self.assertEqual(mux_signal_hello.multiplexer_signal, 'Multiplexed_selector1')
        self.assertEqual(mux_signal_hello.multiplexer_ids, [0])

        mux_signal_world2 = mux_message.signals[3]
        self.assertEqual(mux_signal_world2.name, 'World2')
        self.assertEqual(mux_signal_world2.start, 3)
        self.assertEqual(mux_signal_world2.length, 1)
        self.assertEqual(mux_signal_world2.receivers, [])
        self.assertEqual(mux_signal_world2.byte_order, 'little_endian')
        self.assertEqual(mux_signal_world2.initial, None)
        self.assertEqual(mux_signal_world2.is_signed, False)
        self.assertEqual(mux_signal_world2.is_float, False)
        self.assertEqual(mux_signal_world2.scale, 1.0)
        self.assertEqual(mux_signal_world2.offset, 0.0)
        self.assertEqual(mux_signal_world2.minimum, None)
        self.assertEqual(mux_signal_world2.maximum, None)
        self.assertEqual(mux_signal_world2.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_world2.decimal.offset, 0.0)
        self.assertEqual(mux_signal_world2.decimal.minimum, None)
        self.assertEqual(mux_signal_world2.decimal.maximum, None)
        self.assertEqual(mux_signal_world2.unit, None)
        self.assertEqual(mux_signal_world2.choices, None)
        self.assertEqual(mux_signal_world2.comments, None)
        self.assertEqual(mux_signal_world2.is_multiplexer, False)
        self.assertEqual(mux_signal_world2.multiplexer_signal, 'Multiplexed_selector1')
        self.assertEqual(mux_signal_world2.multiplexer_ids, [1])

        mux_signal_world1 = mux_message.signals[4]
        self.assertEqual(mux_signal_world1.name, 'World1')
        self.assertEqual(mux_signal_world1.start, 4)
        self.assertEqual(mux_signal_world1.length, 2)
        self.assertEqual(mux_signal_world1.receivers, [])
        self.assertEqual(mux_signal_world1.byte_order, 'little_endian')
        self.assertEqual(mux_signal_world1.initial, None)
        self.assertEqual(mux_signal_world1.is_signed, False)
        self.assertEqual(mux_signal_world1.is_float, False)
        self.assertEqual(mux_signal_world1.scale, 1.0)
        self.assertEqual(mux_signal_world1.offset, 0.0)
        self.assertEqual(mux_signal_world1.minimum, None)
        self.assertEqual(mux_signal_world1.maximum, None)
        self.assertEqual(mux_signal_world1.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_world1.decimal.offset, 0.0)
        self.assertEqual(mux_signal_world1.decimal.minimum, None)
        self.assertEqual(mux_signal_world1.decimal.maximum, None)
        self.assertEqual(mux_signal_world1.unit, None)
        self.assertEqual(mux_signal_world1.choices, None)
        self.assertEqual(mux_signal_world1.comments, None)
        self.assertEqual(mux_signal_world1.is_multiplexer, False)
        self.assertEqual(mux_signal_world1.multiplexer_signal, 'Multiplexed_selector1')
        self.assertEqual(mux_signal_world1.multiplexer_ids, [1])

        message_1 = db.messages[1]
        self.assertEqual(message_1.frame_id, 123)
        self.assertEqual(message_1.is_extended_frame, False)
        self.assertEqual(message_1.is_fd, False)
        self.assertEqual(message_1.name, 'Status')
        self.assertEqual(message_1.length, 3)
        self.assertEqual(message_1.senders, [])
        self.assertEqual(message_1.send_type, None)
        self.assertEqual(message_1.cycle_time, 500)
        self.assertEqual(len(message_1.signals), 6)
        self.assertEqual(message_1.comments["EN"], 'The lonely frame description')
        self.assertEqual(message_1.bus_name, 'Network')
        self.assertTrue(message_1.dbc is None)
        self.assertTrue(message_1.autosar is not None)
        self.assertEqual(message_1.autosar.pdu_paths, [ '/Network/CanCluster/CAN/PDUs/Status' ])

        signal_1 = message_1.signals[0]
        self.assertEqual(signal_1.name, 'Checksum')
        self.assertEqual(signal_1.start, 0)
        self.assertEqual(signal_1.length, 8)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.initial, None)
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1.0)
        self.assertEqual(signal_1.offset, 0.0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, Decimal('1'))
        self.assertEqual(signal_1.decimal.offset, 0.0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comments, {'EN': 'AUTOSAR end-to-end protection CRC according to profile 2'})
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = message_1.signals[1]
        self.assertEqual(signal_2.name, 'SequenceCounter')
        self.assertEqual(signal_2.start, 12)
        self.assertEqual(signal_2.length, 4)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.initial, None)
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1.0)
        self.assertEqual(signal_2.offset, 0.0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, Decimal('1'))
        self.assertEqual(signal_2.decimal.offset, 0.0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comments, {'EN': 'AUTOSAR end-to-end protection sequence counter according to profile 2'})
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = message_1.signals[2]
        self.assertEqual(signal_3.name, 'Alive')
        self.assertEqual(signal_3.start, 16)
        self.assertEqual(signal_3.length, 1)
        self.assertEqual(signal_3.receivers, [])
        self.assertEqual(signal_3.byte_order, 'little_endian')
        self.assertEqual(signal_3.initial, False)
        self.assertEqual(signal_3.is_signed, False)
        self.assertEqual(signal_3.is_float, False)
        self.assertEqual(signal_3.scale, 1.0)
        self.assertEqual(signal_3.offset, 0.0)
        self.assertEqual(signal_3.minimum, None)
        self.assertEqual(signal_3.maximum, None)
        self.assertEqual(signal_3.decimal.scale, Decimal('1'))
        self.assertEqual(signal_3.decimal.offset, 0.0)
        self.assertEqual(signal_3.decimal.minimum, None)
        self.assertEqual(signal_3.decimal.maximum, None)
        self.assertEqual(signal_3.unit, None)
        self.assertEqual(signal_3.choices, None)
        self.assertEqual(signal_3.comments, {'DE': 'Lebt', 'EN': 'Is alive'})
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        signal_4 = message_1.signals[3]
        self.assertEqual(signal_4.name, 'Sleeps')
        self.assertEqual(signal_4.start, 17)
        self.assertEqual(signal_4.length, 1)
        self.assertEqual(signal_4.receivers, [])
        self.assertEqual(signal_4.byte_order, 'little_endian')
        self.assertEqual(signal_4.initial, True)
        self.assertEqual(signal_4.is_signed, False)
        self.assertEqual(signal_4.is_float, False)
        self.assertEqual(signal_4.scale, 1.0)
        self.assertEqual(signal_4.offset, 0.0)
        self.assertEqual(signal_4.minimum, None)
        self.assertEqual(signal_4.maximum, None)
        self.assertEqual(signal_4.decimal.scale, Decimal('1'))
        self.assertEqual(signal_4.decimal.offset, 0.0)
        self.assertEqual(signal_4.decimal.minimum, None)
        self.assertEqual(signal_4.decimal.maximum, None)
        self.assertEqual(signal_4.unit, None)
        self.assertEqual(signal_4.choices, None)
        self.assertEqual(signal_4.comments, None)
        self.assertEqual(signal_4.is_multiplexer, False)
        self.assertEqual(signal_4.multiplexer_ids, None)

        signal_5 = message_1.signals[4]
        self.assertEqual(signal_5.name, 'Position')
        self.assertEqual(signal_5.start, 19)
        self.assertEqual(signal_5.length, 2)
        self.assertEqual(signal_5.receivers, [])
        self.assertEqual(signal_5.byte_order, 'little_endian')
        self.assertEqual(signal_5.is_signed, False)
        self.assertEqual(signal_5.is_float, False)
        self.assertEqual(signal_5.scale, 1.0)
        self.assertEqual(signal_5.offset, 0.0)
        self.assertEqual(signal_5.minimum, None)
        self.assertEqual(signal_5.maximum, None)
        self.assertEqual(signal_5.decimal.scale, 1.0)
        self.assertEqual(signal_5.decimal.offset, 0.0)
        self.assertEqual(signal_5.decimal.minimum, None)
        self.assertEqual(signal_5.decimal.maximum, None)
        self.assertEqual(signal_5.unit, None)
        self.assertEqual(signal_5.choices, { 0: 'STANDARD_POSITION',
                                             1: 'FORWARD_POSITION',
                                             2: 'BACKWARD_POSITION'})
        self.assertEqual(signal_5.comments, None)
        self.assertEqual(signal_5.is_multiplexer, False)
        self.assertEqual(signal_5.multiplexer_ids, None)

        signal_6 = message_1.signals[5]
        self.assertEqual(signal_6.name, 'Movement')
        self.assertEqual(signal_6.start, 21)
        self.assertEqual(signal_6.length, 3)
        self.assertEqual(signal_6.receivers, [])
        self.assertEqual(signal_6.byte_order, 'little_endian')
        self.assertEqual(signal_6.is_signed, False)
        self.assertEqual(signal_6.is_float, False)
        self.assertEqual(signal_6.scale, 10.0/4)
        self.assertEqual(signal_6.offset, 40.0/4)
        self.assertEqual(signal_6.minimum, 10.0)
        self.assertEqual(signal_6.maximum, 10.0 + 3*10.0/4)
        self.assertEqual(signal_6.decimal.scale, 10.0/4)
        self.assertEqual(signal_6.decimal.offset, 40.0/4)
        self.assertEqual(signal_6.decimal.minimum, 10.0)
        self.assertEqual(signal_6.decimal.maximum, 10.0 + 3*10.0/4)
        self.assertEqual(signal_6.unit, None)
        self.assertEqual(signal_6.choices, None)
        self.assertEqual(signal_6.comment, 'Lonely system signal comment')
        self.assertEqual(signal_6.is_multiplexer, False)
        self.assertEqual(signal_6.multiplexer_ids, None)

    def test_system_4_arxml(self):
        db = cantools.db.load_file('tests/files/arxml/system-4.2.arxml')

        self.assertEqual(len(db.buses), 1)
        bus = db.buses[0]
        self.assertEqual(bus.name, 'Cluster0')
        self.assertEqual(bus.comment, 'The great CAN cluster')
        self.assertEqual(bus.comments, { 'FOR-ALL': 'The great CAN cluster' })
        self.assertEqual(bus.baudrate, 500000)
        self.assertEqual(bus.fd_baudrate, 2000000)

        self.assertEqual(len(db.nodes), 3)
        self.assertEqual(len(db.messages), 7)
        self.assertTrue(db.autosar is not None)
        self.assertTrue(db.dbc is None)
        self.assertEqual(db.autosar.arxml_version, "4.0.0")

        node1 = db.nodes[0]
        self.assertEqual(node1.name, 'DJ')
        self.assertEqual(node1.comments, None)

        node2 = db.nodes[1]
        self.assertEqual(node2.name, 'Dancer')
        self.assertEqual(node2.comments, {'FOR-ALL': 'Rhythm is a Dancer!'})
        self.assertEqual(node2.comment, 'Rhythm is a Dancer!')

        node3 = db.nodes[2]
        self.assertEqual(node3.name, 'Guard')

        # multiplexed message
        mux_message = db.messages[0]
        self.assertEqual(mux_message.frame_id, 4)
        self.assertEqual(mux_message.is_extended_frame, False)
        self.assertEqual(mux_message.is_fd, True)
        self.assertEqual(mux_message.name, 'MultiplexedMessage')
        self.assertEqual(mux_message.length, 2)
        self.assertEqual(mux_message.unused_bit_pattern, 0xff)
        self.assertEqual(mux_message.senders, [])
        self.assertEqual(mux_message.send_type, None)
        self.assertEqual(mux_message.cycle_time, None)
        self.assertEqual(len(mux_message.signals), 6)
        self.assertEqual(mux_message.bus_name, 'Cluster0')
        self.assertEqual(mux_message.comments, None)
        self.assertEqual(mux_message.comment, None)
        self.assertTrue(mux_message.dbc is None)
        self.assertTrue(mux_message.autosar is not None)
        self.assertEqual(mux_message.autosar.pdu_paths, [
            '/MultiplexedIPdu/multiplexed_message',
            '/ISignalIPdu/multiplexed_message_0',
            '/ISignalIPdu/multiplexed_message_1',
            '/ISignalIPdu/multiplexed_message_static',
        ])

        mux_signal_static = mux_message.signals[0]
        self.assertEqual(mux_signal_static.name, 'MultiplexedStatic')
        self.assertEqual(mux_signal_static.start, 0)
        self.assertEqual(mux_signal_static.length, 3)
        self.assertEqual(mux_signal_static.initial, 7)
        self.assertEqual(mux_signal_static.receivers, [])
        self.assertEqual(mux_signal_static.byte_order, 'little_endian')
        self.assertEqual(mux_signal_static.is_signed, True)
        self.assertEqual(mux_signal_static.is_float, False)
        self.assertEqual(mux_signal_static.scale, 1.0)
        self.assertEqual(mux_signal_static.offset, 0.0)
        self.assertEqual(mux_signal_static.minimum, None)
        self.assertEqual(mux_signal_static.maximum, None)
        self.assertEqual(mux_signal_static.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_static.decimal.offset, 0.0)
        self.assertEqual(mux_signal_static.decimal.minimum, None)
        self.assertEqual(mux_signal_static.decimal.maximum, None)
        self.assertEqual(mux_signal_static.unit, None)
        self.assertEqual(mux_signal_static.choices, None)
        self.assertEqual(mux_signal_static.comment, None)
        self.assertEqual(mux_signal_static.comments, None)
        self.assertEqual(mux_signal_static.is_multiplexer, False)
        self.assertEqual(mux_signal_static.multiplexer_ids, None)

        mux_signal_hello = mux_message.signals[1]
        self.assertEqual(mux_signal_hello.name, 'Hello')
        self.assertEqual(mux_signal_hello.start, 3)
        self.assertEqual(mux_signal_hello.length, 3)
        self.assertEqual(mux_signal_hello.initial, 0)
        self.assertEqual(mux_signal_hello.receivers, [])
        self.assertEqual(mux_signal_hello.byte_order, 'little_endian')
        self.assertEqual(mux_signal_hello.is_signed, True)
        self.assertEqual(mux_signal_hello.is_float, False)
        self.assertEqual(mux_signal_hello.scale, 1.0)
        self.assertEqual(mux_signal_hello.offset, 0.0)
        self.assertEqual(mux_signal_hello.minimum, None)
        self.assertEqual(mux_signal_hello.maximum, None)
        self.assertEqual(mux_signal_hello.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_hello.decimal.offset, 0.0)
        self.assertEqual(mux_signal_hello.decimal.minimum, None)
        self.assertEqual(mux_signal_hello.decimal.maximum, None)
        self.assertEqual(mux_signal_hello.unit, None)
        self.assertEqual(mux_signal_hello.choices, None)
        self.assertEqual(mux_signal_hello.comment, None)
        self.assertEqual(mux_signal_hello.comments, None)
        self.assertEqual(mux_signal_hello.is_multiplexer, False)
        self.assertEqual(mux_signal_hello.multiplexer_signal, 'MultiplexedMessage_selector1')
        self.assertEqual(mux_signal_hello.multiplexer_ids, [ 0 ])

        mux_signal_world2 = mux_message.signals[2]
        self.assertEqual(mux_signal_world2.name, 'World2')
        self.assertEqual(mux_signal_world2.start, 3)
        self.assertEqual(mux_signal_world2.length, 1)
        self.assertEqual(mux_signal_world2.initial, 0)
        self.assertEqual(mux_signal_world2.receivers, [])
        self.assertEqual(mux_signal_world2.byte_order, 'little_endian')
        self.assertEqual(mux_signal_world2.is_signed, True)
        self.assertEqual(mux_signal_world2.is_float, False)
        self.assertEqual(mux_signal_world2.scale, 1.0)
        self.assertEqual(mux_signal_world2.offset, 0.0)
        self.assertEqual(mux_signal_world2.minimum, None)
        self.assertEqual(mux_signal_world2.maximum, None)
        self.assertEqual(mux_signal_world2.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_world2.decimal.offset, 0.0)
        self.assertEqual(mux_signal_world2.decimal.minimum, None)
        self.assertEqual(mux_signal_world2.decimal.maximum, None)
        self.assertEqual(mux_signal_world2.unit, None)
        self.assertEqual(mux_signal_world2.choices, None)
        self.assertEqual(mux_signal_world2.comment, None)
        self.assertEqual(mux_signal_world2.comments, None)
        self.assertEqual(mux_signal_world2.is_multiplexer, False)
        self.assertEqual(mux_signal_world2.multiplexer_signal, 'MultiplexedMessage_selector1')
        self.assertEqual(mux_signal_world2.multiplexer_ids, [ 1 ])

        mux_signal_world1 = mux_message.signals[3]
        self.assertEqual(mux_signal_world1.name, 'World1')
        self.assertEqual(mux_signal_world1.start, 4)
        self.assertEqual(mux_signal_world1.length, 2)
        self.assertEqual(mux_signal_world1.initial, 3)
        self.assertEqual(mux_signal_world1.receivers, [])
        self.assertEqual(mux_signal_world1.byte_order, 'little_endian')
        self.assertEqual(mux_signal_world1.is_signed, True)
        self.assertEqual(mux_signal_world1.is_float, False)
        self.assertEqual(mux_signal_world1.scale, 1.0)
        self.assertEqual(mux_signal_world1.offset, 0.0)
        self.assertEqual(mux_signal_world1.minimum, None)
        self.assertEqual(mux_signal_world1.maximum, None)
        self.assertEqual(mux_signal_world1.decimal.scale, Decimal('1'))
        self.assertEqual(mux_signal_world1.decimal.offset, 0.0)
        self.assertEqual(mux_signal_world1.decimal.minimum, None)
        self.assertEqual(mux_signal_world1.decimal.maximum, None)
        self.assertEqual(mux_signal_world1.unit, None)
        self.assertEqual(mux_signal_world1.choices, None)
        self.assertEqual(mux_signal_world1.comment, None)
        self.assertEqual(mux_signal_world1.comments, None)
        self.assertEqual(mux_signal_world1.is_multiplexer, False)
        self.assertEqual(mux_signal_world1.multiplexer_signal, 'MultiplexedMessage_selector1')
        self.assertEqual(mux_signal_world1.multiplexer_ids, [ 1 ])

        message_1 = db.messages[1]
        self.assertEqual(message_1.frame_id, 5)
        self.assertEqual(message_1.is_extended_frame, False)
        self.assertEqual(message_1.is_fd, True)
        self.assertEqual(message_1.name, 'Message1')
        self.assertEqual(message_1.length, 9)
        self.assertEqual(message_1.senders, ['DJ'])
        self.assertEqual(message_1.send_type, None)
        self.assertEqual(message_1.cycle_time, None)
        self.assertEqual(len(message_1.signals), 5)
        self.assertEqual(message_1.comments["DE"], 'Kommentar1')
        self.assertEqual(message_1.comments["EN"], 'Comment1')
        self.assertEqual(message_1.bus_name, 'Cluster0')
        self.assertEqual(message_1.comment, 'Comment1')
        message_1.comments = {'DE': 'Kommentar eins', 'EN': 'Comment one'}
        self.assertEqual(message_1.comments,
                         {
                             'DE': 'Kommentar eins',
                             'EN': 'Comment one'
                         })
        self.assertTrue(message_1.dbc is None)
        self.assertTrue(message_1.autosar is not None)
        self.assertEqual(message_1.autosar.pdu_paths, [ '/ISignalIPdu/message1' ])

        signal_1 = message_1.signals[0]
        self.assertEqual(signal_1.name, 'message1_SeqCounter')
        self.assertEqual(signal_1.start, 0)
        self.assertEqual(signal_1.length, 16)
        self.assertEqual(signal_1.initial, None)
        self.assertEqual(signal_1.receivers, ['Dancer'])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, Decimal('1'))
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = message_1.signals[1]
        self.assertEqual(signal_2.name, 'message1_CRC')
        self.assertEqual(signal_2.start, 16)
        self.assertEqual(signal_2.length, 16)
        self.assertEqual(signal_2.initial, None)
        self.assertEqual(signal_2.receivers, ['Dancer'])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, Decimal('1'))
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comment, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = message_1.signals[2]
        self.assertEqual(signal_3.name, 'signal6')
        self.assertEqual(signal_3.start, 32)
        self.assertEqual(signal_3.length, 1)
        self.assertEqual(signal_3.initial, 'zero')
        self.assertEqual(signal_3.receivers, ['Dancer'])
        self.assertEqual(signal_3.byte_order, 'little_endian')
        self.assertEqual(signal_3.is_signed, False)
        self.assertEqual(signal_3.is_float, False)
        self.assertEqual(signal_3.scale, 0.1)
        self.assertEqual(signal_3.offset, 0.0)
        self.assertEqual(signal_3.minimum, 0)
        self.assertEqual(signal_3.maximum, 0.1)
        self.assertEqual(signal_3.decimal.scale, Decimal('0.1'))
        self.assertEqual(signal_3.decimal.offset, 0.0)
        self.assertEqual(signal_3.decimal.minimum, 0)
        self.assertEqual(signal_3.decimal.maximum, 0.1)
        self.assertEqual(signal_3.unit, "wp")
        self.assertEqual(signal_3.choices, {0: 'zero'})
        self.assertEqual(signal_3.comment, None)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        signal_4 = message_1.signals[3]
        self.assertEqual(signal_4.name, 'signal1')
        self.assertEqual(signal_4.start, 36)
        self.assertEqual(signal_4.length, 3)
        self.assertEqual(signal_4.initial, 25.0)
        self.assertEqual(signal_4.receivers, ['Dancer'])
        self.assertEqual(signal_4.byte_order, 'big_endian')
        self.assertEqual(signal_4.is_signed, False)
        self.assertEqual(signal_4.is_float, False)
        self.assertEqual(signal_4.scale, 5.0)
        self.assertEqual(signal_4.offset, 0.0)
        self.assertEqual(signal_4.minimum, 0.0)
        self.assertEqual(signal_4.maximum, 20.0)
        self.assertEqual(signal_4.decimal.scale, 5.0)
        self.assertEqual(signal_4.decimal.offset, 0.0)
        self.assertEqual(signal_4.decimal.minimum, 0.0)
        self.assertEqual(signal_4.decimal.maximum, 20.0)
        self.assertEqual(signal_4.unit, 'm')
        self.assertEqual(signal_4.choices, None)
        self.assertEqual(signal_4.comments["EN"], 'Signal comment!')
        self.assertEqual(signal_4.comments["DE"], 'Signalkommentar!')
        self.assertEqual(signal_4.comment, 'Signal comment!')
        signal_4.comments = {'DE': 'Kein Kommentar!', 'EN': 'No comment!'}
        self.assertEqual(signal_4.comments,
                         {
                             'DE': 'Kein Kommentar!',
                             'EN': 'No comment!'
                         })

        self.assertEqual(signal_4.is_multiplexer, False)
        self.assertEqual(signal_4.multiplexer_ids, None)

        signal_5 = message_1.signals[4]
        self.assertEqual(signal_5.name, 'signal5')
        self.assertEqual(signal_5.start, 40)
        self.assertEqual(signal_5.initial, None)
        self.assertEqual(signal_5.length, 32)
        self.assertEqual(signal_5.receivers, ['Dancer'])
        self.assertEqual(signal_5.byte_order, 'little_endian')
        self.assertEqual(signal_5.is_signed, False)
        self.assertEqual(signal_5.is_float, True)
        self.assertEqual(signal_5.scale, 1)
        self.assertEqual(signal_5.offset, 0)
        self.assertEqual(signal_5.minimum, None)
        self.assertEqual(signal_5.maximum, None)
        self.assertEqual(signal_5.decimal.scale, 1)
        self.assertEqual(signal_5.decimal.offset, 0)
        self.assertEqual(signal_5.decimal.minimum, None)
        self.assertEqual(signal_5.decimal.maximum, None)
        self.assertEqual(signal_5.unit, None)
        self.assertEqual(signal_5.choices, None)
        self.assertEqual(signal_5.comment, None)
        self.assertEqual(signal_5.is_multiplexer, False)
        self.assertEqual(signal_5.multiplexer_ids, None)

        message_2 = db.messages[2]
        self.assertEqual(message_2.frame_id, 6)
        self.assertEqual(message_2.is_extended_frame, True)
        self.assertEqual(message_2.is_fd, True)
        self.assertEqual(message_2.name, 'Message2')
        self.assertEqual(message_2.length, 7)
        self.assertEqual(message_2.senders, ['Dancer'])
        self.assertEqual(message_2.send_type, None)
        self.assertEqual(message_2.cycle_time, 200)
        self.assertEqual(len(message_2.signals), 3)
        self.assertEqual(message_2.comment, None)
        self.assertEqual(message_2.bus_name, 'Cluster0')
        self.assertTrue(message_2.dbc is None)
        self.assertTrue(message_2.autosar is not None)
        self.assertEqual(message_2.autosar.pdu_paths, [ '/ISignalIPdu/message2' ])

        signal_1 = message_2.signals[0]
        self.assertEqual(signal_1.name, 'signal3')
        self.assertEqual(signal_1.start, 6)
        self.assertEqual(signal_1.length, 2)
        self.assertEqual(signal_1.receivers, ['DJ'])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = message_2.signals[1]
        self.assertEqual(signal_2.name, 'signal2')
        self.assertEqual(signal_2.start, 18)
        self.assertEqual(signal_2.length, 11)
        self.assertEqual(signal_2.receivers, ['DJ'])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, True)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comments["FOR-ALL"], 'Signal comment!')
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = message_2.signals[2]
        self.assertEqual(signal_3.name, 'signal4')
        self.assertEqual(signal_3.start, 30)
        self.assertEqual(signal_3.length, 4)
        self.assertEqual(signal_3.receivers, ['DJ'])
        self.assertEqual(signal_3.byte_order, 'little_endian')
        self.assertEqual(signal_3.is_signed, False)
        self.assertEqual(signal_3.is_float, False)
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 0)
        self.assertEqual(signal_3.minimum, None)
        self.assertEqual(signal_3.maximum, None)
        self.assertEqual(signal_3.decimal.scale, 1)
        self.assertEqual(signal_3.decimal.offset, 0)
        self.assertEqual(signal_3.decimal.minimum, None)
        self.assertEqual(signal_3.decimal.maximum, None)
        self.assertEqual(signal_3.unit, None)
        self.assertEqual(signal_3.choices, {1: 'one', 2: 'two'})
        self.assertEqual(signal_3.choices[1].comments,
                         {'EN': 'One Comment', 'DE': 'Ein Kommentar'})
        self.assertEqual(signal_3.comment, None)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        message_3 = db.messages[3]
        self.assertEqual(message_3.frame_id, 100)
        self.assertEqual(message_3.is_extended_frame, False)
        self.assertEqual(message_3.is_fd, False)
        self.assertEqual(message_3.name, 'Message3')
        self.assertEqual(message_3.length, 6)
        self.assertEqual(message_3.unused_bit_pattern, 0xff)
        self.assertEqual(message_3.senders, [])
        self.assertEqual(message_3.send_type, None)
        self.assertEqual(message_3.cycle_time, None)
        self.assertEqual(len(message_3.signals), 4)
        self.assertEqual(message_3.comment, None)
        self.assertEqual(message_3.bus_name, 'Cluster0')
        self.assertTrue(message_3.dbc is None)
        self.assertTrue(message_3.autosar is not None)
        self.assertEqual(message_3.autosar.pdu_paths,
                         [
                             '/SecuredIPdu/message3_secured',
                             '/ISignalIPdu/message3'
                         ])

        signal_1 = message_3.signals[0]
        self.assertEqual(signal_1.name, 'message3_CRC')
        self.assertEqual(signal_1.start, 0)
        self.assertEqual(signal_1.length, 8)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = message_3.signals[1]
        self.assertEqual(signal_2.name, 'message3_SeqCounter')
        self.assertEqual(signal_2.start, 8)
        self.assertEqual(signal_2.length, 4)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comment, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = message_3.signals[2]
        self.assertEqual(signal_3.name, 'Message3_Freshness')
        self.assertEqual(signal_3.start, 39)
        self.assertEqual(signal_3.length, 6)
        self.assertEqual(signal_3.receivers, [])
        self.assertEqual(signal_3.byte_order, 'big_endian')
        self.assertEqual(signal_3.is_signed, False)
        self.assertEqual(signal_3.is_float, False)
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 0)
        self.assertEqual(signal_3.minimum, None)
        self.assertEqual(signal_3.maximum, None)
        self.assertEqual(signal_3.decimal.scale, 1)
        self.assertEqual(signal_3.decimal.offset, 0)
        self.assertEqual(signal_3.decimal.minimum, None)
        self.assertEqual(signal_3.decimal.maximum, None)
        self.assertEqual(signal_3.unit, None)
        self.assertEqual(signal_3.choices, None)
        self.assertEqual(signal_3.comment, "Truncated freshness value for 'Message3'")
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        signal_4 = message_3.signals[3]
        self.assertEqual(signal_4.name, 'Message3_Authenticator')
        self.assertEqual(signal_4.start, 33)
        self.assertEqual(signal_4.length, 10)
        self.assertEqual(signal_4.receivers, [])
        self.assertEqual(signal_4.byte_order, 'big_endian')
        self.assertEqual(signal_4.is_signed, False)
        self.assertEqual(signal_4.is_float, False)
        self.assertEqual(signal_4.scale, 1)
        self.assertEqual(signal_4.offset, 0)
        self.assertEqual(signal_4.minimum, None)
        self.assertEqual(signal_4.maximum, None)
        self.assertEqual(signal_4.decimal.scale, 1)
        self.assertEqual(signal_4.decimal.offset, 0)
        self.assertEqual(signal_4.decimal.minimum, None)
        self.assertEqual(signal_4.decimal.maximum, None)
        self.assertEqual(signal_4.unit, None)
        self.assertEqual(signal_4.choices, None)
        self.assertEqual(signal_4.comment, "Truncated authenticator value for 'Message3'")
        self.assertEqual(signal_4.is_multiplexer, False)
        self.assertEqual(signal_4.multiplexer_ids, None)

        # message 4 tests different base encodings
        message_4 = db.messages[4]
        self.assertEqual(message_4.frame_id, 101)
        self.assertEqual(message_4.is_extended_frame, False)
        self.assertEqual(message_4.is_fd, False)
        self.assertEqual(message_4.name, 'Message4')
        self.assertEqual(message_4.length, 6)
        self.assertEqual(message_4.unused_bit_pattern, 0x55)
        self.assertEqual(message_4.senders, [])
        self.assertEqual(message_4.send_type, None)
        self.assertEqual(message_4.cycle_time, None)
        self.assertEqual(len(message_4.signals), 3)
        self.assertEqual(message_4.comment, None)
        self.assertEqual(message_4.bus_name, 'Cluster0')
        self.assertTrue(message_4.dbc is None)
        self.assertTrue(message_4.autosar is not None)
        self.assertEqual(message_4.autosar.pdu_paths, [ '/ISignalIPdu/message4' ])

        signal_2 = message_4.signals[0]
        self.assertEqual(signal_2.name, 'signal2')
        self.assertEqual(signal_2.is_signed, True)
        self.assertEqual(signal_2.is_float, False)

        signal_2_1c = message_4.signals[1]
        self.assertEqual(signal_2_1c.name, 'signal2_1c')
        self.assertEqual(signal_2_1c.is_signed, True)
        self.assertEqual(signal_2_1c.is_float, False)

        signal_2_sm = message_4.signals[2]
        self.assertEqual(signal_2_sm.name, 'signal2_sm')
        self.assertEqual(signal_2_sm.is_signed, True)
        self.assertEqual(signal_2_sm.is_float, False)

        container_message = db.messages[5]
        self.assertEqual(container_message.frame_id, 102)
        self.assertEqual(container_message.is_extended_frame, False)
        self.assertEqual(container_message.name, 'OneToContainThemAll')
        self.assertEqual(container_message.length, 64)
        self.assertEqual(container_message.unused_bit_pattern, 0xff)
        self.assertEqual(container_message.senders, ['DJ', 'Dancer'])
        self.assertEqual(container_message.send_type, None)
        self.assertEqual(container_message.cycle_time, None)
        self.assertEqual(len(container_message.signals), 0)
        self.assertEqual(container_message.comment, None)
        self.assertEqual(container_message.bus_name, 'Cluster0')
        self.assertEqual(len(container_message._contained_messages), 5)
        header_ids = sorted([cm.header_id for cm in container_message._contained_messages])
        self.assertEqual(header_ids, [ 0x010203, 0x040506, 0x070809, 0x0a0b0c, 0x1d2e3f ])
        self.assertTrue(container_message.dbc is None)
        self.assertTrue(container_message.autosar is not None)
        self.assertEqual(container_message.autosar.pdu_paths,
            [
                '/ContainerIPdu/OneToContainThemAll',
                '/ISignalIPdu/message1',
                '/ISignalIPdu/message2',
                '/ISignalIPdu/message3',
                '/ISignalIPdu/message3',
                '/SecuredIPdu/message3_secured',
                '/ISignalIPdu/multiplexed_message_0',
                '/ISignalIPdu/multiplexed_message_1',
                '/ISignalIPdu/multiplexed_message_static',
                '/MultiplexedIPdu/multiplexed_message'
            ])

        nm_message = db.messages[6]
        self.assertEqual(nm_message.frame_id, 1001)
        self.assertEqual(nm_message.is_extended_frame, False)
        self.assertEqual(nm_message.name, 'AlarmStatus')
        self.assertEqual(nm_message.length, 1)
        self.assertEqual(nm_message.senders, ['Guard'])
        self.assertEqual(nm_message.send_type, None)
        self.assertEqual(nm_message.cycle_time, None)
        self.assertEqual(len(nm_message.signals), 1)
        self.assertEqual(nm_message.comment, None)
        self.assertEqual(nm_message.bus_name, 'Cluster0')
        self.assertTrue(nm_message.dbc is None)
        self.assertTrue(nm_message.autosar is not None)

    def test_system_arxml_traversal(self):
        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(
                'tests/files/arxml/system-dangling-reference-4.2.arxml')
        self.assertEqual(str(cm.exception),
                         'ARXML: \"Encountered dangling reference FRAME-REF ' \
                         'of type "CAN-FRAME": /PackageDoesNotExist/Message1"')

        root = ElementTree.parse('tests/files/arxml/system-4.2.arxml').getroot()
        loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=True)

        # a base node must always be specified
        with self.assertRaises(ValueError) as cm:
            no_base_elem = loader._get_arxml_children(None, ["AR-PACKAGES", "*AR-PACKAGE"])
        self.assertEqual(str(cm.exception), "Cannot retrieve a child element of a non-existing node!")

        # test multiple child node matches
        children1 = loader._get_arxml_children(loader._root, ["AR-PACKAGES", "*AR-PACKAGE"])
        childen1_short_names = \
            list(map(lambda x: x.find("ns:SHORT-NAME", loader._xml_namespaces).text, children1))

        self.assertEqual(childen1_short_names,
                         [
                             'ECU',
                             'PDU_GROUPS',
                             'NMConfig',
                             'Cluster',
                             'CanFrame',
                             'ISignal',
                             'Transformers',
                             'Constants',
                             'MultiplexedIPdu',
                             'ContainerIPdu',
                             'ISignalIPdu',
                             'NMPdu',
                             'SecuredIPdu',
                             'SecOCProps',
                             'Unit',
                             'CompuMethod',
                             'SystemSignal',
                             'SwBaseType',
                             'System'
                         ])

        # test unique location specifier if child nodes exist
        with self.assertRaises(ValueError) as cm:
            non_unique = loader._get_arxml_children(loader._root, ["AR-PACKAGES", "AR-PACKAGE"])
        self.assertEqual(str(cm.exception),
                         "Encountered a a non-unique child node of type AR-PACKAGE which ought to be unique")

        # test the reference cache
        foo = loader._follow_arxml_reference(loader._root, "/CanFrame/Message1", "CAN-FRAME")
        bar = loader._follow_arxml_reference(loader._root, "/CanFrame/Message1", "CAN-FRAME")
        self.assertEqual(foo, bar)

        # test non-unique location while assuming that it is unique
        with self.assertRaises(ValueError) as cm:
            no_base_elem = loader._get_unique_arxml_child(loader._root, ["AR-PACKAGES", "*AR-PACKAGE"])
        self.assertEqual(str(cm.exception), "['AR-PACKAGES', '*AR-PACKAGE'] does not resolve into a unique node")

    def test_no_compu_method_category_arxml(self):
        db = cantools.db.load_file('tests/files/arxml/compu_method_no_category.arxml')

        self.assertEqual(len(db.buses), 1)
        bus = db.buses[0]
        self.assertEqual(bus.name, 'MY_CLUSTER')
        self.assertEqual(bus.comment, None)
        self.assertEqual(bus.comments, None)
        self.assertEqual(bus.baudrate, 500000)
        self.assertEqual(bus.fd_baudrate, None)

        self.assertEqual(len(db.nodes), 0)

        self.assertEqual(len(db.messages), 1)

        # message
        message_1 = db.messages[0]
        self.assertEqual(message_1.frame_id, 0x5f0)
        self.assertEqual(message_1.is_extended_frame, False)
        self.assertEqual(message_1.is_fd, False)
        self.assertEqual(message_1.name, 'MY_MESSAGE_XIX_MY_CLUSTER')
        self.assertEqual(message_1.length, 8)
        self.assertEqual(message_1.senders, [])
        self.assertEqual(message_1.send_type, None)
        self.assertEqual(message_1.cycle_time, 200)
        self.assertEqual(message_1.comments, None)
        self.assertEqual(message_1.bus_name, 'MY_CLUSTER')
        self.assertEqual(len(message_1.signals), 1)

        # signal
        signal_1 = message_1.signals[0]
        self.assertEqual(signal_1.name, 'MY_SIGNAL_XIX_MY_MESSAGE_XIX_MY_CLUSTER')
        self.assertEqual(signal_1.start, 15)
        self.assertEqual(signal_1.length, 1)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.initial, 0)
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comments, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

    def test_unused_bit_pattern(self):
        db = cantools.db.load_file('tests/files/arxml/system-4.2.arxml')

        message4 = db.get_message_by_name('Message4')
        input_dict = {
            'signal2': 0x3ff,
            'signal2_1c': 0x3ff,
            'signal2_sm': 0x3ff,
        }

        message4.unused_bit_pattern = 0x00
        encoded_ref = b'\xff\x03\xff\x03\xff\x03'
        encoded1 = message4.encode(input_dict, padding=False)
        encoded2 = message4.encode(input_dict, padding=True)
        self.assertEqual(encoded1, encoded_ref)
        self.assertEqual(encoded2, encoded_ref)

        input_dict = {
            'signal2': 0,
            'signal2_1c': 0,
            'signal2_sm': 0,
        }

        message4.unused_bit_pattern = 0xff
        encoded_ref1 = b'\x00\x00\x00\x00\x00\x00'
        encoded_ref2 = b'\x00\xf8\x00\xf8\x00\xf8'
        encoded1 = message4.encode(input_dict, padding=False)
        encoded2 = message4.encode(input_dict, padding=True)
        self.assertEqual(encoded1, encoded_ref1)
        self.assertEqual(encoded2, encoded_ref2)

        message4.unused_bit_pattern = 0x55
        encoded_ref = b'\x00\x50\x00\x50\x00\x50'
        encoded = message4.encode(input_dict, padding=True)
        self.assertEqual(encoded, encoded_ref)

    def test_system_arxml_float_values(self):
        db = cantools.db.load_file('tests/files/arxml/system-float-values.arxml')

        self.assertEqual(len(db.buses), 1)
        bus = db.buses[0]
        self.assertEqual(bus.name, 'CCB')
        self.assertEqual(bus.comment, None)
        self.assertEqual(bus.comments, None)
        self.assertEqual(bus.baudrate, 500000)
        self.assertEqual(bus.fd_baudrate, 2000000)

        self.assertEqual([x.name for x in db.nodes], [ 'IDCM_A' ])

        self.assertEqual(len(db.messages), 1)

        # message
        message_1 = db.messages[0]
        self.assertEqual(message_1.frame_id, 0xda)
        self.assertEqual(message_1.is_extended_frame, False)
        self.assertEqual(message_1.name, 'Test_Frame')
        self.assertEqual(message_1.length, 2)
        self.assertEqual(message_1.senders, ['IDCM_A'])
        self.assertEqual(message_1.send_type, None)
        self.assertEqual(message_1.cycle_time, 100)
        self.assertEqual(message_1.comments, None)
        self.assertEqual(message_1.bus_name, 'CCB')
        self.assertEqual(len(message_1.signals), 2)

        # first signal
        signal_1 = message_1.signals[0]
        self.assertEqual(signal_1.name, 'IMessage_2')
        self.assertEqual(signal_1.start, 7)
        self.assertEqual(signal_1.length, 7)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'big_endian')
        self.assertEqual(signal_1.initial, 0.0)
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, 0.0)
        self.assertEqual(signal_1.maximum, 127.0)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, 0)
        self.assertEqual(signal_1.decimal.maximum, 127)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comments, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        # second signal
        signal_2 = message_1.signals[1]
        self.assertEqual(signal_2.name, 'IMessage_1')
        self.assertEqual(signal_2.start, 15)
        self.assertEqual(signal_2.length, 5)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'big_endian')
        self.assertEqual(signal_2.initial, 0.0)
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, 0.0)
        self.assertEqual(signal_2.maximum, 31.0)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, 0)
        self.assertEqual(signal_2.decimal.maximum, 31)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comments, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

    def test_system_missing_factor_arxml(self):
        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(
                'tests/files/arxml/system-missing-factor-4.2.arxml')

        self.assertEqual(
            str(cm.exception),
            'ARXML: "Expected 2 numerator values for linear scaling, but got 1."')

    def test_system_missing_denominator_arxml(self):
        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(
                'tests/files/arxml/system-missing-denominator-4.2.arxml')

        self.assertEqual(
            str(cm.exception),
            'ARXML: "Expected 1 denominator value for linear scaling, but got 0."')

    def test_system_missing_rational_arxml(self):
        db = cantools.db.load_file(
            'tests/files/arxml/system-missing-rational-4.2.arxml')

        signal_1 = db.messages[0].signals[0]
        self.assertEqual(signal_1.scale, 1.0)
        self.assertEqual(signal_1.offset, 0.0)
        self.assertEqual(signal_1.minimum, 0.0)
        self.assertEqual(signal_1.maximum, 4.0)
        self.assertEqual(signal_1.decimal.scale, 1.0)
        self.assertEqual(signal_1.decimal.offset, 0.0)
        self.assertEqual(signal_1.decimal.minimum, 0.0)
        self.assertEqual(signal_1.decimal.maximum, 4.0)

    def test_system_bad_root_tag(self):
        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(
                'tests/files/arxml/system-bad-root-tag-4.2.arxml')

        self.assertEqual(
            str(cm.exception),
            'ARXML: "No XML namespace specified or illegal root tag name \'{http://autosar.org/schema/r4.0}NOT-AUTOSAR\'"')

    def test_ecu_extract_arxml(self):
        db = cantools.database.Database()
        db.add_arxml_file('tests/files/arxml/ecu-extract-4.2.arxml')

        self.assertEqual(len(db.nodes), 0)

        self.assertEqual(len(db.messages), 3)

        message_1 = db.messages[0]
        self.assertEqual(message_1.frame_id, 5)
        self.assertEqual(message_1.is_extended_frame, False)
        self.assertEqual(message_1.is_fd, False)
        self.assertEqual(message_1.name, 'Message1')
        self.assertEqual(message_1.length, 6)
        self.assertEqual(message_1.senders, [])
        self.assertEqual(message_1.send_type, None)
        self.assertEqual(message_1.cycle_time, None)
        self.assertEqual(len(message_1.signals), 2)
        self.assertEqual(message_1.comment, None)  # ToDo: 'Comment1'
        self.assertEqual(message_1.bus_name, None)

        signal_1 = message_1.signals[0]
        self.assertEqual(signal_1.name, 'Signal1')
        self.assertEqual(signal_1.start, 4)
        self.assertEqual(signal_1.length, 3)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'big_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        # Signal scale, offset, minimum and maximum are not part of an
        # ECU extract it seems.
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0.0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = message_1.signals[1]
        self.assertEqual(signal_2.name, 'Signal5')
        self.assertEqual(signal_2.start, 16)
        self.assertEqual(signal_2.length, 32)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.is_float, True)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comment, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        message_2 = db.messages[1]
        self.assertEqual(message_2.frame_id, 6)
        self.assertEqual(message_2.is_extended_frame, True)
        self.assertEqual(message_2.is_fd, False)
        self.assertEqual(message_2.name, 'Message2')
        self.assertEqual(message_2.length, 7)
        self.assertEqual(message_2.senders, [])
        self.assertEqual(message_2.send_type, None)
        self.assertEqual(message_2.cycle_time, None)
        self.assertEqual(len(message_2.signals), 3)
        self.assertEqual(message_2.comment, None)
        self.assertEqual(message_2.bus_name, None)

        signal_1 = message_2.signals[0]
        self.assertEqual(signal_1.name, 'Signal3')
        self.assertEqual(signal_1.start, 6)
        self.assertEqual(signal_1.length, 2)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.is_float, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, None)
        self.assertEqual(signal_1.decimal.scale, 1)
        self.assertEqual(signal_1.decimal.offset, 0)
        self.assertEqual(signal_1.decimal.minimum, None)
        self.assertEqual(signal_1.decimal.maximum, None)
        self.assertEqual(signal_1.unit, None)
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)

        signal_2 = message_2.signals[1]
        self.assertEqual(signal_2.name, 'Signal2')
        self.assertEqual(signal_2.start, 18)
        self.assertEqual(signal_2.length, 11)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, True)
        self.assertEqual(signal_2.is_float, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 0)
        self.assertEqual(signal_2.minimum, None)
        self.assertEqual(signal_2.maximum, None)
        self.assertEqual(signal_2.decimal.scale, 1)
        self.assertEqual(signal_2.decimal.offset, 0)
        self.assertEqual(signal_2.decimal.minimum, None)
        self.assertEqual(signal_2.decimal.maximum, None)
        self.assertEqual(signal_2.unit, None)
        self.assertEqual(signal_2.choices, None)
        # Signal comment is not part of an ECU extract it seems.
        self.assertEqual(signal_2.comment, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)

        signal_3 = message_2.signals[2]
        self.assertEqual(signal_3.name, 'Signal4')
        self.assertEqual(signal_3.start, 30)
        self.assertEqual(signal_3.length, 4)
        self.assertEqual(signal_3.receivers, [])
        self.assertEqual(signal_3.byte_order, 'little_endian')
        self.assertEqual(signal_3.is_signed, False)
        self.assertEqual(signal_3.is_float, False)
        # Signal scale, offset, minimum, maximum and choices are not
        # part of an ECU extract it seems.
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 0)
        self.assertEqual(signal_3.minimum, None)
        self.assertEqual(signal_3.maximum, None)
        self.assertEqual(signal_3.decimal.scale, 1)
        self.assertEqual(signal_3.decimal.offset, 0)
        self.assertEqual(signal_3.decimal.minimum, None)
        self.assertEqual(signal_3.decimal.maximum, None)
        self.assertEqual(signal_3.unit, None)
        self.assertEqual(signal_3.choices, None)
        self.assertEqual(signal_3.comment, None)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)

        message_3 = db.messages[2]
        self.assertEqual(message_3.frame_id, 100)
        self.assertEqual(message_3.is_extended_frame, False)
        self.assertEqual(message_3.is_fd, False)
        self.assertEqual(message_3.name, 'Message3')
        self.assertEqual(message_3.length, 8)
        self.assertEqual(message_3.senders, [])
        self.assertEqual(message_3.send_type, None)
        self.assertEqual(message_3.cycle_time, None)
        self.assertEqual(len(message_3.signals), 0)
        self.assertEqual(message_3.comment, None)
        self.assertEqual(message_3.bus_name, None)

    def test_encode_mixed_signal(self):
        # cf issue #373
        db = cantools.db.load_file('tests/files/arxml/system-4.2.arxml')

        decoded_message = {
            'message1_SeqCounter': 123,
            'message1_CRC': 456,
            'signal1' : 0.0,
            'signal5' : 1e5,
            'signal6' : 'zero',
        }

        encoded_message = db.encode_message('Message1', decoded_message)

        self.assertEqual(encoded_message, b'{\x00\xc8\x01\x00\x00P\xc3G')

        decoded_message2 = db.decode_message('Message1', encoded_message)
        encoded_message2 = db.encode_message('Message1', decoded_message2)

        self.assertEqual(encoded_message2, encoded_message)

    def test_issue_138(self):
        """Test issue 138.

        """

        filename = 'tests/files/sym/issue_138.sym'

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            str(cm.exception),
            'The signal M length 0 is not greater than 0 in message Status.')

    def test_multiple_senders(self):
        filename = 'tests/files/dbc/multiple_senders.dbc'
        db = cantools.database.load_file(filename)
        message = db.get_message_by_frame_id(1)
        self.assertEqual(message.senders, ['FOO', 'BAR', 'FIE'])
        self.assert_dbc_dump(db, filename)

    def test_issue_168_upper_case_file_extension(self):
        filename = 'tests/files/dbc/issue_168.DBC'
        db = cantools.db.load_file(filename)

        message = db.get_message_by_name('Foo')
        self.assertEqual(message.name, 'Foo')

    def test_issue_163_dbc_newlines(self):
        filename_in = 'tests/files/dbc/issue_163_newline.dbc'
        filename_dump = 'issue_163_newline_dump.dbc'

        db = cantools.database.load_file(filename_in)
        cantools.database.dump_file(db, filename_dump)

        with open(filename_dump, 'rb') as fin:
            dumped_content = fin.read()

        self.assertIn(b'\r\n', dumped_content)
        self.assertFalse(re.search(br'\r[^\n]', dumped_content))
        self.assertFalse(re.search(br'[^\r]\n', dumped_content))
        os.remove(filename_dump)

    def test_issue_167_long_names_from_scratch(self):
        """Test dbc export with mixed short and long symbol names. Create the
        database by code, i.e. start with an empty database object, add
        nodes, messages and signals, dump that to dbc format. Reload
        the dumped data and check that it matches the original data.

        """

        can = cantools.database.can
        db = cantools.database.Database(
            messages=[
                can.message.Message(
                    frame_id=1,
                    name='MSG456789_123456789_123456789_ABC',
                    length=8,
                    signals=[
                        can.signal.Signal(name='SIG456789_123456789_123456789_ABC',
                                          start=9,
                                          length=8)
                    ],
                    senders=['NODE56789_abcdefghi_ABCDEFGHI_XYZ']),
                can.message.Message(
                    frame_id=2,
                    name='MSG_short',
                    length=8,
                    signals=[
                        can.signal.Signal(name='SIG_short', start=1, length=8)
                    ],
                    senders=['NODE_short'])
            ],
            nodes=[
                can.node.Node('NODE_short', None),
                can.node.Node('NODE56789_abcdefghi_ABCDEFGHI_XYZ', None)
            ],
            version='')

        db.refresh()
        db = cantools.database.load_string(db.as_dbc_string())

        self.assertEqual(len(db.nodes), 2)
        self.assertEqual(db.nodes[0].name, 'NODE_short')
        self.assertEqual(db.nodes[1].name, 'NODE56789_abcdefghi_ABCDEFGHI_XYZ')

        self.assertEqual(len(db.messages), 2)

        message = db.messages[0]
        self.assertEqual(message.name, 'MSG456789_123456789_123456789_ABC')
        self.assertEqual(message.senders, ['NODE56789_abcdefghi_ABCDEFGHI_XYZ'])
        self.assertEqual(message.signals[0].name, 'SIG456789_123456789_123456789_ABC')

        message = db.messages[1]
        self.assertEqual(message.name, 'MSG_short')
        self.assertEqual(message.senders, ['NODE_short'])
        self.assertEqual(message.signals[0].name, 'SIG_short')

    def test_long_names_multiple_relations(self):
        """Test if long names are resolved correctly.

        """

        filename = 'tests/files/dbc/long_names_multiple_relations.dbc'
        filename_dumped = 'tests/files/dbc/long_names_multiple_relations_dumped.dbc'
        db = cantools.database.load_file(filename)
        self.assert_dbc_dump(db, filename_dumped)

    def test_database_version(self):
        # default value if db created from scratch (map None to ''):
        db = cantools.database.Database()
        self.assertIsNone(db.version)
        self.assertTrue(db.as_dbc_string().startswith('VERSION ""'))

        # write access to version attribute
        my_version = "my_version"
        db.version = my_version
        self.assertTrue(db.as_dbc_string().startswith('VERSION "{}"'.
                                                      format(my_version)))

    def test_dbc_modify_names(self):
        """Test that modified object names are dumped correctly to dbc.
        (Names with original length >32 and new length <= 32 chars)

        """

        filename_src = 'tests/files/dbc/mod_name_len_src.dbc'
        filename_dest = 'tests/files/dbc/mod_name_len_dest.dbc'
        db = cantools.database.load_file(filename_src)

        # Now change the names (to <= 32 chars), dump and readback again:
        db.messages[0].name = 'msg_now_short'
        db.messages[0].signals[0].name = 'sig_now_short'
        db.messages[0].senders[0] = 'node_now_short'
        db.nodes[0].name = 'node_now_short'
        db.refresh()

        self.assert_dbc_dump(db, filename_dest)

    def test_dbc_with_signal_groups(self):
        """Test that signal groups can be loaded and dumped.

        """

        # Read and dump.
        filename = 'tests/files/dbc/sig_groups.dbc'
        db = cantools.database.load_file(filename)

        message = db.get_message_by_name('SGMsg_m')
        self.assertEqual(len(message.signal_groups), 2)

        signal_group = message.signal_groups[0]
        self.assertEqual(signal_group.name, 'Sub2')
        self.assertEqual(signal_group.repetitions, 1)
        self.assertEqual(signal_group.signal_names, ['dupsig', 'subSig2_1'])

        signal_group = message.signal_groups[1]
        self.assertEqual(signal_group.name, 'sub1')
        self.assertEqual(signal_group.repetitions, 1)
        self.assertEqual(signal_group.signal_names,
                         ['dupsig', 'subSig1_2', 'subSig1_1'])

        self.assert_dbc_dump(db, filename)

        # Delete all signal groups.
        for message in db.messages:
            message.signal_groups = None

        filename = 'tests/files/dbc/sig_groups_del.dbc'
        self.assert_dbc_dump(db, filename)

        # Add one signal group to each message with all its signals.
        for message in db.messages[:-1]:
            message.signal_groups = [
                cantools.database.can.signal_group.SignalGroup(
                    message.name,
                    signal_names=[signal.name for signal in message.signals])
            ]

        # Create the last signal group with default values and update
        # it by changing its attributes.
        message = db.messages[-1]
        signal_group = cantools.database.can.signal_group.SignalGroup(
            'New_Signal_Group')
        signal_group.name = message.name
        signal_group.repetitions = 1
        signal_group.signal_names = [signal.name for signal in message.signals]
        message.signal_groups = [signal_group]

        filename = 'tests/files/dbc/sig_groups_out.dbc'
        self.assert_dbc_dump(db, filename)

    def test_dbc_issue_228_empty_signal_groups(self):
        filename = 'tests/files/dbc/issue_228.dbc'

        db = cantools.database.load_file(filename)
        message = db.get_message_by_name('SGMsg')
        signal_group = message.signal_groups[0]
        self.assertEqual(signal_group.name, 'Empty_Signal_Group')
        self.assertEqual(signal_group.repetitions, 1)
        self.assertEqual(signal_group.signal_names, [])

    def test_dbc_issue_199_more_than_11_bits_standard_frame_id(self):
        filename = 'tests/files/dbc/issue_199.dbc'

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            str(cm.exception),
            'Standard frame id 0x10630000 is more than 11 bits in message '
            'DriverDoorStatus.')

    def test_dbc_issue_199_more_than_29_bits_extended_frame_id(self):
        filename = 'tests/files/dbc/issue_199_extended.dbc'

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            str(cm.exception),
            'Extended frame id 0x7fffffff is more than 29 bits in message '
            'DriverDoorStatus.')

    def test_issue_207_tolerate_plus_in_dbc_sig_def(self):
        db = cantools.database.load_file('tests/files/dbc/issue_207_sig_plus.dbc')
        message = db.messages[0]

        for signal in message.signals:
            self.assertEqual(signal.minimum, -128)
            self.assertEqual(signal.maximum, 127)

    def test_issue_184_multiple_mux_values(self):
        db = cantools.database.load_file(
            'tests/files/dbc/issue_184_extended_mux_multiple_values.dbc')

        self.assertEqual(db.messages[0].signal_tree,
                         [
                             {
                                 'MUX': {
                                     0: ['muxed_0_3_4_5'],
                                     1: ['muxed_1'],
                                     2: ['muxed_2'],
                                     3: ['muxed_0_3_4_5'],
                                     4: ['muxed_0_3_4_5'],
                                     5: ['muxed_0_3_4_5']
                                 }
                             }
        ])

        self.assert_dbc_dump(
            db,
            'tests/files/dbc/issue_184_extended_mux_multiple_values_dumped.dbc')

    def test_dbc_BU_BO_REL(self):
        # Loading the file should not generate an exception
        db = cantools.database.load_file(
            'tests/files/dbc/BU_BO_REL_.dbc')

    def test_issue_184_independent_multiplexors(self):
        db = cantools.database.load_file(
            'tests/files/dbc/issue_184_extended_mux_independent_multiplexors.dbc')

        self.assertEqual(db.messages[0].signal_tree,
                         [
                             {
                                 'MUX_A': {
                                     0: ['muxed_A_0'],
                                     1: ['muxed_A_1']
                                 }
                             },
                             {
                                 'MUX_B': {
                                     1: ['muxed_B_1'],
                                     2: ['muxed_B_2']
                                 }
                             }
        ])

        self.assert_dbc_dump(
            db,
            'tests/files/dbc/issue_184_extended_mux_independent_multiplexors_dumped.dbc')

    def test_issue_184_cascaded_multiplexors(self):
        db = cantools.database.load_file(
            'tests/files/dbc/issue_184_extended_mux_cascaded.dbc')

        self.assertEqual(db.messages[0].signal_tree,
                         [
                             {
                                 'MUX_A': {
                                     1: ['muxed_A_1'],
                                     2: [
                                         {
                                             'muxed_A_2_MUX_B': {
                                                 0: ['muxed_B_0'],
                                                 1: ['muxed_B_1']
                                             }
                                         }
                                     ]
                                 }
                             }
        ])

        self.assert_dbc_dump(
            db,
            'tests/files/dbc/issue_184_extended_mux_cascaded_dumped.dbc')

    def test_bus_comment(self):
        filename = 'tests/files/dbc/bus_comment.dbc'
        db = cantools.database.load_file(filename)
        self.assertEqual(db.buses[0].comment, 'SpecialRelease')
        self.assert_dbc_dump(db, filename)

    def test_relation_attributes(self):
        filename = 'tests/files/dbc/attributes_relation.dbc'
        db = cantools.database.load_file(filename)
        for key, frame in db.dbc.attributes_rel.items():
            signal = frame.get("signal")
            if "signal_1" in signal.keys():
                rel_attributes = signal["signal_1"]["node"]["ECU2"]
                timeout_attr = rel_attributes["SigTimeoutTime"]
                self.assertEqual(timeout_attr.value, 6000)
                break
        self.assert_dbc_dump(db, filename)

    def test_relation_message_attributes(self):
        filename = 'tests/files/dbc/BU_BO_REL_Message.dbc'
        db = cantools.database.load_file(filename)
        for key, frame in db.dbc.attributes_rel.items():
            node = frame.get("node")
            rel_attributes = node["ECU1"]
            msg_attr = rel_attributes["MsgProject"]
            self.assertEqual(msg_attr.value, 2)
            break
        self.assert_dbc_dump(db, filename)

    def test_cache_prune_choices(self):
        filename = 'tests/files/dbc/socialledge.dbc'
        db = cantools.database.load_file(filename, prune_choices=False, cache_dir=self.cache_dir)
        msg = db.get_message_by_name('DRIVER_HEARTBEAT')
        sig = msg.signals[0]

        self.assertEqual(sig.choices[0], 'DRIVER_HEARTBEAT_cmd_NOOP')
        self.assertEqual(sig.choices[1], 'DRIVER_HEARTBEAT_cmd_SYNC')
        self.assertEqual(sig.choices[2], 'DRIVER_HEARTBEAT_cmd_REBOOT')


    def test_sort_signals_by_name(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        sort_signals = lambda signals: list(sorted(signals, key=lambda sig: sig.name))
        db = cantools.database.load_file(filename, sort_signals=sort_signals)
        msg = db.get_message_by_name('RT_DL1MK3_GPS_Speed')

        expected_signal_names = [
            'Accuracy_GPS_Speed',
            'GPS_Speed_2D',
            'GPS_Speed_3D',
            'Validity_GPS_Speed_2D',
            'Validity_GPS_Speed_3D',
        ]

        actual_signal_names = [sig.name for sig in msg.signals]

        self.assertEqual(actual_signal_names, expected_signal_names)

    def test_dont_sort_signals(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        db = cantools.database.load_file(filename, sort_signals=None)
        msg = db.get_message_by_name('RT_DL1MK3_GPS_Speed')

        expected_signal_names = [
            'GPS_Speed_3D',
            'GPS_Speed_2D',
            'Accuracy_GPS_Speed',
            'Validity_GPS_Speed_3D',
            'Validity_GPS_Speed_2D',
        ]

        actual_signal_names = [sig.name for sig in msg.signals]

        self.assertEqual(actual_signal_names, expected_signal_names)

    def test_default_sort_signals(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        db = cantools.database.load_file(filename)
        msg = db.get_message_by_name('RT_DL1MK3_GPS_Speed')

        expected_signal_names = [
            'Validity_GPS_Speed_2D',
            'Validity_GPS_Speed_3D',
            'Accuracy_GPS_Speed',
            'GPS_Speed_2D',
            'GPS_Speed_3D',
        ]

        actual_signal_names = [sig.name for sig in msg.signals]

        self.assertEqual(actual_signal_names, expected_signal_names)

    def test_dbc_sort_attribute_signals(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        db = cantools.database.load_file(filename)
        msg = db.get_message_by_name('RT_SB_INS_Attitude')

        def get_value_defs(msg, db_dump: str):
            # eg: VAL_ 2304273698 Validity_Roll 1 "Valid" 0 "Invalid" ;
            # extract the Validity_Roll part
            frame_id = msg.frame_id
            if msg.is_extended_frame:
                frame_id |= 0x80000000
            prefix = f'VAL_ {frame_id}'
            defs = filter(lambda x: x.startswith(prefix), db_dump.split('\r\n'))

            return [line.split(' ')[2] for line in defs]

        expected_order = [
            'Validity_Roll',
            'Validity_Pitch',
            'Validity_Yaw',
        ]

        actual_order = get_value_defs(msg, db.as_dbc_string())
        self.assertEqual(actual_order, expected_order)

        expected_order = [
            'Validity_Pitch',
            'Validity_Roll',
            'Validity_Yaw',
        ]

        dump = db.as_dbc_string(sort_attribute_signals=sort_signals_by_name)
        actual_order = get_value_defs(msg, dump)
        self.assertEqual(actual_order, expected_order)

    def test_dbc_sort_attributes(self):
        filename = 'tests/files/dbc/attributes.dbc'
        db = cantools.database.load_file(filename)

        def attributes_from_dump(dump: str):
            dump = dump.split('\r\n')
            return [line.split(' ')[1].strip('"') for line in dump if line.startswith('BA_ ')]

        expected_order = [
            'TheNetworkAttribute',
            'BusType',
            'TheNodeAttribute',
            'TheFloatAttribute',
            'TheHexAttribute',
            'GenMsgSendType',
            'GenMsgCycleTime',
            'TheSignalStringAttribute',
            'GenSigSendType',
            'GenSigSendType',
        ]

        dump = db.as_dbc_string()
        actual_order = attributes_from_dump(dump)

        self.assertEqual(actual_order, expected_order)

        expected_order = [
            'BusType',
            'GenMsgCycleTime',
            'GenMsgSendType',
            'GenSigSendType',
            'GenSigSendType',
            'TheFloatAttribute',
            'TheHexAttribute',
            'TheNetworkAttribute',
            'TheNodeAttribute',
            'TheSignalStringAttribute',
        ]

        def sort_attributes_by_name(attributes):
            return sorted(attributes, key = lambda x: x[1].name)

        dump = db.as_dbc_string(sort_attributes=sort_attributes_by_name)
        actual_order = attributes_from_dump(dump)

        self.assertEqual(actual_order, expected_order)

    def test_dbc_sort_choices(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        db = cantools.database.load_file(filename)
        msg = db.get_message_by_name('RT_SB_GPS_Status')
        sig = msg.get_signal_by_name('RTK_Status')

        expected_value_order = [4, 3, 2, 1, 0]
        actual_value_order = list(sig.choices.keys())

        self.assertEqual(actual_value_order, expected_value_order)

        db = cantools.database.load_string(db.as_dbc_string(
            sort_choices=sort_choices_by_value
        ), 'dbc')
        msg = db.get_message_by_name('RT_SB_GPS_Status')
        sig = msg.get_signal_by_name('RTK_Status')

        expected_value_order = [0, 1, 2, 3, 4]
        actual_value_order = list(sig.choices.keys())

        self.assertEqual(actual_value_order, expected_value_order)

    def test_dbc_shorten_long_names(self):
        filename = 'tests/files/dbc/long_names.dbc'
        db = cantools.database.load_file(filename)

        normal_output = db.as_dbc_string()

        self.assertIn('BA_ "SystemSignalLongSymbol"', normal_output)

        long_output = db.as_dbc_string(shorten_long_names=False)

        self.assertNotIn('BA_ "SystemSignalLongSymbol"', long_output)


# This file is not '__main__' when executed via 'python setup.py3
# test'.
logging.basicConfig(level=logging.WARNING)

if __name__ == '__main__':
    unittest.main()
