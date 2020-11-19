import unittest
import curses

try:
    from unittest.mock import Mock
    from unittest.mock import patch
    from unittest.mock import call
except ImportError:
    from mock import Mock
    from mock import patch
    from mock import call

import can
from cantools.subparsers.monitor import Monitor


class Args(object):

    def __init__(self,
                 database,
                 single_line=False):
        self.database = database
        self.encoding = None
        self.frame_id_mask = None
        self.no_strict = False
        self.single_line = single_line
        self.bit_rate = None
        self.fd = False
        self.bus_type = 'socketcan'
        self.channel = 'vcan0'


class StdScr(object):

    def __init__(self, user_input=None, resolution=None):
        if resolution is None:
            resolution = [(30, 64)]

        self.getmaxyx = Mock(side_effect=resolution)
        self.nodelay = Mock()
        self.clear = Mock()
        self.addstr = Mock()
        self.refresh = Mock()
        self.keypad = Mock()

        if user_input is None:
            user_input = ['q']

        self.getkey = Mock(side_effect=user_input)
        self.move = Mock()


class CanToolsMonitorTest(unittest.TestCase):

    maxDiff = None

    def assert_called(self, mock, expected):
        self.assertEqual(mock.call_args_list, expected)

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
        color_pair.side_effect = ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.run()

        # Check mocks.
        self.assert_called(use_default_colors, [call()])
        self.assert_called(curs_set, [call(False)])
        self.assert_called(
            init_pair,
            [
                call(1, curses.COLOR_BLACK, curses.COLOR_GREEN),
                call(2, curses.COLOR_BLACK, curses.COLOR_CYAN)
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
        monitor.run()

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
        color_pair.side_effect = ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00'))
        monitor.run()

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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
        color_pair.side_effect = ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00'))
        monitor.run()

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
                     "       0.000  ExampleMessage(Enable: 'Enabled' -, "
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
        color_pair.side_effect = ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x24\x00\x98\x98\x0b\x00'))
        monitor.run()

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 1, Errors: 0'),
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
        color_pair.side_effect = ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x00\x00\x98\x98\x0b\x00'))
        monitor.run()

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  BATTERY_VT('),
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
        color_pair.side_effect = ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=1025,
            data=b'\x00\x00\x98\x98\x0b\x00'))
        monitor.run()

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
                     "       0.000  BATTERY_VT(BATTERY_VT_INDEX: 0, "
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
        color_pair.side_effect = ['green', 'cyan']
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
        monitor.run()

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
                     "       1.000  Extended(S0: 0, S1: 0, S2: 0, S3: 0, S6: 1, S7: 0)"),
                call(3,
                     0,
                     "       0.000  Extended(S0: 0, S1: 0, S2: 0, S3: 0, S6: 2, S8: 0)"),
                call(4,
                     0,
                     "       3.000  Extended(S0: 0, S1: 2, S4: 0, S6: 1, S7: 0)"),
                call(5,
                     0,
                     "       2.000  Extended(S0: 1, S5: 0, S6: 1, S7: 0)"),
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
        color_pair.side_effect = ['green', 'cyan']
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
        monitor.run()

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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
            'f', 'Y', '[', '\b', '\n', 'f', '\b', 'E', '\n', 'q'
        ])
        args = Args('tests/files/dbc/motohawk.dbc')
        color_pair.side_effect = 10 * ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00'))
        monitor.run()

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # No filter.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                # 'f' pressed.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'Filter:                                                         ',
                     'cyan'),

                # No match on 'Y'.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(29,
                     0,
                     'Filter: Y                                                       ',
                     'cyan'),

                # Invalid filter 'Y['.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'Filter: Y[                                                      ',
                     'cyan'),

                # No match on 'Y'.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(29,
                     0,
                     'Filter: Y                                                       ',
                     'cyan'),

                # Hit enter to hide filter.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(29,
                     0,
                     'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                # 'f' pressed again.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(29,
                     0,
                     'Filter: Y                                                       ',
                     'cyan'),

                # Backspace.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'Filter:                                                         ',
                     'cyan'),

                # Match on 'E'.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29,
                     0,
                     'Filter: E                                                       ',
                     'cyan'),

                # Hit enter to hide filter.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
        color_pair.side_effect = 10 * ['green', 'cyan']
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

        monitor.tick()
        monitor.tick()
        monitor.tick()

        # Input another before pause.
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xc0\x00\x00\x00\x00\x00',
            timestamp=7))

        monitor.tick()

        # Input when paused. Will not be displayed.
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xd0\x00\x00\x00\x00\x00',
            timestamp=10))

        monitor.tick()
        monitor.tick()
        monitor.tick()

        # Input after reset.
        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\x00\x00\x00\x00\x00\x00',
            timestamp=11))

        monitor.run()

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # One ok and one with bad frame id.
                call(0, 0, 'Received: 2, Discarded: 1, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                # 'f' pressed.
                call(0, 0, 'Received: 2, Discarded: 1, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'Filter:                                                         ',
                     'cyan'),

                # 'E' pressed.
                call(0, 0, 'Received: 2, Discarded: 1, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'Filter: E                                                       ',
                     'cyan'),

                # '\n' pressed.
                call(0, 0, 'Received: 3, Discarded: 1, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       4.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.54 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                # 'p' pressed. Input frame not displayed.

                # 'r' pressed.
                call(0, 0, 'Received: 0, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan'),

                # Input after reset. 'f' pressed.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.48 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'Filter:                                                         ',
                     'cyan'),

                # '\n' pressed.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1,
                     0,
                     '   TIMESTAMP  MESSAGE                                           ',
                     'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.48 degK'),
                call(6, 0, '              )'),
                call(29
                     ,
                     0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ',
                     'cyan')

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
        color_pair.side_effect = 8 * ['green', 'cyan']
        is_term_resized.return_value = False

        # Run monitor.
        monitor = Monitor(stdscr, args)

        for timestamp in range(4):
            monitor.on_message_received(can.Message(
                arbitration_id=496,
                data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
                timestamp=timestamp))

            monitor.tick()

        # Display most recently received at unpause.
        monitor.tick()
        monitor.tick()
        monitor.tick()

        for timestamp in range(5, 7):
            monitor.on_message_received(can.Message(
                arbitration_id=496,
                data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
                timestamp=timestamp))

            monitor.tick()

        monitor.run()

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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
        color_pair.side_effect = 3 * ['green', 'cyan']
        is_term_resized.return_value = True

        # Run monitor.
        monitor = Monitor(stdscr, args)

        monitor.on_message_received(can.Message(
            arbitration_id=496,
            data=b'\xc0\x06\xe0\x00\x00\x00\x00\x00',
            timestamp=1))

        monitor.tick()
        monitor.run()

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # 25 x 35.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE              ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(24, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset', 'cyan'),

                # 25 x 35.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE              ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
                call(4, 0, '                  AverageRadius: 3.2 m,'),
                call(5, 0, '                  Temperature: 250.55 degK'),
                call(6, 0, '              )'),
                call(24, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset', 'cyan'),

                # 20 x 30.
                call(0, 0, 'Received: 1, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE         ', 'green'),
                call(2, 0, '       0.000  ExampleMessage('),
                call(3, 0, "                  Enable: 'Enabled' -,"),
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
        color_pair.side_effect = 5 * ['green', 'cyan']
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
        monitor.tick()
        monitor.run()

        self.maxDiff = None
        unittest.util._MAX_LENGTH=20000

        # Check mocks.
        self.assert_called(
            stdscr.addstr,
            [
                # Start on page 1
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '       0.000  BATTERY_VT('),
                call(3, 0, '                  BATTERY_VT_INDEX: 0,'),
                call(4, 0, '                  MODULE_VOLTAGE_00: 39064,'),
                call(5, 0, '                  MODULE_TEMP_00: 11'),
                call(6, 0, '              )'),
                call(7, 0, '       1.000  BATTERY_VT('),
                call(8, 0, '                  BATTERY_VT_INDEX: 1,'),
                call(9, 0, '                  MODULE_VOLTAGE_01: 39064,'),
                call(10, 0, '                  MODULE_TEMP_01: 11'),
                call(11, 0, '              )'),
                call(12, 0, '      10.000  BATTERY_VT('),
                call(13, 0, '                  BATTERY_VT_INDEX: 10,'),
                call(14, 0, '                  MODULE_VOLTAGE_10: 39064,'),
                call(15, 0, '                  MODULE_TEMP_10: 11'),
                call(16, 0, '              )'),
                call(17, 0, '      11.000  BATTERY_VT('),
                call(18, 0, '                  BATTERY_VT_INDEX: 11,'),
                call(19, 0, '                  MODULE_VOLTAGE_11: 39064,'),
                call(20, 0, '                  MODULE_TEMP_11: 11'),
                call(21, 0, '              )'),
                call(22, 0, '      12.000  BATTERY_VT('),
                call(23, 0, '                  BATTERY_VT_INDEX: 12,'),
                call(24, 0, '                  MODULE_VOLTAGE_12: 39064,'),
                call(25, 0, '                  MODULE_TEMP_12: 11'),
                call(26, 0, '              )'),
                call(27, 0, '      13.000  BATTERY_VT('),
                call(28, 0, '                  BATTERY_VT_INDEX: 13,'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move to page 2
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '                  MODULE_VOLTAGE_13: 39064,'),
                call(3, 0, '                  MODULE_TEMP_13: 11'),
                call(4, 0, '              )'),
                call(5, 0, '      14.000  BATTERY_VT('),
                call(6, 0, '                  BATTERY_VT_INDEX: 14,'),
                call(7, 0, '                  MODULE_VOLTAGE_14: 39064,'),
                call(8, 0, '                  MODULE_TEMP_14: 11'),
                call(9, 0, '              )'),
                call(10, 0, '      15.000  BATTERY_VT('),
                call(11, 0, '                  BATTERY_VT_INDEX: 15,'),
                call(12, 0, '                  MODULE_VOLTAGE_15: 39064,'),
                call(13, 0, '                  MODULE_TEMP_15: 11'),
                call(14, 0, '              )'),
                call(15, 0, '      16.000  BATTERY_VT('),
                call(16, 0, '                  BATTERY_VT_INDEX: 16,'),
                call(17, 0, '                  MODULE_VOLTAGE_16: 39064,'),
                call(18, 0, '                  MODULE_TEMP_16: 11'),
                call(19, 0, '              )'),
                call(20, 0, '      17.000  BATTERY_VT('),
                call(21, 0, '                  BATTERY_VT_INDEX: 17,'),
                call(22, 0, '                  MODULE_VOLTAGE_17: 39064,'),
                call(23, 0, '                  MODULE_TEMP_17: 11'),
                call(24, 0, '              )'),
                call(25, 0, '      18.000  BATTERY_VT('),
                call(26, 0, '                  BATTERY_VT_INDEX: 18,'),
                call(27, 0, '                  MODULE_VOLTAGE_18: 39064,'),
                call(28, 0, '                  MODULE_TEMP_18: 11'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move to page 3
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '              )'),
                call(3, 0, '       2.000  BATTERY_VT('),
                call(4, 0, '                  BATTERY_VT_INDEX: 2,'),
                call(5, 0, '                  MODULE_VOLTAGE_02: 39064,'),
                call(6, 0, '                  MODULE_TEMP_02: 11'),
                call(7, 0, '              )'),
                call(8, 0, '       3.000  BATTERY_VT('),
                call(9, 0, '                  BATTERY_VT_INDEX: 3,'),
                call(10, 0, '                  MODULE_VOLTAGE_03: 39064,'),
                call(11, 0, '                  MODULE_TEMP_03: 11'),
                call(12, 0, '              )'),
                call(13, 0, '       4.000  BATTERY_VT('),
                call(14, 0, '                  BATTERY_VT_INDEX: 4,'),
                call(15, 0, '                  MODULE_VOLTAGE_04: 39064,'),
                call(16, 0, '                  MODULE_TEMP_04: 11'),
                call(17, 0, '              )'),
                call(18, 0, '       5.000  BATTERY_VT('),
                call(19, 0, '                  BATTERY_VT_INDEX: 5,'),
                call(20, 0, '                  MODULE_VOLTAGE_05: 39064,'),
                call(21, 0, '                  MODULE_TEMP_05: 11'),
                call(22, 0, '              )'),
                call(23, 0, '       6.000  BATTERY_VT('),
                call(24, 0, '                  BATTERY_VT_INDEX: 6,'),
                call(25, 0, '                  MODULE_VOLTAGE_06: 39064,'),
                call(26, 0, '                  MODULE_TEMP_06: 11'),
                call(27, 0, '              )'),
                call(28, 0, '       7.000  BATTERY_VT('),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move to page 4
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '                  BATTERY_VT_INDEX: 7,'),
                call(3, 0, '                  MODULE_VOLTAGE_07: 39064,'),
                call(4, 0, '                  MODULE_TEMP_07: 11'),
                call(5, 0, '              )'),
                call(6, 0, '       8.000  BATTERY_VT('),
                call(7, 0, '                  BATTERY_VT_INDEX: 8,'),
                call(8, 0, '                  MODULE_VOLTAGE_08: 39064,'),
                call(9, 0, '                  MODULE_TEMP_08: 11'),
                call(10, 0, '              )'),
                call(11, 0, '       9.000  BATTERY_VT('),
                call(12, 0, '                  BATTERY_VT_INDEX: 9,'),
                call(13, 0, '                  MODULE_VOLTAGE_09: 39064,'),
                call(14, 0, '                  MODULE_TEMP_09: 11'),
                call(15, 0, '              )'),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),

                # Move back to page 3
                call(0, 0, 'Received: 19, Discarded: 0, Errors: 0'),
                call(1, 0, '   TIMESTAMP  MESSAGE                                           ', 'green'),
                call(2, 0, '              )'),
                call(3, 0, '       2.000  BATTERY_VT('),
                call(4, 0, '                  BATTERY_VT_INDEX: 2,'),
                call(5, 0, '                  MODULE_VOLTAGE_02: 39064,'),
                call(6, 0, '                  MODULE_TEMP_02: 11'),
                call(7, 0, '              )'),
                call(8, 0, '       3.000  BATTERY_VT('),
                call(9, 0, '                  BATTERY_VT_INDEX: 3,'),
                call(10, 0, '                  MODULE_VOLTAGE_03: 39064,'),
                call(11, 0, '                  MODULE_TEMP_03: 11'),
                call(12, 0, '              )'),
                call(13, 0, '       4.000  BATTERY_VT('),
                call(14, 0, '                  BATTERY_VT_INDEX: 4,'),
                call(15, 0, '                  MODULE_VOLTAGE_04: 39064,'),
                call(16, 0, '                  MODULE_TEMP_04: 11'),
                call(17, 0, '              )'),
                call(18, 0, '       5.000  BATTERY_VT('),
                call(19, 0, '                  BATTERY_VT_INDEX: 5,'),
                call(20, 0, '                  MODULE_VOLTAGE_05: 39064,'),
                call(21, 0, '                  MODULE_TEMP_05: 11'),
                call(22, 0, '              )'),
                call(23, 0, '       6.000  BATTERY_VT('),
                call(24, 0, '                  BATTERY_VT_INDEX: 6,'),
                call(25, 0, '                  MODULE_VOLTAGE_06: 39064,'),
                call(26, 0, '                  MODULE_TEMP_06: 11'),
                call(27, 0, '              )'),
                call(28, 0, '       7.000  BATTERY_VT('),
                call(29, 0, 'q: Quit, f: Filter, p: Play/Pause, r: Reset                     ', 'cyan'),
            ])


if __name__ == '__main__':
    unittest.main()
