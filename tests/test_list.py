import unittest
import traceback

try:
    from unittest.mock import Mock
    from unittest.mock import patch
    from unittest.mock import call
except ImportError:
    from mock import Mock
    from mock import patch
    from mock import call

import can
import cantools.subparsers.list as list_module

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

class Args(object):

    def __init__(self, database):
        self.exclude_normal = False
        self.exclude_extended = False
        self.print_all = False
        self.file = (database, )
        self.messages = []

class CanToolsListTest(unittest.TestCase):
    def test_dbc(self):
        # Prepare mocks.
        args = Args('tests/files/dbc/motohawk.dbc')

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
ExampleMessage
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        # Prepare mocks.
        args = Args('tests/files/dbc/motohawk.dbc')
        args.print_all = True 

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
ExampleMessage:
  Comment[None]: Example message used as template in MotoHawk models.
  Frame ID: 0x1f0 (496)
  Size: 8 bytes
  Is extended frame: False
  Signals:
    Enable:
      Type: Integer
      Start bit: 7
      Length: 1 bits
      Unit: -
      Is signed: False
      Named values:
        0: Disabled
        1: Enabled
    AverageRadius:
      Type: Integer
      Start bit: 6
      Length: 6 bits
      Unit: m
      Is signed: False
      Minimum: 0
      Maximum: 5
      Offset: 0
      Scaling factor: 0.1
    Temperature:
      Type: Integer
      Start bit: 0
      Length: 12 bits
      Unit: degK
      Is signed: True
      Minimum: 229.52
      Maximum: 270.47
      Offset: 250
      Scaling factor: 0.01
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

    def test_arxml4(self):
        # Prepare mocks.
        args = Args('tests/files/arxml/system-4.2.arxml')
        args.messages = ["Message2"]

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Message2:
  Frame ID: 0x6 (6)
  Size: 7 bytes
  Is extended frame: True
  Cycle time: 200 ms
  Signals:
    signal3:
      Type: Integer
      Start bit: 6
      Length: 2 bits
      Is signed: False
    signal2:
      Comment[FOR-ALL]: Signal comment!
      Type: Integer
      Start bit: 18
      Length: 11 bits
      Is signed: True
    signal4:
      Type: Integer
      Start bit: 30
      Length: 4 bits
      Is signed: False
      Minimum: 0
      Maximum: 3
      Named values:
        1: one
          Comment[EN]: One Comment
          Comment[DE]: Ein Kommentar
        2: two
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        args = Args('tests/files/arxml/system-4.2.arxml')
        args.exclude_normal = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = 'Message2\n'

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        args = Args('tests/files/arxml/system-4.2.arxml')
        args.exclude_extended = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Message1
Message3
Message4
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        args = Args('tests/files/arxml/system-4.2.arxml')
        args.messages = [ "IAmAGhost" ]

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
No message named "IAmAGhost" has been found in input file.
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        args = Args('tests/files/arxml/system-4.2.arxml')
        args.messages = [ "Message1" ]

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Message1:
  Comment[EN]: Comment1
  Comment[DE]: Kommentar1
  Frame ID: 0x5 (5)
  Size: 6 bytes
  Is extended frame: False
  Signals:
    signal6:
      Type: Integer
      Start bit: 0
      Length: 1 bits
      Unit: wp
      Initial value: False (0x0)
      Is signed: False
      Minimum: 0
      Maximum: 1
      Offset: 0.0
      Scaling factor: 0.1
      Named values:
        0: zero
          Comment[EN]: Nothing
          Comment[DE]: Nichts
    signal1:
      Comment[EN]: Signal comment!
      Comment[DE]: Signalkommentar!
      Type: Integer
      Start bit: 4
      Length: 3 bits
      Unit: m
      Initial value: 5 (0x5)
      Is signed: False
      Minimum: 0
      Maximum: 4
      Offset: 0.0
      Scaling factor: 5.0
    signal5:
      Type: Float
      Start bit: 16
      Length: 32 bits
      Is signed: False
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

    def test_kcd(self):
        # Prepare mocks.
        args = Args('tests/files/kcd/dump.kcd')
        args.exclude_extended = True
        args.print_all = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = ""
            expected_output = """\
Message1:
  Bus: Bus
  Frame ID: 0x1 (1)
  Size: 5 bytes
  Is extended frame: False
  Signals:
    Signal1:
      Type: Integer
      Start bit: 0
      Length: 1 bits
      Is signed: False
    Signal2:
      Type: Float
      Start bit: 8
      Length: 32 bits
      Is signed: False
      Named values:
        0: label1
        1: label2
Message2:
  Comment[None]: Note message 2.
  Bus: Bus
  Frame ID: 0x2 (2)
  Size: 4 bytes
  Is extended frame: False
  Cycle time: 100 ms
  Signals:
    Signal1:
      Type: Integer
      Start bit: 0
      Length: 8 bits
      Is signed: False
    Signal3:
      Type: Integer
      Start bit: 0
      Length: 8 bits
      Is signed: False
    Signal2:
      Type: Integer
      Start bit: 8
      Length: 8 bits
      Is signed: False
    Signal4:
      Type: Integer
      Start bit: 8
      Length: 8 bits
      Is signed: False
    Mux1:
      Type: Multiplexer
      Start bit: 16
      Length: 2 bits
      Is signed: False
    Mux2:
      Type: Multiplexer
      Start bit: 18
      Length: 1 bits
      Is signed: False
    Signal5:
      Type: Integer
      Start bit: 19
      Length: 1 bits
      Is signed: False
    Signal6:
      Comment[None]: Note signal 6.
      Type: Integer
      Start bit: 20
      Length: 12 bits
      Unit: Cel
      Is signed: True
      Minimum: 0
      Maximum: 100
      Offset: -40
      Scaling factor: 0.05
      Named values:
        0: init
Message4:
  Bus: Bus
  Frame ID: 0x4 (4)
  Size: 5 bytes
  Is extended frame: False
  Signals:
    Signal1:
      Type: Integer
      Start bit: 7
      Length: 1 bits
      Is signed: False
    Signal2:
      Type: Integer
      Start bit: 8
      Length: 12 bits
      Is signed: False
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        # Prepare mocks.
        args = Args('tests/files/kcd/dump.kcd')
        args.exclude_normal = True
        args.print_all = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Message3:
  Bus: Bus
  Frame ID: 0x3 (3)
  Size: 8 bytes
  Is extended frame: True
  Signals:
    Signal1:
      Type: Float
      Start bit: 0
      Length: 64 bits
      Is signed: False
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        # Prepare mocks.
        args = Args('tests/files/kcd/dump.kcd')
        args.exclude_normal = True
        args.exclude_extended = True
        args.print_all = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = ''

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

if __name__ == '__main__':
    unittest.main()
