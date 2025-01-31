import traceback
import unittest

try:
    import curses
    have_curses = True
except (ImportError, ModuleNotFoundError):
    # on windows, the batteries for the curses package are not
    # included by default (there is a "windows-curses" package
    # available, though)
    have_curses = False

from unittest.mock import Mock, call, patch

import can

if have_curses:
    from cantools.subparsers.monitor import Monitor


class Args:

    def __init__(self,
                 database,
                 single_line=False):
        self.database = database
        self.encoding = None
        self.frame_id_mask = None
        self.prune = False
        self.no_strict = False
        self.single_line = single_line
        self.bit_rate = None
        self.fd = False
        self.bus_type = 'socketcan'
        self.channel = 'vcan0'


class StdScr:

    def __init__(self, user_input=None, resolution=None):
        if resolution is None:
            resolution = [(30, 64)]

        self.getmaxyx = Mock(side_effect=resolution)
        self.nodelay = Mock()
        self.clear = Mock()
        self.addstr = Mock()
        self.refresh = Mock()
        self.keypad = Mock()
        self.erase = Mock()

        if user_input is None:
            user_input = ['q']

        self.getkey = Mock(side_effect=user_input)
        self.move = Mock()

@unittest.skipIf(not have_curses,
                 "The curses module is not available on your system")
