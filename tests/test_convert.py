#!/usr/bin/env python3

import os
import shutil
import tempfile

import unittest
from unittest import mock

import cantools


class CanToolsConvertFullTest(unittest.TestCase):

    SYM_FILE_COMMENTS_HEX_AND_MOTOROLA = os.path.join(os.path.split(__file__)[0], 'files', 'sym', 'comments_hex_and_motorola.sym')
    DBC_FILE_COMMENTS_HEX_AND_MOTOROLA = os.path.join(os.path.split(__file__)[0], 'files', 'dbc', 'comments_hex_and_motorola_converted_from_sym.dbc')

    maxDiff = None

    # ------- aux functions -------

    def get_out_file_name(self, fn_in, ext):
        fn = fn_in
        fn = os.path.splitext(fn)[0] + ext
        fn = os.path.split(fn)[1]
        fn = os.path.join(tempfile.mkdtemp(), fn)
        return fn

    def assertFileEqual(self, fn1, fn2):
        with open(fn1, 'rt') as f:
            content1 = list(f)
        with open(fn2, 'rt') as f:
            content2 = list(f)
        self.assertListEqual(content1, content2)

    def remove_out_file(self, fn_out):
        path = os.path.split(fn_out)[0]
        shutil.rmtree(path)

    # ------- test sym -> dbc -------

    def test_sym_to_dbc(self):
        sym = self.SYM_FILE_COMMENTS_HEX_AND_MOTOROLA
        dbc = self.DBC_FILE_COMMENTS_HEX_AND_MOTOROLA
        fn_out = self.get_out_file_name(sym, ext='.dbc')
        argv = ['cantools', 'convert', sym, fn_out]

        with mock.patch('sys.argv', argv):
            cantools._main()

        fn_expected_output = dbc
        self.assertFileEqual(fn_expected_output, fn_out)

        self.remove_out_file(fn_out)


if __name__ == '__main__':
    unittest.main()
