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

    reo_lines_that_are_expected_to_differ = re.compile(
            r'^\\newcommand{\\(?P<key0>infile|outfile)}{(?P<val0>.*?)}|'
            r'^\\(?P<key1>title){(?P<val1>.*?)}|'
            r'^\\(?P<key2>date){(?P<val2>.*?)}')

    maxDiff = None

    # ------- aux functions -------

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
        read_is = lambda f: [self.reo_lines_that_are_expected_to_differ.sub(self.sub_line, ln) for ln in f]

        with open(fn_is, 'rt') as f:
            content_is = read_is(f)
        with open(fn_expected, 'rt') as f:
            content_expected = list(f)
        self.assertEqual(content_expected, content_is)

    def sub_line(self, m):
        key, val, group = self.find_group(m)
        if key in ('infile', 'outfile'):
            val = os.path.split(val)[1]
        elif key == 'date':
            val = '09.09.2021'
        elif key == 'title':
            val = os.path.split(val)[1]
            val = val[0].upper() + val[1:]
        else:
            raise ValueError('invalid key: %s' % key)

        return self.replace(m, group, val)

    def find_group(self, m):
        for i in range(10):
            key = m.group('key%s' % i)
            if key:
                val = m.group('val%s' % i)
                return key, val, i

        raise ValueError('no key found in match object')

    def replace(self, m, i, val):
        group = 'val%s' % i
        s = m.group()
        im0, im1 = m.span()
        iv0, iv1 = m.span(group)
        i0 = iv0 - im0
        i1 = iv1 - im0
        return s[:i0] + val + s[i1:]


    # ------- test cases -------

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
