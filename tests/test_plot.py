#!/usr/bin/env python3

import os
import unittest

try:
    from unittest import mock
except ImportError:
    import mock

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

import cantools
import matplotlib.pyplot


class CanToolsCommandLineTest(unittest.TestCase):

    DBC_FILE = os.path.join(os.path.split(__file__)[0], 'files/dbc/abs.dbc')

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


if __name__ == '__main__':
    unittest.main()
