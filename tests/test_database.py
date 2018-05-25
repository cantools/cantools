import math
import os
import unittest

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


class CanToolsDatabaseTest(unittest.TestCase):

    maxDiff = None

    def test_vehicle(self):
        filename = os.path.join('tests', 'files', 'vehicle.dbc')
        db = cantools.db.load_file(filename)
        self.assertEqual(len(db.nodes), 1)
        self.assertEqual(db.nodes[0].name, 'Vector__XXX')
        self.assertEqual(len(db.messages), 217)
        self.assertEqual(db.messages[216].frame_id, 155872546)
        self.assertEqual(db.messages[216].senders, ['Vector__XXX'])
        self.assertEqual(str(db.messages[0]),
                         "message('RT_SB_INS_Vel_Body_Axes', 0x9588322, False, 8, None)")
        self.assertEqual(repr(db.messages[0].signals[0]),
                         "signal('Validity_INS_Vel_Forwards', 0, 1, 'little_endian', "
                         "False, 1, 0, 0, 1, 'None', False, None, None, 'Valid when "
                         "bit is set, invalid when bit is clear.')")
        self.assertEqual(db.messages[0].signals[0].receivers, ['Vector__XXX'])
        self.assertEqual(db.messages[0].cycle_time, None)
        self.assertEqual(db.messages[0].send_type, None)
        self.assertEqual(repr(db.nodes[0]), "node('Vector__XXX', None)")
        i = 0

        for message in db.messages:
            for signal in message.signals:
                if signal.choices is not None:
                    i += 1

        self.assertEqual(i, 15)

        with open(filename, 'r') as fin:
            self.assertEqual(db.as_dbc_string(), fin.read())

    def test_motohawk(self):
        filename = os.path.join('tests', 'files', 'motohawk.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        self.assertEqual(len(db.nodes), 2)
        self.assertEqual(db.nodes[0].name, 'PCM1')
        self.assertEqual(db.nodes[1].name, 'FOO')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(len(db.messages[0].signals[2].receivers), 2)
        self.assertEqual(db.messages[0].signals[2].receivers[0], 'Vector__XXX')
        self.assertEqual(db.messages[0].signals[2].receivers[1], 'FOO')
        self.assertEqual(db.messages[0].signals[1].receivers[0], 'Vector__XXX')

        with open(filename, 'r') as fin:
            self.assertEqual(db.as_dbc_string(), fin.read())

    def test_emc32(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'emc32.dbc')

        with open(filename, 'r') as fin:
            db.add_dbc(fin)

        self.assertEqual(len(db.nodes), 1)
        self.assertEqual(db.nodes[0].name, 'EMV_Statusmeldungen')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(len(db.messages[0].signals[0].receivers), 1)

    def test_foobar(self):
        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'foobar.dbc')
        db.add_dbc_file(filename)

        self.assertEqual(len(db.nodes), 3)
        self.assertEqual(db.version, '2.0')
        self.assertEqual(repr(db),
                         "version('2.0')\n"
                         "\n"
                         "node('FOO', None)\n"
                         "node('BAR', 'fam \"1\"')\n"
                         "node('FIE', None)\n"
                         "\n"
                         "message('Foo', 0x12331, True, 8, 'Foo.')\n"
                         "  signal('Bar', 6, 6, 'big_endian', False, 0.1, "
                         "0, 1.0, 5.0, 'm', False, None, None, '')\n"
                         "  signal('Foo', 0, 12, 'big_endian', True, 0.01, "
                         "250, 229.53, 270.47, 'degK', False, None, {-1: \'Foo\', "
                         "-2: \'Fie\'}, None)\n"
                         "\n"
                         "message('Fum', 0x12331, True, 5, 'Foo.')\n"
                         "  signal('Fum', 0, 12, 'little_endian', True, 1, 0, 0, 1, "
                         "'None', False, None, None, None)\n"
                         "  signal('Fam', 12, 12, 'little_endian', True, 1.0, 0.0, "
                         "0, 1, 'None', False, None, None, None)\n"
                         "\n"
                         "message('Bar', 0x12332, True, 4, None)\n"
                         "  signal('Binary32', 0, 32, 'little_endian', True, 1, 0, 0, "
                         "0, 'None', False, None, None, None)\n"
                         "\n"
                         "message('CanFd', 0x12333, True, 64, None)\n"
                         "  signal('Foo', 0, 512, 'little_endian', True, 1, 0, 0, 0, "
                         "'None', False, None, None, None)\n")

        message = db.get_message_by_frame_id(0x12331)
        self.assertEqual(message.name, 'Fum')
        self.assertEqual(message.senders, ['FOO'])
        self.assertEqual(message.signals[0].is_float, False)

        message = db.get_message_by_frame_id(0x12332)
        self.assertEqual(message.name, 'Bar')
        self.assertEqual(message.senders, ['FOO', 'BAR'])
        self.assertEqual(message.signals[0].receivers, ['Vector__XXX', 'FUM'])
        self.assertEqual(message.signals[0].is_float, True)
        self.assertEqual(message.signals[0].length, 32)

        message = db.get_message_by_frame_id(0x12333)
        self.assertEqual(message.name, 'CanFd')
        self.assertEqual(message.senders, ['FOO'])
        self.assertEqual(message.signals[0].receivers, ['Vector__XXX', 'FUM'])
        self.assertEqual(message.signals[0].is_float, False)
        self.assertEqual(message.signals[0].length, 512)

    def test_foobar_encode_decode(self):
        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'foobar.dbc')
        db.add_dbc_file(filename)

        messages = [
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
                {'Foo': 98723498729384782349872340000000},
                b'\x00\xdd\x85\x4c\xf5\x42\x25\x72\x00\x27\xd4\x10\xde\x04\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00'
                b'\x00\x00\x00\x00'
            )
        ]

        for name, decoded_message, encoded_message in messages:
            encoded = db.encode_message(name, decoded_message)
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message(name, encoded)
            self.assertEqual(decoded, decoded_message)

    def test_foobar_decode_masked_frame_id(self):
        db = cantools.db.Database(frame_id_mask=0xff)
        filename = os.path.join('tests', 'files', 'foobar.dbc')
        db.add_dbc_file(filename)

        frame_ids = [
            0x12331,
            0xfff31,
            0x00031
        ]

        for frame_id in frame_ids:
            db.get_message_by_frame_id(frame_id)

    def test_padding_bit_order(self):
        """Encode and decode signals with reversed bit order.

        """

        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'padding_bit_order.dbc')
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
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
        db.add_dbc_file(filename)

        example_message_name = 'ExampleMessage'
        example_message_frame_id = 496

        # Encode with non-enumerated values.
        decoded_message = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 1
        }
        encoded_message = b'\xc0\x06\xe0\x00\x00\x00\x00\x00'

        encoded = db.encode_message(example_message_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)

        # Encode with enumerated values.
        decoded_message = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 'Enabled'
        }

        # By frame id.
        encoded = db.encode_message(example_message_frame_id, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(example_message_frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # By name.
        encoded = db.encode_message(example_message_name, decoded_message)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(example_message_name, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_big_endian_no_decode_choices(self):
        """Decode a big endian signal with `decode_choices` set to False.

        """

        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
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
        self.assertEqual(decoded, decoded_message)

    def test_little_endian_no_decode_choices(self):
        """Decode a little endian signal with `decode_choices` set to False.

        """

        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'socialledge.dbc')
        db.add_dbc_file(filename)

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
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
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
        self.assertEqual(encoded, encoded_message)

        decoded = db.decode_message(496,
                                    encoded,
                                    scaling=False)
        self.assertEqual(decoded, decoded_message)

    def test_encode_decode_no_scaling_no_decode_choices(self):
        """Encode and decode a message without scaling the signal values, not
        decoding choices.

        """

        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
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
        self.assertEqual(encoded, encoded_message)

        decoded = db.decode_message(496,
                                    encoded,
                                    decode_choices=False,
                                    scaling=False)
        self.assertEqual(decoded, decoded_message)

    def test_socialledge(self):
        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'socialledge.dbc')
        db.add_dbc_file(filename)

        # Verify nodes.
        self.assertEqual(len(db.nodes), 5)
        self.assertEqual(db.nodes[0].name, 'DBG')
        self.assertEqual(db.nodes[0].comment, None)
        self.assertEqual(db.nodes[1].name, 'DRIVER')
        self.assertEqual(db.nodes[1].comment,
                         'The driver controller driving the car')
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
        filename = os.path.join('tests', 'files', 'socialledge.dbc')
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
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_socialledge_encode_decode_mux_1(self):
        """Encode and decode the signals in a SENSOR_SONARS frame with mux 1.

        """

        db = cantools.db.Database()
        filename = os.path.join('tests', 'files', 'socialledge.dbc')
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
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_add_message(self):
        db = cantools.db.Database()

        signals = [
            cantools.db.Signal(name='signal',
                               start=0,
                               length=4,
                               receivers=['foo'],
                               byte_order='big_endian',
                               scale=1.0,
                               offset=10,
                               minimum=10.0,
                               maximum=100.0,
                               unit='m/s')
        ]

        message = cantools.db.Message(frame_id=37,
                                      name='message',
                                      length=8,
                                      senders=['bar'],
                                      signals=signals,
                                      comment='')
        db.add_message(message)
        self.assertEqual(len(db.messages), 1)

    def test_get_message_by_frame_id_and_name(self):
        filename = os.path.join('tests', 'files', 'motohawk.dbc')

        with open(filename, 'r') as fin:
            db = cantools.db.load(fin)

        message = db.get_message_by_name('ExampleMessage')
        self.assertEqual(message.name, 'ExampleMessage')

        message = db.get_message_by_frame_id(496)
        self.assertEqual(message.frame_id, 496)

    def test_get_signal_by_name(self):
        filename = os.path.join('tests', 'files', 'foobar.dbc')
        db = cantools.db.load_file(filename)

        message = db.get_message_by_name('Foo')

        signal = message.get_signal_by_name('Foo')
        self.assertEqual(signal.name, 'Foo')

        signal = message.get_signal_by_name('Bar')
        self.assertEqual(signal.name, 'Bar')

        with self.assertRaises(KeyError) as cm:
            message.get_signal_by_name('Fum')

        self.assertEqual(str(cm.exception), "'Fum'")

    def test_command_line_decode(self):
        argv = ['cantools', 'decode', 'tests/files/socialledge.dbc']
        input_data = """\
  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
  vcan0  064   [10]  F0 01 FF FF FF FF FF FF FF FF
  vcan0  ERROR

  vcan0  1F4   [4]  01 02 03 04
  vcan0  1F4   [3]  01 02 03
  vcan0  1F3   [3]  01 02 03
"""

        expected_output = """\
  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00 ::
SENSOR_SONARS(
    SENSOR_SONARS_mux: 0,
    SENSOR_SONARS_err_count: 15,
    SENSOR_SONARS_left: 0.0,
    SENSOR_SONARS_middle: 0.0,
    SENSOR_SONARS_right: 0.0,
    SENSOR_SONARS_rear: 0.0
)
  vcan0  064   [10]  F0 01 FF FF FF FF FF FF FF FF ::
DRIVER_HEARTBEAT(
    DRIVER_HEARTBEAT_cmd: 240
)
  vcan0  ERROR

  vcan0  1F4   [4]  01 02 03 04 ::
IO_DEBUG(
    IO_DEBUG_test_unsigned: 1,
    IO_DEBUG_test_enum: 'IO_DEBUG_test2_enum_two',
    IO_DEBUG_test_signed: 3,
    IO_DEBUG_test_float: 2.0
)
  vcan0  1F4   [3]  01 02 03 :: unpack requires at least 32 bits to unpack (got 24)
  vcan0  1F3   [3]  01 02 03 :: Unknown frame id 499 (0x1f3)
"""

        stdout = StringIO()

        with patch('sys.stdin', StringIO(input_data)):
            with patch('sys.stdout', stdout):
                with patch('sys.argv', argv):
                    cantools._main()
                    actual_output = stdout.getvalue()
                    self.assertEqual(actual_output, expected_output)

    def test_command_line_single_line_decode(self):
        argv = [
            'cantools',
            'decode',
            '--single-line',
            'tests/files/socialledge.dbc'
        ]

        input_data = """\
  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
  vcan0  064   [10]  F0 01 FF FF FF FF FF FF FF FF
  vcan0  ERROR

  vcan0  1F4   [4]  01 02 03 04
  vcan0  1F4   [3]  01 02 03
  vcan0  1F3   [3]  01 02 03
"""

        expected_output = """\
  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00 :: SENSOR_SONARS(SENSOR_SONARS_mux: 0, SENSOR_SONARS_err_count: 15, SENSOR_SONARS_left: 0.0, SENSOR_SONARS_middle: 0.0, SENSOR_SONARS_right: 0.0, SENSOR_SONARS_rear: 0.0)
  vcan0  064   [10]  F0 01 FF FF FF FF FF FF FF FF :: DRIVER_HEARTBEAT(DRIVER_HEARTBEAT_cmd: 240)
  vcan0  ERROR

  vcan0  1F4   [4]  01 02 03 04 :: IO_DEBUG(IO_DEBUG_test_unsigned: 1, IO_DEBUG_test_enum: 'IO_DEBUG_test2_enum_two', IO_DEBUG_test_signed: 3, IO_DEBUG_test_float: 2.0)
  vcan0  1F4   [3]  01 02 03 :: unpack requires at least 32 bits to unpack (got 24)
  vcan0  1F3   [3]  01 02 03 :: Unknown frame id 499 (0x1f3)
"""

        stdout = StringIO()

        with patch('sys.stdin', StringIO(input_data)):
            with patch('sys.stdout', stdout):
                with patch('sys.argv', argv):
                    cantools._main()
                    actual_output = stdout.getvalue()
                    self.assertEqual(actual_output, expected_output)

    def test_the_homer(self):
        filename = os.path.join('tests', 'files', 'the_homer.kcd')
        db = cantools.db.load_file(filename)

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

        self.assertEqual(len(db.messages), 27)
        self.assertEqual(db.messages[0].frame_id, 0xa)
        self.assertEqual(db.messages[0].is_extended_frame, False)
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
        self.assertEqual(tank_temperature.unit, 'Cel')
        self.assertEqual(tank_temperature.choices, None)
        self.assertEqual(tank_temperature.comment, None)

        speed_km = db.messages[1].signals[1]

        self.assertEqual(speed_km.name, 'SpeedKm')
        self.assertEqual(speed_km.start, 30)
        self.assertEqual(speed_km.length, 24)
        self.assertEqual(speed_km.receivers, [])
        self.assertEqual(speed_km.byte_order, 'little_endian')
        self.assertEqual(speed_km.is_signed, False)
        self.assertEqual(speed_km.is_float, False)
        self.assertEqual(speed_km.scale, 0.2)
        self.assertEqual(speed_km.offset, 0)
        self.assertEqual(speed_km.minimum, None)
        self.assertEqual(speed_km.maximum, None)
        self.assertEqual(speed_km.unit, 'km/h')
        self.assertEqual(speed_km.choices, {16777215: 'invalid'})
        self.assertEqual(speed_km.comment,
                         'Middle speed of front wheels in kilometers per hour.')

        outside_temp = db.messages[1].signals[0]

        self.assertEqual(outside_temp.name, 'OutsideTemp')
        self.assertEqual(outside_temp.start, 18)
        self.assertEqual(outside_temp.length, 12)
        self.assertEqual(outside_temp.receivers, [])
        self.assertEqual(outside_temp.byte_order, 'big_endian')
        self.assertEqual(outside_temp.is_signed, False)
        self.assertEqual(outside_temp.is_float, False)
        self.assertEqual(outside_temp.scale, 0.05)
        self.assertEqual(outside_temp.offset, -40)
        self.assertEqual(outside_temp.minimum, 0)
        self.assertEqual(outside_temp.maximum, 100)
        self.assertEqual(outside_temp.unit, 'Cel')
        self.assertEqual(outside_temp.choices, {0: 'init'})
        self.assertEqual(outside_temp.comment, 'Outside temperature.')

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

    def test_the_homer_encode_length(self):
        filename = os.path.join('tests', 'files', 'the_homer.kcd')
        db = cantools.db.Database()
        db.add_kcd_file(filename)

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
        filename = os.path.join('tests', 'files', 'the_homer.kcd')
        db = cantools.db.Database()
        db.add_kcd_file(filename)

        # Message 1 (binary64).
        frame_id = 0x832

        decoded_message = {'AmbientLux': math.pi}
        encoded_message = b'\x18\x2d\x44\x54\xfb\x21\x09\x40'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(len(encoded), 8)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

        # Message 2 (binary32).
        frame_id = 0x845

        decoded_message = {'Windshield': 3.1415927410125732}
        encoded_message = b'\xdb\x0f\x49\x40'

        encoded = db.encode_message(frame_id, decoded_message)
        self.assertEqual(len(encoded), 4)
        self.assertEqual(encoded, encoded_message)
        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, decoded_message)

    def test_the_homer_encode_decode_choices(self):
        filename = os.path.join('tests', 'files', 'the_homer.kcd')
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
            self.assertEqual(encoded, encoded_message)
            decoded = db.decode_message('Gear', encoded)
            self.assertEqual(decoded, decoded_message)

    def test_empty_kcd(self):
        filename = os.path.join('tests', 'files', 'empty.kcd')
        db = cantools.db.load_file(filename)

        self.assertEqual(db.version, None)
        self.assertEqual(db.nodes, [])

    def test_jopp_5_0_sym(self):
        filename = os.path.join('tests', 'files', 'jopp-5.0.sym')
        db = cantools.db.Database()

        with self.assertRaises(cantools.db.ParseError) as cm:
            db.add_sym_file(filename)

        self.assertEqual(str(cm.exception), 'Only SYM version 6.0 is supported.')

    def test_jopp_6_0_sym(self):
        filename = os.path.join('tests', 'files', 'jopp-6.0.sym')
        db = cantools.db.Database()
        db.add_sym_file(filename)

        self.assertEqual(len(db.messages), 6)
        self.assertEqual(len(db.messages[0].signals), 0)

        # Message1.
        message_1 = db.messages[3]
        self.assertEqual(len(message_1.signals), 2)
        self.assertEqual(message_1.frame_id, 0)
        self.assertEqual(message_1.is_extended_frame, False)
        self.assertEqual(message_1.name, 'Message1')
        self.assertEqual(message_1.length, 8)
        self.assertEqual(message_1.senders, [])
        self.assertEqual(message_1.send_type, None)
        self.assertEqual(message_1.cycle_time, 30)
        self.assertEqual(len(message_1.signals), 2)
        self.assertEqual(message_1.comment, None)
        self.assertEqual(message_1.bus_name, None)

        signal_1 = message_1.signals[0]
        self.assertEqual(signal_1.name, 'Signal1')
        self.assertEqual(signal_1.start, 0)
        self.assertEqual(signal_1.length, 11)
        self.assertEqual(signal_1.receivers, [])
        self.assertEqual(signal_1.byte_order, 'little_endian')
        self.assertEqual(signal_1.is_signed, False)
        self.assertEqual(signal_1.scale, 1)
        self.assertEqual(signal_1.offset, 0)
        self.assertEqual(signal_1.minimum, None)
        self.assertEqual(signal_1.maximum, 255)
        self.assertEqual(signal_1.unit, 'A')
        self.assertEqual(signal_1.choices, None)
        self.assertEqual(signal_1.comment, None)
        self.assertEqual(signal_1.is_multiplexer, False)
        self.assertEqual(signal_1.multiplexer_ids, None)
        self.assertEqual(signal_1.is_float, False)

        signal_2 = message_1.signals[1]
        self.assertEqual(signal_2.name, 'Signal2')
        self.assertEqual(signal_2.start, 32)
        self.assertEqual(signal_2.length, 32)
        self.assertEqual(signal_2.receivers, [])
        self.assertEqual(signal_2.byte_order, 'little_endian')
        self.assertEqual(signal_2.is_signed, False)
        self.assertEqual(signal_2.scale, 1)
        self.assertEqual(signal_2.offset, 48)
        self.assertEqual(signal_2.minimum, 16)
        self.assertEqual(signal_2.maximum, 130)
        self.assertEqual(signal_2.unit, 'V')
        self.assertEqual(signal_2.choices, None)
        self.assertEqual(signal_2.comment, None)
        self.assertEqual(signal_2.is_multiplexer, False)
        self.assertEqual(signal_2.multiplexer_ids, None)
        self.assertEqual(signal_2.is_float, True)

        # Message2.
        message_2 = db.messages[1]
        self.assertEqual(message_2.frame_id, 0x22)
        self.assertEqual(message_2.is_extended_frame, True)
        self.assertEqual(message_2.name, 'Message2')
        self.assertEqual(message_2.length, 8)
        self.assertEqual(message_2.senders, [])
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
        self.assertEqual(signal_3.scale, 1)
        self.assertEqual(signal_3.offset, 0)
        self.assertEqual(signal_3.minimum, 0)
        self.assertEqual(signal_3.maximum, 1)
        self.assertEqual(signal_3.unit, None)
        self.assertEqual(signal_3.choices, {0: 'foo', 1: 'bar'})
        self.assertEqual(signal_3.comment, None)
        self.assertEqual(signal_3.is_multiplexer, False)
        self.assertEqual(signal_3.multiplexer_ids, None)
        self.assertEqual(signal_3.is_float, False)

        # Symbol2.
        signal_4 = db.messages[4].signals[0]
        self.assertEqual(signal_4.name, 'Signal4')
        self.assertEqual(signal_4.start, 0)
        self.assertEqual(signal_4.length, 64)
        self.assertEqual(signal_4.receivers, [])
        self.assertEqual(signal_4.byte_order, 'little_endian')
        self.assertEqual(signal_4.is_signed, False)
        self.assertEqual(signal_4.scale, 6)
        self.assertEqual(signal_4.offset, 5)
        self.assertEqual(signal_4.minimum, -1.7e+308)
        self.assertEqual(signal_4.maximum, 1.7e+308)
        self.assertEqual(signal_4.unit, '*UU')
        self.assertEqual(signal_4.choices, None)
        self.assertEqual(signal_4.comment, None)
        self.assertEqual(signal_4.is_multiplexer, False)
        self.assertEqual(signal_4.multiplexer_ids, None)
        self.assertEqual(signal_4.is_float, True)

        # Symbol3.
        symbol_3 = db.messages[5]
        self.assertEqual(symbol_3.frame_id, 0x33)
        self.assertEqual(symbol_3.length, 8)
        self.assertEqual(symbol_3.is_multiplexed(), True)
        self.assertEqual(len(symbol_3.signals), 4)
        multiplexer = symbol_3.signals[0]
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

    def test_add_bad_sym_string(self):
        db = cantools.db.Database()

        with self.assertRaises(cantools.db.ParseError) as cm:
            db.add_sym_string('FormatVersion=6.0\n'
                              'Foo="Jopp"')

        self.assertEqual(
            str(cm.exception),
            'Invalid SYM syntax at line 2, column 1: \'>!<Foo="Jopp"\': '
            'Expected "Title".')

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
        self.assertEqual(cantools.db.formats.utils.num('1'), 1)
        self.assertEqual(cantools.db.formats.utils.num('1.0'), 1.0)

        with self.assertRaises(ValueError):
            cantools.db.formats.utils.num('x')

    def test_timing(self):
        filename = os.path.join('tests', 'files', 'timing.dbc')
        db = cantools.db.load_file(filename)

        # Message cycle time is 200, as given by BA_.
        message = db.get_message_by_frame_id(1)
        self.assertEqual(message.cycle_time, 200)
        self.assertEqual(message.send_type, 'cyclic')

        # Default message cycle time is 0, as given by BA_DEF_DEF_.
        message = db.get_message_by_frame_id(2)
        self.assertEqual(message.cycle_time, 0)
        self.assertEqual(message.send_type, 'none')

        with open(filename, 'r') as fin:
            self.assertEqual(db.as_dbc_string(), fin.read())

    def test_multiplex(self):
        filename = os.path.join('tests', 'files', 'multiplex.dbc')
        db = cantools.db.load_file(filename)

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

    def test_multiplex_choices(self):
        filename = os.path.join('tests', 'files', 'multiplex_choices.dbc')
        db = cantools.db.load_file(filename)

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

    def test_dbc_parse_error_messages(self):
        # No valid entry.
        with self.assertRaises(cantools.db.ParseError) as cm:
            cantools.db.formats.dbc.load_string('abc')

        self.assertEqual(
            str(cm.exception),
            "Invalid DBC syntax at line 1, column 1: '>!<abc': Expected "
            "{BO_ | CM_ | BA_ | VAL_ | BA_DEF_ | BA_DEF_DEF_ | BA_REL_ "
            "| BA_DEF_REL_ | BA_DEF_DEF_REL_ | SIG_GROUP_ | EV_ | BO_TX_BU_ "
            "| VAL_TABLE_ | SIG_VALTYPE_ | SG_MUL_VAL_ | BS_ | BU_ | NS_ "
            "| VERSION}.")

        # Bad message frame id.
        with self.assertRaises(cantools.db.ParseError) as cm:
            cantools.db.formats.dbc.load_string(
                'VERSION "1.0"\n'
                'BO_ dssd\n')

        self.assertEqual(
            str(cm.exception),
            "Invalid DBC syntax at line 2, column 5: 'BO_ >!<dssd': Expected "
            "frame id.")

        # Bad entry key.
        with self.assertRaises(cantools.db.ParseError) as cm:
            cantools.db.formats.dbc.load_string(
                'VERSION "1.0"\n'
                'dd\n')

        self.assertEqual(
            str(cm.exception),
            "Invalid DBC syntax at line 2, column 1: '>!<dd': Expected end "
            "of text.")

        # Missing colon in message.
        with self.assertRaises(cantools.db.ParseError) as cm:
            cantools.db.formats.dbc.load_string(
                'VERSION "1.0"\n'
                'BO_ 546 EMV_Stati 8 EMV_Statusmeldungen\n')

        self.assertEqual(
            str(cm.exception),
            "Invalid DBC syntax at line 2, column 19: 'BO_ 546 EMV_Stati "
            ">!<8 EMV_Statusmeldungen': Expected \":\".")

        # Missing frame id in message comment.
        with self.assertRaises(cantools.db.ParseError) as cm:
            cantools.db.formats.dbc.load_string('CM_ BO_ "Foo.";')

        self.assertEqual(
            str(cm.exception),
            "Invalid DBC syntax at line 1, column 9: 'CM_ BO_ >!<\"Foo.\";': "
            "Expected frame id.")

        # Missing frame id in message comment, using load_string().
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_string('CM_ BO_ "Foo.";')

        self.assertEqual(
            str(cm.exception),
            "DBC: \"Invalid DBC syntax at line 1, column 9: 'CM_ BO_ >!<\"Foo"
            ".\";': Expected frame id.\", KCD: \"syntax error: line 1, "
            "column 0\", SYM: \"Only SYM version 6.0 is supported.\"")

    def test_get_node_by_name(self):
        filename = os.path.join('tests', 'files', 'the_homer.kcd')
        db = cantools.db.load_file(filename)

        self.assertIs(db.get_node_by_name('Motor alternative supplier'),
                      db.nodes[1])

        with self.assertRaises(KeyError) as cm:
            db.get_node_by_name('Missing')

        self.assertEqual(str(cm.exception), "'Missing'")

    def test_get_bus_by_name(self):
        filename = os.path.join('tests', 'files', 'the_homer.kcd')
        db = cantools.db.load_file(filename)

        self.assertIs(db.get_bus_by_name('Comfort'), db.buses[2])

        with self.assertRaises(KeyError) as cm:
            db.get_bus_by_name('Missing')

        self.assertEqual(str(cm.exception), "'Missing'")

    def test_load_file_with_database_format(self):
        filename_dbc = os.path.join('tests', 'files', 'foobar.dbc')
        filename_kcd = os.path.join('tests', 'files', 'the_homer.kcd')
        filename_sym = os.path.join('tests', 'files', 'jopp-6.0.sym')

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
            "KCD: \"syntax error: line 1, column 0\"")

        # DBC database format, but file is KCD.
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormatError) as cm:
            cantools.db.load_file(filename_kcd, database_format='dbc')

        self.assertEqual(
            str(cm.exception),
            "DBC: \"Invalid DBC syntax at line 1, column 1: \'>!<<!--\': "
            "Expected {BO_ | CM_ | BA_ | VAL_ | BA_DEF_ | BA_DEF_DEF_ "
            "| BA_REL_ | BA_DEF_REL_ | BA_DEF_DEF_REL_ | SIG_GROUP_ | EV_ "
            "| BO_TX_BU_ | VAL_TABLE_ | SIG_VALTYPE_ | SG_MUL_VAL_ | BS_ "
            "| BU_ | NS_ | VERSION}.\"")

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
            "expected database format 'dbc', 'kcd', 'sym' or None, but "
            "got 'bad'")

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
        self.assertEqual(encoded, encoded_message)
        decoded = message.decode(encoded)
        self.assertEqual(decoded, decoded_message)

    def test_multiplex_choices_padding_one(self):
        """Test to encode a multiplexed message with padding as one.

        """

        filename = os.path.join('tests', 'files', 'multiplex_choices.dbc')
        db = cantools.db.load_file(filename)

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

        filename = os.path.join('tests', 'files', 'multiplex_choices.dbc')
        db = cantools.db.load_file(filename)

        message_1 = db.messages[0]

        # Encode.
        with self.assertRaises(cantools.db.EncodeError) as cm:
            message_1.encode({'Multiplexor': 7})

        self.assertEqual(str(cm.exception),
                         'expected multiplexer id 8, 16 or 24, but got 7')

        # Decode.
        with self.assertRaises(cantools.db.DecodeError) as cm:
            message_1.decode(b'\x1f\xff\x73\xfe\xff\xff\xff\xff')

        self.assertEqual(str(cm.exception),
                         'expected multiplexer id 8, 16 or 24, but got 7')

    def test_multiplex_dump(self):
        filename = os.path.join('tests', 'files', 'test_multiplex_dump.dbc')
        db = cantools.db.load_file(filename)
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

    def test_string_attribute_definition_dump(self):
        filename = os.path.join('tests', 'files', 'test_multiplex_dump.dbc')
        db = cantools.db.load_file(filename)
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        attr = dumped_db._attribute_definitions

        self.assertEqual(attr[0][0], "BA_DEF_")
        self.assertEqual(attr[0][1], "BusType")
        self.assertEqual(attr[0][2], "STRING")

    def test_extended_id_dump(self):
        filename = os.path.join('tests', 'files', 'test_extended_id_dump.dbc')
        db = cantools.db.load_file(filename)
        dumped_db = cantools.db.load_string(db.as_dbc_string())
        reg_id_msg = dumped_db.get_message_by_frame_id(0x100)
        ext_id_msg = dumped_db.get_message_by_frame_id(0x1c2a2a2a)

        self.assertEqual(reg_id_msg.is_extended_frame, False)
        self.assertEqual(ext_id_msg.is_extended_frame, True)

# This file is not '__main__' when executed via 'python setup.py
# test'.
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    unittest.main()
