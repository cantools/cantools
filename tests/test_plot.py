#!/usr/bin/env python3

import os
import datetime
import re
import unittest
from unittest import mock
from io import StringIO
import cantools
import matplotlib.pyplot


class CanToolsPlotTest(unittest.TestCase):

    DBC_FILE = os.path.join(os.path.split(__file__)[0], 'files/dbc/abs.dbc')
    REO_TIMESTAMP = re.compile('\(([^)]+)\)')

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

        xs = self.parse_time(input_data, datetime.datetime.fromisoformat)
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
            with mock.patch('matplotlib.pyplot') as plt:
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
            with mock.patch('matplotlib.pyplot') as plt:
                with mock.patch('sys.argv', argv):
                    cantools._main()
                    self.assertListEqual(plt.mock_calls, expected_calls)


    def parse_time(self, log, parse):
        out = []
        for ln in log.splitlines():
            if not ln:
                continue
            m = self.REO_TIMESTAMP.search(ln)
            timestamp = m.group(1)
            out.append(parse(timestamp))
        return out


if __name__ == '__main__':
    unittest.main()