class CanToolsMonitorTest(unittest.TestCase):

    maxDiff = None

    color_pair_side_effect = [ "default", "green", "cyan", "cyan inverted" ]

    def assert_called(self, mock, expected, verbose=False):
        try:
            self.assertEqual(mock.call_args_list, expected)
        except AssertionError as e:
            if verbose:
                nl = ",\n "
                print(f"Assertion failed:")
                print(f"Expected: {nl.join(str(x) for x in expected)}")
                print(f"Got: {nl.join(str(x) for x in mock.call_args_list)}")
                print("Traceback:")
                traceback.print_stack()
            raise e

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_immediate_quit(self,
                            use_default_colors,
                            curs_set,
                            init_pair,
                            is_term_resized,
                            color_pair,
                            bus,
                            notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.run(1)

        # Check mocks.
        self.assert_called(use_default_colors, [call()])
        self.assert_called(curs_set, [call(False)])
        self.assert_called(
            init_pair,
            [
                call(1, curses.COLOR_BLACK, curses.COLOR_GREEN),
                call(2, curses.COLOR_BLACK, curses.COLOR_CYAN),
                call(3, curses.COLOR_CYAN, curses.COLOR_BLACK)
            ])
        self.assert_called(color_pair, [call(1), call(2)])
        self.assert_called(bus, [call(bustype='socketcan', channel='vcan0')])
        self.assert_called(
            stdscr.addstr,
            [
                call(0,
                     0,
                     'Received: 0, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_can_fd(self,
                    use_default_colors,
                    curs_set,
                    init_pair,
                    is_term_resized,
                    color_pair,
                    bus,
                    notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/motohawk.dbc')
        args.fd = True
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.run(1)

        # Check mocks.
        self.assert_called(bus, [call(bustype='socketcan', channel='vcan0', fd=True)])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_display_one_frame(self,
                               _use_default_colors,
                               _curs_set,
                               _init_pair,
                               is_term_resized,
                               color_pair,
                               _bus,
                               _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_display_one_frame_single_line(self,
                                           _use_default_colors,
                                           _curs_set,
                                           _init_pair,
                                           is_term_resized,
                                           color_pair,
                                           _bus,
                                           _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/motohawk.dbc',
                    single_line=True)
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2,
                     0,
                     "       0.000  ExampleMessage(Enable: Enabled, "
                     "AverageRadius: 3.2 m, Temperature: 250.55 degK)"),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_reject_muxed_data_invalid_mux_index(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/msxii_system_can.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x24\x00\x98\x98\x0b\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 1'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT(undecoded, expected multiplexer id 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34 or 35, but got 36: 0x240098980b00)'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_display_muxed_data(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/msxii_system_can.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x00\x00\x98\x98\x0b\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0('),
                call(3, 0, "                  BATTERY_VT_INDEX: 0,"),
                call(4, 0, '                  MODULE_VOLTAGE_00: 39064,'),
                call(5, 0, '                  MODULE_TEMP_00: 11'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_display_muxed_data_single_line(self,
                                            _use_default_colors,
                                            _curs_set,
                                            _init_pair,
                                            is_term_resized,
                                            color_pair,
                                            _bus,
                                            _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/msxii_system_can.dbc',
                    single_line=True)
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x00\x00\x98\x98\x0b\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2,
                     0,
                     "       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0(BATTERY_VT_INDEX: 0, "
                     "MODULE_VOLTAGE_00: 39064, MODULE_TEMP_00: 11)"),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_display_muxed_data_multiple_single_line(self,
                                                     _use_default_colors,
                                                     _curs_set,
                                                     _init_pair,
                                                     is_term_resized,
                                                     color_pair,
                                                     _bus,
                                                     _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/multiplex_2.dbc',
                    single_line=True)
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=0xc00fefe,
            data=b'\x00\x00\x00\x00\x02\x00\x00\x00',
            timestamp=0.0))
        monitor.on_message_received(can.Message(
            arbitration_id=0xc00fefe,
            data=b'\x00\x00\x00\x00\x01\x00\x00\x00',
            timestamp=1.0))
        monitor.on_message_received(can.Message(
            arbitration_id=0xc00fefe,
            data=b'\x01\x00\x00\x00\x01\x00\x00\x00',
            timestamp=2.0))
        monitor.on_message_received(can.Message(
            arbitration_id=0xc00fefe,
            data=b'\x20\x00\x00\x00\x01\x00\x00\x00',
            timestamp=3.0))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 4, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2,
                     0,
                     "       1.000  Extended :: S0=0 :: S1=0 :: S6=1(S0: 0, S1: 0, S2: 0, S3: 0, S6: 1, S7: 0)"),
                call(3,
                     0,
                     "       0.000  Extended :: S0=0 :: S1=0 :: S6=2(S0: 0, S1: 0, S2: 0, S3: 0, S6: 2, S8: 0)"),
                call(4,
                     0,
                     "       3.000  Extended :: S0=0 :: S1=2 :: S6=1(S0: 0, S1: 2, S4: 0, S6: 1, S7: 0)"),
                call(5,
                     0,
                     "       2.000  Extended :: S0=1 :: S1=0 :: S6=1(S0: 1, S5: 0, S6: 1, S7: 0)"),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_display_one_frame_input_twice(self,
                                           _use_default_colors,
                                           _curs_set,
                                           _init_pair,
                                           is_term_resized,
                                           color_pair,
                                           _bus,
                                           _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
            timestamp=1.0))
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xd0\x00\x00\x00\x00\x00',
            timestamp=2.1))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       1.100  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.54 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_filter(self,
                    _use_default_colors,
                    _curs_set,
                    _init_pair,
                    is_term_resized,
                    color_pair,
                    _bus,
                    _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[
            'f', 'Y', '[', '\b', '\n', 'f', '\b', 'E', 'X', '\n', 'q'
        ])
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # No filter.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # 'f' pressed.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: ', 'cyan'),
                call(29, 14, ' ', 'cyan inverted'),
                call(29, 15, '                                                 ', 'cyan'),

                # No match on 'Y'.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: Y'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(29, 0, 'Filter regex: Y', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # Invalid filter 'Y['.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: Y['),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: Y[', 'cyan'),
                call(29, 16, ' ', 'cyan inverted'),
                call(29, 17, '                                               ', 'cyan'),

                # No match on 'Y'.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: Y'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(29, 0, 'Filter regex: Y', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # Hit enter to hide filter prompt.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: Y'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # 'f' pressed again.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: Y'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(29, 0, 'Filter regex: Y', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # Backspace.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: ', 'cyan'),
                call(29, 14, ' ', 'cyan inverted'),
                call(29, 15, '                                                 ', 'cyan'),

                # Match on 'E'.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: E'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: E', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # Match 'x' with uppercase 'X'
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: EX'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: EX', 'cyan'),
                call(29, 16, ' ', 'cyan inverted'),
                call(29, 17, '                                               ', 'cyan'),

                # Hit enter to hide filter.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: EX'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_filter_signal(self,
                    _use_default_colors,
                    _curs_set,
                    _init_pair,
                    is_term_resized,
                    color_pair,
                    _bus,
                    _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[
            'f', 'f', 'a', '\n', 'q'
        ])
        args = Args('tests/files/dbc/foobar.dbc')
        args.fd = True
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=74544,
            is_extended_id=True,
            data=b'\x00\x00\x00\x00\x00\x00\x00\x00'))
        monitor.on_message_received(can.Message(
            arbitration_id=74545,
            is_extended_id=True,
            data=b'\x00\x00\x00\x00\x00'))
        monitor.on_message_received(can.Message(
            arbitration_id=74546,
            is_extended_id=True,
            data=b'\x00\x00\x00\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 3, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  Bar('),
                call(3, 0, '                  Binary32: 0.0'),
                call(4, 0, '              )'),
                call(5, 0, '       0.000  Foo('),
                call(6, 0, '                  Foo: 250.0 degK,'),
                call(7, 0, '                  Bar: 0.0 m'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  Fum('),
                call(10, 0, '                  Fum: 0,'),
                call(11, 0, '                  Fam: Disabled'),
                call(12, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # 'f' pressed to start filtering
                call(0, 0, 'Received: 3, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  Bar('),
                call(3, 0, '                  Binary32: 0.0'),
                call(4, 0, '              )'),
                call(5, 0, '       0.000  Foo('),
                call(6, 0, '                  Foo: 250.0 degK,'),
                call(7, 0, '                  Bar: 0.0 m'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  Fum('),
                call(10, 0, '                  Fum: 0,'),
                call(11, 0, '                  Fam: Disabled'),
                call(12, 0, '              )'),
                call(29, 0, 'Filter regex: ', 'cyan'),
                call(29, 14, ' ', 'cyan inverted'),
                call(29, 15, '                                                 ', 'cyan'),

                # Match on 'f'
                call(0, 0, 'Received: 3, Discarded: 0, Errors: 0, Filter: f'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  Foo('),
                call(3, 0, '                  Foo: 250.0 degK,'),
                call(4, 0, '                  Bar: 0.0 m'),
                call(5, 0, '              )'),
                call(6, 0, '       0.000  Fum('),
                call(7, 0, '                  Fum: 0,'),
                call(8, 0, '                  Fam: Disabled'),
                call(9, 0, '              )'),
                call(29, 0, 'Filter regex: f', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # Match on 'fa'
                call(0, 0, 'Received: 3, Discarded: 0, Errors: 0, Filter: fa'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  Fum('),
                call(3, 0, '                  Fam: Disabled'),
                call(4, 0, '              )'),
                call(29, 0, 'Filter regex: fa', 'cyan'),
                call(29, 16, ' ', 'cyan inverted'),
                call(29, 17, '                                               ', 'cyan'),

                # Enter
                call(0, 0, 'Received: 3, Discarded: 0, Errors: 0, Filter: fa'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  Fum('),
                call(3, 0, '                  Fam: Disabled'),
                call(4, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_filter_muxed_signal(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[
            'f', 'E', '_', '0', '1', '\n', 'q'
        ])
        args = Args('tests/files/dbc/msxii_system_can.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x00\x00\x98\x98\x0b\x00'))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x01\x00\x98\x98\x0b\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0('),
                call(3, 0, '                  BATTERY_VT_INDEX: 0,'),
                call(4, 0, '                  MODULE_VOLTAGE_00: 39064,'),
                call(5, 0, '                  MODULE_TEMP_00: 11'),
                call(6, 0, '              )'),
                call(7, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(8, 0, '                  BATTERY_VT_INDEX: 1,'),
                call(9, 0, '                  MODULE_VOLTAGE_01: 39064,'),
                call(10, 0, '                  MODULE_TEMP_01: 11'),
                call(11, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # 'f' pressed to start filtering
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0('),
                call(3, 0, '                  BATTERY_VT_INDEX: 0,'),
                call(4, 0, '                  MODULE_VOLTAGE_00: 39064,'),
                call(5, 0, '                  MODULE_TEMP_00: 11'),
                call(6, 0, '              )'),
                call(7, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(8, 0, '                  BATTERY_VT_INDEX: 1,'),
                call(9, 0, '                  MODULE_VOLTAGE_01: 39064,'),
                call(10, 0, '                  MODULE_TEMP_01: 11'),
                call(11, 0, '              )'),
                call(29, 0, 'Filter regex: ', 'cyan'),
                call(29, 14, ' ', 'cyan inverted'),
                call(29, 15, '                                                 ', 'cyan'),

                # Match on 'E'
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0, Filter: E'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0('),
                call(3, 0, '                  BATTERY_VT_INDEX: 0,'),
                call(4, 0, '                  MODULE_VOLTAGE_00: 39064,'),
                call(5, 0, '                  MODULE_TEMP_00: 11'),
                call(6, 0, '              )'),
                call(7, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(8, 0, '                  BATTERY_VT_INDEX: 1,'),
                call(9, 0, '                  MODULE_VOLTAGE_01: 39064,'),
                call(10, 0, '                  MODULE_TEMP_01: 11'),
                call(11, 0, '              )'),
                call(29, 0, 'Filter regex: E', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # Match on 'E_'
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0, Filter: E_'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0('),
                call(3, 0, '                  MODULE_VOLTAGE_00: 39064,'),
                call(4, 0, '                  MODULE_TEMP_00: 11'),
                call(5, 0, '              )'),
                call(6, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(7, 0, '                  MODULE_VOLTAGE_01: 39064,'),
                call(8, 0, '                  MODULE_TEMP_01: 11'),
                call(9, 0, '              )'),
                call(29, 0, 'Filter regex: E_', 'cyan'),
                call(29, 16, ' ', 'cyan inverted'),
                call(29, 17, '                                               ', 'cyan'),

                # Match on 'E_0'
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0, Filter: E_0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0('),
                call(3, 0, '                  MODULE_VOLTAGE_00: 39064'),
                call(4, 0, '              )'),
                call(5, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(6, 0, '                  MODULE_VOLTAGE_01: 39064'),
                call(7, 0, '              )'),
                call(29, 0, 'Filter regex: E_0', 'cyan'),
                call(29, 17, ' ', 'cyan inverted'),
                call(29, 18, '                                              ', 'cyan'),

                # Match on 'E_01'
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0, Filter: E_01'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(3, 0, '                  MODULE_VOLTAGE_01: 39064'),
                call(4, 0, '              )'),
                call(29, 0, 'Filter regex: E_01', 'cyan'),
                call(29, 18, ' ', 'cyan inverted'),
                call(29, 19, '                                             ', 'cyan'),

                # Enter
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0, Filter: E_01'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(3, 0, '                  MODULE_VOLTAGE_01: 39064'),
                call(4, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_filter_container_signal(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[
            'f', 'e', '3', '_', 'c', '\n', 'q'
        ])
        args = Args('tests/files/arxml/system-4.2.arxml')
        args.no_strict = True
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=102,
            data=b'\x0A\x0B\x0C\x09\xE2\xD8\x7F\xD6\x00\x86\xB2\x65\x4F\x1D\x2E\x3F\x07\xC0\x00\x5C\x84\x00\x00\x00\x01\x02\x03\x04\x7A\x0E\x00\x00\x04\x05\x06\x06\x2D\x04\x00\x00\x76\x03\x07\x08\x09\x0A\xC6\xEA\x00\x00\x00\x00\x00\x00\x00\x00'))
        monitor.run(1)


        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll('),
                call(3, 0, '                  message1,'),
                call(4, 0, '                  message2,'),
                call(5, 0, '                  message3,'),
                call(6, 0, '                  message3_secured,'),
                call(7, 0, '                  multiplexed_message'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  OneToContainThemAll :: message1('),
                call(10, 0, '                  message1_SeqCounter: 55522,'),
                call(11, 0, '                  message1_CRC: 54911,'),
                call(12, 0, '                  signal6: zero,'),
                call(13, 0, '                  signal1: 0 m,'),
                call(14, 0, '                  signal5: 3853682176.0'),
                call(15, 0, '              )'),
                call(16, 0, '       0.000  OneToContainThemAll :: message2('),
                call(17, 0, '                  signal3: 3,'),
                call(18, 0, '                  signal2: 279,'),
                call(19, 0, '                  signal4: two'),
                call(20, 0, '              )'),
                call(21, 0, '       0.000  OneToContainThemAll :: message3('),
                call(22, 0, '                  message3_CRC: 122,'),
                call(23, 0, '                  message3_SeqCounter: 14'),
                call(24, 0, '              )'),
                call(25, 0, '       0.000  OneToContainThemAll :: message3_secured('),
                call(26, 0, '                  message3_CRC: 45,'),
                call(27, 0, '                  message3_SeqCounter: 4,'),
                call(28, 0, '                  message3_secured_Freshness: 29,'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # 'f' pressed to start filtering
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll('),
                call(3, 0, '                  message1,'),
                call(4, 0, '                  message2,'),
                call(5, 0, '                  message3,'),
                call(6, 0, '                  message3_secured,'),
                call(7, 0, '                  multiplexed_message'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  OneToContainThemAll :: message1('),
                call(10, 0, '                  message1_SeqCounter: 55522,'),
                call(11, 0, '                  message1_CRC: 54911,'),
                call(12, 0, '                  signal6: zero,'),
                call(13, 0, '                  signal1: 0 m,'),
                call(14, 0, '                  signal5: 3853682176.0'),
                call(15, 0, '              )'),
                call(16, 0, '       0.000  OneToContainThemAll :: message2('),
                call(17, 0, '                  signal3: 3,'),
                call(18, 0, '                  signal2: 279,'),
                call(19, 0, '                  signal4: two'),
                call(20, 0, '              )'),
                call(21, 0, '       0.000  OneToContainThemAll :: message3('),
                call(22, 0, '                  message3_CRC: 122,'),
                call(23, 0, '                  message3_SeqCounter: 14'),
                call(24, 0, '              )'),
                call(25, 0, '       0.000  OneToContainThemAll :: message3_secured('),
                call(26, 0, '                  message3_CRC: 45,'),
                call(27, 0, '                  message3_SeqCounter: 4,'),
                call(28, 0, '                  message3_secured_Freshness: 29,'),
                call(29, 0, 'Filter regex: ', 'cyan'),
                call(29, 14, ' ', 'cyan inverted'),
                call(29, 15, '                                                 ', 'cyan'),

                # Match on 'e'
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: e'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll('),
                call(3, 0, '                  message1,'),
                call(4, 0, '                  message2,'),
                call(5, 0, '                  message3,'),
                call(6, 0, '                  message3_secured,'),
                call(7, 0, '                  multiplexed_message'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  OneToContainThemAll :: message1('),
                call(10, 0, '                  message1_SeqCounter: 55522,'),
                call(11, 0, '                  message1_CRC: 54911,'),
                call(12, 0, '                  signal6: zero,'),
                call(13, 0, '                  signal1: 0 m,'),
                call(14, 0, '                  signal5: 3853682176.0'),
                call(15, 0, '              )'),
                call(16, 0, '       0.000  OneToContainThemAll :: message2('),
                call(17, 0, '                  signal3: 3,'),
                call(18, 0, '                  signal2: 279,'),
                call(19, 0, '                  signal4: two'),
                call(20, 0, '              )'),
                call(21, 0, '       0.000  OneToContainThemAll :: message3('),
                call(22, 0, '                  message3_CRC: 122,'),
                call(23, 0, '                  message3_SeqCounter: 14'),
                call(24, 0, '              )'),
                call(25, 0, '       0.000  OneToContainThemAll :: message3_secured('),
                call(26, 0, '                  message3_CRC: 45,'),
                call(27, 0, '                  message3_SeqCounter: 4,'),
                call(28, 0, '                  message3_secured_Freshness: 29,'),
                call(29, 0, 'Filter regex: e', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # Match on 'e3'
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: e3'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll('),
                call(3, 0, '                  message1,'),
                call(4, 0, '                  message2,'),
                call(5, 0, '                  message3,'),
                call(6, 0, '                  message3_secured,'),
                call(7, 0, '                  multiplexed_message'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  OneToContainThemAll :: message3('),
                call(10, 0, '                  message3_CRC: 122,'),
                call(11, 0, '                  message3_SeqCounter: 14'),
                call(12, 0, '              )'),
                call(13, 0, '       0.000  OneToContainThemAll :: message3_secured('),
                call(14, 0, '                  message3_CRC: 45,'),
                call(15, 0, '                  message3_SeqCounter: 4,'),
                call(16, 0, '                  message3_secured_Freshness: 29,'),
                call(17, 0, '                  message3_secured_Authenticator: 515'),
                call(18, 0, '              )'),
                call(29, 0, 'Filter regex: e3', 'cyan'),
                call(29, 16, ' ', 'cyan inverted'),
                call(29, 17, '                                               ', 'cyan'),

                # Match on 'e3_'
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: e3_'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll('),
                call(3, 0, '                  message1,'),
                call(4, 0, '                  message2,'),
                call(5, 0, '                  message3,'),
                call(6, 0, '                  message3_secured,'),
                call(7, 0, '                  multiplexed_message'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  OneToContainThemAll :: message3('),
                call(10, 0, '                  message3_CRC: 122,'),
                call(11, 0, '                  message3_SeqCounter: 14'),
                call(12, 0, '              )'),
                call(13, 0, '       0.000  OneToContainThemAll :: message3_secured('),
                call(14, 0, '                  message3_CRC: 45,'),
                call(15, 0, '                  message3_SeqCounter: 4,'),
                call(16, 0, '                  message3_secured_Freshness: 29,'),
                call(17, 0, '                  message3_secured_Authenticator: 515'),
                call(18, 0, '              )'),
                call(29, 0, 'Filter regex: e3_', 'cyan'),
                call(29, 17, ' ', 'cyan inverted'),
                call(29, 18, '                                              ', 'cyan'),

                # Match on 'e3_c'
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: e3_c'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll :: message3('),
                call(3, 0, '                  message3_CRC: 122'),
                call(4, 0, '              )'),
                call(5, 0, '       0.000  OneToContainThemAll :: message3_secured('),
                call(6, 0, '                  message3_CRC: 45'),
                call(7, 0, '              )'),
                call(29, 0, 'Filter regex: e3_c', 'cyan'),
                call(29, 18, ' ', 'cyan inverted'),
                call(29, 19, '                                             ', 'cyan'),

                # Enter
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0, Filter: e3_c'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll :: message3('),
                call(3, 0, '                  message3_CRC: 122'),
                call(4, 0, '              )'),
                call(5, 0, '       0.000  OneToContainThemAll :: message3_secured('),
                call(6, 0, '                  message3_CRC: 45'),
                call(7, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])
        
    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_container_multiplex(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/arxml/system-4.2.arxml')
        args.no_strict = True
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        # OneToContainThemAll with message1 and multiplexed SELECT_HELLO
        monitor.on_message_received(can.Message(
            arbitration_id=102,
            data=b'\n\x0b\x0c\t{\x00\xc8\x01\x01\x00\x00\xa0@\x07\x08\t\n\x11\x02\x00\x00\x00\x00\x00\x00\x00\x00'))
        # OneToContainThemAll with message1 and multiplexed SELECT_WORLD
        monitor.on_message_received(can.Message(
            arbitration_id=102,
            timestamp=10,
            data=b'\n\x0b\x0c\tA\x01\x8e\x02\x00\x00\x00\x80@\x07\x08\t\nQ\x02\x00\x00\x00\x00\x00\x00\x00\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '      10.000  OneToContainThemAll('),
                call(3, 0, '                  message1,'),
                call(4, 0, '                  multiplexed_message'),
                call(5, 0, '              )'),
                call(6, 0, '      10.000  OneToContainThemAll :: message1('),
                call(7, 0, '                  message1_SeqCounter: 321,'),
                call(8, 0, '                  message1_CRC: 654,'),
                call(9, 0, '                  signal6: zero,'),
                call(10, 0, '                  signal1: 0 m,'),
                call(11, 0, '                  signal5: 4.0'),
                call(12, 0, '              )'),
                call(13, 0, '       0.000  OneToContainThemAll :: multiplexed_message :: OneToContainThemAll_selector1=SELECT_HELLO('),
                call(14, 0, '                  MultiplexedStatic: 1,'),
                call(15, 0, '                  Hello: 2,'),
                call(16, 0, '                  OneToContainThemAll_selector1: SELECT_HELLO,'),
                call(17, 0, '                  MultiplexedStatic2: 2'),
                call(18, 0, '              )'),
                call(19, 0, '      10.000  OneToContainThemAll :: multiplexed_message :: OneToContainThemAll_selector1=SELECT_WORLD('),
                call(20, 0, '                  MultiplexedStatic: 1,'),
                call(21, 0, '                  World2: 0,'),
                call(22, 0, '                  World1: 1,'),
                call(23, 0, '                  OneToContainThemAll_selector1: SELECT_WORLD,'),
                call(24, 0, '                  MultiplexedStatic2: 2'),
                call(25, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])
        
    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_container_undecoded(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/arxml/system-4.2.arxml')
        args.no_strict = True
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        # OneToContainThemAll with message1 and undecoded trailing data
        monitor.on_message_received(can.Message(
            arbitration_id=102,
            data=b'\n\x0b\x0c\t{\x00\xc8\x01\x04V\x0eI@\x00\x00\x00\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  OneToContainThemAll('),
                call(3, 0, '                  message1,'),
                call(4, 0, '                  0x0'),
                call(5, 0, '              )'),
                call(6, 0, '       0.000  OneToContainThemAll :: 0x0('),
                call(7, 0, '                  undecoded: <empty>'),
                call(8, 0, '              )'),
                call(9, 0, '       0.000  OneToContainThemAll :: message1('),
                call(10, 0, '                  message1_SeqCounter: 123,'),
                call(11, 0, '                  message1_CRC: 456,'),
                call(12, 0, '                  signal6: zero,'),
                call(13, 0, '                  signal1: 5 m,'),
                call(14, 0, '                  signal5: 3.1414999961853027'),
                call(15, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_container_multiplex_singleline(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/arxml/system-4.2.arxml')
        args.no_strict = True
        args.single_line = True
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        # OneToContainThemAll with message1 and multiplexed SELECT_HELLO
        monitor.on_message_received(can.Message(
            arbitration_id=102,
            data=b'\n\x0b\x0c\t{\x00\xc8\x01\x01\x00\x00\xa0@\x07\x08\t\n\x11\x02\x00\x00\x00\x00\x00\x00\x00\x00'))
        # OneToContainThemAll with message1 and multiplexed SELECT_WORLD
        monitor.on_message_received(can.Message(
            arbitration_id=102,
            timestamp=10,
            data=b'\n\x0b\x0c\tA\x01\x8e\x02\x00\x00\x00\x80@\x07\x08\t\nQ\x02\x00\x00\x00\x00\x00\x00\x00\x00'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '      10.000  OneToContainThemAll(message1, multiplexed_message)'),
                call(3, 0, '      10.000  OneToContainThemAll :: message1(message1_SeqCounter: 321, message1_CRC: 654, signal6: zero, signal1: 0 m, signal5: 4.0)'),
                call(4, 0, '       0.000  OneToContainThemAll :: multiplexed_message :: OneToContainThemAll_selector1=SELECT_HELLO(MultiplexedStatic: 1, Hello: 2, OneToContainThemAll_selector1: SELECT_HELLO, MultiplexedStatic2: 2)'),
                call(5, 0, '      10.000  OneToContainThemAll :: multiplexed_message :: OneToContainThemAll_selector1=SELECT_WORLD(MultiplexedStatic: 1, World2: 0, World1: 1, OneToContainThemAll_selector1: SELECT_WORLD, MultiplexedStatic2: 2)'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_reset(self,
                   _use_default_colors,
                   _curs_set,
                   _init_pair,
                   is_term_resized,
                   color_pair,
                   _bus,
                   _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[
            'f', 'E', '\n', 'p', ' ', 'r', 'f', '\n', 'q'
        ])
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
            timestamp=3))

        # Discarded.
        monitor.on_message_received(can.Message(
            arbitration_id=497,
            data=b'\xc0\x06\xb0\x00\x00\x00\x00\x00',
            timestamp=6))

        monitor.tick(1)
        monitor.tick(1)
        monitor.tick(1)

        # Input another before pause.
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xc0\x00\x00\x00\x00\x00',
            timestamp=7))

        monitor.tick(1)

        # Input when paused. Will not be displayed.
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xd0\x00\x00\x00\x00\x00',
            timestamp=10))

        monitor.tick(1)
        monitor.tick(1)
        monitor.tick(1)

        # Input after reset.
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\x00\x00\x00\x00\x00\x00',
            timestamp=11))

        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # One ok and one with bad frame id.
                call(0, 0, 'Received: 2, Discarded: 1, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # 'f' pressed.
                call(0, 0, 'Received: 2, Discarded: 1, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: ', 'cyan'),
                call(29, 14, ' ', 'cyan inverted'),
                call(29, 15, '                                                 ', 'cyan'),

                # 'E' pressed.
                call(0, 0, 'Received: 2, Discarded: 1, Errors: 0, Filter: E'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: E', 'cyan'),
                call(29, 15, ' ', 'cyan inverted'),
                call(29, 16, '                                                ', 'cyan'),

                # '\n' pressed.
                call(0, 0, 'Received: 3, Discarded: 1, Errors: 0, Filter: E'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       4.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.54 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # 'p' pressed. Input frame not displayed.

                # 'r' pressed.
                call(0, 0, 'Received: 0, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Input after reset. 'f' pressed.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.48 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'Filter regex: ', 'cyan'),
                call(29, 14, ' ', 'cyan inverted'),
                call(29, 15, '                                                 ', 'cyan'),

                # '\n' pressed.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.48 degK'),
                call(6, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')

                # 'q' pressed, no redraw.
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_play_pause(self,
                        _use_default_colors,
                        _curs_set,
                        _init_pair,
                        is_term_resized,
                        color_pair,
                        _bus,
                        _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[
            ' ', ' ', 'p', ' ', ' ', 'p', ' ', ' ', ' ', 'q'
        ])
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)

        for timestamp in range(4):
            monitor.on_message_received(can.Message(
                arbitration_id=496,
                data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
                timestamp=timestamp))

            monitor.tick(1)

        # Display most recently received at unpause.
        monitor.tick(1)
        monitor.tick(1)
        monitor.tick(1)

        for timestamp in range(5, 7):
            monitor.on_message_received(can.Message(
                arbitration_id=496,
                data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
                timestamp=timestamp))

            monitor.tick(1)

        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # Received when playing.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                call(0, 0, 'Received: 2, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       1.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                call(0, 0, 'Received: 3, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       2.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                # Received when paused, displayed at unpause.
                call(0, 0, 'Received: 4, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       3.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                # Received when playing.
                call(0, 0, 'Received: 5, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       5.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                call(0, 0, 'Received: 6, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       6.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_resize(self,
                    _use_default_colors,
                    _curs_set,
                    _init_pair,
                    is_term_resized,
                    color_pair,
                    _bus,
                    _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[' ', ' ', 'q'],
                        resolution=[(30, 40), (25, 35), (25, 35), (20, 30)])
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = True

        # Run monitor.
        monitor = Monitor(stdscr, args)

        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
            timestamp=1))

        monitor.tick(1)
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # 25 x 35.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE              ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(24, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset', 'cyan'),

                # 25 x 35.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE              ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(24, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset', 'cyan'),

                # 20 x 30.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE         ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: Enabled,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(19, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset', 'cyan')
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_display_paginated_data(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr(user_input=[
            ' ', 'KEY_NPAGE', 'KEY_NPAGE', 'KEY_NPAGE', 'KEY_PPAGE', 'q'
        ])
        args = Args('tests/files/dbc/msxii_system_can.dbc')
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x00\x00\x98\x98\x0b\x00',
            timestamp=0))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x01\x00\x98\x98\x0b\x00',
            timestamp=1))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x02\x00\x98\x98\x0b\x00',
            timestamp=2))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x03\x00\x98\x98\x0b\x00',
            timestamp=3))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x04\x00\x98\x98\x0b\x00',
            timestamp=4))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x05\x00\x98\x98\x0b\x00',
            timestamp=5))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x06\x00\x98\x98\x0b\x00',
            timestamp=6))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x07\x00\x98\x98\x0b\x00',
            timestamp=7))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x08\x00\x98\x98\x0b\x00',
            timestamp=8))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x09\x00\x98\x98\x0b\x00',
            timestamp=9))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x0a\x00\x98\x98\x0b\x00',
            timestamp=10))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x0b\x00\x98\x98\x0b\x00',
            timestamp=11))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x0c\x00\x98\x98\x0b\x00',
            timestamp=12))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x0d\x00\x98\x98\x0b\x00',
            timestamp=13))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x0e\x00\x98\x98\x0b\x00',
            timestamp=14))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x0f\x00\x98\x98\x0b\x00',
            timestamp=15))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x10\x00\x98\x98\x0b\x00',
            timestamp=16))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x11\x00\x98\x98\x0b\x00',
            timestamp=17))
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x12\x00\x98\x98\x0b\x00',
            timestamp=18))
        monitor.tick(1)
        monitor.run(1)

        self.maxDiff = None
        unittest.util._MAX_LENGTH=20000

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # Start on page 1
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT :: BATTERY_VT_INDEX=0('),
                call(3, 0, '                  BATTERY_VT_INDEX: 0,'),
                call(4, 0, '                  MODULE_VOLTAGE_00: 39064,'),
                call(5, 0, '                  MODULE_TEMP_00: 11'),
                call(6, 0, '              )'),
                call(7, 0, '       1.000  BATTERY_VT :: BATTERY_VT_INDEX=1('),
                call(8, 0, '                  BATTERY_VT_INDEX: 1,'),
                call(9, 0, '                  MODULE_VOLTAGE_01: 39064,'),
                call(10, 0, '                  MODULE_TEMP_01: 11'),
                call(11, 0, '              )'),
                call(12, 0, '      10.000  BATTERY_VT :: BATTERY_VT_INDEX=10('),
                call(13, 0, '                  BATTERY_VT_INDEX: 10,'),
                call(14, 0, '                  MODULE_VOLTAGE_10: 39064,'),
                call(15, 0, '                  MODULE_TEMP_10: 11'),
                call(16, 0, '              )'),
                call(17, 0, '      11.000  BATTERY_VT :: BATTERY_VT_INDEX=11('),
                call(18, 0, '                  BATTERY_VT_INDEX: 11,'),
                call(19, 0, '                  MODULE_VOLTAGE_11: 39064,'),
                call(20, 0, '                  MODULE_TEMP_11: 11'),
                call(21, 0, '              )'),
                call(22, 0, '      12.000  BATTERY_VT :: BATTERY_VT_INDEX=12('),
                call(23, 0, '                  BATTERY_VT_INDEX: 12,'),
                call(24, 0, '                  MODULE_VOLTAGE_12: 39064,'),
                call(25, 0, '                  MODULE_TEMP_12: 11'),
                call(26, 0, '              )'),
                call(27, 0, '      13.000  BATTERY_VT :: BATTERY_VT_INDEX=13('),
                call(28, 0, '                  BATTERY_VT_INDEX: 13,'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move to page 2
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '                  MODULE_VOLTAGE_13: 39064,'),
                call(3, 0, '                  MODULE_TEMP_13: 11'),
                call(4, 0, '              )'),
                call(5, 0, '      14.000  BATTERY_VT :: BATTERY_VT_INDEX=14('),
                call(6, 0, '                  BATTERY_VT_INDEX: 14,'),
                call(7, 0, '                  MODULE_VOLTAGE_14: 39064,'),
                call(8, 0, '                  MODULE_TEMP_14: 11'),
                call(9, 0, '              )'),
                call(10, 0, '      15.000  BATTERY_VT :: BATTERY_VT_INDEX=15('),
                call(11, 0, '                  BATTERY_VT_INDEX: 15,'),
                call(12, 0, '                  MODULE_VOLTAGE_15: 39064,'),
                call(13, 0, '                  MODULE_TEMP_15: 11'),
                call(14, 0, '              )'),
                call(15, 0, '      16.000  BATTERY_VT :: BATTERY_VT_INDEX=16('),
                call(16, 0, '                  BATTERY_VT_INDEX: 16,'),
                call(17, 0, '                  MODULE_VOLTAGE_16: 39064,'),
                call(18, 0, '                  MODULE_TEMP_16: 11'),
                call(19, 0, '              )'),
                call(20, 0, '      17.000  BATTERY_VT :: BATTERY_VT_INDEX=17('),
                call(21, 0, '                  BATTERY_VT_INDEX: 17,'),
                call(22, 0, '                  MODULE_VOLTAGE_17: 39064,'),
                call(23, 0, '                  MODULE_TEMP_17: 11'),
                call(24, 0, '              )'),
                call(25, 0, '      18.000  BATTERY_VT :: BATTERY_VT_INDEX=18('),
                call(26, 0, '                  BATTERY_VT_INDEX: 18,'),
                call(27, 0, '                  MODULE_VOLTAGE_18: 39064,'),
                call(28, 0, '                  MODULE_TEMP_18: 11'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move to page 3
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '              )'),
                call(3, 0, '       2.000  BATTERY_VT :: BATTERY_VT_INDEX=2('),
                call(4, 0, '                  BATTERY_VT_INDEX: 2,'),
                call(5, 0, '                  MODULE_VOLTAGE_02: 39064,'),
                call(6, 0, '                  MODULE_TEMP_02: 11'),
                call(7, 0, '              )'),
                call(8, 0, '       3.000  BATTERY_VT :: BATTERY_VT_INDEX=3('),
                call(9, 0, '                  BATTERY_VT_INDEX: 3,'),
                call(10, 0, '                  MODULE_VOLTAGE_03: 39064,'),
                call(11, 0, '                  MODULE_TEMP_03: 11'),
                call(12, 0, '              )'),
                call(13, 0, '       4.000  BATTERY_VT :: BATTERY_VT_INDEX=4('),
                call(14, 0, '                  BATTERY_VT_INDEX: 4,'),
                call(15, 0, '                  MODULE_VOLTAGE_04: 39064,'),
                call(16, 0, '                  MODULE_TEMP_04: 11'),
                call(17, 0, '              )'),
                call(18, 0, '       5.000  BATTERY_VT :: BATTERY_VT_INDEX=5('),
                call(19, 0, '                  BATTERY_VT_INDEX: 5,'),
                call(20, 0, '                  MODULE_VOLTAGE_05: 39064,'),
                call(21, 0, '                  MODULE_TEMP_05: 11'),
                call(22, 0, '              )'),
                call(23, 0, '       6.000  BATTERY_VT :: BATTERY_VT_INDEX=6('),
                call(24, 0, '                  BATTERY_VT_INDEX: 6,'),
                call(25, 0, '                  MODULE_VOLTAGE_06: 39064,'),
                call(26, 0, '                  MODULE_TEMP_06: 11'),
                call(27, 0, '              )'),
                call(28, 0, '       7.000  BATTERY_VT :: BATTERY_VT_INDEX=7('),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move to page 4
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '                  MODULE_TEMP_04: 11'),
                call(3, 0, '              )'),
                call(4, 0, '       5.000  BATTERY_VT :: BATTERY_VT_INDEX=5('),
                call(5, 0, '                  BATTERY_VT_INDEX: 5,'),
                call(6, 0, '                  MODULE_VOLTAGE_05: 39064,'),
                call(7, 0, '                  MODULE_TEMP_05: 11'),
                call(8, 0, '              )'),
                call(9, 0, '       6.000  BATTERY_VT :: BATTERY_VT_INDEX=6('),
                call(10, 0, '                  BATTERY_VT_INDEX: 6,'),
                call(11, 0, '                  MODULE_VOLTAGE_06: 39064,'),
                call(12, 0, '                  MODULE_TEMP_06: 11'),
                call(13, 0, '              )'),
                call(14, 0, '       7.000  BATTERY_VT :: BATTERY_VT_INDEX=7('),
                call(15, 0, '                  BATTERY_VT_INDEX: 7,'),
                call(16, 0, '                  MODULE_VOLTAGE_07: 39064,'),
                call(17, 0, '                  MODULE_TEMP_07: 11'),
                call(18, 0, '              )'),
                call(19, 0, '       8.000  BATTERY_VT :: BATTERY_VT_INDEX=8('),
                call(20, 0, '                  BATTERY_VT_INDEX: 8,'),
                call(21, 0, '                  MODULE_VOLTAGE_08: 39064,'),
                call(22, 0, '                  MODULE_TEMP_08: 11'),
                call(23, 0, '              )'),
                call(24, 0, '       9.000  BATTERY_VT :: BATTERY_VT_INDEX=9('),
                call(25, 0, '                  BATTERY_VT_INDEX: 9,'),
                call(26, 0, '                  MODULE_VOLTAGE_09: 39064,'),
                call(27, 0, '                  MODULE_TEMP_09: 11'),
                call(28, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move back to page 3
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '                  BATTERY_VT_INDEX: 16,'),
                call(3, 0, '                  MODULE_VOLTAGE_16: 39064,'),
                call(4, 0, '                  MODULE_TEMP_16: 11'),
                call(5, 0, '              )'),
                call(6, 0, '      17.000  BATTERY_VT :: BATTERY_VT_INDEX=17('),
                call(7, 0, '                  BATTERY_VT_INDEX: 17,'),
                call(8, 0, '                  MODULE_VOLTAGE_17: 39064,'),
                call(9, 0, '                  MODULE_TEMP_17: 11'),
                call(10, 0, '              )'),
                call(11, 0, '      18.000  BATTERY_VT :: BATTERY_VT_INDEX=18('),
                call(12, 0, '                  BATTERY_VT_INDEX: 18,'),
                call(13, 0, '                  MODULE_VOLTAGE_18: 39064,'),
                call(14, 0, '                  MODULE_TEMP_18: 11'),
                call(15, 0, '              )'),
                call(16, 0, '       2.000  BATTERY_VT :: BATTERY_VT_INDEX=2('),
                call(17, 0, '                  BATTERY_VT_INDEX: 2,'),
                call(18, 0, '                  MODULE_VOLTAGE_02: 39064,'),
                call(19, 0, '                  MODULE_TEMP_02: 11'),
                call(20, 0, '              )'),
                call(21, 0, '       3.000  BATTERY_VT :: BATTERY_VT_INDEX=3('),
                call(22, 0, '                  BATTERY_VT_INDEX: 3,'),
                call(23, 0, '                  MODULE_VOLTAGE_03: 39064,'),
                call(24, 0, '                  MODULE_TEMP_03: 11'),
                call(25, 0, '              )'),
                call(26, 0, '       4.000  BATTERY_VT :: BATTERY_VT_INDEX=4('),
                call(27, 0, '                  BATTERY_VT_INDEX: 4,'),
                call(28, 0, '                  MODULE_VOLTAGE_04: 39064,'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),
            ])

    @patch('can.Notifier')
    @patch('can.Bus')
    @patch('curses.color_pair')
    @patch('curses.is_term_resized')
    @patch('curses.init_pair')
    @patch('curses.curs_set')
    @patch('curses.use_default_colors')
    def test_bad_message_length_error(self,
                                _use_default_colors,
                                _curs_set,
                                _init_pair,
                                is_term_resized,
                                color_pair,
                                _bus,
                                _notifier):
        # Prepare mocks.
        stdscr = StdScr()
        args = Args('tests/files/dbc/bad_message_length.dbc')
        args.no_strict = True
        color_pair.side_effect = lambda i: self.color_pair_side_effect[i]
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1,
            data=b'\x24'))
        monitor.run(1)

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 1'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  Message1(undecoded, unpacking failed: 0x24)'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan')
            ])

if __name__ == '__main__':
    unittest.main()
