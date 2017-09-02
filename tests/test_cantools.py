import os
import unittest
import sys
import logging
from xml.etree import ElementTree

import cantools

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO


class CanToolsTest(unittest.TestCase):

    maxDiff = None

    def test_vehicle(self):
        filename = os.path.join('tests', 'files', 'vehicle.dbc')
        db = cantools.db.load_file(filename)
        self.assertEqual(len(db.nodes), 1)
        self.assertEqual(db.nodes[0].name, 'Vector__XXX')
        self.assertEqual(len(db.messages), 217)
        self.assertEqual(db.messages[216].frame_id, 155872546)
        self.assertEqual(str(db.messages[0]),
                         "message('RT_SB_INS_Vel_Body_Axes', 0x9588322, False, 8, None)")
        self.assertEqual(repr(db.messages[0].signals[0]),
                         "signal('Validity_INS_Vel_Forwards', 0, 1, 'little_endian', "
                         "False, 1, 0, 0, 1, 'None', False, None, None, 'Valid when "
                         "bit is set, invalid when bit is clear.')")
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
        self.assertEqual(len(db.messages[0].signals[2].nodes), 2)
        self.assertEqual(db.messages[0].signals[2].nodes[0], 'Vector__XXX')
        self.assertEqual(db.messages[0].signals[2].nodes[1], 'FOO')
        self.assertEqual(db.messages[0].signals[1].nodes[0], 'Vector__XXX')

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
        self.assertEqual(len(db.messages[0].signals[0].nodes), 1)

    def test_foobar(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'foobar.dbc')
        db.add_dbc_file(filename)

        self.assertEqual(len(db.nodes), 2)
        self.assertEqual(db.version, '2.0')
        self.assertEqual(repr(db),
                         "version('2.0')\n"
                         "\n"
                         "node('FOO', None)\n"
                         "node('BAR', 'fam')\n"
                         "\n"
                         "message('Foo', 0x12331, True, 8, 'Foo.')\n"
                         "  signal('Bar', 1, 6, 'big_endian', False, 0.1, "
                         "0, 1.0, 5.0, 'm', False, None, None, '')\n"
                         "  signal('Foo', 7, 12, 'big_endian', True, 0.01, "
                         "250, 229.53, 270.47, 'degK', False, None, {-1: \'Foo\', "
                         "-2: \'Fie\'}, None)\n"
                         "\n"
                         "message('Fum', 0x12331, True, 8, 'Foo.')\n"
                         "  signal('Fum', 0, 12, 'little_endian', True, 1, 0, 0, 1, "
                         "'None', False, None, None, None)\n")

        message = db.lookup_message(0x12331)
        self.assertEqual(message.name, 'Fum')

    def test_padding_bit_order(self):
        """Encode and decode signals with reversed bit order.

        """

        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'padding_bit_order.dbc')
        db.add_dbc_file(filename)

        # Message 0.
        msg0_frame_id = 1

        data = {
            'B': 1,      # should set byte[0]bit[7]=1
            'A': 0x2c9,  # should set byte[0]bit[1]=1 and byte[1]=c9
            'D': 0,      # should set byte[5]bit[7]=0
            'C': 0x2c9   # should set byte[4]bit[1]=1 and byte [5]=c9
        }

        encoded = db.encode_message(msg0_frame_id, data)
        self.assertEqual(encoded, b'\x82\xc9\x00\x00\x02\xc9\x00\x00')

        decoded = db.decode_message(msg0_frame_id, encoded)
        self.assertEqual(decoded, data)

        # Message 1.
        msg1_frame_id = 2

        data = {
            'E': 1,      # should set byte[0]bit[0]=1
            'F': 0x2c9,  # should set byte[0]bit[7:1]=92 and byte[1]=05
            'G': 0,      # should set byte[4]bit[0]=0
            'H': 0x2c9   # should set byte[4]bit[7:1]=92 and byte[5]=05
        }

        encoded = db.encode_message(msg1_frame_id, data)
        self.assertEqual(encoded, b'\x93\x05\x00\x00\x92\x05\x00\x00')

        decoded = db.decode_message(msg1_frame_id, encoded)
        self.assertEqual(decoded, data)

        # Message 2.
        msg2_frame_id = 3

        data = {
            'I': 1,  # should set byte[0]bit[3:0]=1
            'J': 2,  # should set byte[0]bit[7:4]=2
            'K': 3   # should set byte[1]bit[3:0]=3
        }

        encoded = db.encode_message(msg2_frame_id, data)
        self.assertEqual(encoded, b'\x21\x03\x00\x00\x00\x00\x00\x00')

        decoded = db.decode_message(msg2_frame_id, encoded)
        self.assertEqual(decoded, data)

        # Message 3.
        msg3_frame_id = 4

        data = {
            'L': 0x0123456789abcdef
        }

        encoded = db.encode_message(msg3_frame_id, data)
        self.assertTrue(encoded in [b'\x01\x23\x45\x67\x89\xab\xcd\xf0',
                                    b'\x01\x23\x45\x67\x89\xab\xcd\xef'])

        decoded = db.decode_message(msg3_frame_id, encoded)
        self.assertTrue(decoded in [data, {'L': 0x0123456789abcdef + 1}])

        # Message 4.
        msg4_frame_id = 5

        data = {
            'M': 0x0123456789abcdef
        }

        encoded = db.encode_message(msg4_frame_id, data)
        self.assertTrue(encoded in [b'\xf0\xcd\xab\x89\x67\x45\x23\x01',
                                    b'\xef\xcd\xab\x89\x67\x45\x23\x01'])

        decoded = db.decode_message(msg4_frame_id, encoded)
        self.assertTrue(decoded in [data, {'M': 0x0123456789abcdef + 1}])

    def test_motohawk_encode_decode(self):
        """Encode and decode the signals in a ExampleMessage frame.

        """

        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
        db.add_dbc_file(filename)

        example_message_frame_id = 496

        # Encode with non-enumerated values.
        data = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 1
        }

        encoded = db.encode_message(example_message_frame_id, data)
        self.assertEqual(encoded, b'\xc0\x06\xe0\x00\x00\x00\x00\x00')

        # Encode with enumerated values.
        data = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 'Enabled'
        }

        encoded = db.encode_message(example_message_frame_id, data)
        self.assertEqual(encoded, b'\xc0\x06\xe0\x00\x00\x00\x00\x00')

        decoded = db.decode_message(example_message_frame_id, encoded)
        self.assertEqual(decoded, data)

    def test_big_endian_no_decode_choices(self):
        """Decode a big endian signal with `decode_choices` set to False.

        """

        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
        db.add_dbc_file(filename)

        data = {
            'Temperature': 250.55,
            'AverageRadius': 3.2,
            'Enable': 1
        }

        decoded = db.decode_message(496,
                                    b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
                                    decode_choices=False)
        self.assertEqual(decoded, data)

    def test_little_endian_no_decode_choices(self):
        """Decode a little endian signal with `decode_choices` set to False.

        """

        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'socialledge.dbc')
        db.add_dbc_file(filename)

        data = {
            'DRIVER_HEARTBEAT_cmd': 1
        }

        decoded = db.decode_message(100,
                                    b'\x01\x00\x00\x00\x00\x00\x00\x00',
                                    decode_choices=False)
        self.assertEqual(decoded, data)

        data = {
            'DRIVER_HEARTBEAT_cmd': 'DRIVER_HEARTBEAT_cmd_SYNC'
        }

        decoded = db.decode_message(100,
                                    b'\x01\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(decoded, data)

    def test_socialledge(self):
        db = cantools.db.File()
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
        self.assertEqual(sensor_sonars.signals[-1].multiplexer_id, 1)
        self.assertEqual(sensor_sonars.signals[2].name, 'SENSOR_SONARS_left')
        self.assertEqual(sensor_sonars.signals[2].multiplexer_id, 0)
        self.assertEqual(sensor_sonars.signals[0].name, 'SENSOR_SONARS_mux')
        self.assertEqual(sensor_sonars.signals[0].is_multiplexer, True)

        self.assertEqual(sensor_sonars.get_multiplexer_signal_name(),
                         'SENSOR_SONARS_mux')
        signals = sensor_sonars.get_signals_by_multiplexer_id(0)
        self.assertEqual(len(signals), 6)
        self.assertEqual(signals[-1].name, 'SENSOR_SONARS_rear')

        self.assertEqual(db.version, '')

    def test_socialledge_encode_decode_mux_0(self):
        """Encode and decode the signals in a SENSOR_SONARS frame with mux 0.

        """

        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'socialledge.dbc')
        db.add_dbc_file(filename)

        frame_id = 200
        data = {
            'SENSOR_SONARS_mux': 0,
            'SENSOR_SONARS_err_count': 1,
            'SENSOR_SONARS_left': 2,
            'SENSOR_SONARS_middle': 3,
            'SENSOR_SONARS_right': 4,
            'SENSOR_SONARS_rear': 5
        }

        encoded = db.encode_message(frame_id, data)
        self.assertEqual(encoded, b'\x10\x00\x14\xe0\x01( \x03')

        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, data)

    def test_socialledge_encode_decode_mux_1(self):
        """Encode and decode the signals in a SENSOR_SONARS frame with mux 1.

        """

        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'socialledge.dbc')
        db.add_dbc_file(filename)

        frame_id = 200
        data = {
            'SENSOR_SONARS_mux': 1,
            'SENSOR_SONARS_err_count': 2,
            'SENSOR_SONARS_no_filt_left': 3,
            'SENSOR_SONARS_no_filt_middle': 4,
            'SENSOR_SONARS_no_filt_right': 5,
            'SENSOR_SONARS_no_filt_rear': 6
        }

        encoded = db.encode_message(frame_id, data)
        self.assertEqual(encoded, b'!\x00\x1e\x80\x022\xc0\x03')

        decoded = db.decode_message(frame_id, encoded)
        self.assertEqual(decoded, data)

    def test_add_message(self):
        db = cantools.db.File()
        signals = [cantools.db.Signal(name='signal',
                                      start=0,
                                      length=4,
                                      nodes=['foo'],
                                      byte_order='big_endian',
                                      is_signed=False,
                                      scale=1.0,
                                      offset=10,
                                      minimum=10.0,
                                      maximum=100.0,
                                      unit='m/s',
                                      choices=None,
                                      comment=None)]
        message = cantools.db.Message(frame_id=37,
                                      name='message',
                                      length=8,
                                      nodes=['bar'],
                                      signals=signals,
                                      comment='')
        db.add_message(message)
        self.assertEqual(len(db.messages), 1)

    def test_command_line_decode(self):
        argv = ['cantools', 'decode', 'tests/files/socialledge.dbc']
        input_data = """  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
  vcan0  064   [8]  F0 01 FF FF FF FF FF FF
"""
        expected_output = """  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00 :: SENSOR_SONARS(SENSOR_SONARS_mux: 0, SENSOR_SONARS_err_count: 15, SENSOR_SONARS_left: 0.0, SENSOR_SONARS_middle: 0.0, SENSOR_SONARS_right: 0.0, SENSOR_SONARS_rear: 0.0)
  vcan0  064   [8]  F0 01 FF FF FF FF FF FF :: DRIVER_HEARTBEAT(DRIVER_HEARTBEAT_cmd: 240)
"""

        stdin = sys.stdin
        stdout = sys.stdout
        sys.argv = argv
        sys.stdin = StringIO(input_data)
        sys.stdout = StringIO()

        try:
            cantools._main()

        finally:
            actual_output = sys.stdout.getvalue()
            sys.stdin = stdin
            sys.stdout = stdout

        self.assertEqual(actual_output, expected_output)

    def test_the_homer(self):
        filename = os.path.join('tests', 'files', 'the_homer.kcd')
        db = cantools.db.load_file(filename)

        self.assertEqual(db.version, None)
        self.assertEqual(len(db.nodes), 18)
        self.assertEqual(db.nodes[0].name, 'Motor ACME')
        self.assertEqual(db.nodes[1].name, 'Motor alternative supplier')
        self.assertEqual(len(db.buses), 3)
        self.assertEqual(db.buses[0].name, 'Motor')
        self.assertEqual(db.buses[1].name, 'Instrumentation')
        self.assertEqual(db.buses[2].name, 'Comfort')
        self.assertEqual(db.buses[0].comment, None)
        self.assertEqual(len(db.messages), 25)
        self.assertEqual(db.messages[0].frame_id, 0xa)
        self.assertEqual(db.messages[0].is_extended_frame, False)
        self.assertEqual(db.messages[0].name, 'Airbag')
        self.assertEqual(db.messages[0].length, 8)
        self.assertEqual(len(db.messages[0].signals), 8)
        self.assertEqual(db.messages[0].comment, None)
        self.assertEqual(db.messages[0].send_type, None)
        self.assertEqual(db.messages[0].cycle_time, None)
        self.assertEqual(db.messages[0].bus_name, 'Motor')

        self.assertEqual(db.messages[-1].bus_name, 'Comfort')

        seat_configuration = db.messages[0].signals[-1]

        self.assertEqual(seat_configuration.name, 'SeatConfiguration')
        self.assertEqual(seat_configuration.start, 16)
        self.assertEqual(seat_configuration.length, 8)
        self.assertEqual(seat_configuration.nodes, [])
        self.assertEqual(seat_configuration.byte_order, 'big_endian')
        self.assertEqual(seat_configuration.is_signed, False)
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
        self.assertEqual(tank_temperature.nodes, [])
        self.assertEqual(tank_temperature.byte_order, 'big_endian')
        self.assertEqual(tank_temperature.is_signed, True)
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
        self.assertEqual(speed_km.nodes, [])
        self.assertEqual(speed_km.byte_order, 'big_endian')
        self.assertEqual(speed_km.is_signed, False)
        self.assertEqual(speed_km.scale, 0.2)
        self.assertEqual(speed_km.offset, 0)
        self.assertEqual(speed_km.minimum, None)
        self.assertEqual(speed_km.maximum, None)
        self.assertEqual(speed_km.unit, 'km/h')
        self.assertEqual(speed_km.choices, None)
        self.assertEqual(speed_km.comment,
                         'Middle speed of front wheels in kilometers per hour.')

        outside_temp = db.messages[1].signals[0]

        self.assertEqual(outside_temp.name, 'OutsideTemp')
        self.assertEqual(outside_temp.start, 18)
        self.assertEqual(outside_temp.length, 12)
        self.assertEqual(outside_temp.nodes, [])
        self.assertEqual(outside_temp.byte_order, 'little_endian')
        self.assertEqual(outside_temp.is_signed, False)
        self.assertEqual(outside_temp.scale, 0.05)
        self.assertEqual(outside_temp.offset, -40)
        self.assertEqual(outside_temp.minimum, 0)
        self.assertEqual(outside_temp.maximum, 100)
        self.assertEqual(outside_temp.unit, 'Cel')
        self.assertEqual(outside_temp.choices, None)
        self.assertEqual(outside_temp.comment, 'Outside temperature.')

    def test_load_bad_format(self):
        with self.assertRaises(cantools.db.UnsupportedDatabaseFormat):
            cantools.db.load(StringIO(''))

    def test_add_bad_kcd_string(self):
        db = cantools.db.File()

        with self.assertRaises(ElementTree.ParseError) as cm:
            db.add_kcd_string('not xml')

        self.assertEqual(str(cm.exception), 'syntax error: line 1, column 0')


# This file is not '__main__' when executed via 'python setup.py
# test'.
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    unittest.main()
