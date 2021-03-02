#!/usr/bin/env python3

import os
import sys
import unittest
from unittest import mock
from io import StringIO
import cantools
import matplotlib.pyplot as plt


class CanToolsPlotTest(unittest.TestCase):

    DBC_FILE = os.path.join(os.path.split(__file__)[0], 'files/dbc/abs.dbc')
    FN_OUT = "out.pdf"

    def test_plot_tz(self):
        self.assertFalse(os.path.exists(self.FN_OUT))
        argv = ['cantools', 'plot', '-o', self.FN_OUT, self.DBC_FILE]
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

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()

        self.assertTrue(os.path.exists(self.FN_OUT))
        os.remove(self.FN_OUT)

    def test_plot_style(self):
        self.assertFalse(os.path.exists(self.FN_OUT))
        argv = ['cantools', 'plot', '--style', 'seaborn', '-o', self.FN_OUT, self.DBC_FILE]
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

        with mock.patch('sys.stdin', StringIO(input_data)):
            with mock.patch('sys.argv', argv):
                cantools._main()

        self.assertTrue(os.path.exists(self.FN_OUT))
        os.remove(self.FN_OUT)

    def test_plot_list_styles(self):
        self.assertFalse(os.path.exists(self.FN_OUT))
        argv = ['cantools', 'plot', '--list-styles', '']
        stdout = StringIO()

        expected = "available matplotlib styles:"
        expected += "".join("\n- %s" % s for s in plt.style.available)
        expected += "\n"

        with mock.patch('sys.stdout', stdout):
            with mock.patch('sys.argv', argv):
                cantools._main()

        self.assertEqual(stdout.getvalue(), expected)

if __name__ == '__main__':
    unittest.main()
