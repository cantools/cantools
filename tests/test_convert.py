#!/usr/bin/env python3

import os
import shutil
import tempfile
import re

import unittest
from unittest import mock

import cantools


class CanToolsConvertFullTest(unittest.TestCase):

    DBC_FILE_CHOICES = os.path.join(os.path.split(__file__)[0], 'files/dbc/choices.dbc')

    reo_ignore_lines = re.compile(r'^\\newcommand{\\(in|out)file}{.*?}|'
                                  r'^\\title{.*?}')

    maxDiff = None

    def get_out_file_name(self, fn_dbc):
        fn = fn_dbc
        fn = os.path.splitext(fn)[0] + '.tex'
        fn = os.path.split(fn)[1]
        fn = os.path.join(tempfile.mkdtemp(), fn)
        return fn

    def remove_out_file(self, fn_out):
        path = os.path.split(fn_out)[0]
        shutil.rmtree(path)

    def get_golden_master_file_name(self, fn_dbc):
        return os.path.splitext(fn_dbc)[0] + '.tex'

    def assert_file_content_equal(self, fn_expected, fn_is):
        read = lambda f: [ln for ln in f if not self.reo_ignore_lines.match(ln)]

        with open(fn_is, 'rt') as f:
            content_is = read(f)
        with open(fn_expected, 'rt') as f:
            content_expected = read(f)
        self.assertEqual(content_expected, content_is)


    # ------- test different timestamp formats -------

    def test_choices(self):
        dbc = self.DBC_FILE_CHOICES
        fn_out = self.get_out_file_name(dbc)
        fn_golden_master = self.get_golden_master_file_name(dbc)
        argv = ['cantools', 'convert', dbc, fn_out]

        with mock.patch('sys.argv', argv):
            cantools._main()

        self.assert_file_content_equal(fn_golden_master, fn_out)


if __name__ == '__main__':
    unittest.main()
