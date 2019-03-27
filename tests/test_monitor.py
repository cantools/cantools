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


if __name__ == '__main__':
    unittest.main()
