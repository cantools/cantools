# -*- coding: utf-8 -*-

import sys
import math
import unittest
from decimal import Decimal
from collections import namedtuple
import textparser
import os
import re

try:
    from unittest.mock import patch
except ImportError:
    from mock import patch

import logging
from xml.etree import ElementTree
import timeit

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

import cantools
from cantools.database.can.formats import dbc
from cantools.database import UnsupportedDatabaseFormatError


BASE_PATH = os.path.dirname(__file__)


class CanToolsDatabaseTest(unittest.TestCase):

    maxDiff = None

    def assert_dbc_dump(self, db, filename):
        actual = db.as_dbc_string()
        # open(filename, 'wb').write(actual.encode('cp1252'))

        with open(filename, 'rb') as fin:
            expected = fin.read().decode('cp1252')

        self.assertEqual(expected, actual)

    def test_vehicle(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/vehicle.dbc')
        db = cantools.database.load_file(filename)
        self.assertEqual(1, len(db.nodes))
        self.assertEqual('UnusedNode', db.nodes[0].name)
        self.assertEqual(217, len(db.messages))
        self.assertEqual(None, db.messages[216].protocol)
        self.assertEqual('RT_SB_Gyro_Rates', db.messages[216].name)
        self.assertEqual(155872546, db.messages[216].frame_id)
        self.assertEqual([], db.messages[216].senders)
        self.assertEqual("message('RT_SB_INS_Vel_Body_Axes', 0x9588322, True, 8, None)", str(db.messages[0]))
        self.assertEqual(
                         "signal('Validity_INS_Vel_Forwards', 0, 1, 'little_endian', "
                         "False, 0, 1, 0, 0, 1, 'None', False, None, None, None, {None: 'Valid when "
                         "bit is set, invalid when bit is clear.'})",
                         repr(db.messages[0].signals[0]))
        self.assertEqual(0, db.messages[0].signals[0].initial)
        self.assertEqual([], db.messages[0].signals[0].receivers)
        self.assertEqual(None, db.messages[0].cycle_time)
        self.assertEqual(None, db.messages[0].send_type)
        self.assertEqual("node('UnusedNode', None)", repr(db.nodes[0]))
        i = 0

        for message in db.messages:
            for signal in message.signals:
                if signal.choices is not None:
                    i += 1

        self.assertEqual(15, i)
        self.assert_dbc_dump(db, filename)

    def test_dbc_signal_initial_value(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/vehicle.dbc')
        db = cantools.database.load_file(filename)

        message = db.get_message_by_name('RT_IMU06_Accel')

        signal = message.get_signal_by_name('Validity_Accel_Longitudinal')
        self.assertEqual(0, signal.initial)
        self.assertEqual(
            "signal('Validity_Accel_Longitudinal', 0, 1, 'little_endian', False, "
            "0, 1, 0, None, None, 'None', False, None, None, None, {None: 'Valid when bit is "
            "set, invalid when bit is clear.'})",
            repr(signal)
        )

        signal = message.get_signal_by_name('Validity_Accel_Lateral')
        self.assertEqual(1, signal.initial )

        signal = message.get_signal_by_name('Validity_Accel_Vertical')
        self.assertEqual(0, signal.initial)

        signal = message.get_signal_by_name('Accuracy_Accel')
        self.assertEqual(127, signal.initial )

        signal = message.get_signal_by_name('Accel_Longitudinal')
        self.assertEqual(32767, signal.initial )
        self.assertEqual(
            repr(signal),
            "signal('Accel_Longitudinal', 16, 16, 'little_endian', True, 32767, "
            "0.001, 0, -65, 65, 'g', False, None, None, None, {None: 'Longitudinal "
            "acceleration.  This is positive when the vehicle accelerates in a "
            "forwards direction.'})")

        signal = message.get_signal_by_name('Accel_Lateral')
        self.assertEqual(-30000, signal.initial )

        signal = message.get_signal_by_name('Accel_Vertical')
        self.assertEqual(16120, signal.initial )

    def test_motohawk(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/motohawk.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        self.assertEqual([], db.buses)
        self.assertEqual(2, len(db.nodes))
        self.assertEqual('PCM1', db.nodes[0].name)
        self.assertEqual('FOO', db.nodes[1].name)
        self.assertEqual(1, len(db.messages))
        self.assertEqual(None, db.messages[0].bus_name)
        self.assertEqual(2, len(db.messages[0].signals[2].receivers))
        self.assertEqual('PCM1', db.messages[0].signals[2].receivers[0])
        self.assertEqual('FOO', db.messages[0].signals[2].receivers[1])
        self.assertEqual([], db.messages[0].signals[1].receivers)

        self.assert_dbc_dump(db, filename)

    def test_emc32(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/emc32.dbc')

        with open(filename, 'r') as fin:
            db.add_dbc(fin)

        self.assertEqual(1, len(db.nodes))
        self.assertEqual('EMV_Statusmeldungen', db.nodes[0].name)
        self.assertEqual(1, len(db.messages))
        self.assertEqual('EMV_Aktion_Status_3', db.messages[0].signals[0].name)
        self.assertEqual(1, len(db.messages[0].signals[0].receivers))
        self.assertEqual('EMV_Aktion_Status_4', db.messages[0].signals[1].name)
        self.assertEqual(0, len(db.messages[0].signals[1].receivers))

        self.assertEqual(17, len(db.dbc.environment_variables))
        env_var_1 = db.dbc.environment_variables['EMC_Azimuth']
        self.assertEqual('EMC_Azimuth', env_var_1.name)
        self.assertEqual(1, env_var_1.env_type)
        self.assertEqual(-180, env_var_1.minimum)
        self.assertEqual(400, env_var_1.maximum)
        self.assertEqual('deg', env_var_1.unit)
        self.assertEqual(0, env_var_1.initial_value)
        self.assertEqual(12, env_var_1.env_id)
        self.assertEqual('DUMMY_NODE_VECTOR0', env_var_1.access_type)
        self.assertEqual('Vector__XXX', env_var_1.access_node)
        self.assertEqual('Elevation Head', env_var_1.comment)
        self.assertEqual(
            "environment_variable('EMC_Azimuth', 1, -180, 400, 'deg', 0, 12,"
            " 'DUMMY_NODE_VECTOR0', 'Vector__XXX', 'Elevation Head')",
            repr(env_var_1))

        env_var_2 = db.dbc.environment_variables['EMC_TrdPower']
        self.assertEqual('EMC_TrdPower', env_var_2.name)
        self.assertEqual(None, env_var_2.comment)
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
        self.assertEqual(1, env_var_2.env_type)
        self.assertEqual(-180, env_var_2.minimum)
        self.assertEqual(400, env_var_2.maximum)
        self.assertEqual('deg', env_var_2.unit)
        self.assertEqual(0, env_var_2.initial_value)
        self.assertEqual(12, env_var_2.env_id)
        self.assertEqual('DUMMY_NODE_VECTOR0', env_var_2.access_type)
        self.assertEqual('Vector__XXX', env_var_2.access_node)
        self.assertEqual('Elevation Head', env_var_2.comment)
        self.assertEqual(
            "environment_variable('EMC_TrdPower', 1, -180, 400, 'deg', 0, 12,"
            " 'DUMMY_NODE_VECTOR0', 'Vector__XXX', 'Elevation Head')",
            repr(env_var_2))


    def test_foobar(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/foobar.dbc')

        db.add_dbc_file(filename)

        self.assertEqual(4, len(db.nodes))
        self.assertEqual('2.0', db.version)
        self.assertEqual(
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
            "'None', False, None, None, None, None)\n",
            repr(db))

        self.assertEqual(1, len(db.buses))
        self.assertEqual('TheBusName', db.buses[0].name)
        self.assertEqual(None, db.buses[0].comment)
        self.assertEqual(125000, db.buses[0].baudrate)

        message = db.get_message_by_frame_id(0x12331)
        self.assertEqual('Fum', message.name)
        self.assertEqual('TheBusName', message.bus_name)
        self.assertEqual(['FOO'], message.senders)
        self.assertEqual(False, message.signals[0].is_float)

        message = db.get_message_by_frame_id(0x12332)
        self.assertEqual('Bar', message.name)
        self.assertEqual('TheBusName', message.bus_name)
        self.assertEqual(['FOO', 'BAR'], message.senders)
        self.assertEqual(['FUM'], message.signals[0].receivers)
        self.assertEqual(True, message.signals[0].is_float)
        self.assertEqual(32, message.signals[0].length)

        message = db.get_message_by_frame_id(0x12333)
        self.assertEqual('CanFd', message.name)
        self.assertEqual('TheBusName', message.bus_name)
        self.assertEqual(['FOO'], message.senders)
        self.assertEqual(['FUM'], message.signals[0].receivers)
        self.assertEqual(False, message.signals[0].is_float)
        self.assertEqual(64, message.signals[0].length)

    def test_foobar_encode_decode(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/foobar.dbc')

        db.add_dbc_file(filename)

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
            self.assertEqual(encoded_message, encoded)
            decoded = db.decode_message(name, encoded)
            self.assertEqual(decoded_message, decoded)

    def test_foobar_encode_decode_frame_ids(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/foobar.dbc')

        db.add_dbc_file(filename)

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
            self.assertEqual(encoded_message, encoded)
            decoded = db.decode_message(frame_id, encoded)
            self.assertEqual(decoded_message, decoded)

    def test_foobar_decode_masked_frame_id(self):
        db = cantools.db.Database(frame_id_mask=0xff)
        filename = os.path.join(BASE_PATH, 'files/dbc/foobar.dbc')

        db.add_dbc_file(filename)

        frame_ids = [
            0x12331,
            0xfff31,
            0x00031
        ]

        for frame_id in frame_ids:
            db.get_message_by_frame_id(frame_id)

    def test_dbc_dump_val_table(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/val_table.dbc')
        db = cantools.database.load_file(filename)

        self.assertEqual(
            {
                'Table3': {16: '16', 0: '0', 2: '2', 7: '7'},
                'Table2': {},
                'Table1': {0: 'Zero', 1: 'One'}
            },
            db.dbc.value_tables)
        self.assert_dbc_dump(db, filename)

    def test_dbc_load_empty_choice(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/empty_choice.dbc')
        db = cantools.database.load_file(filename)

        message = db.get_message_by_frame_id(10)
        self.assertEqual('non_empty_choice', message.signals[0].name)
        self.assertEqual({254: 'Error', 255: 'Not available'}, message.signals[0].choices)
        self.assertEqual('empty_choice', message.signals[1].name)
        self.assertEqual(None, message.signals[1].choices)
        self.assertEqual('no_choice', message.signals[2].name)
        self.assertEqual(None, message.signals[2].choices)

    def test_padding_bit_order(self):
        """Encode and decode signals with reversed bit order.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/padding_bit_order.dbc')

        db.add_dbc_file(filename)

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
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(msg0_frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

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
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(msg1_frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

        # Message 2.
        msg2_frame_id = 3

        decoded_message = {
            'I': 1,  # should set byte[0]bit[3:0]=1
            'J': 2,  # should set byte[0]bit[7:4]=2
            'K': 3   # should set byte[1]bit[3:0]=3
        }
        encoded_message = b'\x21\x03\x00\x00\x00\x00\x00\x00'

        encoded = db.encode_message(msg2_frame_id, decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(msg2_frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

        # Message 3.
        msg3_frame_id = 4

        decoded_message = {
            'L': 0x0123456789abcdef
        }
        encoded_message = b'\x01\x23\x45\x67\x89\xab\xcd\xef'

        encoded = db.encode_message(msg3_frame_id, decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(msg3_frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

        # Message 4.
        msg4_frame_id = 5

        decoded_message = {
            'M': 0x0123456789abcdef
        }
        encoded_message = b'\xef\xcd\xab\x89\x67\x45\x23\x01'

        encoded = db.encode_message(msg4_frame_id, decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(msg4_frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

    def test_motohawk_encode_decode(self):
        """Encode and decode the signals in a ExampleMessage frame.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/motohawk.dbc')

        db.add_dbc_file(filename)

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
            self.assertEqual(encoded_message, encoded)
            decoded = db.decode_message(example_message_frame_id,
                                        encoded,
                                        decode_choices=False)
            self.assertEqual(decoded_message, decoded)

        # Encode with enumerated values.
        decoded_message = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 'Enabled'
        }
        encoded_message = b'\xc0\x06\xe0\x00\x00\x00\x00\x00'

        # By frame id.
        encoded = db.encode_message(example_message_frame_id, decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(example_message_frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

        # By name.
        encoded = db.encode_message(example_message_name, decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(example_message_name, encoded)
        self.assertEqual(decoded_message, decoded)

    def test_big_endian_no_decode_choices(self):
        """Decode a big endian signal with `decode_choices` set to False.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/motohawk.dbc')

        db.add_dbc_file(filename)

        decoded_message = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 1
        }
        encoded_message = b'\xc0\x06\xe0\x00\x00\x00\x00\x00'

        decoded = db.decode_message(496,
                                    encoded_message,
                                    decode_choices=False)
        self.assertEqual(decoded_message, decoded)

    def test_little_endian_no_decode_choices(self):
        """Decode a little endian signal with `decode_choices` set to False.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/socialledge.dbc')

        db.add_dbc_file(filename)

        decoded_message = {
            'DRIVER_HEARTBEAT_cmd': 1
        }
        encoded_message = b'\x01\x00\x00\x00\x00\x00\x00\x00'

        decoded = db.decode_message(100,
                                    encoded_message,
                                    decode_choices=False)
        self.assertEqual(decoded_message, decoded)

        decoded_message = {
            'DRIVER_HEARTBEAT_cmd': 'DRIVER_HEARTBEAT_cmd_SYNC'
        }

        decoded = db.decode_message(100,
                                    encoded_message)
        self.assertEqual(decoded_message, decoded)

    def test_encode_decode_no_scaling(self):
        """Encode and decode a message without scaling the signal values.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/motohawk.dbc')

        db.add_dbc_file(filename)

        decoded_message = {
            'Temperature': 55,
            'AverageRadius': 32,
            'Enable': 'Enabled'
        }
        encoded_message = b'\xc0\x06\xe0\x00\x00\x00\x00\x00'

        encoded = db.encode_message(496,
                                    decoded_message,
                                    scaling=False)
        self.assertEqual(encoded_message, encoded)

        decoded = db.decode_message(496,
                                    encoded,
                                    scaling=False)
        self.assertEqual(decoded_message, decoded)

    def test_encode_signal_strict(self):
        """Test signal out of range errors.

        """

        filename = os.path.join(BASE_PATH, 'files/kcd/signal_range.kcd')
        db = cantools.database.load_file(filename)

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
                "Expected signal 'Signal1' value greater than or equal to 1 in "
                "message 'Message1', but got 0."
            ),
            (
                'Message1',
                3,
                "Expected signal 'Signal1' value less than or equal to 2 in "
                "message 'Message1', but got 3."
            ),
            (
                'Message2',
                0,
                "Expected signal 'Signal1' value greater than or equal to 1 in "
                "message 'Message2', but got 0."
            ),
            (
                'Message3',
                3,
                "Expected signal 'Signal1' value less than or equal to 2 in "
                "message 'Message3', but got 3."
            ),
            (
                'Message4',
                1.9,
                "Expected signal 'Signal1' value greater than or equal to 2 in "
                "message 'Message4', but got 1.9."
            ),
            (
                'Message4',
                8.1,
                "Expected signal 'Signal1' value less than or equal to 8 in "
                "message 'Message4', but got 8.1."
            )
        ]

        for message_name, signal_value, error_message in datas:
            with self.assertRaises(cantools.database.EncodeError) as cm:
                db.encode_message(message_name, {'Signal1': signal_value})

            self.assertEqual(error_message, str(cm.exception))

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
            "Expected signal value for 'Signal1' in data, but got {'Foo': 1}.",
            str(cm.exception))

        # Missing value, but checks disabled.
        with self.assertRaises(KeyError):
            db.encode_message('Message1', {'Foo': 1}, strict=False)

    def test_encode_decode_no_scaling_no_decode_choices(self):
        """Encode and decode a message without scaling the signal values, not
        decoding choices.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/motohawk.dbc')

        db.add_dbc_file(filename)

        decoded_message = {
            'Temperature': 3,
            'AverageRadius': 2,
            'Enable': 1
        }
        encoded_message = b'\x84\x00\x60\x00\x00\x00\x00\x00'

        encoded = db.encode_message(496,
                                    decoded_message,
                                    scaling=False)
        self.assertEqual(encoded_message, encoded)

        decoded = db.decode_message(496,
                                    encoded,
                                    decode_choices=False,
                                    scaling=False)
        self.assertEqual(decoded_message, decoded)

    def test_socialledge(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/socialledge.dbc')

        db.add_dbc_file(filename)

        # Verify nodes.
        self.assertEqual(5, len(db.nodes))
        self.assertEqual('DBG', db.nodes[0].name)
        self.assertEqual(None, db.nodes[0].comment)
        self.assertEqual('DRIVER', db.nodes[1].name)
        self.assertEqual('// The driver controller driving the car //', db.nodes[1].comment)
        self.assertEqual('IO', db.nodes[2].name)
        self.assertEqual(None, db.nodes[2].comment)
        self.assertEqual('MOTOR', db.nodes[3].name)
        self.assertEqual('The motor controller of the car', db.nodes[3].comment)
        self.assertEqual('SENSOR', db.nodes[4].name)
        self.assertEqual('The sensor controller of the car', db.nodes[4].comment)

        # Verify messages and their signals.
        self.assertEqual(5, len(db.messages))
        self.assertEqual('DRIVER_HEARTBEAT', db.messages[0].name)
        self.assertEqual('Sync message used to synchronize the controllers', db.messages[0].comment)
        self.assertEqual('DRIVER_HEARTBEAT_cmd_NOOP', db.messages[0].signals[0].choices[0])
        self.assertEqual('DRIVER_HEARTBEAT_cmd_SYNC', db.messages[0].signals[0].choices[1])
        self.assertEqual('DRIVER_HEARTBEAT_cmd_REBOOT', db.messages[0].signals[0].choices[2])
        self.assertEqual('IO_DEBUG', db.messages[1].name)
        self.assertEqual('MOTOR_CMD', db.messages[2].name)
        self.assertEqual('MOTOR_STATUS', db.messages[3].name)
        self.assertEqual('SENSOR_SONARS', db.messages[4].name)

        sensor_sonars = db.messages[-1]

        self.assertFalse(db.messages[0].is_multiplexed())
        self.assertTrue(sensor_sonars.is_multiplexed())
        self.assertEqual('SENSOR_SONARS_no_filt_rear', sensor_sonars.signals[-1].name)
        self.assertEqual([1], sensor_sonars.signals[-1].multiplexer_ids)
        self.assertEqual('SENSOR_SONARS_left', sensor_sonars.signals[2].name)
        self.assertEqual([0], sensor_sonars.signals[2].multiplexer_ids)
        self.assertEqual('SENSOR_SONARS_mux', sensor_sonars.signals[0].name)
        self.assertEqual(True, sensor_sonars.signals[0].is_multiplexer)

        self.assertEqual([
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
                         ],
                         sensor_sonars.signal_tree)

        self.assertEqual('', db.version)

    def test_socialledge_encode_decode_mux_0(self):
        """Encode and decode the signals in a SENSOR_SONARS frame with mux 0.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/socialledge.dbc')

        db.add_dbc_file(filename)

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
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

    def test_socialledge_encode_decode_mux_1(self):
        """Encode and decode the signals in a SENSOR_SONARS frame with mux 1.

        """

        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/socialledge.dbc')

        db.add_dbc_file(filename)

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
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

    def test_get_message_by_frame_id_and_name(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/motohawk.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        message = db.get_message_by_name('ExampleMessage')
        self.assertEqual('ExampleMessage', message.name)

        message = db.get_message_by_frame_id(496)
        self.assertEqual(496, message.frame_id)

    def test_get_signal_by_name(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/foobar.dbc')

        db = cantools.db.load_file(filename)

        message = db.get_message_by_name('Foo')

        signal = message.get_signal_by_name('Foo')
        self.assertEqual('Foo', signal.name)

        signal = message.get_signal_by_name('Bar')
        self.assertEqual('Bar', signal.name)

        with self.assertRaises(KeyError) as cm:
            message.get_signal_by_name('Fum')

        self.assertEqual("'Fum'", str(cm.exception))

    def test_cp1252_dbc(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/cp1252.dbc')

        db = cantools.database.load_file(filename)

        self.assertEqual(
            bytearray([
                v for v in range(256)
                if v not in [34, 129, 141, 143, 144, 157]
            ])[32:].decode('cp1252'),
            db.nodes[0].comment)

    def test_the_homer(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db = cantools.db.load_file(filename)

        self.assertEqual('1.23', db.version)
        self.assertEqual(18, len(db.nodes))
        self.assertEqual('Motor ACME', db.nodes[0].name)
        self.assertEqual('Motor alternative supplier', db.nodes[1].name)
        self.assertEqual(3, len(db.buses))
        self.assertEqual('Motor', db.buses[0].name)
        self.assertEqual('Instrumentation', db.buses[1].name)
        self.assertEqual('Comfort', db.buses[2].name)
        self.assertEqual(None, db.buses[0].comment)
        self.assertEqual(500000, db.buses[0].baudrate)
        self.assertEqual(125000, db.buses[1].baudrate)

        self.assertEqual(33, len(db.messages))
        self.assertEqual(0xa, db.messages[0].frame_id)
        self.assertEqual(False, db.messages[0].is_extended_frame)
        self.assertEqual('Airbag', db.messages[0].name)
        self.assertEqual(3, db.messages[0].length)
        self.assertEqual(['Brake ACME'], db.messages[0].senders)
        self.assertEqual(8, len(db.messages[0].signals))
        self.assertEqual(None, db.messages[0].comment)
        self.assertEqual(None, db.messages[0].send_type)
        self.assertEqual(None, db.messages[0].cycle_time)
        self.assertEqual('Motor', db.messages[0].bus_name)

        self.assertEqual(0x0B2, db.messages[1].frame_id)
        self.assertEqual('ABS', db.messages[1].name)
        self.assertEqual(100, db.messages[1].cycle_time)
        self.assertEqual(['Brake ACME', 'Brake alternative supplier'], db.messages[1].senders)

        self.assertEqual(0x400, db.messages[3].frame_id)
        self.assertEqual('Emission', db.messages[3].name)
        self.assertEqual(5, db.messages[3].length)
        self.assertEqual([], db.messages[3].senders)

        self.assertEqual('Comfort', db.messages[-1].bus_name)

        seat_configuration = db.messages[0].signals[-1]

        self.assertEqual('SeatConfiguration', seat_configuration.name)
        self.assertEqual(16, seat_configuration.start)
        self.assertEqual(8, seat_configuration.length)
        self.assertEqual([], seat_configuration.receivers)
        self.assertEqual('little_endian', seat_configuration.byte_order)
        self.assertEqual(False, seat_configuration.is_signed)
        self.assertEqual(False, seat_configuration.is_float)
        self.assertEqual(1, seat_configuration.scale)
        self.assertEqual(0, seat_configuration.offset)
        self.assertEqual(None, seat_configuration.minimum)
        self.assertEqual(None, seat_configuration.maximum)
        self.assertEqual(1, seat_configuration.decimal.scale)
        self.assertEqual(0, seat_configuration.decimal.offset)
        self.assertEqual(None, seat_configuration.decimal.minimum)
        self.assertEqual(None, seat_configuration.decimal.maximum)
        self.assertEqual(None, seat_configuration.unit)
        self.assertEqual(None, seat_configuration.choices)
        self.assertEqual(None, seat_configuration.comment)

        tank_temperature = db.messages[10].signals[1]

        self.assertEqual('TankTemperature', tank_temperature.name)
        self.assertEqual(16, tank_temperature.start)
        self.assertEqual(16, tank_temperature.length)
        self.assertEqual([], tank_temperature.receivers)
        self.assertEqual('little_endian', tank_temperature.byte_order)
        self.assertEqual(True, tank_temperature.is_signed)
        self.assertEqual(False, tank_temperature.is_float)
        self.assertEqual(1, tank_temperature.scale)
        self.assertEqual(0, tank_temperature.offset)
        self.assertEqual(None, tank_temperature.minimum)
        self.assertEqual(None, tank_temperature.maximum)
        self.assertEqual(1, tank_temperature.decimal.scale)
        self.assertEqual(0, tank_temperature.decimal.offset)
        self.assertEqual(None, tank_temperature.decimal.minimum)
        self.assertEqual(None, tank_temperature.decimal.maximum)
        self.assertEqual('Cel', tank_temperature.unit)
        self.assertEqual(None, tank_temperature.choices)
        self.assertEqual(None, tank_temperature.comment)

        info_0 = db.messages[1].signals[0]

        self.assertEqual('Info0', info_0.name)
        self.assertEqual(0, info_0.start)
        self.assertEqual(8, info_0.length)
        self.assertEqual([], info_0.receivers)
        self.assertEqual('little_endian', info_0.byte_order)
        self.assertEqual(False, info_0.is_signed)
        self.assertEqual(False, info_0.is_float)
        self.assertEqual(False, info_0.is_multiplexer)
        self.assertEqual([0], info_0.multiplexer_ids)
        self.assertEqual('ABS_InfoMux', info_0.multiplexer_signal)

        info_2 = db.messages[1].signals[1]

        self.assertEqual('Info2', info_2.name)
        self.assertEqual(0, info_2.start)
        self.assertEqual(8, info_2.length)
        self.assertEqual([], info_2.receivers)
        self.assertEqual('little_endian', info_2.byte_order)
        self.assertEqual(False, info_2.is_signed)
        self.assertEqual(False, info_2.is_float)
        self.assertEqual(False, info_2.is_multiplexer)
        self.assertEqual([1], info_2.multiplexer_ids)
        self.assertEqual('ABS_InfoMux', info_2.multiplexer_signal)

        info_4 = db.messages[1].signals[2]

        self.assertEqual('Info4', info_4.name)
        self.assertEqual(0, info_4.start)
        self.assertEqual(8, info_4.length)
        self.assertEqual([], info_4.receivers)
        self.assertEqual('little_endian', info_4.byte_order)
        self.assertEqual(False, info_4.is_signed)
        self.assertEqual(False, info_4.is_float)
        self.assertEqual(False, info_4.is_multiplexer)
        self.assertEqual([2], info_4.multiplexer_ids)
        self.assertEqual('ABS_InfoMux', info_4.multiplexer_signal)

        info_6 = db.messages[1].signals[3]

        self.assertEqual('Info6', info_6.name)
        self.assertEqual(0, info_6.start)
        self.assertEqual(8, info_6.length)
        self.assertEqual([], info_6.receivers)
        self.assertEqual('little_endian', info_6.byte_order)
        self.assertEqual(False, info_6.is_signed)
        self.assertEqual(False, info_6.is_float)
        self.assertEqual(False, info_6.is_multiplexer)
        self.assertEqual([3], info_6.multiplexer_ids)
        self.assertEqual('ABS_InfoMux', info_6.multiplexer_signal)

        abs_info_mux = db.messages[1].signals[8]
        self.assertEqual('ABS_InfoMux', abs_info_mux.name)
        self.assertEqual(16, abs_info_mux.start)
        self.assertEqual(2, abs_info_mux.length)
        self.assertEqual([], abs_info_mux.receivers)
        self.assertEqual('little_endian', abs_info_mux.byte_order)
        self.assertEqual(False, abs_info_mux.is_signed)
        self.assertEqual(False, abs_info_mux.is_float)
        self.assertEqual(True, abs_info_mux.is_multiplexer)
        self.assertEqual(None, abs_info_mux.multiplexer_ids)
        self.assertEqual(None, abs_info_mux.multiplexer_signal)

        outside_temp = db.messages[1].signals[9]

        self.assertEqual('OutsideTemp', outside_temp.name)
        self.assertEqual(18, outside_temp.start)
        self.assertEqual(12, outside_temp.length)
        self.assertEqual(['BodyComputer'], outside_temp.receivers)
        self.assertEqual('little_endian', outside_temp.byte_order)
        self.assertEqual(False, outside_temp.is_signed)
        self.assertEqual(False, outside_temp.is_float)
        self.assertEqual(0.05, outside_temp.scale)
        self.assertEqual(-40, outside_temp.offset)
        self.assertEqual(0, outside_temp.minimum)
        self.assertEqual(100, outside_temp.maximum)
        self.assertEqual(Decimal('0.05'), outside_temp.decimal.scale)
        self.assertEqual(-40, outside_temp.decimal.offset)
        self.assertEqual(0, outside_temp.decimal.minimum)
        self.assertEqual(100, outside_temp.decimal.maximum)
        self.assertEqual('Cel', outside_temp.unit)
        self.assertEqual({0: 'init'}, outside_temp.choices)
        self.assertEqual('Outside temperature.', outside_temp.comment)

        speed_km = db.messages[1].signals[10]

        self.assertEqual('SpeedKm', speed_km.name)
        self.assertEqual(30, speed_km.start)
        self.assertEqual(24, speed_km.length)
        self.assertEqual(['BodyComputer'], speed_km.receivers)
        self.assertEqual('little_endian', speed_km.byte_order)
        self.assertEqual(False, speed_km.is_signed)
        self.assertEqual(False, speed_km.is_float)
        self.assertEqual(0.2, speed_km.scale)
        self.assertEqual(0, speed_km.offset)
        self.assertEqual(None, speed_km.minimum)
        self.assertEqual(None, speed_km.maximum)
        self.assertEqual(Decimal('0.2'), speed_km.decimal.scale)
        self.assertEqual(0, speed_km.decimal.offset)
        self.assertEqual(None, speed_km.decimal.minimum)
        self.assertEqual(None, speed_km.decimal.maximum)
        self.assertEqual('km/h', speed_km.unit)
        self.assertEqual({16777215: 'invalid'}, speed_km.choices)
        self.assertEqual('Middle speed of front wheels in kilometers per hour.', speed_km.comment)

        self.assertEqual([
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
                         ],
                         db.messages[1].signal_tree)

        self.assertEqual(
            '-- {root}\n'
            '   +-- ABS_InfoMux\n'
            '   |   +-- 0\n'
            '   |   |   +-- Info0\n'
            '   |   |   +-- Info1\n'
            '   |   +-- 1\n'
            '   |   |   +-- Info2\n'
            '   |   |   +-- Info3\n'
            '   |   +-- 2\n'
            '   |   |   +-- Info4\n'
            '   |   |   +-- Info5\n'
            '   |   +-- 3\n'
            '   |       +-- Info6\n'
            '   |       +-- Info7\n'
            '   +-- OutsideTemp\n'
            '   +-- SpeedKm\n'
            '   +-- Handbrake',
            db.messages[1].signal_tree_string())

        ambient_lux = db.messages[24].signals[0]

        self.assertEqual('AmbientLux', ambient_lux.name)
        self.assertEqual(0, ambient_lux.start)
        self.assertEqual(64, ambient_lux.length)
        self.assertEqual([], ambient_lux.receivers)
        self.assertEqual('little_endian', ambient_lux.byte_order)
        self.assertEqual(False, ambient_lux.is_signed)
        self.assertEqual(True, ambient_lux.is_float)
        self.assertEqual(1, ambient_lux.scale)
        self.assertEqual(0, ambient_lux.offset)
        self.assertEqual(None, ambient_lux.minimum)
        self.assertEqual(None, ambient_lux.maximum)
        self.assertEqual(1, ambient_lux.decimal.scale)
        self.assertEqual(0, ambient_lux.decimal.offset)
        self.assertEqual(None, ambient_lux.decimal.minimum)
        self.assertEqual(None, ambient_lux.decimal.maximum)
        self.assertEqual('Lux', ambient_lux.unit)
        self.assertEqual(None, ambient_lux.choices)
        self.assertEqual(None, ambient_lux.comment)

        windshield_humidity = db.messages[25].signals[0]

        self.assertEqual('Windshield', windshield_humidity.name)
        self.assertEqual(0, windshield_humidity.start)
        self.assertEqual(32, windshield_humidity.length)
        self.assertEqual([], windshield_humidity.receivers)
        self.assertEqual('little_endian', windshield_humidity.byte_order)
        self.assertEqual(False, windshield_humidity.is_signed)
        self.assertEqual(True, windshield_humidity.is_float)
        self.assertEqual(1, windshield_humidity.scale)
        self.assertEqual(0, windshield_humidity.offset)
        self.assertEqual(None, windshield_humidity.minimum)
        self.assertEqual(None, windshield_humidity.maximum)
        self.assertEqual(1, windshield_humidity.decimal.scale)
        self.assertEqual(0, windshield_humidity.decimal.offset)
        self.assertEqual(None, windshield_humidity.decimal.minimum)
        self.assertEqual(None, windshield_humidity.decimal.maximum)
        self.assertEqual('% RH', windshield_humidity.unit)
        self.assertEqual(None, windshield_humidity.choices)
        self.assertEqual(None, windshield_humidity.comment)

        wheel_angle = db.messages[4].signals[1]

        self.assertEqual('WheelAngle', wheel_angle.name)
        self.assertEqual(1, wheel_angle.start)
        self.assertEqual(14, wheel_angle.length)
        self.assertEqual([], wheel_angle.receivers)
        self.assertEqual('little_endian', wheel_angle.byte_order)
        self.assertEqual(False, wheel_angle.is_signed)
        self.assertEqual(False, wheel_angle.is_float)
        self.assertEqual(0.1, wheel_angle.scale)
        self.assertEqual(-800, wheel_angle.offset)
        self.assertEqual(None, wheel_angle.minimum)
        self.assertEqual(None, wheel_angle.maximum)
        self.assertEqual(Decimal('0.1'), wheel_angle.decimal.scale)
        self.assertEqual(-800, wheel_angle.decimal.offset)
        self.assertEqual(None, wheel_angle.decimal.minimum)
        self.assertEqual(None, wheel_angle.decimal.maximum)
        self.assertEqual('deg', wheel_angle.unit)
        self.assertEqual({
                             0: 'left',
                             8000: 'straight',
                             16000: 'right',
                             16382: 'init',
                             16383: 'sensor '
                        },
                        wheel_angle.choices)
        self.assertEqual(None, wheel_angle.comment)

        big_endian_a = db.get_message_by_name('BigEndian').signals[0]

        self.assertEqual('A', big_endian_a.name)
        self.assertEqual(7, big_endian_a.start)
        self.assertEqual(17, big_endian_a.length)
        self.assertEqual([], big_endian_a.receivers)
        self.assertEqual('big_endian', big_endian_a.byte_order)
        self.assertEqual(False, big_endian_a.is_signed)
        self.assertEqual(False, big_endian_a.is_float)
        self.assertEqual(1, big_endian_a.scale)
        self.assertEqual(0, big_endian_a.offset)
        self.assertEqual(None, big_endian_a.minimum)
        self.assertEqual(None, big_endian_a.maximum)
        self.assertEqual(1, big_endian_a.decimal.scale)
        self.assertEqual(0, big_endian_a.decimal.offset)
        self.assertEqual(None, big_endian_a.decimal.minimum)
        self.assertEqual(None, big_endian_a.decimal.maximum)
        self.assertEqual(None, big_endian_a.unit)
        self.assertEqual(None, big_endian_a.choices)
        self.assertEqual(None, big_endian_a.comment)

        message = db.get_message_by_name('LittleEndianAuto')
        self.assertEqual(1, message.length)

        message = db.get_message_by_name('BigEndianAuto')
        self.assertEqual(1, message.length)

        message = db.get_message_by_name('LittleBigEndianAuto')
        self.assertEqual(1, message.length)

    def test_the_homer_encode_length(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db.add_kcd_file(filename)

        frame_id = 0x400
        decoded_message = {
            'MIL': 0,
            'Enginespeed': 127,
            'NoxSensor': 127,
        }
        encoded_message = b'\xfe\x00\xfe\x00\x00'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(5, len(encoded))
        self.assertEqual(encoded_message, encoded)

    def test_the_homer_float(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db.add_kcd_file(filename)

        # Message 1 (binary64).
        frame_id = 0x732

        decoded_message = {'AmbientLux': math.pi}
        encoded_message = b'\x18\x2d\x44\x54\xfb\x21\x09\x40'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(8, len(encoded))
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

        # Message 2 (binary32).
        frame_id = 0x745

        decoded_message = {'Windshield': 3.1415927410125732}
        encoded_message = b'\xdb\x0f\x49\x40'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(4, len(encoded))
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded_message, decoded)

    def test_the_homer_encode_decode_choices(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db = cantools.db.load_file(filename)

        messages = [
            (         {'EngagedGear': 'disengaged'}, b'\x00\x00'),
            (                  {'EngagedGear': '1'}, b'\x00\x10'),
            (                  {'EngagedGear': '2'}, b'\x00\x20'),
            (                  {'EngagedGear': '3'}, b'\x00\x30'),
            (                  {'EngagedGear': '4'}, b'\x00\x40'),
            (                  {'EngagedGear': '5'}, b'\x00\x50'),
            (                  {'EngagedGear': '6'}, b'\x00\x60'),
            (                    {'EngagedGear': 7}, b'\x00\x70'),
            (                    {'EngagedGear': 8}, b'\x00\x80'),
            (                    {'EngagedGear': 9}, b'\x00\x90'),
            (            {'EngagedGear': 'reverse'}, b'\x00\xa0'),
            (   {'EngagedGear': 'Unspecific error'}, b'\x00\xf0')
        ]

        for decoded_message, encoded_message in messages:
            encoded = db.encode_message('Gear', decoded_message)
            self.assertEqual(encoded_message, encoded)
            decoded = db.decode_message('Gear', encoded)
            self.assertEqual(decoded_message, decoded)

    def test_the_homer_encode_decode_choice_scaling(self):
        """Verify a label/enum matches the raw value, and is not affected by
        scaling.

        """
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db = cantools.database.load_file(filename)

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
            self.assertEqual(encoded,encoded_message)
            decoded = db.decode_message(message_id, encoded)
            self.assertEqual(decoded_message, decoded)

    def test_the_homer_encode_decode_big_endian(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db = cantools.db.load_file(filename)

        decoded_message = {
            'A': 0x140fa,
            'B': 1,
            'C': 18
        }
        encoded_message = b'\xa0\x7d\x64'

        encoded = db.encode_message('BigEndian', decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = db.decode_message('BigEndian', encoded)
        self.assertEqual(decoded_message, decoded)

    def test_the_homer_encode_decode_signed(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db = cantools.db.load_file(filename)

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
            self.assertEqual(encoded_message, encoded)
            decoded = db.decode_message('TankController', encoded)
            self.assertEqual(decoded_message, decoded)

    def test_empty_kcd(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/empty.kcd')

        db = cantools.db.load_file(filename)

        self.assertEqual(None, db.version)
        self.assertEqual([], db.nodes)

    def test_invalid_kcd(self):
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_string('<WrongRootElement/>',
                                    database_format='kcd')

        self.assertEqual(
            'KCD: "Expected root element tag '
            '{http://kayak.2codeornot2code.org/1.0}NetworkDefinition, but '
            'got WrongRootElement."',
            str(cm.exception))

    def test_jopp_5_0_sym(self):
        db = cantools.db.Database()

        filename = os.path.join(BASE_PATH, 'files/sym/jopp-5.0.sym')

        with self.assertRaises(cantools.db.ParseError) as cm:
            db.add_sym_file(filename)

        self.assertEqual('Only SYM version 6.0 is supported.', str(cm.exception))

    def test_jopp_6_0_sym(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/sym/jopp-6.0.sym')

        db.add_sym_file(filename)

        self.assertEqual(6, len(db.messages))
        self.assertEqual(0, len(db.messages[0].signals))

        # Message1.
        message_1 = db.messages[3]
        self.assertEqual(0, message_1.frame_id)
        self.assertEqual(False, message_1.is_extended_frame)
        self.assertEqual('Message1', message_1.name)
        self.assertEqual(8, message_1.length)
        self.assertEqual([], message_1.senders)
        self.assertEqual(None, message_1.send_type)
        self.assertEqual(30, message_1.cycle_time)
        self.assertEqual(2, len(message_1.signals))
        self.assertEqual('apa', message_1.comment)
        self.assertEqual(None, message_1.bus_name)

        signal_1 = message_1.signals[0]
        self.assertEqual('Signal1', signal_1.name)
        self.assertEqual(0, signal_1.start)
        self.assertEqual(11, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('little_endian', signal_1.byte_order)
        self.assertEqual(False, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        self.assertEqual(1, signal_1.scale)
        self.assertEqual(0, signal_1.offset)
        self.assertEqual(None, signal_1.minimum)
        self.assertEqual(255, signal_1.maximum)
        self.assertEqual(1, signal_1.decimal.scale)
        self.assertEqual(0, signal_1.decimal.offset)
        self.assertEqual(None, signal_1.decimal.minimum)
        self.assertEqual(255, signal_1.decimal.maximum)
        self.assertEqual('A', signal_1.unit)
        self.assertEqual(None, signal_1.choices)
        self.assertEqual(None, signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = message_1.signals[1]
        self.assertEqual('Signal2', signal_2.name)
        self.assertEqual(32, signal_2.start)
        self.assertEqual(32, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('little_endian', signal_2.byte_order)
        self.assertEqual(False, signal_2.is_signed)
        self.assertEqual(True, signal_2.is_float)
        self.assertEqual(1, signal_2.scale)
        self.assertEqual(48, signal_2.offset)
        self.assertEqual(16, signal_2.minimum)
        self.assertEqual(130, signal_2.maximum)
        self.assertEqual(1, signal_2.decimal.scale)
        self.assertEqual(48, signal_2.decimal.offset)
        self.assertEqual(16, signal_2.decimal.minimum)
        self.assertEqual(130, signal_2.decimal.maximum)
        self.assertEqual('V', signal_2.unit)
        self.assertEqual(None, signal_2.choices)
        self.assertEqual('bbb', signal_2.comment)
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        # Message2.
        message_2 = db.messages[1]
        self.assertEqual(0x22, message_2.frame_id)
        self.assertEqual(True, message_2.is_extended_frame)
        self.assertEqual('Message2', message_2.name)
        self.assertEqual(8, message_2.length)
        self.assertEqual([], message_2.senders)
        self.assertEqual(None, message_2.send_type)
        self.assertEqual(None, message_2.cycle_time)
        self.assertEqual(1, len(message_2.signals))
        self.assertEqual(None, message_2.comment)
        self.assertEqual(None, message_2.bus_name)
        self.assertEqual(False, message_2.is_multiplexed())

        signal_3 = message_2.signals[0]
        self.assertEqual('Signal3', signal_3.name)
        self.assertEqual(5, signal_3.start)
        self.assertEqual(11, signal_3.length)
        self.assertEqual([], signal_3.receivers)
        self.assertEqual('big_endian', signal_3.byte_order)
        self.assertEqual(True, signal_3.is_signed)
        self.assertEqual(False, signal_3.is_float)
        self.assertEqual(1, signal_3.scale)
        self.assertEqual(0, signal_3.offset)
        self.assertEqual(0, signal_3.minimum)
        self.assertEqual(1, signal_3.maximum)
        self.assertEqual(1, signal_3.decimal.scale)
        self.assertEqual(0, signal_3.decimal.offset)
        self.assertEqual(0, signal_3.decimal.minimum)
        self.assertEqual(1, signal_3.decimal.maximum)
        self.assertEqual(None, signal_3.unit)
        self.assertEqual({0: 'foo', 1: 'bar'}, signal_3.choices)
        self.assertEqual(None, signal_3.comment)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual(None, signal_3.multiplexer_ids)

        # Symbol2.
        signal_4 = db.messages[4].signals[0]
        self.assertEqual('Signal4', signal_4.name)
        self.assertEqual(0, signal_4.start)
        self.assertEqual(64, signal_4.length)
        self.assertEqual([], signal_4.receivers)
        self.assertEqual('little_endian', signal_4.byte_order)
        self.assertEqual(False, signal_4.is_signed)
        self.assertEqual(True, signal_4.is_float)
        self.assertEqual(6, signal_4.scale)
        self.assertEqual(5, signal_4.offset)
        self.assertEqual(-1.7e+308, signal_4.minimum)
        self.assertEqual(1.7e+308, signal_4.maximum)
        self.assertEqual(6, signal_4.decimal.scale)
        self.assertEqual(5, signal_4.decimal.offset)
        self.assertEqual(Decimal('-1.7e+308'), signal_4.decimal.minimum)
        self.assertEqual(Decimal('1.7e+308'), signal_4.decimal.maximum)
        self.assertEqual('*UU', signal_4.unit)
        self.assertEqual(None, signal_4.choices)
        self.assertEqual(None, signal_4.comment)
        self.assertEqual(False, signal_4.is_multiplexer)
        self.assertEqual(None, signal_4.multiplexer_ids)

        # Symbol3.
        symbol_3 = db.messages[5]
        self.assertEqual(0x33, symbol_3.frame_id)
        self.assertEqual(8, symbol_3.length)
        self.assertEqual(True, symbol_3.is_multiplexed())
        self.assertEqual(4, len(symbol_3.signals))
        multiplexer = symbol_3.signals[0]
        self.assertEqual('Multiplexer1', multiplexer.name)
        self.assertEqual(0, multiplexer.start)
        self.assertEqual(3, multiplexer.length)
        self.assertEqual(True, multiplexer.is_multiplexer)
        self.assertEqual(None, multiplexer.multiplexer_ids)
        signal_1 = symbol_3.signals[1]
        self.assertEqual('Signal1', signal_1.name)
        self.assertEqual(3, signal_1.start)
        self.assertEqual(11, signal_1.length)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual([0], signal_1.multiplexer_ids)
        signal_2 = symbol_3.signals[2]
        self.assertEqual('Signal2', signal_2.name)
        self.assertEqual(6, signal_2.start)
        self.assertEqual(32, signal_2.length)
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual([1], signal_2.multiplexer_ids)
        signal_3 = symbol_3.signals[3]
        self.assertEqual('Signal3', signal_3.name)
        self.assertEqual(14, signal_3.start)
        self.assertEqual(11, signal_3.length)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual([2], signal_3.multiplexer_ids)

        # Encode and decode.
        frame_id = 0x009
        encoded = db.encode_message(frame_id, {})
        self.assertEqual(8, len(encoded))
        self.assertEqual(8 * b'\x00', encoded)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual({}, decoded)

        frame_id = 0x022
        encoded = db.encode_message(frame_id, {'Signal3': 'bar'})
        self.assertEqual(8, len(encoded))
        self.assertEqual(b'\x00\x08\x00\x00\x00\x00\x00\x00', encoded)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual('bar', decoded['Signal3'])

    def test_empty_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/empty-6.0.sym')

        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(0, len(db.messages))

    def test_send_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/send-6.0.sym')

        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(1, len(db.messages))
        self.assertEqual('Symbol1', db.messages[0].name)

    def test_receive_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/receive-6.0.sym')

        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(1, len(db.messages))
        self.assertEqual('Symbol1', db.messages[0].name)

    def test_sendreceive_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/sendreceive-6.0.sym')

        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(1, len(db.messages))
        self.assertEqual('Symbol1', db.messages[0].name)

    def test_signal_types_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/signal-types-6.0.sym')

        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(1, len(db.messages))
        self.assertEqual(8, len(db.messages[0].signals))

        signal_0 = db.messages[0].signals[0]
        self.assertEqual('Bit', signal_0.name)
        self.assertEqual(0, signal_0.start)
        self.assertEqual(1, signal_0.length)
        self.assertEqual([], signal_0.receivers)
        self.assertEqual('little_endian', signal_0.byte_order)
        self.assertEqual(False, signal_0.is_signed)
        self.assertEqual(False, signal_0.is_float)
        self.assertEqual(1, signal_0.scale)
        self.assertEqual(0, signal_0.offset)
        self.assertEqual(0, signal_0.minimum)
        self.assertEqual(1, signal_0.maximum)
        self.assertEqual(1, signal_0.decimal.scale)
        self.assertEqual(0, signal_0.decimal.offset)
        self.assertEqual(0, signal_0.decimal.minimum)
        self.assertEqual(1, signal_0.decimal.maximum)
        self.assertEqual(None, signal_0.unit)
        self.assertEqual(None, signal_0.choices)
        self.assertEqual(None, signal_0.comment)
        self.assertEqual(False, signal_0.is_multiplexer)
        self.assertEqual(None, signal_0.multiplexer_ids)

        signal_1 = db.messages[0].signals[1]
        self.assertEqual('Char', signal_1.name)
        self.assertEqual(1, signal_1.start)
        self.assertEqual(8, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('little_endian', signal_1.byte_order)
        self.assertEqual(False, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        self.assertEqual(1, signal_1.scale)
        self.assertEqual(0, signal_1.offset)
        self.assertEqual(None, signal_1.minimum)
        self.assertEqual(None, signal_1.maximum)
        self.assertEqual(1, signal_1.decimal.scale)
        self.assertEqual(0, signal_1.decimal.offset)
        self.assertEqual(None, signal_1.decimal.minimum)
        self.assertEqual(None, signal_1.decimal.maximum)
        self.assertEqual(None, signal_1.unit)
        self.assertEqual(None, signal_1.choices)
        self.assertEqual(None, signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = db.messages[0].signals[2]
        self.assertEqual('Enum', signal_2.name)
        self.assertEqual(9, signal_2.start)
        self.assertEqual(4, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('little_endian', signal_2.byte_order)
        self.assertEqual(False, signal_2.is_signed)
        self.assertEqual(False, signal_2.is_float)
        self.assertEqual(1, signal_2.scale)
        self.assertEqual(0, signal_2.offset)
        self.assertEqual(None, signal_2.minimum)
        self.assertEqual(None, signal_2.maximum)
        self.assertEqual(1, signal_2.decimal.scale)
        self.assertEqual(0, signal_2.decimal.offset)
        self.assertEqual(None, signal_2.decimal.minimum)
        self.assertEqual(None, signal_2.decimal.maximum)
        self.assertEqual(None, signal_2.unit)
        self.assertEqual({}, signal_2.choices)
        self.assertEqual(None, signal_2.comment)
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        signal_3 = db.messages[0].signals[3]
        self.assertEqual('Signed', signal_3.name)
        self.assertEqual(13, signal_3.start)
        self.assertEqual(3, signal_3.length)
        self.assertEqual([], signal_3.receivers)
        self.assertEqual('little_endian', signal_3.byte_order)
        self.assertEqual(True, signal_3.is_signed)
        self.assertEqual(False, signal_3.is_float)
        self.assertEqual(1, signal_3.scale)
        self.assertEqual(0, signal_3.offset)
        self.assertEqual(None, signal_3.minimum)
        self.assertEqual(None, signal_3.maximum)
        self.assertEqual(1, signal_3.decimal.scale)
        self.assertEqual(0, signal_3.decimal.offset)
        self.assertEqual(None, signal_3.decimal.minimum)
        self.assertEqual(None, signal_3.decimal.maximum)
        self.assertEqual(None, signal_3.unit)
        self.assertEqual(None, signal_3.choices)
        self.assertEqual(None, signal_3.comment)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual(None, signal_3.multiplexer_ids)

        signal_4 = db.messages[0].signals[4]
        self.assertEqual('String', signal_4.name)
        self.assertEqual(16, signal_4.start)
        self.assertEqual(16, signal_4.length)
        self.assertEqual([], signal_4.receivers)
        self.assertEqual('little_endian', signal_4.byte_order)
        self.assertEqual(False, signal_4.is_signed)
        self.assertEqual(False, signal_4.is_float)
        self.assertEqual(1, signal_4.scale)
        self.assertEqual(0, signal_4.offset)
        self.assertEqual(None, signal_4.minimum)
        self.assertEqual(None, signal_4.maximum)
        self.assertEqual(1, signal_4.decimal.scale)
        self.assertEqual(0, signal_4.decimal.offset)
        self.assertEqual(None, signal_4.decimal.minimum)
        self.assertEqual(None, signal_4.decimal.maximum)
        self.assertEqual(None, signal_4.unit)
        self.assertEqual(None, signal_4.choices)
        self.assertEqual(None, signal_4.comment)
        self.assertEqual(False, signal_4.is_multiplexer)
        self.assertEqual(None, signal_4.multiplexer_ids)

        signal_5 = db.messages[0].signals[5]
        self.assertEqual('Raw', signal_5.name)
        self.assertEqual(32, signal_5.start)
        self.assertEqual(16, signal_5.length)
        self.assertEqual([], signal_5.receivers)
        self.assertEqual('little_endian', signal_5.byte_order)
        self.assertEqual(False, signal_5.is_signed)
        self.assertEqual(False, signal_5.is_float)
        self.assertEqual(1, signal_5.scale)
        self.assertEqual(0, signal_5.offset)
        self.assertEqual(None, signal_5.minimum)
        self.assertEqual(None, signal_5.maximum)
        self.assertEqual(1, signal_5.decimal.scale)
        self.assertEqual(0, signal_5.decimal.offset)
        self.assertEqual(None, signal_5.decimal.minimum)
        self.assertEqual(None, signal_5.decimal.maximum)
        self.assertEqual(None, signal_5.unit)
        self.assertEqual(None, signal_5.choices)
        self.assertEqual(None, signal_5.comment)
        self.assertEqual(False, signal_5.is_multiplexer)
        self.assertEqual(None, signal_5.multiplexer_ids)

        signal_6 = db.messages[0].signals[6]
        self.assertEqual('Unsigned', signal_6.name)
        self.assertEqual(48, signal_6.start)
        self.assertEqual(2, signal_6.length)
        self.assertEqual([], signal_6.receivers)
        self.assertEqual('little_endian', signal_6.byte_order)
        self.assertEqual(False, signal_6.is_signed)
        self.assertEqual(False, signal_6.is_float)
        self.assertEqual(1, signal_6.scale)
        self.assertEqual(0, signal_6.offset)
        self.assertEqual(None, signal_6.minimum)
        self.assertEqual(None, signal_6.maximum)
        self.assertEqual(1, signal_6.decimal.scale)
        self.assertEqual(0, signal_6.decimal.offset)
        self.assertEqual(None, signal_6.decimal.minimum)
        self.assertEqual(None, signal_6.decimal.maximum)
        self.assertEqual(None, signal_6.unit)
        self.assertEqual(None, signal_6.choices)
        self.assertEqual(None, signal_6.comment)
        self.assertEqual(False, signal_6.is_multiplexer)
        self.assertEqual(None, signal_6.multiplexer_ids)

        signal_7 = db.messages[0].signals[7]
        self.assertEqual('Enum2', signal_7.name)
        self.assertEqual(50, signal_7.start)
        self.assertEqual(3, signal_7.length)
        self.assertEqual([], signal_7.receivers)
        self.assertEqual('little_endian', signal_7.byte_order)
        self.assertEqual(False, signal_7.is_signed)
        self.assertEqual(False, signal_7.is_float)
        self.assertEqual(1, signal_7.scale)
        self.assertEqual(0, signal_7.offset)
        self.assertEqual(None, signal_7.minimum)
        self.assertEqual(None, signal_7.maximum)
        self.assertEqual(1, signal_7.decimal.scale)
        self.assertEqual(0, signal_7.decimal.offset)
        self.assertEqual(None, signal_7.decimal.minimum)
        self.assertEqual(None, signal_7.decimal.maximum)
        self.assertEqual(None, signal_7.unit)
        self.assertEqual(None, signal_7.choices)
        self.assertEqual(None, signal_7.comment)
        self.assertEqual(False, signal_7.is_multiplexer)
        self.assertEqual(None, signal_7.multiplexer_ids)

    def test_variables_color_enum_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/variables-color-enum-6.0.sym')

        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(1, len(db.messages))

        signal_0 = db.messages[0].signals[0]
        self.assertEqual('Enum', signal_0.name)
        self.assertEqual(5, signal_0.start)
        self.assertEqual(8, signal_0.length)
        self.assertEqual([], signal_0.receivers)
        self.assertEqual('big_endian', signal_0.byte_order)
        self.assertEqual(False, signal_0.is_signed)
        self.assertEqual(False, signal_0.is_float)
        self.assertEqual(1, signal_0.scale)
        self.assertEqual(0, signal_0.offset)
        self.assertEqual(None, signal_0.minimum)
        self.assertEqual(None, signal_0.maximum)
        self.assertEqual(1, signal_0.decimal.scale)
        self.assertEqual(0, signal_0.decimal.offset)
        self.assertEqual(None, signal_0.decimal.minimum)
        self.assertEqual(None, signal_0.decimal.maximum)
        self.assertEqual(None, signal_0.unit)
        self.assertEqual({0: 'Foo'}, signal_0.choices)
        self.assertEqual(None, signal_0.comment)
        self.assertEqual(False, signal_0.is_multiplexer)
        self.assertEqual(None, signal_0.multiplexer_ids)

        signal_1 = db.messages[0].signals[1]
        self.assertEqual('Variable2', signal_1.name)
        self.assertEqual(11, signal_1.start)
        self.assertEqual(10, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('big_endian', signal_1.byte_order)
        self.assertEqual(True, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        self.assertEqual(1, signal_1.scale)
        self.assertEqual(0, signal_1.offset)
        self.assertEqual(None, signal_1.minimum)
        self.assertEqual(None, signal_1.maximum)
        self.assertEqual(1, signal_1.decimal.scale)
        self.assertEqual(0, signal_1.decimal.offset)
        self.assertEqual(None, signal_1.decimal.minimum)
        self.assertEqual(None, signal_1.decimal.maximum)
        self.assertEqual(None, signal_1.unit)
        self.assertEqual({0: 'Foo'}, signal_1.choices)
        self.assertEqual(None, signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = db.messages[0].signals[2]
        self.assertEqual('Variable1', signal_2.name)
        self.assertEqual(12, signal_2.start)
        self.assertEqual(1, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('little_endian', signal_2.byte_order)
        self.assertEqual(False, signal_2.is_signed)
        self.assertEqual(False, signal_2.is_float)
        self.assertEqual(1, signal_2.scale)
        self.assertEqual(0, signal_2.offset)
        self.assertEqual(None, signal_2.minimum)
        self.assertEqual(None, signal_2.maximum)
        self.assertEqual(1, signal_2.decimal.scale)
        self.assertEqual(0, signal_2.decimal.offset)
        self.assertEqual(None, signal_2.decimal.minimum)
        self.assertEqual(None, signal_2.decimal.maximum)
        self.assertEqual(None, signal_2.unit)
        self.assertEqual({}, signal_2.choices)
        self.assertEqual(None, signal_2.comment)
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        signal_3 = db.messages[0].signals[3]
        self.assertEqual('Color', signal_3.name)
        self.assertEqual(17, signal_3.start)
        self.assertEqual(32, signal_3.length)
        self.assertEqual([], signal_3.receivers)
        self.assertEqual('big_endian', signal_3.byte_order)
        self.assertEqual(False, signal_3.is_signed)
        self.assertEqual(True, signal_3.is_float)
        self.assertEqual(1, signal_3.scale)
        self.assertEqual(2, signal_3.offset)
        self.assertEqual(None, signal_3.minimum)
        self.assertEqual(None, signal_3.maximum)
        self.assertEqual(1, signal_3.decimal.scale)
        self.assertEqual(2, signal_3.decimal.offset)
        self.assertEqual(None, signal_3.decimal.minimum)
        self.assertEqual(None, signal_3.decimal.maximum)
        self.assertEqual('A', signal_3.unit)
        self.assertEqual(None, signal_3.choices)
        self.assertEqual(None, signal_3.comment)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual(None, signal_3.multiplexer_ids)

    def test_empty_enum_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/empty-enum-6.0.sym')
        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(1, len(db.messages))

        signal_0 = db.messages[0].signals[0]
        self.assertEqual('Signal1', signal_0.name)
        self.assertEqual({}, signal_0.choices)

    def test_special_chars_6_0_sym(self):
        filename = os.path.join(BASE_PATH, 'files/sym/special-chars-6.0.sym')

        db = cantools.database.load_file(filename)

        self.assertEqual('6.0', db.version)
        self.assertEqual(1, len(db.messages))

        message = db.messages[0]
        self.assertEqual(1, message.frame_id)
        self.assertEqual(False, message.is_extended_frame)
        self.assertEqual('A/=*', message.name)
        self.assertEqual(8, message.length)
        self.assertEqual([], message.senders)
        self.assertEqual(None, message.send_type)
        self.assertEqual(5, message.cycle_time)
        self.assertEqual(6, len(message.signals))
        self.assertEqual('dd', message.comment)
        self.assertEqual(None, message.bus_name)
        self.assertEqual(False, message.is_multiplexed())

        signal_0 = message.signals[0]
        self.assertEqual('A B', signal_0.name)
        self.assertEqual(0, signal_0.start)
        self.assertEqual(8, signal_0.length)
        self.assertEqual([], signal_0.receivers)
        self.assertEqual('little_endian', signal_0.byte_order)
        self.assertEqual(False, signal_0.is_signed)
        self.assertEqual(False, signal_0.is_float)
        self.assertEqual(1, signal_0.scale)
        self.assertEqual(0, signal_0.offset)
        self.assertEqual(None, signal_0.minimum)
        self.assertEqual(None, signal_0.maximum)
        self.assertEqual(1, signal_0.decimal.scale)
        self.assertEqual(0, signal_0.decimal.offset)
        self.assertEqual(None, signal_0.decimal.minimum)
        self.assertEqual(None, signal_0.decimal.maximum)
        self.assertEqual('A B', signal_0.unit)
        self.assertEqual(None, signal_0.choices)
        self.assertEqual('A B', signal_0.comment)
        self.assertEqual(False, signal_0.is_multiplexer)
        self.assertEqual(None, signal_0.multiplexer_ids)

        signal_1 = message.signals[1]
        self.assertEqual('S/', signal_1.name)
        self.assertEqual(15, signal_1.start)
        self.assertEqual(8, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('big_endian', signal_1.byte_order)
        self.assertEqual(False, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        self.assertEqual(1, signal_1.scale)
        self.assertEqual(0, signal_1.offset)
        self.assertEqual(None, signal_1.minimum)
        self.assertEqual(None, signal_1.maximum)
        self.assertEqual(1, signal_1.decimal.scale)
        self.assertEqual(0, signal_1.decimal.offset)
        self.assertEqual(None, signal_1.decimal.minimum)
        self.assertEqual(None, signal_1.decimal.maximum)
        self.assertEqual('/', signal_1.unit)
        self.assertEqual(None, signal_1.choices)
        self.assertEqual('/', signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = message.signals[2]
        self.assertEqual('S=', signal_2.name)
        self.assertEqual(23, signal_2.start)
        self.assertEqual(8, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('big_endian', signal_2.byte_order)
        self.assertEqual(False, signal_2.is_signed)
        self.assertEqual(False, signal_2.is_float)
        self.assertEqual(1, signal_2.scale)
        self.assertEqual(-55, signal_2.offset)
        self.assertEqual(None, signal_2.minimum)
        self.assertEqual(None, signal_2.maximum)
        self.assertEqual(1, signal_2.decimal.scale)
        self.assertEqual(-55, signal_2.decimal.offset)
        self.assertEqual(None, signal_2.decimal.minimum)
        self.assertEqual(None, signal_2.decimal.maximum)
        self.assertEqual('=', signal_2.unit)
        self.assertEqual(None, signal_2.choices)
        self.assertEqual('=', signal_2.comment)
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        signal_3 = message.signals[3]
        self.assertEqual('S{SEND}', signal_3.name)
        self.assertEqual(24, signal_3.start)
        self.assertEqual(8, signal_3.length)
        self.assertEqual([], signal_3.receivers)
        self.assertEqual('little_endian', signal_3.byte_order)
        self.assertEqual(False, signal_3.is_signed)
        self.assertEqual(False, signal_3.is_float)
        self.assertEqual(1, signal_3.scale)
        self.assertEqual(0, signal_3.offset)
        self.assertEqual(None, signal_3.minimum)
        self.assertEqual(None, signal_3.maximum)
        self.assertEqual(1, signal_3.decimal.scale)
        self.assertEqual(0, signal_3.decimal.offset)
        self.assertEqual(None, signal_3.decimal.minimum)
        self.assertEqual(None, signal_3.decimal.maximum)
        self.assertEqual('{SEND}', signal_3.unit)
        self.assertEqual(None, signal_3.choices)
        self.assertEqual(']', signal_3.comment)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual(None, signal_3.multiplexer_ids)

        signal_4 = message.signals[4]
        self.assertEqual('a/b', signal_4.name)
        self.assertEqual(32, signal_4.start)
        self.assertEqual(8, signal_4.length)
        self.assertEqual([], signal_4.receivers)
        self.assertEqual('little_endian', signal_4.byte_order)
        self.assertEqual(False, signal_4.is_signed)
        self.assertEqual(False, signal_4.is_float)
        self.assertEqual(1, signal_4.scale)
        self.assertEqual(0, signal_4.offset)
        self.assertEqual(None, signal_4.minimum)
        self.assertEqual(None, signal_4.maximum)
        self.assertEqual(1, signal_4.decimal.scale)
        self.assertEqual(0, signal_4.decimal.offset)
        self.assertEqual(None, signal_4.decimal.minimum)
        self.assertEqual(None, signal_4.decimal.maximum)
        self.assertEqual('][', signal_4.unit)
        self.assertEqual(None, signal_4.choices)
        self.assertEqual('ö', signal_4.comment)

        self.assertEqual(False, signal_4.is_multiplexer)
        self.assertEqual(None, signal_4.multiplexer_ids)

        signal_5 = message.signals[5]
        self.assertEqual('Variable1', signal_5.name)
        self.assertEqual(40, signal_5.start)
        self.assertEqual(1, signal_5.length)
        self.assertEqual([], signal_5.receivers)
        self.assertEqual('little_endian', signal_5.byte_order)
        self.assertEqual(False, signal_5.is_signed)
        self.assertEqual(False, signal_5.is_float)
        self.assertEqual(1, signal_5.scale)
        self.assertEqual(0, signal_5.offset)
        self.assertEqual(0, signal_5.minimum)
        self.assertEqual(1, signal_5.maximum)
        self.assertEqual(1, signal_5.decimal.scale)
        self.assertEqual(0, signal_5.decimal.offset)
        self.assertEqual(0, signal_5.decimal.minimum)
        self.assertEqual(1, signal_5.decimal.maximum)
        self.assertEqual('m/s', signal_5.unit)
        self.assertEqual(None, signal_5.choices)
        self.assertEqual('comment', signal_5.comment)
        self.assertEqual(False, signal_5.is_multiplexer)
        self.assertEqual(None, signal_5.multiplexer_ids)

    def test_add_bad_sym_string(self):
        db = cantools.db.Database()

        with self.assertRaises(textparser.ParseError) as cm:
            db.add_sym_string('FormatVersion=6.0\n'
                              'Foo="Jopp"')

        self.assertEqual(
            'Invalid syntax at line 2, column 1: ">>!<<Foo="Jopp""',
            str(cm.exception))

    def test_multiplexed_variables_sym(self):
        db = cantools.db.Database()
        filename = os.path.join(BASE_PATH, 'files/sym/multiplexed_variables.sym')

        db.add_sym_file(filename)
        message = db.get_message_by_name('TestAlert')
        self.assertEqual([
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
                         ],
                         message.signal_tree)

    def test_load_bad_format(self):
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError):
            cantools.db.load(StringIO(''))

    def test_add_bad_kcd_string(self):
        db = cantools.db.Database()

        with self.assertRaises(ElementTree.ParseError) as cm:
            db.add_kcd_string('not xml')

        self.assertEqual('syntax error: line 1, column 0', str(cm.exception))

    def test_bus(self):
        bus = cantools.db.bus.Bus('foo')
        self.assertEqual("bus('foo', None)", repr(bus))

        bus = cantools.db.bus.Bus('foo', 'bar')
        self.assertEqual("bus('foo', 'bar')", repr(bus))

    def test_num(self):
        self.assertEqual(1, cantools.database.can.formats.utils.num('1'))
        self.assertEqual(1.0, cantools.database.can.formats.utils.num('1.0'))

        with self.assertRaises(ValueError):
            cantools.database.can.formats.utils.num('x')

    def test_timing(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/timing.dbc')

        db = cantools.db.load_file(filename)

        # Message cycle time is 200, as given by BA_.
        message = db.get_message_by_frame_id(1)
        self.assertEqual(200, message.cycle_time)
        self.assertEqual('Cyclic', message.send_type)

        # Default message cycle time is 0, as given by BA_DEF_DEF_.
        message = db.get_message_by_frame_id(2)
        self.assertEqual(0, message.cycle_time)
        self.assertEqual('NoMsgSendType', message.send_type)

        self.assert_dbc_dump(db, filename)

    def test_multiplex(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex.dbc')

        db = cantools.db.load_file(filename)

        message_1 = db.messages[0]
        self.assertTrue(message_1.is_multiplexed())

        self.assertEqual([
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
                         ],
                         message_1.signal_tree)

        self.assertEqual(
            '-- {root}\n'
            '   +-- Multiplexor\n'
            '       +-- 8\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- 16\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- 24\n'
            '           +-- BIT_J\n'
            '           +-- BIT_C\n'
            '           +-- BIT_G\n'
            '           +-- BIT_L\n'
            '           +-- BIT_A\n'
            '           +-- BIT_K\n'
            '           +-- BIT_E\n'
            '           +-- BIT_D\n'
            '           +-- BIT_B\n'
            '           +-- BIT_H\n'
            '           +-- BIT_F',
            message_1.signal_tree_string())

        signal_multiplexor = message_1.signals[0]
        self.assertEqual(True, signal_multiplexor.is_multiplexer)

        signal_bit_j = message_1.signals[1]
        self.assertEqual(False, signal_bit_j.is_multiplexer)
        self.assertEqual([8, 16, 24], signal_bit_j.multiplexer_ids)

        signal_bit_a = message_1.signals[5]
        self.assertEqual(False, signal_bit_a.is_multiplexer)
        self.assertEqual([24], signal_bit_a.multiplexer_ids)

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
            self.assertEqual(encoded_message, encoded)
            decoded = message_1.decode(encoded)
            self.assertEqual(decoded_message, decoded)

        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex_dumped.dbc')

        self.assert_dbc_dump(db, filename)

    def test_multiplex_choices(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex_choices.dbc')

        db = cantools.db.load_file(filename)

        # With Multiplexor and BIT_L as strings.
        decoded_message = {
            'Multiplexor': 'MULTIPLEXOR_8',
            'BIT_C': 1, 'BIT_G': 1, 'BIT_J': 1, 'BIT_L': 'On'
        }
        encoded_message = b'\x20\x00\x8c\x01\x00\x00\x00\x00'

        message_1 = db.messages[0]

        encoded = message_1.encode(decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = message_1.decode(encoded)
        self.assertEqual(decoded_message, decoded)

        # With Multiplexor and BIT_L as numbers.
        decoded_message = {
            'Multiplexor': 8,
            'BIT_C': 1, 'BIT_G': 1, 'BIT_J': 1, 'BIT_L': 1
        }
        encoded_message = b'\x20\x00\x8c\x01\x00\x00\x00\x00'

        message_1 = db.messages[0]

        encoded = message_1.encode(decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = message_1.decode(encoded, decode_choices=False)
        self.assertEqual(decoded_message, decoded)

        self.assertEqual(
            '\n'
            'Multiplexor\n'
            '    8 MULTIPLEXOR_8\n'
            '    16 MULTIPLEXOR_16\n'
            '    24 MULTIPLEXOR_24\n'
            '\n'
            'BIT_L\n'
            '    0 Off\n'
            '    1 On',
            message_1.signal_choices_string()
        )

        # With Multiplexor as the only signal.
        decoded_message = {
            'Multiplexor': 4
        }
        encoded_message = b'\x10\x00\x00\x00\x00\x00\x00\x00'

        message_2 = db.messages[1]

        encoded = message_2.encode(decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = message_2.decode(encoded, decode_choices=False)
        self.assertEqual(decoded_message, decoded)

        self.assertEqual([
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
                         ],
                         message_2.signal_tree)

        self.assertEqual(
            '-- {root}\n'
            '   +-- Multiplexor\n'
            '       +-- 4\n'
            '       +-- 8\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- 16\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- 24\n'
            '           +-- BIT_J\n'
            '           +-- BIT_C\n'
            '           +-- BIT_G\n'
            '           +-- BIT_L\n'
            '           +-- BIT_A\n'
            '           +-- BIT_K\n'
            '           +-- BIT_E\n'
            '           +-- BIT_D\n'
            '           +-- BIT_B\n'
            '           +-- BIT_H\n'
            '           +-- BIT_F',
            message_2.signal_tree_string())

        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex_choices_dumped.dbc')

        self.assert_dbc_dump(db, filename)

    def test_multiplex_2(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex_2.dbc')

        db = cantools.db.load_file(filename)

        # Shared.
        message = db.messages[0]
        self.assertEqual([
                             {
                                 'S0': {
                                     1: ['S1'],
                                     2: ['S2'],
                                     3: ['S1', 'S2'],
                                     4: ['S2'],
                                     5: ['S2']
                                 }
                             }
                         ],
                         message.signal_tree)

        # Normal.
        message = db.messages[1]
        self.assertEqual([
                             {
                                 'S0': {
                                     0: ['S1'],
                                     1: ['S2']
                                 }
                             }
                         ],
                         message.signal_tree)

        # Extended.
        message = db.messages[2]
        self.assertEqual([
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
                         ],
                         message.signal_tree)

        # Extended types.
        message = db.messages[3]
        self.assertEqual([
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
                         ],
                         message.signal_tree)

        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex_2_dumped.dbc')

        self.assert_dbc_dump(db, filename)

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

        self.assertEqual([
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
                         ],
                         message.signal_tree)

        self.assertEqual(
            '-- {root}\n'
            '   +-- S0\n'
            '   |   +-- 0\n'
            '   |   |   +-- S1\n'
            '   |   |       +-- 0\n'
            '   |   |       |   +-- S2\n'
            '   |   |       |   +-- S3\n'
            '   |   |       +-- 2\n'
            '   |   |           +-- S4\n'
            '   |   +-- 1\n'
            '   |       +-- S5\n'
            '   +-- S6\n'
            '       +-- 1\n'
            '       |   +-- S7\n'
            '       +-- 2\n'
            '           +-- S8',
            message.signal_tree_string())

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
            self.assertEqual(encoded_message, encoded)
            decoded = message.decode(encoded)
            self.assertEqual(decoded_message, decoded)

    def test_dbc_parse_error_messages(self):
        # No valid entry.
        with self.assertRaises(textparser.ParseError) as cm:
            dbc.load_string('abc')

        self.assertEqual(
            'Invalid syntax at line 1, column 1: ">>!<<abc"',
            str(cm.exception))

        # Bad message frame id.
        with self.assertRaises(textparser.ParseError) as cm:
            dbc.load_string('VERSION "1.0"\n'
                            'BO_ dssd\n')

        self.assertEqual(
            'Invalid syntax at line 2, column 5: "BO_ >>!<<dssd"',
            str(cm.exception))

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
            'Invalid syntax at line 2, column 19: "BO_ 546 EMV_Stati '
            '>>!<<8 EMV_Statusmeldungen"',
            str(cm.exception))

        # Missing frame id in message comment.
        with self.assertRaises(textparser.ParseError) as cm:
            dbc.load_string('CM_ BO_ "Foo.";')

        self.assertEqual(
            'Invalid syntax at line 1, column 9: "CM_ BO_ >>!<<"Foo.";"',
            str(cm.exception))

        # Missing frame id in message comment, using load_string().
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_string('CM_ BO_ "Foo.";')

        self.assertEqual(
            "ARXML: \"syntax error: line 1, column 0\", DBC: \"Invalid syntax "
            "at line 1, column 9: \"CM_ BO_ >>!<<\"Foo.\";\"\", KCD: \"syntax "
            "error: line 1, column 0\", SYM: \"Only SYM version 6.0 is "
            "supported.\", CDD: \"syntax error: line 1, column 0\"",
            str(cm.exception))

    def test_get_node_by_name(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db = cantools.db.load_file(filename)

        self.assertIs(db.get_node_by_name('Motor alternative supplier'),
                      db.nodes[1])

        with self.assertRaises(KeyError) as cm:
            db.get_node_by_name('Missing')

        self.assertEqual("'Missing'", str(cm.exception))

    def test_get_bus_by_name(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')

        db = cantools.db.load_file(filename)

        self.assertIs(db.get_bus_by_name('Comfort'), db.buses[2])

        with self.assertRaises(KeyError) as cm:
            db.get_bus_by_name('Missing')

        self.assertEqual("'Missing'", str(cm.exception))

    def test_load_file_with_database_format(self):
        filename_dbc = os.path.join(BASE_PATH, 'files/dbc/foobar.dbc')
        filename_kcd = os.path.join(BASE_PATH, 'files/kcd/the_homer.kcd')
        filename_sym = os.path.join(BASE_PATH, 'files/sym/jopp-6.0.sym')

        # Matching file contents and database format.
        cantools.db.load_file(filename_dbc, database_format=None)
        cantools.db.load_file(filename_dbc, database_format='dbc')
        cantools.db.load_file(filename_kcd, database_format='kcd')
        cantools.db.load_file(filename_sym, database_format='sym')

        # KCD database format, but file is DBC.
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename_dbc, database_format='kcd')

        self.assertEqual(
            "KCD: \"not well-formed (invalid token): line 1, column 0\"",
            str(cm.exception))

        # DBC database format, but file is KCD.
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename_kcd, database_format='dbc')

        self.assertEqual(
            "DBC: \"Invalid syntax at line 1, column 1: \">>!<<<!--\"\"",
            str(cm.exception))

        # SYM database format, but file is KCD.
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename_kcd, database_format='sym')

        self.assertEqual(
            "SYM: \"Only SYM version 6.0 is supported.\"",
            str(cm.exception))

        # Unsupported database format.
        with self.assertRaises(ValueError) as cm:
            cantools.db.load_file(filename_kcd, database_format='bad')

        self.assertEqual(
            "expected database format 'arxml', 'dbc', 'kcd', 'sym', 'cdd' or "
            "None, but got 'bad'",
            str(cm.exception))

    def test_load_file_encoding(self):
        # Override default encoding.
        #
        # Does not fail to load using UTF-8 encoding because encoding
        # errors are replaced (fopen(errors='replace')).

        filename = os.path.join(BASE_PATH, 'files/dbc/cp1252.dbc')

        db = cantools.database.load_file(filename, encoding='utf-8')

        replaced = 123 * b'\xef\xbf\xbd'.decode('utf-8')

        self.assertEqual(
            r" !#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTU"
            r"VWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
            + replaced,
            db.nodes[0].comment)

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
                                      signals=signals)

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
        self.assertEqual(encoded_message, encoded)
        decoded = message.decode(encoded)
        self.assertEqual(decoded_message, decoded)

    def test_multiplex_choices_padding_one(self):
        """Test to encode a multiplexed message with padding as one.

        """
        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex_choices.dbc')

        db = cantools.db.load_file(filename)

        decoded_message = {
            'Multiplexor': 'MULTIPLEXOR_8',
            'BIT_C': 0, 'BIT_G': 0, 'BIT_J': 0, 'BIT_L': 'Off'
        }
        encoded_message = b'\x23\xff\x73\xfe\xff\xff\xff\xff'

        message_1 = db.messages[0]

        encoded = message_1.encode(decoded_message, padding=True)
        self.assertEqual(encoded_message, encoded)
        encoded = db.encode_message(message_1.name,
                                    decoded_message,
                                    padding=True)
        self.assertEqual(encoded_message, encoded)
        decoded = message_1.decode(encoded)
        self.assertEqual(decoded_message, decoded)

    def test_multiplex_bad_multiplexer(self):
        """Test to encode and decode a multiplexer not part of the
        multiplexer.

        """
        filename = os.path.join(BASE_PATH, 'files/dbc/multiplex_choices.dbc')

        db = cantools.db.load_file(filename)

        message_1 = db.messages[0]

        # Encode.
        with self.assertRaises(cantools.db.EncodeError) as cm:
            message_1.encode({'Multiplexor': 7})

        self.assertEqual('expected multiplexer id 8, 16 or 24, but got 7', str(cm.exception))

        # Decode.
        with self.assertRaises(cantools.db.DecodeError) as cm:
            message_1.decode(b'\x1f\xff\x73\xfe\xff\xff\xff\xff')

        self.assertEqual('expected multiplexer id 8, 16 or 24, but got 7', str(cm.exception))

        message_3 = db.messages[2]

        # Encode with single multiplexer id 8.
        with self.assertRaises(cantools.db.EncodeError) as cm:
            message_3.encode({'Multiplexor': 7})

        self.assertEqual('expected multiplexer id 8, but got 7', str(cm.exception))

        # Decode with single multiplexer id 8.
        with self.assertRaises(cantools.db.DecodeError) as cm:
            message_3.decode(b'\x1f\xff\x73\xfe\xff\xff\xff\xff')

        self.assertEqual('expected multiplexer id 8, but got 7', str(cm.exception))

    def test_multiplex_dump(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/test_multiplex_dump.dbc')

        db = cantools.db.load_file(filename)
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        dumped_msg = dumped_db.get_message_by_frame_id(0x100)

        self.assertEqual("MultiplexorSig", dumped_msg.signals[0].name)
        self.assertEqual(True, dumped_msg.signals[0].is_multiplexer)
        self.assertEqual(None, dumped_msg.signals[0].multiplexer_ids)
        self.assertEqual("MultiplexedSig", dumped_msg.signals[1].name)
        self.assertEqual(False, dumped_msg.signals[1].is_multiplexer)
        self.assertEqual(0x2a, dumped_msg.signals[1].multiplexer_ids[0])
        self.assertEqual("UnmultiplexedSig", dumped_msg.signals[2].name)
        self.assertEqual(None, dumped_msg.signals[2].multiplexer_ids)
        self.assertEqual(False, dumped_msg.signals[2].is_multiplexer)

    def test_string_attribute_definition_dump(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/test_multiplex_dump.dbc')

        db = cantools.db.load_file(filename)
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        attribute = dumped_db.dbc.attribute_definitions

        self.assertEqual("STRING", attribute['BusType'].type_name)

    def test_extended_id_dump(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/test_extended_id_dump.dbc')

        db = cantools.db.load_file(filename)
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        reg_id_msg = dumped_db.get_message_by_frame_id(0x100)
        ext_id_msg = dumped_db.get_message_by_frame_id(0x1c2a2a2a)

        self.assertEqual(False, reg_id_msg.is_extended_frame)
        self.assertEqual(True, ext_id_msg.is_extended_frame)

    def test_attributes(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/attributes.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        # Signal attributes.
        attributes = db.messages[0].signals[0].dbc.attributes

        attribute = attributes['TheSignalStringAttribute']
        self.assertEqual('TheSignalStringAttribute', attribute.name)
        self.assertEqual('TestString', attribute.value)
        self.assertEqual(db.dbc.attribute_definitions['TheSignalStringAttribute'], attribute.definition)
        self.assertEqual('100', attribute.definition.default_value)
        self.assertEqual('SG_', attribute.definition.kind)
        self.assertEqual('STRING', attribute.definition.type_name)
        self.assertEqual(None, attribute.definition.minimum)
        self.assertEqual(None, attribute.definition.maximum)
        self.assertEqual(None, attribute.definition.choices)

        attribute = attributes['GenSigSendType']
        self.assertEqual('GenSigSendType', attribute.name)
        self.assertEqual(1, attribute.value)
        self.assertEqual(db.dbc.attribute_definitions['GenSigSendType'], attribute.definition)
        self.assertEqual('Cyclic', attribute.definition.default_value)
        self.assertEqual('SG_', attribute.definition.kind)
        self.assertEqual('ENUM', attribute.definition.type_name)
        self.assertEqual(None, attribute.definition.minimum)
        self.assertEqual(None, attribute.definition.maximum)
        self.assertEqual([
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
                         ],
                         attribute.definition.choices)

        # Message attribute.
        attributes = db.messages[0].dbc.attributes
        self.assertEqual(11, len(attributes))

        attribute = attributes['TheHexAttribute']
        self.assertEqual('TheHexAttribute', attribute.name)
        self.assertEqual(5, attribute.value)
        self.assertEqual("attribute('TheHexAttribute', 5)", repr(attribute))
        self.assertEqual(db.dbc.attribute_definitions['TheHexAttribute'], attribute.definition)
        self.assertEqual("attribute_definition('TheHexAttribute', 4)", repr(attribute.definition))
        self.assertEqual(4, attribute.definition.default_value)
        self.assertEqual('BO_', attribute.definition.kind)
        self.assertEqual('HEX', attribute.definition.type_name)
        self.assertEqual(0, attribute.definition.minimum)
        self.assertEqual(8, attribute.definition.maximum)
        self.assertEqual(None, attribute.definition.choices)

        attribute = attributes['TheFloatAttribute']
        self.assertEqual('TheFloatAttribute', attribute.name)
        self.assertEqual(Decimal('58.7'), attribute.value)
        self.assertEqual(db.dbc.attribute_definitions['TheFloatAttribute'], attribute.definition)
        self.assertEqual(55.0, attribute.definition.default_value)
        self.assertEqual('BO_', attribute.definition.kind)
        self.assertEqual('FLOAT', attribute.definition.type_name)
        self.assertEqual(5.0, attribute.definition.minimum)
        self.assertEqual(87.0, attribute.definition.maximum)
        self.assertEqual(None, attribute.definition.choices)

        # Node attributes.
        node = db.nodes[0]
        self.assertEqual('TheNode', node.name)
        self.assertEqual('TheNodeComment', node.comment)

        attribute = node.dbc.attributes['TheNodeAttribute']
        self.assertEqual('TheNodeAttribute', attribute.name)
        self.assertEqual(99, attribute.value)
        self.assertEqual(db.dbc.attribute_definitions['TheNodeAttribute'], attribute.definition)
        self.assertEqual(100, attribute.definition.default_value)
        self.assertEqual('BU_', attribute.definition.kind)
        self.assertEqual('INT', attribute.definition.type_name)
        self.assertEqual(50, attribute.definition.minimum)
        self.assertEqual(150, attribute.definition.maximum)
        self.assertEqual(None, attribute.definition.choices)

        # Database attributes.
        attribute = db.dbc.attributes['BusType']
        self.assertEqual('BusType', attribute.name)
        self.assertEqual('CAN', attribute.value)
        self.assertEqual(db.dbc.attribute_definitions['BusType'], attribute.definition)
        self.assertEqual('', attribute.definition.default_value)
        self.assertEqual(None, attribute.definition.kind)
        self.assertEqual('STRING', attribute.definition.type_name)
        self.assertEqual(None, attribute.definition.minimum)
        self.assertEqual(None, attribute.definition.maximum)
        self.assertEqual(None, attribute.definition.choices)

        attribute = db.dbc.attributes['TheNetworkAttribute']
        self.assertEqual('TheNetworkAttribute', attribute.name)
        self.assertEqual(51, attribute.value)
        self.assertEqual(db.dbc.attribute_definitions['TheNetworkAttribute'], attribute.definition)
        self.assertEqual(50, attribute.definition.default_value)
        self.assertEqual(None, attribute.definition.kind)
        self.assertEqual('INT', attribute.definition.type_name)
        self.assertEqual(0, attribute.definition.minimum)
        self.assertEqual(100, attribute.definition.maximum)
        self.assertEqual(None, attribute.definition.choices)

        # Message send type.
        message = db.get_message_by_frame_id(0x39)
        self.assertEqual(1000, message.cycle_time)
        self.assertEqual('Cyclic', message.send_type)

        self.assert_dbc_dump(db, filename)

    def test_big_numbers(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/big_numbers.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        # Node attributes.
        node = db.nodes[0]
        attribute = node.dbc.attributes['TheNodeAttribute']
        self.assertEqual('TheNodeAttribute', attribute.name)
        self.assertEqual(99, attribute.value)
        self.assertEqual(db.dbc.attribute_definitions['TheNodeAttribute'], attribute.definition)
        self.assertEqual(100, attribute.definition.default_value)
        self.assertEqual('BU_', attribute.definition.kind)
        self.assertEqual('INT', attribute.definition.type_name)
        self.assertEqual(-9223372036854780000, attribute.definition.minimum)
        self.assertEqual(18446744073709600000, attribute.definition.maximum)
        self.assertEqual(None, attribute.definition.choices)

    def test_setters(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/attributes.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        # Calling the setters for coverage. Assertions are not
        # necessary here since functionality is trivial.
        db.nodes[0].name = 'SetterName'
        db.nodes[0].comment = 'SetterComment'
        db.dbc.attributes['BusType'].value = 'LIN'
        db.messages[0].name = 'SetterName'
        db.messages[0].frame_id = 0x12121212
        db.messages[0].is_extended_frame = True
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
        filename = os.path.join(BASE_PATH, 'files/dbc/attributes.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        message = db.get_message_by_frame_id(0x39)
        self.assertEqual('TheMessage', message.name)
        message.frame_id = 0x40
        db.refresh()
        message = db.get_message_by_frame_id(0x40)
        self.assertEqual('TheMessage', message.name)
        self.assertEqual(message.frame_id,0x40)

        message.name = 'TheNewMessage'
        db.refresh()
        message = db.get_message_by_name('TheNewMessage')
        self.assertEqual('TheNewMessage', message.name)
        self.assertEqual(0x40, message.frame_id)

        with self.assertRaises(KeyError) as cm:
            db.get_message_by_name('TheMissingMessage')

        self.assertEqual("'TheMissingMessage'", str(cm.exception))

        with self.assertRaises(KeyError) as cm:
            db.get_message_by_frame_id(0x41)

        self.assertEqual(0x41, cm.exception.args[0])

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

            self.assertEqual('The signals S1 and S0 are overlapping in message M.', str(cm.exception))

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

            self.assertEqual('The signal S does not fit in message M.', str(cm.exception))

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

            self.assertEqual(expected_overlpping, str(cm.exception))

    def test_strict_load(self):
        filenames = [
            'files/kcd/bad_message_length.kcd',
            'files/dbc/bad_message_length.dbc',
            'files/sym/bad_message_length.sym'
        ]

        for filename in filenames:
            filename = os.path.join(BASE_PATH, filename)

            # Strict true.
            with self.assertRaises(cantools.database.Error) as cm:
                cantools.database.load_file(filename, strict=True)

            self.assertEqual('The signal Signal1 does not fit in message Message1.', str(cm.exception))

            # Strict false.
            db = cantools.database.load_file(filename, strict=False)

            # Sanity checks.
            message_1 = db.get_message_by_frame_id(1)
            self.assertEqual(1, message_1.length)
            self.assertEqual(8, message_1.signals[0].start)
            self.assertEqual(1, message_1.signals[0].length)

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

        self.assertEqual('The signal S does not fit in message M.', str(cm.exception))

    def test_message_layout(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/message_layout.kcd')

        db = cantools.database.load_file(filename, strict=False)

        # Message 1.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B',
            ' y',
            ' t',
            ' e'
        ]

        actual = db.get_message_by_name('Message1').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 2.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |<-----------------------------x|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t                                   +-- Signal1',
            ' e'
        ]

        actual = db.get_message_by_name('Message2').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 3.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |   |   |   |   |   |   |   |<--|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |--x|   |   |   |   |   |   |   |',
            ' e     +---+---+---+---+---+---+---+---+',
            '         +-- Signal1',
        ]

        actual = db.get_message_by_name('Message3').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 4.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |   |   |   |   |   |   |   |<--|',
            ' B     +---+---+---+---+---+---+---+---+',
            ' y   1 |--x|   |   |   |   |   |   |   |',
            ' t     +---+---+---+---+---+---+---+---+',
            ' e       +-- Signal1',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        actual = db.get_message_by_name('Message4').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 5.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |<---XXXXXXX-------x|XXX|   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '                 |       |   +-- Signal7',
            '                 |       |   +-- Signal8',
            '                 |       +-- Signal1',
            ' B               +-- Signal2',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |   |   |   |   |   |<-x|XXXXXXX|',
            ' e     +---+---+---+---+---+---+---+---+',
            '                             |   +-- Signal4',
            '                             +-- Signal6',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |XXX---x|   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '         |   +-- Signal5',
            '         +-- Signal3'
        ]

        actual = db.get_message_by_name('Message5').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 6.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |   |   |   |   |   |<----------|',
            '       +---+---+---+---+---+---+---+---+',
            '     1 |------x|   |   |   |   |<-x|   |',
            '       +---+---+---+---+---+---+---+---+',
            '             |                   +-- Signal2',
            '             +-- Signal1',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            ' B     +---+---+---+---+---+---+---+---+',
            ' y   3 |--------------x|   |   |   |   |',
            ' t     +---+---+---+---+---+---+---+---+',
            ' e   4 |-------------------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '     5 |   |   |<----------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '                 +-- Signal3',
            '       +---+---+---+---+---+---+---+---+',
            '     6 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '     7 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        actual = db.get_message_by_name('Message6').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 7.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            ' B    7 |-------------------------------|',
            ' y      +---+---+---+---+---+---+---+---+',
            ' t    8 |-------------------------------|',
            ' e      +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '          +-- Signal1'
        ]

        actual = db.get_message_by_name('Message7').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 8.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            ' B    7 |-------------------------------|',
            ' y      +---+---+---+---+---+---+---+---+',
            ' t    8 |-------------------------------|',
            ' e      +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+',
            '                                      +-- Signal1'
        ]

        actual = db.get_message_by_name('Message8').layout_string()
        self.assertEqual('\n'.join(expected_lines), actual)

    def test_message_layout_without_signal_names(self):
        filename = os.path.join(BASE_PATH, 'files/kcd/message_layout.kcd')

        db = cantools.database.load_file(filename, strict=False)

        # Message 1.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B',
            ' y',
            ' t',
            ' e'
        ]

        message = db.get_message_by_name('Message1')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 2.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |<-----------------------------x|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t',
            ' e'
        ]

        message = db.get_message_by_name('Message2')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 3.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |   |   |   |   |   |   |   |<--|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |--x|   |   |   |   |   |   |   |',
            ' e     +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message3')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 4.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |   |   |   |   |   |   |   |<--|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |--x|   |   |   |   |   |   |   |',
            ' e     +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message4')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 5.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |<---XXXXXXX-------x|XXX|   |   |',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |   |   |   |   |   |<-x|XXXXXXX|',
            ' e     +---+---+---+---+---+---+---+---+',
            '     2 |XXX---x|   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message5')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 6.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |   |   |   |   |   |<----------|',
            '       +---+---+---+---+---+---+---+---+',
            '     1 |------x|   |   |   |   |<-x|   |',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            ' B     +---+---+---+---+---+---+---+---+',
            ' y   3 |--------------x|   |   |   |   |',
            ' t     +---+---+---+---+---+---+---+---+',
            ' e   4 |-------------------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '     5 |   |   |<----------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '     6 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '     7 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message6')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 7.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            ' B      +---+---+---+---+---+---+---+---+',
            ' y    7 |-------------------------------|',
            ' t      +---+---+---+---+---+---+---+---+',
            ' e    8 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message7')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

        # Message 8.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            ' B      +---+---+---+---+---+---+---+---+',
            ' y    7 |-------------------------------|',
            ' t      +---+---+---+---+---+---+---+---+',
            ' e    8 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message8')
        actual = message.layout_string(signal_names=False)
        self.assertEqual('\n'.join(expected_lines), actual)

    def test_add_two_dbc_files(self):
        """Test adding two DBC-files to the same database.

        """

        db = cantools.database.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/add_two_dbc_files_1.dbc')

        db.add_dbc_file(filename)
        self.assertEqual(2, len(db.messages))
        self.assertEqual(1, db.get_message_by_name('M1').frame_id)
        self.assertEqual('M2', db.get_message_by_frame_id(2).name)

        filename = os.path.join(BASE_PATH, 'files/dbc/add_two_dbc_files_2.dbc')

        db.add_dbc_file(filename)
        self.assertEqual(3, len(db.messages))
        self.assertEqual(2, db.get_message_by_name('M1').frame_id)
        self.assertEqual('M1', db.get_message_by_frame_id(2).name)

    def test_empty_ns_dbc(self):
        """Test loading a DBC-file with empty NS_.

        """

        db = cantools.database.Database()
        filename = os.path.join(BASE_PATH, 'files/dbc/empty_ns.dbc')

        db.add_dbc_file(filename)
        self.assertEqual(0, len(db.nodes))

    def test_as_kcd_string(self):
        """Test the KCD dump functionality.

        """

        if sys.version_info < (3, 8):
            return

        filename = os.path.join(BASE_PATH, 'files/kcd/dump.kcd')

        db = cantools.database.load_file(filename)

        with open(filename, 'r') as fin:
            self.assertEqual(fin.read(), db.as_kcd_string())

    def test_issue_62(self):
        """Test issue 62.

        """
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_62.dbc')

        db = cantools.database.load_file(filename)
        self.assertEqual(0, len(db.messages))

    def test_issue_63(self):
        """Test issue 63.

        """

        filename = os.path.join(BASE_PATH, 'files/dbc/issue_63.dbc')

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            'The signals HtrRes and MaxRes are overlapping in message '
            'AFT1PSI2.',
            str(cm.exception))

        db = cantools.database.load_file(filename, strict=False)
        self.assertEqual(
            '                      Bit\n'
            '\n'
            '         7   6   5   4   3   2   1   0\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     0 |   |   |<-----x|<-------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '                 |       +-- DetectionStatus\n'
            '                 +-- PwrSupply\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     1 |<-----------------------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '         +-- RegenFailedCount\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     2 |------------------------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            ' B   3 |<------------------------------|\n'
            ' y     +---+---+---+---+---+---+---+---+\n'
            ' t       +-- Temp\n'
            ' e     +---+---+---+---+---+---+---+---+\n'
            '     4 |------------------------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     5 |XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '         +-- MaxRes\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     6 |<------------------------------|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '         +-- HtrRes\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     7 |   |   |   |   |   |   |   |   |\n'
            '       +---+---+---+---+---+---+---+---+',
            db.messages[0].layout_string())

    def test_j1939_dbc(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/j1939.dbc')

        db = cantools.database.load_file(filename)

        self.assertEqual('Message1', db.messages[0].name)
        self.assertEqual(0x15340201, db.messages[0].frame_id)
        self.assertEqual('J1939PG', db.messages[0].frame_format)

        signal = db.messages[0].signals[0]
        self.assertEqual(500, signal.spn)

        signal = db.messages[1].signals[0]
        self.assertEqual(0, signal.spn)

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
            self.assertEqual(data.packed, packed)
            unpacked = cantools.j1939.frame_id_unpack(packed)
            self.assertEqual(data[:-1], unpacked)

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

            self.assertEqual(data.message, str(cm.exception))

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

            self.assertEqual(data.message, str(cm.exception))

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
            self.assertEqual(data.packed, packed)
            unpacked = cantools.j1939.pgn_unpack(packed)
            self.assertEqual(data[:4], unpacked)

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

            self.assertEqual(data.message, str(cm.exception))

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

            self.assertEqual(data.message, str(cm.exception))

    def test_float_dbc(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/floating_point.dbc')

        db = cantools.database.load_file(filename)

        # Double.
        message = db.get_message_by_frame_id(1024)
        signal = message.get_signal_by_name('Signal1')
        self.assertEqual(True, signal.is_float)
        self.assertEqual(64, signal.length)

        decoded_message = {'Signal1': -129.448}
        encoded_message = b'\x75\x93\x18\x04\x56\x2e\x60\xc0'

        encoded = message.encode(decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = message.decode(encoded)
        self.assertEqual(decoded_message, decoded)

        # Float.
        message = db.get_message_by_frame_id(1025)
        signal = message.get_signal_by_name('Signal1')
        self.assertEqual(True, signal.is_float)
        self.assertEqual(32, signal.length)

        decoded_message = {
            'Signal1': 129.5,
            'Signal2': 1234500.5
        }
        encoded_message = b'\x00\x80\x01\x43\x24\xb2\x96\x49'

        encoded = message.encode(decoded_message)
        self.assertEqual(encoded_message, encoded)
        decoded = message.decode(encoded)
        self.assertEqual(decoded_message, decoded)

        self.assert_dbc_dump(db, filename)

    def test_signed_dbc(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/signed.dbc')

        db = cantools.database.load_file(filename)

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
            self.assertEqual(encoded_message, encoded)
            decoded = db.decode_message(name, encoded)
            self.assertEqual(decoded_message, decoded)

    def test_long_names_dbc(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/long_names.dbc')

        db = cantools.database.load_file(filename)

        # Nodes.
        self.assertEqual('NN123456789012345678901234567890123', db.nodes[0].name)
        self.assertEqual('N123456789012345678901234567890123', db.nodes[1].name)
        self.assertEqual('N1234567890123456789012345678901', db.nodes[2].name)
        self.assertEqual('N12345678901234567890123456789012', db.nodes[3].name)

        # Messages.
        self.assertEqual('SS12345678901234567890123458789012345', db.messages[0].name)
        self.assertEqual('SS1234567890123456789012345778901', db.messages[1].name)
        self.assertEqual('SS1234567890123456789012345878901234', db.messages[2].name)
        self.assertEqual('SS123456789012345678901234577890', db.messages[3].name)
        self.assertEqual('SS12345678901234567890123456789012', db.messages[4].name)
        self.assertEqual('S12345678901234567890123456789012', db.messages[5].name)
        self.assertEqual('M123456789012345678901234567890123', db.messages[6].name)
        self.assertEqual('M1234567890123456789012345678901', db.messages[7].name)
        self.assertEqual('M12345678901234567890123456789012', db.messages[8].name)
        self.assertEqual('MM12345678901234567890123456789012', db.messages[9].name)

        self.assertEqual(['N1234567890123456789012345678901'], db.messages[7].senders)
        self.assertEqual(['N12345678901234567890123456789012'], db.messages[8].senders)

        # Signals.
        self.assertEqual('SS12345678901234567890123456789012', db.messages[5].signals[0].name)
        self.assertEqual('SSS12345678901234567890123456789012', db.messages[6].signals[0].name)
        self.assertEqual('S1234567890123456789012345678901', db.messages[7].signals[0].name)
        self.assertEqual('S123456789012345678901234567890123', db.messages[7].signals[1].name)
        self.assertEqual('SS12345678901234567890123456789012', db.messages[7].signals[2].name)
        self.assertEqual('SS1234567890123456789012345678901233', db.messages[7].signals[3].name)
        self.assertEqual('SS12345678901234567890123456789012332', db.messages[7].signals[4].name)
        self.assertEqual('S123456789012345678901234567890123', db.messages[8].signals[0].name)
        self.assertEqual('S12345678901234567890123456789012', db.messages[8].signals[1].name)
        self.assertEqual('SS12345678901234567890123456789012', db.messages[8].signals[2].name)
        self.assertEqual('SS12345678901234567890123456789012dw', db.messages[8].signals[3].name)
        self.assertEqual('SS1234567890123456789012345678901233', db.messages[9].signals[0].name)
        self.assertEqual('SSS12345678901234567890123456789012', db.messages[9].signals[1].name)

        self.assertEqual(['N123456789012345678901234567890123'], db.messages[7].signals[2].receivers)

        # environment variables
        envvar_names = db.dbc.environment_variables
        self.assertTrue('E1234567890123456789012345678901' in envvar_names)
        self.assertFalse('E12345678901234567890123456_0000' in envvar_names)
        self.assertTrue('E12345678901234567890123456789012' in envvar_names)

    def test_illegal_namespace(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-illegal-namespace-4.2.arxml')

        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            db = cantools.db.load_file(filename)

        self.assertEqual(
            'ARXML: "Unrecognized XML namespace \'http://autosar.org/schema/argh4.2.1\'"',
            str(cm.exception))

        root = ElementTree.parse(filename).getroot()
        with self.assertRaises(ValueError) as cm:
            loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=False)

        self.assertEqual(
            'Unrecognized AUTOSAR XML namespace \'http://autosar.org/schema/argh4.2.1\'',
            str(cm.exception))

    def test_illegal_root(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-illegal-root-4.2.arxml')

        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            db = cantools.db.load_file(filename)

        self.assertEqual(
            'ARXML: "No XML namespace specified or illegal root tag name \'{http://autosar.org/schema/r4.2.1}AUTOSARGH\'"',
            str(cm.exception))

        root = ElementTree.parse(filename).getroot()
        with self.assertRaises(ValueError) as cm:
            loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=False)

        self.assertEqual(
            'No XML namespace specified or illegal root tag name \'{http://autosar.org/schema/r4.2.1}AUTOSARGH\'',
            str(cm.exception))

    def test_illegal_version(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-illegal-version-4.2.2.1.0.arxml')

        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            db = cantools.db.load_file(filename)

        self.assertEqual(
            'ARXML: "Could not parse AUTOSAR version \'4.2.2.1.0\'"',
            str(cm.exception))

    def test_arxml_version(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-4.2.arxml')

        root = ElementTree.parse(filename).getroot()
        loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=False)

        self.assertEqual(True, loader.autosar_version_newer(3))
        self.assertEqual(True, loader.autosar_version_newer(4))
        self.assertEqual(False, loader.autosar_version_newer(5))

        self.assertEqual(True, loader.autosar_version_newer(4,1))
        self.assertEqual(True, loader.autosar_version_newer(4,2))
        self.assertEqual(False, loader.autosar_version_newer(4,3))

        self.assertEqual(True, loader.autosar_version_newer(4,2,0))
        self.assertEqual(True, loader.autosar_version_newer(4,2,1))
        self.assertEqual(False, loader.autosar_version_newer(4,2,2))

    def test_system_arxml(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-4.2.arxml')

        db = cantools.db.load_file(filename)

        self.assertEqual(0, len(db.nodes))

        self.assertEqual(4, len(db.messages))

        message_1 = db.messages[0]
        self.assertEqual(5, message_1.frame_id)
        self.assertEqual(False, message_1.is_extended_frame)
        self.assertEqual('Message1', message_1.name)
        self.assertEqual(6, message_1.length)
        self.assertEqual([], message_1.senders)
        self.assertEqual(None, message_1.send_type)
        self.assertEqual(None, message_1.cycle_time)
        self.assertEqual(3, len(message_1.signals))
        self.assertEqual('Kommentar1', message_1.comments["DE"])
        self.assertEqual('Comment1', message_1.comments["EN"])
        self.assertEqual(None, message_1.bus_name)
        self.assertEqual('Comment1', message_1.comment)
        message_1.comments = {'DE': 'Kommentar eins', 'EN': 'Comment one'}
        self.assertEqual({
                             'DE': 'Kommentar eins',
                             'EN' : 'Comment one'
                         },
                         message_1.comments)

        
        signal_1 = message_1.signals[0]
        self.assertEqual('signal6', signal_1.name)
        self.assertEqual(0, signal_1.start)
        self.assertEqual(1, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('little_endian', signal_1.byte_order)
        self.assertEqual(False, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        self.assertEqual(0.1, signal_1.scale)
        self.assertEqual(0.0, signal_1.offset)
        self.assertEqual(0, signal_1.minimum)
        self.assertEqual(1, signal_1.maximum)
        self.assertEqual(Decimal('0.1'), signal_1.decimal.scale)
        self.assertEqual(0.0, signal_1.decimal.offset)
        self.assertEqual(0, signal_1.decimal.minimum)
        self.assertEqual(1, signal_1.decimal.maximum)
        self.assertEqual(None, signal_1.unit)
        self.assertEqual({'zero': 0}, signal_1.choices)
        self.assertEqual(None, signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = message_1.signals[1]
        self.assertEqual('signal1', signal_2.name)
        self.assertEqual(4, signal_2.start)
        self.assertEqual(3, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('big_endian', signal_2.byte_order)
        self.assertEqual(False, signal_2.is_signed)
        self.assertEqual(False, signal_2.is_float)
        self.assertEqual(5.0, signal_2.scale)
        self.assertEqual(0.0, signal_2.offset)
        self.assertEqual(0.0, signal_2.minimum)
        self.assertEqual(4.0, signal_2.maximum)
        self.assertEqual(5.0, signal_2.decimal.scale)
        self.assertEqual(0.0, signal_2.decimal.offset)
        self.assertEqual(0.0, signal_2.decimal.minimum)
        self.assertEqual(4.0, signal_2.decimal.maximum)
        self.assertEqual('m', signal_2.unit)
        self.assertEqual(None, signal_2.choices)
        self.assertEqual('Signal comment!', signal_2.comments["EN"])
        self.assertEqual('Signalkommentar!', signal_2.comments["DE"])
        self.assertEqual('Signal comment!', signal_2.comment)
        signal_2.comments = {'DE': 'Kein Kommentar!', 'EN': 'No comment!'}
        self.assertEqual({
                             'DE': 'Kein Kommentar!',
                             'EN': 'No comment!'
                         },
                         signal_2.comments)

        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        signal_3 = message_1.signals[2]
        self.assertEqual('signal5', signal_3.name)
        self.assertEqual(16, signal_3.start)
        self.assertEqual(32, signal_3.length)
        self.assertEqual([], signal_3.receivers)
        self.assertEqual('little_endian', signal_3.byte_order)
        self.assertEqual(False, signal_3.is_signed)
        self.assertEqual(True, signal_3.is_float)
        self.assertEqual(1, signal_3.scale)
        self.assertEqual(0, signal_3.offset)
        self.assertEqual(None, signal_3.minimum)
        self.assertEqual(None, signal_3.maximum)
        self.assertEqual(1, signal_3.decimal.scale)
        self.assertEqual(0, signal_3.decimal.offset)
        self.assertEqual(None, signal_3.decimal.minimum)
        self.assertEqual(None, signal_3.decimal.maximum)
        self.assertEqual(None, signal_3.unit)
        self.assertEqual(None, signal_3.choices)
        self.assertEqual(None, signal_3.comment)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual(None, signal_3.multiplexer_ids)

        message_2 = db.messages[1]
        self.assertEqual(6, message_2.frame_id)
        self.assertEqual(True, message_2.is_extended_frame)
        self.assertEqual('Message2', message_2.name)
        self.assertEqual(7, message_2.length)
        self.assertEqual([], message_2.senders)
        self.assertEqual(None, message_2.send_type)
        self.assertEqual(200, message_2.cycle_time)
        self.assertEqual(3, len(message_2.signals))
        self.assertEqual(None, message_2.comment)
        self.assertEqual(None, message_2.bus_name)

        signal_1 = message_2.signals[0]
        self.assertEqual('signal3', signal_1.name)
        self.assertEqual(6, signal_1.start)
        self.assertEqual(2, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('little_endian', signal_1.byte_order)
        self.assertEqual(False, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        self.assertEqual(1, signal_1.scale)
        self.assertEqual(0, signal_1.offset)
        self.assertEqual(None, signal_1.minimum)
        self.assertEqual(None, signal_1.maximum)
        self.assertEqual(1, signal_1.decimal.scale)
        self.assertEqual(0, signal_1.decimal.offset)
        self.assertEqual(None, signal_1.decimal.minimum)
        self.assertEqual(None, signal_1.decimal.maximum)
        self.assertEqual(None, signal_1.unit)
        self.assertEqual(None, signal_1.choices)
        self.assertEqual(None, signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = message_2.signals[1]
        self.assertEqual('signal2', signal_2.name)
        self.assertEqual(18, signal_2.start)
        self.assertEqual(11, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('little_endian', signal_2.byte_order)
        self.assertEqual(True, signal_2.is_signed)
        self.assertEqual(False, signal_2.is_float)
        self.assertEqual(1, signal_2.scale)
        self.assertEqual(0, signal_2.offset)
        self.assertEqual(None, signal_2.minimum)
        self.assertEqual(None, signal_2.maximum)
        self.assertEqual(1, signal_2.decimal.scale)
        self.assertEqual(0, signal_2.decimal.offset)
        self.assertEqual(None, signal_2.decimal.minimum)
        self.assertEqual(None, signal_2.decimal.maximum)
        self.assertEqual(None, signal_2.unit)
        self.assertEqual(None, signal_2.choices)
        self.assertEqual('Signal comment!', signal_2.comments["FOR-ALL"])
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        signal_3 = message_2.signals[2]
        self.assertEqual('signal4', signal_3.name)
        self.assertEqual(30, signal_3.start)
        self.assertEqual(4, signal_3.length)
        self.assertEqual([], signal_3.receivers)
        self.assertEqual('little_endian', signal_3.byte_order)
        self.assertEqual(False, signal_3.is_signed)
        self.assertEqual(False, signal_3.is_float)
        self.assertEqual(1, signal_3.scale)
        self.assertEqual(0, signal_3.offset)
        self.assertEqual(0, signal_3.minimum)
        self.assertEqual(3, signal_3.maximum)
        self.assertEqual(1, signal_3.decimal.scale)
        self.assertEqual(0, signal_3.decimal.offset)
        self.assertEqual(0, signal_3.decimal.minimum)
        self.assertEqual(3, signal_3.decimal.maximum)
        self.assertEqual(None, signal_3.unit)
        self.assertEqual({'one': 1, 'two': 2}, signal_3.choices)
        self.assertEqual(None, signal_3.comment)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual(None, signal_3.multiplexer_ids)

        message_3 = db.messages[2]
        self.assertEqual(100, message_3.frame_id)
        self.assertEqual(False, message_3.is_extended_frame)
        self.assertEqual('Message3', message_3.name)
        self.assertEqual(8, message_3.length)
        self.assertEqual([], message_3.senders)
        self.assertEqual(None, message_3.send_type)
        self.assertEqual(None, message_3.cycle_time)
        self.assertEqual(0, len(message_3.signals))
        self.assertEqual(None, message_3.comment)
        self.assertEqual(None, message_3.bus_name)

        # message 4 tests different base encodings
        message_4 = db.messages[3]
        self.assertEqual(101, message_4.frame_id)
        self.assertEqual(False, message_4.is_extended_frame)
        self.assertEqual('Message4', message_4.name)
        self.assertEqual(6, message_4.length)
        self.assertEqual([], message_4.senders)
        self.assertEqual(None, message_4.send_type)
        self.assertEqual(None, message_4.cycle_time)
        self.assertEqual(3, len(message_4.signals))
        self.assertEqual(None, message_4.comment)
        self.assertEqual(None, message_4.bus_name)

        signal_2 = message_4.signals[0]
        self.assertEqual('signal2', signal_2.name)
        self.assertEqual(True, signal_2.is_signed)
        self.assertEqual(False, signal_2.is_float)

        signal_2_1c = message_4.signals[1]
        self.assertEqual('signal2_1c', signal_2_1c.name)
        self.assertEqual(True, signal_2_1c.is_signed)
        self.assertEqual(False, signal_2_1c.is_float)

        signal_2_sm = message_4.signals[2]
        self.assertEqual('signal2_sm', signal_2_sm.name)
        self.assertEqual(True, signal_2_sm.is_signed)
        self.assertEqual(False, signal_2_sm.is_float)

    def test_system_arxml_traversal(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-dangling-reference-4.2.arxml')

        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename)
        self.assertEqual("ARXML: \"Encountered dangling reference FRAME-REF: /PackageDoesNotExist/Message1\"", str(cm.exception))

        filename = os.path.join(BASE_PATH, 'files/arxml/system-4.2.arxml')

        root = ElementTree.parse(filename).getroot()
        loader = cantools.db.can.formats.arxml.SystemLoader(root, strict=True)

        # a base node must always be specified
        with self.assertRaises(ValueError) as cm:
            no_base_elem = loader._get_arxml_children(None, ["AR-PACKAGES", "*AR-PACKAGE"])
        self.assertEqual("Cannot retrieve a child element of a non-existing node!", str(cm.exception))

        # test multiple child node matches
        children1 = loader._get_arxml_children(loader._root, ["AR-PACKAGES", "*AR-PACKAGE"])
        childen1_short_names = \
            list(map(lambda x: x.find("ns:SHORT-NAME", loader._xml_namespaces).text, children1))

        self.assertEqual([
                             'Cluster',
                             'CanFrame',
                             'ISignal',
                             'ISignalIPdu',
                             'Unit',
                             'CompuMethod',
                             'SystemSignal',
                             'SwBaseType'
                         ],
                         childen1_short_names)

        # test unique location specifier if child nodes exist
        with self.assertRaises(ValueError) as cm:
            non_unique = loader._get_arxml_children(loader._root, ["AR-PACKAGES", "AR-PACKAGE"])
        self.assertEqual("Encountered a a non-unique child node of type AR-PACKAGE which ought to be unique", str(cm.exception))

        # test the reference cache
        foo = loader._follow_arxml_reference(loader._root, "/CanFrame/Message1", "CAN-FRAME")
        bar = loader._follow_arxml_reference(loader._root, "/CanFrame/Message1", "CAN-FRAME")
        self.assertEqual(bar, foo)

        # test non-unique location while assuming that it is unque
        with self.assertRaises(ValueError) as cm:
            no_base_elem = loader._get_unique_arxml_child(loader._root, ["AR-PACKAGES", "*AR-PACKAGE"])
        self.assertEqual("['AR-PACKAGES', '*AR-PACKAGE'] does not resolve into a unique node", str(cm.exception))
        
    def test_system_missing_factor_arxml(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-missing-factor-4.2.arxml')

        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename)

        self.assertEqual(
            'ARXML: "Expected 2 numerator values for linear scaling, but got 1."',
            str(cm.exception))

    def test_system_missing_denominator_arxml(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-missing-denominator-4.2.arxml')

        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename)

        self.assertEqual(
            'ARXML: "Expected 1 denominator value for linear scaling, but got 0."',
            str(cm.exception))

    def test_system_missing_rational_arxml(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-missing-rational-4.2.arxml')

        db = cantools.db.load_file(filename)

        signal_1 = db.messages[0].signals[0]
        self.assertEqual(1.0, signal_1.scale)
        self.assertEqual(0.0, signal_1.offset)
        self.assertEqual(0.0, signal_1.minimum)
        self.assertEqual(4.0, signal_1.maximum)
        self.assertEqual(1.0, signal_1.decimal.scale)
        self.assertEqual(0.0, signal_1.decimal.offset)
        self.assertEqual(0.0, signal_1.decimal.minimum)
        self.assertEqual(4.0, signal_1.decimal.maximum)

    def test_system_bad_root_tag(self):
        filename = os.path.join(BASE_PATH, 'files/arxml/system-bad-root-tag-4.2.arxml')

        with self.assertRaises(UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename)

        self.assertEqual(
            'ARXML: "No XML namespace specified or illegal root tag name \'{http://autosar.org/schema/r4.0}NOT-AUTOSAR\'"',
            str(cm.exception))

    def test_ecu_extract_arxml(self):
        db = cantools.database.Database()
        filename = os.path.join(BASE_PATH, 'files/arxml/ecu-extract-4.2.arxml')

        db.add_arxml_file(filename)

        self.assertEqual(0, len(db.nodes))

        self.assertEqual(3, len(db.messages))

        message_1 = db.messages[0]
        self.assertEqual(5, message_1.frame_id)
        self.assertEqual(False, message_1.is_extended_frame)
        self.assertEqual('Message1', message_1.name)
        self.assertEqual(6, message_1.length)
        self.assertEqual([], message_1.senders)
        self.assertEqual(None, message_1.send_type)
        self.assertEqual(None, message_1.cycle_time)
        self.assertEqual(2, len(message_1.signals))
        self.assertEqual(None, message_1.comment)  # ToDo: 'Comment1'
        self.assertEqual(None, message_1.bus_name)

        signal_1 = message_1.signals[0]
        self.assertEqual('Signal1', signal_1.name)
        self.assertEqual(4, signal_1.start)
        self.assertEqual(3, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('big_endian', signal_1.byte_order)
        self.assertEqual(False, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        # Signal scale, offset, minimum and maximum are not part of an
        # ECU extract it seems.
        self.assertEqual(1, signal_1.scale)
        self.assertEqual(0.0, signal_1.offset)
        self.assertEqual(None, signal_1.minimum)
        self.assertEqual(None, signal_1.maximum)
        self.assertEqual(1, signal_1.decimal.scale)
        self.assertEqual(0, signal_1.decimal.offset)
        self.assertEqual(None, signal_1.decimal.minimum)
        self.assertEqual(None, signal_1.decimal.maximum)
        self.assertEqual(None, signal_1.unit)
        self.assertEqual(None, signal_1.choices)
        self.assertEqual(None, signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = message_1.signals[1]
        self.assertEqual('Signal5', signal_2.name)
        self.assertEqual(16, signal_2.start)
        self.assertEqual(32, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('little_endian', signal_2.byte_order)
        self.assertEqual(False, signal_2.is_signed)
        self.assertEqual(True, signal_2.is_float)
        self.assertEqual(1, signal_2.scale)
        self.assertEqual(0, signal_2.offset)
        self.assertEqual(None, signal_2.minimum)
        self.assertEqual(None, signal_2.maximum)
        self.assertEqual(1, signal_2.decimal.scale)
        self.assertEqual(0, signal_2.decimal.offset)
        self.assertEqual(None, signal_2.decimal.minimum)
        self.assertEqual(None, signal_2.decimal.maximum)
        self.assertEqual(None, signal_2.unit)
        self.assertEqual(None, signal_2.choices)
        self.assertEqual(None, signal_2.comment)
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        message_2 = db.messages[1]
        self.assertEqual(6, message_2.frame_id)
        self.assertEqual(True, message_2.is_extended_frame)
        self.assertEqual('Message2', message_2.name)
        self.assertEqual(7, message_2.length)
        self.assertEqual([], message_2.senders)
        self.assertEqual(None, message_2.send_type)
        self.assertEqual(None, message_2.cycle_time)
        self.assertEqual(3, len(message_2.signals))
        self.assertEqual(None, message_2.comment)
        self.assertEqual(None, message_2.bus_name)

        signal_1 = message_2.signals[0]
        self.assertEqual('Signal3', signal_1.name)
        self.assertEqual(6, signal_1.start)
        self.assertEqual(2, signal_1.length)
        self.assertEqual([], signal_1.receivers)
        self.assertEqual('little_endian', signal_1.byte_order)
        self.assertEqual(False, signal_1.is_signed)
        self.assertEqual(False, signal_1.is_float)
        self.assertEqual(1, signal_1.scale)
        self.assertEqual(0, signal_1.offset)
        self.assertEqual(None, signal_1.minimum)
        self.assertEqual(None, signal_1.maximum)
        self.assertEqual(1, signal_1.decimal.scale)
        self.assertEqual(0, signal_1.decimal.offset)
        self.assertEqual(None, signal_1.decimal.minimum)
        self.assertEqual(None, signal_1.decimal.maximum)
        self.assertEqual(None, signal_1.unit)
        self.assertEqual(None, signal_1.choices)
        self.assertEqual(None, signal_1.comment)
        self.assertEqual(False, signal_1.is_multiplexer)
        self.assertEqual(None, signal_1.multiplexer_ids)

        signal_2 = message_2.signals[1]
        self.assertEqual('Signal2', signal_2.name)
        self.assertEqual(18, signal_2.start)
        self.assertEqual(11, signal_2.length)
        self.assertEqual([], signal_2.receivers)
        self.assertEqual('little_endian', signal_2.byte_order)
        self.assertEqual(True, signal_2.is_signed)
        self.assertEqual(False, signal_2.is_float)
        self.assertEqual(1, signal_2.scale)
        self.assertEqual(0, signal_2.offset)
        self.assertEqual(None, signal_2.minimum)
        self.assertEqual(None, signal_2.maximum)
        self.assertEqual(1, signal_2.decimal.scale)
        self.assertEqual(0, signal_2.decimal.offset)
        self.assertEqual(None, signal_2.decimal.minimum)
        self.assertEqual(None, signal_2.decimal.maximum)
        self.assertEqual(None, signal_2.unit)
        self.assertEqual(None, signal_2.choices)
        # Signal comment is not part of an ECU extract it seems.
        self.assertEqual(None, signal_2.comment)
        self.assertEqual(False, signal_2.is_multiplexer)
        self.assertEqual(None, signal_2.multiplexer_ids)

        signal_3 = message_2.signals[2]
        self.assertEqual('Signal4', signal_3.name)
        self.assertEqual(30, signal_3.start)
        self.assertEqual(4, signal_3.length)
        self.assertEqual([], signal_3.receivers)
        self.assertEqual('little_endian', signal_3.byte_order)
        self.assertEqual(False, signal_3.is_signed)
        self.assertEqual(False, signal_3.is_float)
        # Signal scale, offset, minimum, maximum and choices are not
        # part of an ECU extract it seems.
        self.assertEqual(1, signal_3.scale)
        self.assertEqual(0, signal_3.offset)
        self.assertEqual(None, signal_3.minimum)
        self.assertEqual(None, signal_3.maximum)
        self.assertEqual(1, signal_3.decimal.scale)
        self.assertEqual(0, signal_3.decimal.offset)
        self.assertEqual(None, signal_3.decimal.minimum)
        self.assertEqual(None, signal_3.decimal.maximum)
        self.assertEqual(None, signal_3.unit)
        self.assertEqual(None, signal_3.choices)
        self.assertEqual(None, signal_3.comment)
        self.assertEqual(False, signal_3.is_multiplexer)
        self.assertEqual(None, signal_3.multiplexer_ids)

        message_3 = db.messages[2]
        self.assertEqual(100, message_3.frame_id)
        self.assertEqual(False, message_3.is_extended_frame)
        self.assertEqual('Message3', message_3.name)
        self.assertEqual(8, message_3.length)
        self.assertEqual([], message_3.senders)
        self.assertEqual(None, message_3.send_type)
        self.assertEqual(None, message_3.cycle_time)
        self.assertEqual(0, len(message_3.signals))
        self.assertEqual(None, message_3.comment)
        self.assertEqual(None, message_3.bus_name)

    def test_encode_decode_dlc_zero(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/message-dlc-zero.dbc')

        db = cantools.database.load_file(filename)

        self.assertEqual(b'', db.encode_message('Message1', {}))
        self.assertEqual({}, db.decode_message('Message1', b''))

    def test_issue_138(self):
        """Test issue 138.

        """

        filename = os.path.join(BASE_PATH, 'files/sym/issue_138.sym')

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            'The signal M length 0 is not greater than 0 in message Status.',
            str(cm.exception))

    def test_multiple_senders(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/multiple_senders.dbc')

        db = cantools.database.load_file(filename)
        message = db.get_message_by_frame_id(1)
        self.assertEqual(['FOO', 'BAR', 'FIE'], message.senders)
        self.assert_dbc_dump(db, filename)

    def test_issue_168_upper_case_file_extension(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_168.dbc')

        db = cantools.db.load_file(filename)

        message = db.get_message_by_name('Foo')
        self.assertEqual('Foo', message.name)

    def test_issue_163_dbc_newlines(self):
        filename_in = os.path.join(BASE_PATH, 'files/dbc/issue_163_newline.dbc')

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

        self.assertEqual(2, len(db.nodes))
        self.assertEqual('NODE_short', db.nodes[0].name)
        self.assertEqual('NODE56789_abcdefghi_ABCDEFGHI_XYZ', db.nodes[1].name)

        self.assertEqual(2, len(db.messages))

        message = db.messages[0]
        self.assertEqual('MSG456789_123456789_123456789_ABC', message.name)
        self.assertEqual(['NODE56789_abcdefghi_ABCDEFGHI_XYZ'], message.senders)
        self.assertEqual('SIG456789_123456789_123456789_ABC', message.signals[0].name)

        message = db.messages[1]
        self.assertEqual('MSG_short', message.name)
        self.assertEqual(['NODE_short'], message.senders)
        self.assertEqual('SIG_short', message.signals[0].name)

    def test_long_names_multiple_relations(self):
        """Test if long names are resolved correctly.

        """
        filename = os.path.join(BASE_PATH, 'files/dbc/long_names_multiple_relations.dbc')
        filename_dumped = os.path.join(BASE_PATH, 'files/dbc/long_names_multiple_relations_dumped.dbc')

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
        filename_src = os.path.join(BASE_PATH, 'files/dbc/mod_name_len_src.dbc')
        filename_dest = os.path.join(BASE_PATH, 'files/dbc/mod_name_len_dest.dbc')

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
        filename = os.path.join(BASE_PATH, 'files/dbc/sig_groups.dbc')

        db = cantools.database.load_file(filename)

        message = db.get_message_by_name('SGMsg_m')
        self.assertEqual(2, len(message.signal_groups))

        signal_group = message.signal_groups[0]
        self.assertEqual('Sub2', signal_group.name)
        self.assertEqual(1, signal_group.repetitions)
        self.assertEqual(['dupsig', 'subSig2_1'], signal_group.signal_names)

        signal_group = message.signal_groups[1]
        self.assertEqual('sub1', signal_group.name)
        self.assertEqual(1, signal_group.repetitions)
        self.assertEqual(['dupsig', 'subSig1_2', 'subSig1_1'], signal_group.signal_names)

        self.assert_dbc_dump(db, filename)

        # Delete all signal groups.
        for message in db.messages:
            message.signal_groups = None

        filename = os.path.join(BASE_PATH, 'files/dbc/sig_groups_del.dbc')
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

        filename = os.path.join(BASE_PATH, 'files/dbc/sig_groups_out.dbc')

        self.assert_dbc_dump(db, filename)

    def test_dbc_issue_228_empty_signal_groups(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_228.dbc')

        db = cantools.database.load_file(filename)
        message = db.get_message_by_name('SGMsg')
        signal_group = message.signal_groups[0]
        self.assertEqual('Empty_Signal_Group', signal_group.name)
        self.assertEqual(1, signal_group.repetitions)
        self.assertEqual([], signal_group.signal_names)

    def test_dbc_issue_199_more_than_11_bits_standard_frame_id(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_199.dbc')

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            'Standard frame id 0x10630000 is more than 11 bits in message '
            'DriverDoorStatus.',
            str(cm.exception))

    def test_dbc_issue_199_more_than_29_bits_extended_frame_id(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_199_extended.dbc')

        with self.assertRaises(cantools.database.errors.Error) as cm:
            cantools.database.load_file(filename)

        self.assertEqual(
            'Extended frame id 0x7fffffff is more than 29 bits in message '
            'DriverDoorStatus.',
            str(cm.exception))

    def test_issue_207_tolerate_plus_in_dbc_sig_def(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_207_sig_plus.dbc')

        db = cantools.database.load_file(filename)
        message = db.messages[0]

        for signal in message.signals:
            self.assertEqual(-128, signal.minimum)
            self.assertEqual(127, signal.maximum)

    def test_issue_184_multiple_mux_values(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_184_extended_mux_multiple_values.dbc')

        db = cantools.database.load_file(filename)

        self.assertEqual([
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
                         ],
                         db.messages[0].signal_tree)

        filename = os.path.join(BASE_PATH, 'files/dbc/issue_184_extended_mux_multiple_values_dumped.dbc')

        self.assert_dbc_dump(db, filename)

    def test_issue_184_independent_multiplexors(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_184_extended_mux_independent_multiplexors.dbc')

        db = cantools.database.load_file(filename)

        self.assertEqual([
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
                         ],
                         db.messages[0].signal_tree)

        filename = os.path.join(BASE_PATH, 'files/dbc/issue_184_extended_mux_independent_multiplexors_dumped.dbc')

        self.assert_dbc_dump(db, filename)

    def test_issue_184_cascaded_multiplexors(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/issue_184_extended_mux_cascaded.dbc')

        db = cantools.database.load_file(filename)

        self.assertEqual([
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
                         ],
                         db.messages[0].signal_tree)

        filename = os.path.join(BASE_PATH, 'files/dbc/issue_184_extended_mux_cascaded_dumped.dbc')

        self.assert_dbc_dump(db, filename)

    def test_bus_comment(self):
        filename = os.path.join(BASE_PATH, 'files/dbc/bus_comment.dbc')

        db = cantools.database.load_file(filename)
        self.assertEqual('SpecialRelease', db.buses[0].comment)
        self.assert_dbc_dump(db, filename)

# This file is not '__main__' when executed via 'python setup.py3
# test'.
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    unittest.main()
