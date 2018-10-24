import os
import re
import unittest

try:
    from unittest.mock import patch
except ImportError:
    from mock import patch

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

import cantools


def remove_date_time(string):
    return re.sub(r' \* This file was generated.*', '', string)


def read_file(filename):
    with open(filename, 'r') as fin:
        return remove_date_time(fin.read())


class CanToolsCommandLineTest(unittest.TestCase):

    maxDiff = None

    def test_decode(self):
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

    def test_single_line_decode(self):
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

    def test_dump(self):
        argv = [
            'cantools',
            'dump',
            'tests/files/motohawk.dbc'
        ]

        expected_output = """\
================================= Messages =================================

  ------------------------------------------------------------------------

  Name:       ExampleMessage
  Id:         0x1f0
  Length:     8 bytes
  Cycle time: - ms
  Senders:    PCM1
  Layout:

                          Bit

             7   6   5   4   3   2   1   0
           +---+---+---+---+---+---+---+---+
         0 |<-x|<---------------------x|<--|
           +---+---+---+---+---+---+---+---+
             |                       +-- AverageRadius
             +-- Enable
           +---+---+---+---+---+---+---+---+
         1 |-------------------------------|
           +---+---+---+---+---+---+---+---+
         2 |----------x|   |   |   |   |   |
     B     +---+---+---+---+---+---+---+---+
     y               +-- Temperature
     t     +---+---+---+---+---+---+---+---+
     e   3 |   |   |   |   |   |   |   |   |
           +---+---+---+---+---+---+---+---+
         4 |   |   |   |   |   |   |   |   |
           +---+---+---+---+---+---+---+---+
         5 |   |   |   |   |   |   |   |   |
           +---+---+---+---+---+---+---+---+
         6 |   |   |   |   |   |   |   |   |
           +---+---+---+---+---+---+---+---+
         7 |   |   |   |   |   |   |   |   |
           +---+---+---+---+---+---+---+---+

  Signal tree:

    -- {root}
       +-- Enable
       +-- AverageRadius
       +-- Temperature

  ------------------------------------------------------------------------
"""

        stdout = StringIO()

        with patch('sys.stdout', stdout):
            with patch('sys.argv', argv):
                cantools._main()
                actual_output = stdout.getvalue()
                self.assertEqual(actual_output, expected_output)

    def test_dump_no_sender(self):
        argv = [
            'cantools',
            'dump',
            '--no-strict',
            'tests/files/no_sender.dbc'
        ]

        expected_output = """\
================================= Messages =================================

  ------------------------------------------------------------------------

  Name:       Foo
  Id:         0x40000000
  Length:     0 bytes
  Cycle time: - ms
  Senders:    -
  Layout:

                          Bit

             7   6   5   4   3   2   1   0
           +---+---+---+---+---+---+---+---+
     B   0 |<-----------------------------x|
     y     +---+---+---+---+---+---+---+---+
     t       +-- signal_without_sender
     e

  Signal tree:

    -- {root}
       +-- signal_without_sender

  ------------------------------------------------------------------------
"""

        stdout = StringIO()

        with patch('sys.stdout', stdout):
            with patch('sys.argv', argv):
                cantools._main()
                actual_output = stdout.getvalue()
                self.assertEqual(actual_output, expected_output)

    def test_convert(self):
        # DBC to KCD.
        argv = [
            'cantools',
            'convert',
            'tests/files/motohawk.dbc',
            'test_command_line_convert.kcd'
        ]

        if os.path.exists('test_command_line_convert.kcd'):
            os.remove('test_command_line_convert.kcd')

        with patch('sys.argv', argv):
            cantools._main()

        db = cantools.database.Database()
        db.add_kcd_file('test_command_line_convert.kcd')
        self.assertEqual(db.version, '1.0')

        # KCD to DBC.
        argv = [
            'cantools',
            'convert',
            'test_command_line_convert.kcd',
            'test_command_line_convert.dbc'
        ]

        if os.path.exists('test_command_line_convert.dbc'):
            os.remove('test_command_line_convert.dbc')

        with patch('sys.argv', argv):
            cantools._main()

        db = cantools.database.Database()
        db.add_dbc_file('test_command_line_convert.dbc')
        self.assertEqual(db.version, '1.0')

    def test_convert_bad_outfile(self):
        argv = [
            'cantools',
            'convert',
            'tests/files/motohawk.dbc',
            'test_command_line_convert.foo'
        ]

        with patch('sys.argv', argv):
            with self.assertRaises(SystemExit) as cm:
                cantools._main()

            self.assertEqual(
                str(cm.exception),
                "error: Unsupported output database format 'foo'.")

    def test_generate(self):
        databases = [
            'motohawk',
            'padding_bit_order',
            'vehicle'
        ]

        for database in databases:
            argv = [
                'cantools',
                'generate_c_source',
                'tests/files/{}.dbc'.format(database)
            ]

            database_h = database + '.h'
            database_c = database + '.c'

            if os.path.exists(database_h):
                os.remove(database_h)

            if os.path.exists(database_c):
                os.remove(database_c)

            with patch('sys.argv', argv):
                cantools._main()

            self.assertEqual(read_file('tests/files/c_source/' + database_h),
                             read_file(database_h))
            self.assertEqual(read_file('tests/files/c_source/' + database_c),
                             read_file(database_c))


if __name__ == '__main__':
    unittest.main()
