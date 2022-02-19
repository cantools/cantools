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
        self.no_strict = False
        self.prune = True
        self.file = (database, )
        self.print_buses = False
        self.print_nodes = False
        self.items = []

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
  Sending ECUs: PCM1
  Frame ID: 0x1f0 (496)
  Size: 8 bytes
  Is extended frame: False
  Is CAN-FD frame: False
  Signal tree:

    -- {root}
       +-- Enable
       +-- AverageRadius
       +-- Temperature

  Signal details:
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
      Receiving ECUs: FOO, PCM1
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

    def test_arxml3(self):
        args = Args('tests/files/arxml/system-3.2.3.arxml')
        args.print_buses = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Network:
  Baudrate: 250000
  CAN-FD enabled: False
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        args = Args('tests/files/arxml/system-3.2.3.arxml')
        args.print_nodes = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Driver:
  Comment[DE]: Der rätselhafte Fahrer
  Comment[EN]: The enigmatic driver
Passenger:
  Comment[FOR-ALL]: A boring passenger
  Comment[DE]: Ein langweiliger Passagier
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

    def test_arxml4(self):
        args = Args('tests/files/arxml/system-4.2.arxml')
        args.print_nodes = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
DJ:
Dancer:
  Comment[FOR-ALL]: Rythm is a Dancer!
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        # Prepare mocks.
        args = Args('tests/files/arxml/system-4.2.arxml')
        args.items = ["Message2"]

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Message2:
  Bus: Cluster0
  Sending ECUs: Dancer
  Frame ID: 0x6 (6)
  Size: 7 bytes
  Is extended frame: True
  Is CAN-FD frame: True
  Cycle time: 200 ms
  Signal tree:

    -- {root}
       +-- signal3
       +-- signal2
       +-- signal4

  Signal details:
    signal3:
      Receiving ECUs: DJ
      Type: Integer
      Start bit: 6
      Length: 2 bits
      Is signed: False
    signal2:
      Comment[FOR-ALL]: Signal comment!
      Receiving ECUs: DJ
      Type: Integer
      Start bit: 18
      Length: 11 bits
      Is signed: True
    signal4:
      Receiving ECUs: DJ
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
MultiplexedMessage
OneToContainThemAll
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        args = Args('tests/files/arxml/system-4.2.arxml')
        args.items = [ "IAmAGhost" ]

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
        args.items = [ "Message1" ]

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Message1:
  Comment[EN]: Comment1
  Comment[DE]: Kommentar1
  Bus: Cluster0
  Sending ECUs: DJ
  Frame ID: 0x5 (5)
  Size: 6 bytes
  Is extended frame: False
  Is CAN-FD frame: True
  Signal tree:

    -- {root}
       +-- signal6
       +-- signal1
       +-- signal5

  Signal details:
    signal6:
      Receiving ECUs: Dancer
      Type: Integer
      Start bit: 0
      Length: 1 bits
      Unit: wp
      Initial value: False
      Is signed: False
      Minimum: 0
      Maximum: 0.1
      Offset: 0.0
      Scaling factor: 0.1
      Named values:
        0: zero
          Comment[EN]: Nothing
          Comment[DE]: Nichts
    signal1:
      Comment[EN]: Signal comment!
      Comment[DE]: Signalkommentar!
      Receiving ECUs: Dancer
      Type: Integer
      Start bit: 4
      Length: 3 bits
      Unit: m
      Initial value: 25.0 m
      Is signed: False
      Minimum: 0.0
      Maximum: 20.0
      Offset: 0.0
      Scaling factor: 5.0
    signal5:
      Receiving ECUs: Dancer
      Type: Float
      Start bit: 16
      Length: 32 bits
      Is signed: False
"""

            actual_output = stdout.getvalue()
            self.assertEqual(actual_output, expected_output)

        args = Args('tests/files/arxml/system-4.2.arxml')
        args.print_buses = True

        stdout = StringIO()
        with patch('sys.stdout', stdout):
            # Run the main function of the subparser
            list_module._do_list(args)

            # check make sure it behaves as expected
            expected_output = """\
Cluster0:
  Comment[FOR-ALL]: The great CAN cluster
  Baudrate: 500000
  CAN-FD enabled: True
  FD Baudrate: 2000000
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
  Sending ECUs: Node1
  Frame ID: 0x1 (1)
  Size: 5 bytes
  Is extended frame: False
  Is CAN-FD frame: False
  Signal tree:

    -- {root}
       +-- Signal1
       +-- Signal2

  Signal details:
    Signal1:
      Type: Integer
      Start bit: 0
      Length: 1 bits
      Is signed: False
    Signal2:
      Receiving ECUs: Node2, Node3
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
  Sending ECUs: Node2, Node3
  Frame ID: 0x2 (2)
  Size: 4 bytes
  Is extended frame: False
  Is CAN-FD frame: False
  Cycle time: 100 ms
  Signal tree:

    -- {root}
       +-- Mux1
       |   +-- 0
       |   |   +-- Signal1
       |   |   +-- Signal2
       |   +-- 1
       |       +-- Signal3
       |       +-- Signal4
       +-- Mux2
       |   +-- 0
       |       +-- Signal5
       +-- Signal6

  Signal details:
    Signal1:
      Type: Integer
      Selector signal: Mux1
      Selector values: 0
      Start bit: 0
      Length: 8 bits
      Is signed: False
    Signal3:
      Type: Integer
      Selector signal: Mux1
      Selector values: 1
      Start bit: 0
      Length: 8 bits
      Is signed: False
    Signal2:
      Type: Integer
      Selector signal: Mux1
      Selector values: 0
      Start bit: 8
      Length: 8 bits
      Is signed: False
    Signal4:
      Type: Integer
      Selector signal: Mux1
      Selector values: 1
      Start bit: 8
      Length: 8 bits
      Is signed: False
    Mux1:
      Type: Multiplex Selector
      Start bit: 16
      Length: 2 bits
      Is signed: False
    Mux2:
      Type: Multiplex Selector
      Start bit: 18
      Length: 1 bits
      Is signed: False
    Signal5:
      Type: Integer
      Selector signal: Mux2
      Selector values: 0
      Start bit: 19
      Length: 1 bits
      Is signed: False
    Signal6:
      Comment[None]: Note signal 6.
      Receiving ECUs: Node1
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
  Is CAN-FD frame: False
  Signal tree:

    -- {root}
       +-- Signal1
       +-- Signal2

  Signal details:
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
  Is CAN-FD frame: False
  Signal tree:

    -- {root}
       +-- Signal1

  Signal details:
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
