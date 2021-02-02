#!/usr/bin/env python3

import os
import sys
import datetime
import re
import unittest
from unittest import mock
from io import StringIO
import cantools
import matplotlib.pyplot


class PyplotMock:

    _attrs = ('subplot', 'show', 'savefig')

    def __init__(self):
        self._mock = mock.Mock()
        for a in self._attrs:
            m = mock.Mock()
            self._mock.attach_mock(m, a)
            setattr(self, a, m)

    @property
    def mock_calls(self):
        return self._mock.mock_calls

    def __enter__(self):
        self._patches = [mock.patch('matplotlib.pyplot.%s' % a, getattr(self,a)) for a in self._attrs]
        for p in self._patches:
            p.__enter__()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        for p in self._patches:
            p.__exit__(exc_type, exc_val, exc_tb)


class SubplotMock(mock.Mock):

    axes = mock.Mock(name="axes-mock")


class CanToolsPlotTest(unittest.TestCase):

    DBC_FILE = os.path.join(os.path.split(__file__)[0], 'files/dbc/abs.dbc')
    DBC_FILE_CHOICES = os.path.join(os.path.split(__file__)[0], 'files/dbc/choices.dbc')
    REO_TIMESTAMP = re.compile('\(([^)]+)\)')
    FORMAT_ABSOLUTE_TIMESTAMP = "%Y-%m-%d %H:%M:%S.%f"

    COLOR_INVALID_SYNTAX = '#ff0000'
    COLOR_UNKNOWN_FRAMES = '#ffab00'
    COLOR_INVALID_DATA   = '#ff00ff'
    ERROR_LINEWIDTH = 1

    XLABEL_tA = "absolute time (start: %s)"
    XLABEL_ta = XLABEL_tA
    XLABEL_tz = "relative time"
    XLABEL_l  = XLABEL_ta
    XLABEL_LINE_NUMBER = "line number"

    # ------- test different timestamp formats -------

    def test_plot_tA(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                ]
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
        ys_whlspeed_fl = [18.75, 18.359375, 17.828125, 17.578125,
                          17.171875, 16.5, 15.5625, 15.28125, 15.28125, 14.609375]
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
            mock.call.subplot().set_xlabel(self.XLABEL_tA % "27.12.2020"),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with PyplotMock() as plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)

    def test_plot_ta(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                ]
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
        xs = [datetime.datetime.fromtimestamp(x) for x in xs]
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
            mock.call.subplot().set_xlabel(self.XLABEL_ta % "24.12.2020"),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with PyplotMock() as plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)

    def test_plot_tz(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                ]
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
        ys_whlspeed_fl = [19.078125, 17.640625, 16.640625, 15.9375,
                          15.46875, 13.96875, 13.578125, 12.359375, 12.0, 11.171875]
        ys_whlspeed_fr = [18.859375, 17.640625, 16.75, 16.5, 15.25, 14.078125, 12.921875, 12.921875, 11.78125, 10.9375]
        ys_whlspeed_rl = [18.421875, 17.859375, 16.640625, 16.5, 15.25, 14.1875, 13.25, 12.8125, 12.328125, 10.828125]
        ys_whlspeed_rr = [19.078125, 17.96875, 16.53125, 15.9375,
                          15.140625, 13.859375, 13.578125, 12.25, 11.890625, 11.28125]

        plt = PyplotMock()
        subplots = [SubplotMock()]
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [[
            mock.call.plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.legend(),
            mock.call.set_xlabel(self.XLABEL_tz),
        ]]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    for i in range(len(expected_subplot_calls)):
                        self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                             msg="calls don't match for subplot %s" % i)

    def test_plot_td(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                '--line-numbers',
                ]
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
            mock.call.subplot().set_xlabel(self.XLABEL_LINE_NUMBER),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with PyplotMock() as plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)

    def test_plot_l(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                ]
        input_data = """\
(1609395080.446193) vcan0 00000343#B504CB04AE04BC04
(1609395081.447989) vcan0 00000343#650457045E047404
(1609395082.449807) vcan0 00000343#1504240415043204
(1609395083.451020) vcan0 00000343#DB03DB03DB03C503
(1609395084.452815) vcan0 00000343#6903850369037703
"""

        xs = self.parse_time(input_data, self.parse_absolute_seconds)
        ys_whlspeed_fl = [18.828125, 17.578125, 16.328125, 15.421875, 13.640625]
        ys_whlspeed_fr = [19.171875, 17.359375, 16.5625, 15.421875, 14.078125]
        ys_whlspeed_rl = [18.71875, 17.46875, 16.328125, 15.421875, 13.640625]
        ys_whlspeed_rr = [18.9375, 17.8125, 16.78125, 15.078125, 13.859375]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.subplot().plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.subplot().plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel(self.XLABEL_l % "31.12.2020"),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with PyplotMock() as plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)

    def test_plot_no_timestamps(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                ]
        input_data = """\
  vcan0  00000343   [8]  7C 05 7C 05 84 05 67 05
  vcan0  00000343   [8]  19 06 20 06 12 06 20 06
  vcan0  00000343   [8]  77 06 94 06 8C 06 77 06
  vcan0  00000343   [8]  25 07 10 07 02 07 1E 07
  vcan0  00000343   [8]  E2 07 F0 07 CC 07 E9 07
"""

        xs = list(range(1, 5+1))
        ys_whlspeed_fl = [21.9375, 24.390625, 25.859375, 28.578125, 31.53125]
        ys_whlspeed_fr = [21.9375, 24.5, 26.3125, 28.25, 31.75]
        ys_whlspeed_rl = [22.0625, 24.28125, 26.1875, 28.03125, 31.1875]
        ys_whlspeed_rr = [21.609375, 24.5, 25.859375, 28.46875, 31.640625]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.subplot().plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.subplot().plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel(self.XLABEL_LINE_NUMBER),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with PyplotMock() as plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)

    def test_plot_cantools_decode(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                ]
        input_data = """\
 (2021-01-02 18:45:26.313349)  vcan0  00000343   [8]  DA 04 DA 04 E8 04 C4 04 ::
BREMSE_33(
    whlspeed_FL: 19.40625 m/s,
    whlspeed_FR: 19.40625 m/s,
    whlspeed_RL: 19.625 m/s,
    whlspeed_RR: 19.0625 m/s
)
 (2021-01-02 18:45:27.314717)  vcan0  00000343   [8]  22 05 14 05 F8 04 0D 05 ::
BREMSE_33(
    whlspeed_FL: 20.53125 m/s,
    whlspeed_FR: 20.3125 m/s,
    whlspeed_RL: 19.875 m/s,
    whlspeed_RR: 20.203125 m/s
)
 (2021-01-02 18:45:28.316559)  vcan0  00000343   [8]  61 05 53 05 68 05 4C 05 ::
BREMSE_33(
    whlspeed_FL: 21.515625 m/s,
    whlspeed_FR: 21.296875 m/s,
    whlspeed_RL: 21.625 m/s,
    whlspeed_RR: 21.1875 m/s
)
"""

        xs = ('2021-01-02 18:45:26.313349', '2021-01-02 18:45:27.314717', '2021-01-02 18:45:28.316559')
        xs = [self.parse_absolute_time(t) for t in xs]
        ys_whlspeed_fl = [19.40625, 20.53125, 21.515625]
        ys_whlspeed_fr = [19.40625, 20.3125, 21.296875]
        ys_whlspeed_rl = [19.625, 19.875, 21.625]
        ys_whlspeed_rr = [19.0625, 20.203125, 21.1875]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
            mock.call.subplot().plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
            mock.call.subplot().plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel(self.XLABEL_tA % "02.01.2021"),
            mock.call.show(),
        ]

        stdout = StringIO()
        expected_output = ""

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with PyplotMock() as plt:
                        cantools._main()
                        self.assertEqual(stdout.getvalue(), expected_output)
                        self.assertListEqual(plt.mock_calls, expected_calls)

    # ------- test signal command line argument(s) -------

    def test_wildcards_caseinsensitive(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                '*fl*',
                ]
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

        subplots = [SubplotMock()]
        plt = PyplotMock()
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
                mock.call.set_xlabel(self.XLABEL_tA % "28.12.2020"),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    for i in range(len(expected_subplot_calls)):
                        self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                             msg="calls don't match for subplot %s" % i)

    def test_subplots(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                'BREMSE_33.*',
                '-',
                'BREMSE_2.*', ]
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

        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
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
                mock.call.set_xlabel(self.XLABEL_tA % "28.12.2020"),
            ],
            [
                mock.call.plot(xs2, whlspeed_fl_bremse2, '', label='BREMSE_2.whlspeed_FL_Bremse2'),
                mock.call.plot(xs2, whlspeed_fr_bremse2, '', label='BREMSE_2.whlspeed_FR_Bremse2'),
                mock.call.plot(xs2, whlspeed_rl_bremse2, '', label='BREMSE_2.whlspeed_RL_Bremse2'),
                mock.call.plot(xs2, whlspeed_rr_bremse2, '', label='BREMSE_2.whlspeed_RR_Bremse2'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "28.12.2020"),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    for i in range(len(expected_subplot_calls)):
                        self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                             msg="calls don't match for subplot %s" % i)

    def test_format(self):
        col_33 = "b"
        col_2  = "r"
        line_fl = "-<"
        line_fr = "-->"
        line_rl = "-.<"
        line_rr = ":>"
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
            '*33.*FL:'+col_33+line_fl, '*33.*FR:'+col_33+line_fr,'*33.*RL:'+col_33+line_rl,'*33.*RR:'+col_33+line_rr,
            '*2.*FL*:'+col_2+line_fl, '*2.*FR*:'+col_2+line_fr,'*2.*RL*:'+col_2+line_rl,'*2.*RR*:'+col_2+line_rr,
        ]
        input_data = """\
 (000.000000)  vcan0  00000343   [8]  64 04 79 04 6B 04 5C 04
 (000.000304)  vcan0  0000024A   [8]  10 05 E5 04 02 05 10 05
 (001.002101)  vcan0  00000343   [8]  E9 03 DB 03 F7 03 E9 03
 (001.002815)  vcan0  0000024A   [8]  29 05 3E 05 30 05 1B 05
 (002.004611)  vcan0  00000343   [8]  B0 03 A2 03 8C 03 94 03
 (002.005398)  vcan0  0000024A   [8]  AB 05 8E 05 A4 05 95 05
 (003.007227)  vcan0  00000343   [8]  20 03 27 03 35 03 0B 03
 (003.007945)  vcan0  0000024A   [8]  F0 05 F0 05 14 06 F7 05
 (004.009759)  vcan0  00000343   [8]  9B 02 A2 02 94 02 94 02
 (004.010451)  vcan0  0000024A   [8]  50 06 50 06 50 06 50 06
"""

        xs2  = self.parse_time(input_data, self.parse_seconds, 2, 1)
        xs33 = self.parse_time(input_data, self.parse_seconds, 2, 0)
        whlspeed_fl_bremse2 = [20.25, 20.640625, 22.671875, 23.75, 25.25]
        whlspeed_fr_bremse2 = [19.578125, 20.96875, 22.21875, 23.75, 25.25]
        whlspeed_rl_bremse2 = [20.03125, 20.75, 22.5625, 24.3125, 25.25]
        whlspeed_rr_bremse2 = [20.25, 20.421875, 22.328125, 23.859375, 25.25]
        whlspeed_fl = [17.5625, 15.640625, 14.75, 12.5, 10.421875]
        whlspeed_fr = [17.890625, 15.421875, 14.53125, 12.609375, 10.53125]
        whlspeed_rl = [17.671875, 15.859375, 14.1875, 12.828125, 10.3125]
        whlspeed_rr = [17.4375, 15.640625, 14.3125, 12.171875, 10.3125]

        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(xs33, whlspeed_fl, col_33+line_fl, label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(xs33, whlspeed_fr, col_33+line_fr, label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(xs33, whlspeed_rl, col_33+line_rl, label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(xs33, whlspeed_rr, col_33+line_rr, label='BREMSE_33.whlspeed_RR'),
                mock.call.plot(xs2, whlspeed_fl_bremse2, col_2+line_fl, label='BREMSE_2.whlspeed_FL_Bremse2'),
                mock.call.plot(xs2, whlspeed_fr_bremse2, col_2+line_fr, label='BREMSE_2.whlspeed_FR_Bremse2'),
                mock.call.plot(xs2, whlspeed_rl_bremse2, col_2+line_rl, label='BREMSE_2.whlspeed_RL_Bremse2'),
                mock.call.plot(xs2, whlspeed_rr_bremse2, col_2+line_rr, label='BREMSE_2.whlspeed_RR_Bremse2'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tz),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    for i in range(len(expected_subplot_calls)):
                        self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                             msg="calls don't match for subplot %s" % i)

    def test_choices_stem(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE_CHOICES,
                'plot',
                'Foo:|',
                ]
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

        subplots = [SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.stem(xs, ys, '', label='Foo.Foo'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    for i in range(len(expected_subplot_calls)):
                        self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                             msg="calls don't match for subplot %s" % i)

    def test_no_decode_choices(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE_CHOICES,
                'plot',
                '--no-decode-choices',
                'Foo:|',
                ]
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

        subplots = [SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.stem(xs, ys, '', label='Foo.Foo'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    for i in range(len(expected_subplot_calls)):
                        self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                             msg="calls don't match for subplot %s" % i)

    def test_case_sensitive(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--case-sensitive', '*fl*']
        input_data = """\
 (2020-12-29 11:18:10.989297)  vcan0  00000343   [8]  C4 04 A7 04 BC 04 A0 04
 (2020-12-29 11:18:11.991153)  vcan0  00000343   [8]  95 04 95 04 87 04 72 04
 (2020-12-29 11:18:12.993029)  vcan0  00000343   [8]  52 04 6E 04 75 04 52 04
"""

        expected_output = r"WARNING: signal '.*\\..*fl.*$' with format '' was not plotted." + "\n"

        subplots = [SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertEqual(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    # ------- test error handling -------

    class data_error_handling:
        input_data = '''\
 (2020-12-29 11:33:24.285921)  vcan0  00000343   [8]  69 05 5B 05 5B 05 69 05
invalid syntax
 (2020-12-29 11:33:24.286209)  vcan0  1000024A   [8]  1B 05 22 05 1B 05 22 05
 (2020-12-29 11:33:25.288070)  vcan0  00000343   [8]  EE 05 D9 05 D9 05 F5 05
 (2020-12-29 11:33:25.288811)  vcan0  1000024A   [8]  25 05 34 05 2C 05 49 05
 (2020-12-29 11:33:26.290617)  vcan0  00000343   [8]  8B 06 AE 06 B5 06 8B 06
 (2020-12-29 11:33:26.291364)  vcan0  0000024A   [8]  32 05 47 05 55 05 40
 (2020-12-29 11:33:27.292325)  vcan0  00000343   [8]  37 07 45 07 37 07 3E 07
invalid syntax
 (2020-12-29 11:33:27.293045)  vcan0  0000024A   [8]  85 05 7E 05 7E 05 94 05
 (2020-12-29 11:33:28.294690)  vcan0  00000343   [8]  E0 07 04 08 FC 07 E0 07
 (2020-12-29 11:33:28.295229)  vcan0  0000024A   [8]  CC 05 CC 05 E9 05 DB 05
'''
        xs33 = [
            datetime.datetime(2020, 12, 29, 11, 33, 24, 285921),
            datetime.datetime(2020, 12, 29, 11, 33, 25, 288070),
            datetime.datetime(2020, 12, 29, 11, 33, 26, 290617),
            datetime.datetime(2020, 12, 29, 11, 33, 27, 292325),
            datetime.datetime(2020, 12, 29, 11, 33, 28, 294690),
        ]
        xs33_ln = [1,4,6,8,11]
        whlspeed_fl_bremse2 = [20.421875, 20.578125, 20.78125, 22.078125, 23.1875]
        whlspeed_fr_bremse2 = [20.53125, 20.8125, 21.109375, 21.96875, 23.1875]
        whlspeed_rl_bremse2 = [20.421875, 20.6875, 21.328125, 21.96875, 23.640625]
        whlspeed_rr_bremse2 = [20.53125, 21.140625, 21.0, 22.3125, 23.421875]
        whlspeed_fl = [21.640625, 23.71875, 26.171875, 28.859375, 31.5]
        whlspeed_fr = [21.421875, 23.390625, 26.71875, 29.078125, 32.0625]
        whlspeed_rl = [21.421875, 23.390625, 26.828125, 28.859375, 31.9375]
        whlspeed_rr = [21.640625, 23.828125, 26.171875, 28.96875, 31.5]

    def test_error_messages(self):
        argv = ['cantools',
                '--database',
                self.DBC_FILE,
                'plot',
                '*33.*',
                ]
        expected_output = '''\
Failed to parse line: 'invalid syntax'
Unknown frame id 268436042 (0x1000024a)
Unknown frame id 268436042 (0x1000024a)
Failed to parse data of frame id 586 (0x24a): ...
Failed to parse line: 'invalid syntax'
'''

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    # --ignore-*

    def test_ignore_invalid_syntax(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--ignore-invalid-syntax', '*33.*']
        expected_output = '''\
Unknown frame id 268436042 (0x1000024a)
Unknown frame id 268436042 (0x1000024a)
Failed to parse data of frame id 586 (0x24a): ...
'''

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    def test_ignore_unknown_frameid(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--ignore-unknown-frames', '*33.*']
        expected_output = '''\
Failed to parse line: 'invalid syntax'
Failed to parse data of frame id 586 (0x24a): ...
Failed to parse line: 'invalid syntax'
'''

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    def test_ignore_invalid_data(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--ignore-invalid-data',  '*33.*']
        expected_output = '''\
Failed to parse line: 'invalid syntax'
Unknown frame id 268436042 (0x1000024a)
Unknown frame id 268436042 (0x1000024a)
Failed to parse line: 'invalid syntax'
'''

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    def test_quiet(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '-q', '*33.*']
        expected_output = '''\
'''

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertEqual(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    # --show-*

    def test_show_invalid_data(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--show-invalid-data', '*33.*']
        expected_output = '''\
Failed to parse line: 'invalid syntax'
Unknown frame id 268436042 (0x1000024a)
Unknown frame id 268436042 (0x1000024a)
Failed to parse data of frame id 586 (0x24a): ...
Failed to parse line: 'invalid syntax'
'''

        x0 = self.parse_absolute_time('2020-12-29 11:33:26.291364')

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.axvline(x0, color=self.COLOR_INVALID_DATA,
                                  linewidth=self.ERROR_LINEWIDTH, label='invalid data (1)'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    def test_show_unknown_frames(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--show-unknown-frames', '*33.*']
        expected_output = '''\
Failed to parse line: 'invalid syntax'
Unknown frame id 268436042 (0x1000024a)
Unknown frame id 268436042 (0x1000024a)
Failed to parse data of frame id 586 (0x24a): ...
Failed to parse line: 'invalid syntax'
'''

        x0 = self.parse_absolute_time('2020-12-29 11:33:24.286209')
        x1 = self.parse_absolute_time('2020-12-29 11:33:25.288811')

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.axvline(x0, color=self.COLOR_UNKNOWN_FRAMES,
                                  linewidth=self.ERROR_LINEWIDTH, label='unknown frames (2)'),
                mock.call.axvline(x1, color=self.COLOR_UNKNOWN_FRAMES, linewidth=self.ERROR_LINEWIDTH),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tA % "29.12.2020"),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    def test_show_invalid_syntax(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--show-invalid-syntax', '*33.*']
        expected_output = '''\
Failed to parse line: 'invalid syntax'
Unknown frame id 268436042 (0x1000024a)
Unknown frame id 268436042 (0x1000024a)
Failed to parse data of frame id 586 (0x24a): ...
Failed to parse line: 'invalid syntax'
'''

        x0 = 2
        x1 = 9

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33_ln, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33_ln, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33_ln, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33_ln, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.axvline(x0, color=self.COLOR_INVALID_SYNTAX,
                                  linewidth=self.ERROR_LINEWIDTH, label='invalid syntax (2)'),
                mock.call.axvline(x1, color=self.COLOR_INVALID_SYNTAX, linewidth=self.ERROR_LINEWIDTH),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_LINE_NUMBER),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    def test_show_all_errors(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '-s', '*33.*']
        expected_output = '''\
Failed to parse line: 'invalid syntax'
Unknown frame id 268436042 (0x1000024a)
Unknown frame id 268436042 (0x1000024a)
Failed to parse data of frame id 586 (0x24a): ...
Failed to parse line: 'invalid syntax'
'''

        xis0 = 2
        xis1 = 9
        xuf0 = 3
        xuf1 = 5
        xid0 = 7

        data = self.data_error_handling
        subplots = [SubplotMock(), SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(data.xs33_ln, data.whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(data.xs33_ln, data.whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(data.xs33_ln, data.whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(data.xs33_ln, data.whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.axvline(xis0, color=self.COLOR_INVALID_SYNTAX,
                                  linewidth=self.ERROR_LINEWIDTH, label='invalid syntax (2)'),
                mock.call.axvline(xis1, color=self.COLOR_INVALID_SYNTAX, linewidth=self.ERROR_LINEWIDTH),
                mock.call.axvline(xuf0, color=self.COLOR_UNKNOWN_FRAMES,
                                  linewidth=self.ERROR_LINEWIDTH, label='unknown frames (2)'),
                mock.call.axvline(xuf1, color=self.COLOR_UNKNOWN_FRAMES, linewidth=self.ERROR_LINEWIDTH),
                mock.call.axvline(xid0, color=self.COLOR_INVALID_DATA,
                                  linewidth=self.ERROR_LINEWIDTH, label='invalid data (1)'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_LINE_NUMBER),
            ],
        ]

        stdout = StringIO()

        with mock.patch('sys.stdin', StringIO(data.input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()

                        actual_output = stdout.getvalue()
                        self.assertLinesMatch(actual_output, expected_output)

                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    # ------- test other command line options -------

    def test_break_time(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--break-time', '5']
        input_data = """\
 (000.000000)  vcan0  00000343   [8]  C7 04 CE 04 EB 04 EB 04
 (001.001788)  vcan0  00000343   [8]  75 04 75 04 99 04 99 04
 (002.003649)  vcan0  00000343   [8]  2C 04 34 04 10 04 25 04
 (003.005497)  vcan0  00000343   [8]  AC 03 BB 03 AC 03 BB 03
 (004.007213)  vcan0  00000343   [8]  70 03 45 03 45 03 5B 03
 (015.495473)  vcan0  00000343   [8]  AC 04 BB 04 C2 04 D0 04
 (016.497347)  vcan0  00000343   [8]  75 04 6E 04 6E 04 6E 04
 (017.499210)  vcan0  00000343   [8]  19 04 35 04 2E 04 35 04
 (018.501066)  vcan0  00000343   [8]  AB 03 C0 03 C7 03 AB 03
 (019.502899)  vcan0  00000343   [8]  50 03 5E 03 42 03 49 03
"""

        xs = self.parse_time(input_data, self.parse_seconds)

        ys_whlspeed_fl_0 = [19.109375, 17.828125, 16.6875, 14.6875, 13.75]
        ys_whlspeed_fr_0 = [19.21875, 17.828125, 16.8125, 14.921875, 13.078125]
        ys_whlspeed_rl_0 = [19.671875, 18.390625, 16.25, 14.6875, 13.078125]
        ys_whlspeed_rr_0 = [19.671875, 18.390625, 16.578125, 14.921875, 13.421875]
        ys_whlspeed_fl_1 = [18.6875, 17.828125, 16.390625, 14.671875, 13.25]
        ys_whlspeed_fr_1 = [18.921875, 17.71875, 16.828125, 15.0, 13.46875]
        ys_whlspeed_rl_1 = [19.03125, 17.71875, 16.71875, 15.109375, 13.03125]
        ys_whlspeed_rr_1 = [19.25, 17.71875, 16.828125, 14.671875, 13.140625]
        whlspeed_0 = [ys_whlspeed_fl_0, ys_whlspeed_fr_0, ys_whlspeed_rl_0, ys_whlspeed_rr_0]
        whlspeed_1 = [ys_whlspeed_fl_1, ys_whlspeed_fr_1, ys_whlspeed_rl_1, ys_whlspeed_rr_1]

        subplots = [SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    subplot = subplots[0]
                    for i in range(len(whlspeed_0)):
                        call = subplot.mock_calls[i]
                        actual_xs = call[1][0]
                        actual_ys = call[1][1]
                        label = call[2]['label']

                        n0 = len(whlspeed_0[i])
                        n1 = len(whlspeed_1[i])
                        self.assertEqual(len(actual_xs), len(actual_ys),
                                         "numbers of actual x and y values do not match for signal %s" % label)
                        self.assertEqual(len(actual_xs), len(
                            xs)+1, "number of x values does not match for signal %s" % label)
                        self.assertEqual(len(actual_ys), n0+n1+1,
                                         "number of y values does not match for signal %s" % label)

                        for j in range(1, len(actual_xs)):
                            self.assertLess(actual_xs[j-1], actual_xs[j],
                                            "actual x values are not strictly increasing for signal %s" % label)
                        self.assertEqual(actual_xs[:n0], xs[:n0],
                                         "first half of x values does not match for signal %s" % label)
                        self.assertEqual(actual_xs[-n1:], xs[-n1:],
                                         "second half of x values does not match for signal %s" % label)

                        self.assertEqual(actual_ys[:n0], whlspeed_0[i],
                                         "first half of y values does not match for signal %s" % label)
                        self.assertEqual(
                            actual_ys[n0], None, "expected separating value is not at it's place for signal %s" % label)
                        self.assertEqual(actual_ys[-n1:], whlspeed_1[i],
                                         "second half of y values does not match for signal %s" % label)
                        #self.assertEqual(actual_ys[-n1:], actual_ys[n1+1:], "this test is a duplicate of len == n0+n1+1")

    def test_break_time_none(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '--break-time', '-1']
        input_data = """\
 (000.000000)  vcan0  00000343   [8]  C7 04 CE 04 EB 04 EB 04
 (001.001788)  vcan0  00000343   [8]  75 04 75 04 99 04 99 04
 (002.003649)  vcan0  00000343   [8]  2C 04 34 04 10 04 25 04
 (003.005497)  vcan0  00000343   [8]  AC 03 BB 03 AC 03 BB 03
 (004.007213)  vcan0  00000343   [8]  70 03 45 03 45 03 5B 03
 (015.495473)  vcan0  00000343   [8]  AC 04 BB 04 C2 04 D0 04
 (016.497347)  vcan0  00000343   [8]  75 04 6E 04 6E 04 6E 04
 (017.499210)  vcan0  00000343   [8]  19 04 35 04 2E 04 35 04
 (018.501066)  vcan0  00000343   [8]  AB 03 C0 03 C7 03 AB 03
 (019.502899)  vcan0  00000343   [8]  50 03 5E 03 42 03 49 03
"""

        xs = self.parse_time(input_data, self.parse_seconds)

        ys_whlspeed_fl = [19.109375, 17.828125, 16.6875, 14.6875, 13.75]
        ys_whlspeed_fr = [19.21875, 17.828125, 16.8125, 14.921875, 13.078125]
        ys_whlspeed_rl = [19.671875, 18.390625, 16.25, 14.6875, 13.078125]
        ys_whlspeed_rr = [19.671875, 18.390625, 16.578125, 14.921875, 13.421875]
        ys_whlspeed_fl += [18.6875, 17.828125, 16.390625, 14.671875, 13.25]
        ys_whlspeed_fr += [18.921875, 17.71875, 16.828125, 15.0, 13.46875]
        ys_whlspeed_rl += [19.03125, 17.71875, 16.71875, 15.109375, 13.03125]
        ys_whlspeed_rr += [19.25, 17.71875, 16.828125, 14.671875, 13.140625]

        subplots = [SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(xs, ys_whlspeed_fr, '', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(xs, ys_whlspeed_rl, '', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(xs, ys_whlspeed_rr, '', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tz),
            ],
        ]

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                with plt:
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)
                    for i in range(len(expected_subplot_calls)):
                        self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                             msg="calls don't match for subplot %s" % i)

    def test_output_file(self):
        fn = "out.pdf"
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '-o', fn]
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
        ys_whlspeed_fl = [18.75, 18.359375, 17.828125, 17.578125,
                          17.171875, 16.5, 15.5625, 15.28125, 15.28125, 14.609375]
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
            mock.call.subplot().set_xlabel(self.XLABEL_tA % "27.12.2020"),
            mock.call.savefig(fn),
        ]

        stdout = StringIO()
        expected_output = "Result written to %s\n" % fn

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with PyplotMock() as plt:
                        cantools._main()
                        self.assertListEqual(plt.mock_calls, expected_calls)
                        self.assertEqual(stdout.getvalue(), expected_output)

    # ------- other tests -------

    def test_empty_line(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '*fl']
        input_data = """\
 (2020-12-27 11:59:14.820230)  vcan0  00000343   [8]  B0 04 B0 04 B0 04 D4 04

 (2020-12-27 11:59:16.823895)  vcan0  00000343   [8]  75 04 84 04 75 04 6E 04
"""

        xs = self.parse_time(input_data, self.parse_absolute_time)
        ys_whlspeed_fl = [18.75, 17.828125]

        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.subplot().plot(xs, ys_whlspeed_fl, '', label='BREMSE_33.whlspeed_FL'),
            mock.call.subplot().legend(),
            mock.call.subplot().set_xlabel(self.XLABEL_tA % "27.12.2020"),
            mock.call.show(),
        ]

        stdout = StringIO()
        expected_output = ""

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with PyplotMock() as plt:
                        cantools._main()
                        self.assertListEqual(plt.mock_calls, expected_calls)

    def test_do_not_replot(self):
        argv = ['cantools', '--database', self.DBC_FILE, 'plot', '*fl:-o', '*:o']
        input_data = """\
 (000.000000)  vcan0  00000343   [8]  80 05 64 05 5C 05 64 05
 (001.001805)  vcan0  00000343   [8]  A0 05 92 05 A0 05 99 05
 (002.003656)  vcan0  00000343   [8]  1B 06 FE 05 F0 05 F7 05
 (003.005470)  vcan0  00000343   [8]  54 06 54 06 54 06 4C 06
 (004.007317)  vcan0  00000343   [8]  DB 06 BE 06 C5 06 B7 06
"""

        xs = self.parse_time(input_data, self.parse_seconds)
        ys_whlspeed_fl = [22.0, 22.5, 24.421875, 25.3125, 27.421875]
        ys_whlspeed_fr = [21.5625, 22.28125, 23.96875, 25.3125, 26.96875]
        ys_whlspeed_rl = [21.4375, 22.5, 23.75, 25.3125, 27.078125]
        ys_whlspeed_rr = [21.5625, 22.390625, 23.859375, 25.1875, 26.859375]

        subplots = [SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(xs, ys_whlspeed_fl, '-o', label='BREMSE_33.whlspeed_FL'),
                mock.call.plot(xs, ys_whlspeed_fr, 'o', label='BREMSE_33.whlspeed_FR'),
                mock.call.plot(xs, ys_whlspeed_rl, 'o', label='BREMSE_33.whlspeed_RL'),
                mock.call.plot(xs, ys_whlspeed_rr, 'o', label='BREMSE_33.whlspeed_RR'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tz),
            ],
        ]

        stdout = StringIO()
        expected_output = ""

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()
                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

    def test_do_replot(self):
        argv = ['cantools', '--database', self.DBC_FILE_CHOICES, 'plot', "Foo:b-", "Foo:rd"]
        input_data = """\
 (000.000000)  vcan0  00000000   [8]  06 00 00 00 00 00 00 00
 (004.212794)  vcan0  00000000   [8]  05 00 00 00 00 00 00 00
 (006.264802)  vcan0  00000000   [8]  00 00 00 00 00 00 00 00
 (010.569680)  vcan0  00000000   [8]  00 00 00 00 00 00 00 00
 (012.432106)  vcan0  00000000   [8]  00 00 00 00 00 00 00 00
"""

        db = cantools.db.load_file(self.DBC_FILE_CHOICES)
        choices = db.get_message_by_name("Foo").get_signal_by_name("Foo").choices

        xs = self.parse_time(input_data, self.parse_seconds)
        ys = [6, 5, 0, 0, 0]
        ys = [choices[y] for y in ys]

        subplots = [SubplotMock()]
        plt = PyplotMock()
        plt.subplot.side_effect = subplots
        expected_calls = [
            mock.call.subplot(1,1,1, sharex=None),
            mock.call.show(),
        ]
        expected_subplot_calls = [
            [
                mock.call.plot(xs, ys, 'b-', label='Foo.Foo'),
                mock.call.plot(xs, ys, 'rd', label='Foo.Foo'),
                mock.call.legend(),
                mock.call.set_xlabel(self.XLABEL_tz),
            ],
        ]

        stdout = StringIO()
        expected_output = ""

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.stdout', stdout):
                with mock.patch('sys.argv', argv):
                    with plt:
                        cantools._main()
                        self.assertListEqual(plt.mock_calls, expected_calls)
                        for i in range(len(expected_subplot_calls)):
                            self.assertListEqual(subplots[i].mock_calls, expected_subplot_calls[i],
                                                 msg="calls don't match for subplot %s" % i)

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

    def parse_absolute_seconds(self, timestamp):
        return datetime.datetime.fromtimestamp(float(timestamp))

    def parse_seconds(self, timestamp):
        return float(timestamp)

    ELLIPSIS = "..."
    LEN_ELLIPSIS = len(ELLIPSIS)

    def assertLinesMatch(self, actual_output, expected_output):
        """
        Compares two strings line by line.
        Differs from assertEqual in that it allows to abbreviate lines
        in expected_output by letting them end on ELLIPSIS.
        """
        actual_output = actual_output.splitlines()
        expected_output = expected_output.splitlines()
        for ln_actual, ln_expected in zip(actual_output, expected_output):
            if ln_expected.endswith(self.ELLIPSIS):
                ln_expected = ln_expected[:-self.LEN_ELLIPSIS]
                ln_actual = ln_actual[:len(ln_expected)]
            self.assertEqual(ln_expected, ln_actual)
        self.assertEqual(len(actual_output), len(expected_output))


if __name__ == '__main__':
    unittest.main()
