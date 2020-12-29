#!/usr/bin/env python3

import os
import sys
import datetime
import re
import unittest
from unittest import mock
from io import StringIO
import cantools

matplotlib_mock = mock.Mock()
sys.modules['matplotlib'] = matplotlib_mock
plt = matplotlib_mock.pyplot

class CanToolsPlotTest(unittest.TestCase):

    DBC_FILE = os.path.join(os.path.split(__file__)[0], 'files/dbc/abs.dbc')
    DBC_FILE_CHOICES = os.path.join(os.path.split(__file__)[0], 'files/dbc/choices.dbc')
    REO_TIMESTAMP = re.compile('\(([^)]+)\)')
    FORMAT_ABSOLUTE_TIMESTAMP = "%Y-%m-%d %H:%M:%S.%f"

    def setUp(self):
        plt.reset_mock()


    # ------- test different timestamp formats -------

    def test_plot_tA(self):
        argv = ['cantools', 'plot', self.DBC_FILE]
        input_data = """\
 (2020-12-27 11:59:14.820230)  vcan0  00000343   [8]  B0 04 B0 04 B0 04 D4 04
 (2020-12-27 11:59:15.821995)  vcan0  00000343   [8]  97 04 BB 04 9E 04 90 04
 (2020-12-27 11:59:16.823895)  vcan0  00000343   [8]  75 04 84 04 75 04 6E 04
 (2020-12-27 11:59:17.825764)  vcan0  00000343   [8]  65 04 57 04 65 04 49 04
 (2020-12-27 11:59:18.827593)  vcan0  00000343   [8]  4B 04 3C 04 2E 04 3C 04
 (2020-12-27 11:59:19.829363)  vcan0  00000343   [8]  20 04 20 04 19 04 FC 03
 (2020-12-27 11:59:20.831170)  vcan0  00000343   [8]  E4 03 EB 03 00 04 DC 03
 (2020-12-27 11:59:21.832984)  vcan0  00000343   [8]  D2 03 CB 03 E0 03 CB 03
 (2020-12-27 11:59:22.834807)  vcan0  00000343   [8]  D2 03 E0 03 B5 03 C4 03
 (2020-12-27 11:59:23.836636)  vcan0  00000343   [8]  A7 03 BC 03 AE 03 BC 03
"""

        xs = self.parse_time(input_data, self.parse_absolute_time)
        ys_whlspeed_fl = [18.75, 18.359375, 17.828125, 17.578125, 17.171875, 16.5, 15.5625, 15.28125, 15.28125, 14.609375]
        ys_whlspeed_fr = [18.75, 18.921875, 18.0625, 17.359375, 16.9375, 16.5, 15.671875, 15.171875, 15.5, 14.9375]
        ys_whlspeed_rl = [18.75, 18.46875, 17.828125, 17.578125, 16.71875, 16.390625, 16.0, 15.5, 14.828125, 14.71875]
        ys_whlspeed_rr = [19.3125, 18.25, 17.71875, 17.140625, 16.9375, 15.9375, 15.4375, 15.171875, 15.0625, 14.9375]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.subplot().plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.subplot().plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel('time'),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()
                self.assertListEqual(plt.mock_calls, expected_calls)


    def test_plot_ta(self):
        argv = ['cantools', 'plot', self.DBC_FILE]
        input_data = """\
(1608822980.872678) vcan0 00000343#15050E051C051505
(1608822980.873316) vcan0 00000343#870595059505A405
(1608822980.873745) vcan0 00000343#6B058E056B059505
(1608822980.874138) vcan0 00000343#32054E054E055505
(1608822980.874524) vcan0 00000343#190519052E052E05
(1608822980.874921) vcan0 00000343#8704800480048704
(1608822980.875305) vcan0 00000343#9C039C039503B203
(1608822980.875685) vcan0 00000343#A702BC02A002A002
(1608822980.876130) vcan0 00000343#F200EB00EB00F200
"""

        xs = [x+1608822980 for x in (.872678, .873316, .873745, .874138, .874524, .874921, .875305, .875685, .876130)]
        ys_fl = [20.328125, 22.109375, 21.671875, 20.78125,  20.390625, 18.109375, 14.4375,   10.609375, 3.78125 ]
        ys_fr = [20.21875,  22.328125, 22.21875,  21.21875,  20.390625, 18.0,      14.4375,   10.9375,   3.671875]
        ys_rl = [20.4375,   22.328125, 21.671875, 21.21875,  20.71875,  18.0,      14.328125, 10.5,      3.671875]
        ys_rr = [20.328125, 22.5625,   22.328125, 21.328125, 20.71875,  18.109375, 14.78125,  10.5,      3.78125 ]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().plot(xs, ys_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.subplot().plot(xs, ys_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.subplot().plot(xs, ys_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel('time / s'),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()
                self.assertListEqual(plt.mock_calls, expected_calls)


    def test_plot_tz(self):
        argv = ['cantools', 'plot', self.DBC_FILE]
        input_data = """\
 (000.000000)  vcan0  00000343   [8]  C5 04 B7 04 9B 04 C5 04
 (001.001787)  vcan0  00000343   [8]  69 04 69 04 77 04 7E 04
 (002.003592)  vcan0  00000343   [8]  29 04 30 04 29 04 22 04
 (003.005400)  vcan0  00000343   [8]  FC 03 20 04 20 04 FC 03
 (004.006942)  vcan0  00000343   [8]  DE 03 D0 03 D0 03 C9 03
 (005.008400)  vcan0  00000343   [8]  7E 03 85 03 8C 03 77 03
 (006.009926)  vcan0  00000343   [8]  65 03 3B 03 50 03 65 03
 (007.011457)  vcan0  00000343   [8]  17 03 3B 03 34 03 10 03
 (008.013215)  vcan0  00000343   [8]  00 03 F2 02 15 03 F9 02
 (009.014779)  vcan0  00000343   [8]  CB 02 BC 02 B5 02 D2 02
"""

        xs = self.parse_time(input_data, self.parse_seconds)
        ys_whlspeed_fl = [19.078125, 17.640625, 16.640625, 15.9375, 15.46875, 13.96875, 13.578125, 12.359375, 12.0, 11.171875]
        ys_whlspeed_fr = [18.859375, 17.640625, 16.75, 16.5, 15.25, 14.078125, 12.921875, 12.921875, 11.78125, 10.9375]
        ys_whlspeed_rl = [18.421875, 17.859375, 16.640625, 16.5, 15.25, 14.1875, 13.25, 12.8125, 12.328125, 10.828125]
        ys_whlspeed_rr = [19.078125, 17.96875, 16.53125, 15.9375, 15.140625, 13.859375, 13.578125, 12.25, 11.890625, 11.28125]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.subplot().plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.subplot().plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel('time / s'),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()
                self.assertListEqual(plt.mock_calls, expected_calls)


    def test_plot_td(self):
        argv = ['cantools', 'plot', '--line-numbers', self.DBC_FILE]
        input_data = """\
 (000.000000)  vcan0  00000343   [8]  C2 04 C9 04 D0 04 C2 04
 (001.001586)  vcan0  00000343   [8]  8C 04 8C 04 94 04 9B 04
 (001.001788)  vcan0  00000343   [8]  29 04 05 04 30 04 14 04
 (001.001798)  vcan0  00000343   [8]  82 03 97 03 89 03 9E 03
 (001.001807)  vcan0  00000343   [8]  15 03 0E 03 2B 03 2B 03
 (001.001802)  vcan0  00000343   [8]  B5 02 AE 02 A0 02 92 02
 (001.001795)  vcan0  00000343   [8]  04 02 F5 01 12 02 F5 01
 (001.001800)  vcan0  00000343   [8]  64 01 6B 01 6B 01 47 01
 (001.001847)  vcan0  00000343   [8]  90 00 97 00 97 00 89 00
"""

        xs = self.parse_time(input_data, self.parse_seconds)
        xs = list(range(1, len(xs)+1))
        ys_whlspeed_fl = [19.03125, 18.1875, 16.640625, 14.03125, 12.328125, 10.828125, 8.0625, 5.5625, 2.25]
        ys_whlspeed_fr = [19.140625, 18.1875, 16.078125, 14.359375, 12.21875, 10.71875, 7.828125, 5.671875, 2.359375]
        ys_whlspeed_rl = [19.25, 18.3125, 16.75, 14.140625, 12.671875, 10.5, 8.28125, 5.671875, 2.359375]
        ys_whlspeed_rr = [19.03125, 18.421875, 16.3125, 14.46875, 12.671875, 10.28125, 7.828125, 5.109375, 2.140625]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.subplot().plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.subplot().plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel('line number'),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()
                self.assertListEqual(plt.mock_calls, expected_calls)


    # ------- test signal command line argument(s) -------
    # format
    # --case-sensitive

    def test_wildcards_caseinsensitive(self):
        argv = ['cantools', 'plot', self.DBC_FILE, '*fl*']
        input_data = """\
 (2020-12-28 09:52:12.179240)  vcan0  00000343   [8]  B5 04 AE 04 A7 04 8B 04
 (2020-12-28 09:52:12.179530)  vcan0  0000024A   [8]  F2 04 F9 04 F9 04 F2 04
 (2020-12-28 09:52:13.181317)  vcan0  00000343   [8]  77 04 70 04 62 04 77 04
 (2020-12-28 09:52:13.181980)  vcan0  0000024A   [8]  49 05 2C 05 2C 05 2C 05
 (2020-12-28 09:52:14.183770)  vcan0  00000343   [8]  2B 04 39 04 2B 04 2B 04
 (2020-12-28 09:52:14.184460)  vcan0  0000024A   [8]  79 05 5C 05 80 05 64 05
 (2020-12-28 09:52:15.185272)  vcan0  00000343   [8]  B7 03 D4 03 CC 03 BE 03
 (2020-12-28 09:52:15.185895)  vcan0  0000024A   [8]  7B 05 82 05 97 05 7B 05
 (2020-12-28 09:52:16.187696)  vcan0  00000343   [8]  82 03 89 03 65 03 74 03
 (2020-12-28 09:52:16.188405)  vcan0  0000024A   [8]  9B 05 9B 05 77 05 9B 05
"""

        xs2  = self.parse_time(input_data, self.parse_absolute_time, 2, 1)
        xs33 = self.parse_time(input_data, self.parse_absolute_time, 2, 0)
        whlspeed_fl_bremse2 = [19.78125, 21.140625, 21.890625, 21.921875, 22.421875]
        whlspeed_fl = [18.828125, 17.859375, 16.671875, 14.859375, 14.03125]

        subplots = [mock.Mock()]
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(xs2, whlspeed_fl_bremse2, '', label='BREMSE_2.whlspeed_FL_Bremse2'),
                mock.call.plot(xs33, whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.legend(),
                mock.call.set_xlabel('time'),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()
                self.assertListEqual(plt.mock_calls, expected_calls)
                for i in range(len(expected_subplot_calls)):
                    self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i], msg="calls don't match for subplot %s" % i)

    def test_subplots(self):
        argv = ['cantools', 'plot', self.DBC_FILE, 'BREMSE_33.*', '-', 'BREMSE_2.*']
        input_data = """\
 (2020-12-28 09:52:12.179240)  vcan0  00000343   [8]  B5 04 AE 04 A7 04 8B 04
 (2020-12-28 09:52:12.179530)  vcan0  0000024A   [8]  F2 04 F9 04 F9 04 F2 04
 (2020-12-28 09:52:13.181317)  vcan0  00000343   [8]  77 04 70 04 62 04 77 04
 (2020-12-28 09:52:13.181980)  vcan0  0000024A   [8]  49 05 2C 05 2C 05 2C 05
 (2020-12-28 09:52:14.183770)  vcan0  00000343   [8]  2B 04 39 04 2B 04 2B 04
 (2020-12-28 09:52:14.184460)  vcan0  0000024A   [8]  79 05 5C 05 80 05 64 05
 (2020-12-28 09:52:15.185272)  vcan0  00000343   [8]  B7 03 D4 03 CC 03 BE 03
 (2020-12-28 09:52:15.185895)  vcan0  0000024A   [8]  7B 05 82 05 97 05 7B 05
 (2020-12-28 09:52:16.187696)  vcan0  00000343   [8]  82 03 89 03 65 03 74 03
 (2020-12-28 09:52:16.188405)  vcan0  0000024A   [8]  9B 05 9B 05 77 05 9B 05
"""

        xs2  = self.parse_time(input_data, self.parse_absolute_time, 2, 1)
        xs33 = self.parse_time(input_data, self.parse_absolute_time, 2, 0)
        whlspeed_fl_bremse2 = [19.78125, 21.140625, 21.890625, 21.921875, 22.421875]
        whlspeed_fr_bremse2 = [19.890625, 20.6875, 21.4375, 22.03125, 22.421875]
        whlspeed_rl_bremse2 = [19.890625, 20.6875, 22.0, 22.359375, 21.859375]
        whlspeed_rr_bremse2 = [19.78125, 20.6875, 21.5625, 21.921875, 22.421875]
        whlspeed_fl = [18.828125, 17.859375, 16.671875, 14.859375, 14.03125]
        whlspeed_fr = [18.71875, 17.75, 16.890625, 15.3125, 14.140625]
        whlspeed_rl = [18.609375, 17.53125, 16.671875, 15.1875, 13.578125]
        whlspeed_rr = [18.171875, 17.859375, 16.671875, 14.96875, 13.8125]

        subplots = [mock.Mock(), mock.Mock()]
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(2,1,1, sharex=None),
            mock.call.subplot(2,1,2, sharex=subplots[0].axes),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(xs33, whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(xs33, whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(xs33, whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(xs33, whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel('time'),
            ],
            [
                mock.call.plot(xs2, whlspeed_fl_bremse2, '', label='BREMSE_2.whlspeed_FL_Bremse2'),
                mock.call.plot(xs2, whlspeed_fr_bremse2, '', label='BREMSE_2.whlspeed_FR_Bremse2'),
                mock.call.plot(xs2, whlspeed_rl_bremse2, '', label='BREMSE_2.whlspeed_RL_Bremse2'),
                mock.call.plot(xs2, whlspeed_rr_bremse2, '', label='BREMSE_2.whlspeed_RR_Bremse2'),
                mock.call.legend(),
                mock.call.set_xlabel('time'),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()
                self.assertListEqual(plt.mock_calls, expected_calls)
                for i in range(len(expected_subplot_calls)):
                    self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i], msg="calls don't match for subplot %s" % i)

    def test_choices_stem(self):
        argv = ['cantools', 'plot', self.DBC_FILE_CHOICES, 'Foo:|']
        input_data = """\
 (2020-12-29 08:48:04.568726)  vcan0  00000000   [8]  01 00 00 00 00 00 00 00
 (2020-12-29 08:48:08.733416)  vcan0  00000000   [8]  02 00 00 00 00 00 00 00
 (2020-12-29 08:48:12.317636)  vcan0  00000000   [8]  FB 00 00 00 00 00 00 00
 (2020-12-29 08:48:14.590522)  vcan0  00000000   [8]  05 00 00 00 00 00 00 00
 (2020-12-29 08:48:18.555082)  vcan0  00000000   [8]  00 00 00 00 00 00 00 00
 (2020-12-29 08:48:21.305794)  vcan0  00000000   [8]  02 00 00 00 00 00 00 00
 (2020-12-29 08:48:22.807889)  vcan0  00000000   [8]  05 00 00 00 00 00 00 00
 (2020-12-29 08:48:25.879604)  vcan0  00000000   [8]  00 00 00 00 00 00 00 00
 (2020-12-29 08:48:30.484820)  vcan0  00000000   [8]  02 00 00 00 00 00 00 00
 (2020-12-29 08:48:34.369165)  vcan0  00000000   [8]  06 00 00 00 00 00 00 00
"""

        db = cantools.db.load_file(self.DBC_FILE_CHOICES)
        choices = db.get_message_by_name("Foo").get_signal_by_name("Foo").choices

        xs  = self.parse_time(input_data, self.parse_absolute_time)
        ys = [1, 2, -5, 5, 0, 2, 5, 0, 2, 6]
        ys = [choices[y] for y in ys]

        subplots = [mock.Mock()]
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.stem(xs, ys, '', label='Foo.Foo'),
                mock.call.legend(),
                mock.call.set_xlabel('time'),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()
                self.assertListEqual(plt.mock_calls, expected_calls)
                for i in range(len(expected_subplot_calls)):
                    self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i], msg="calls don't match for subplot %s" % i)


    # ------- test error handling --ignore+/--show* -------
    # -q, -s

    # ------- test other command line options -------
    # --no-decode-choices
    # --break-time
    # --output-file

    # ------- auxiliary functions -------

    def parse_time(self, log, parse, mod=1, offset=0):
        out = []
        ln_num = 0
        for ln in log.splitlines():
            if not ln:
                continue
            if ln_num % mod == offset:
                m = self.REO_TIMESTAMP.search(ln)
                timestamp = m.group(1)
                out.append(parse(timestamp))
            ln_num += 1
        return out

    def parse_absolute_time(self, timestamp):
        return datetime.datetime.strptime(timestamp, self.FORMAT_ABSOLUTE_TIMESTAMP)

    def parse_seconds(self, timestamp):
        return float(timestamp)


if __name__ == '__main__':
    unittest.main()
