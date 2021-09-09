#!/usr/bin/env python3

import unittest
import cantools.subparsers.convert_to_tex as sut


class CanToolsConvertTest(unittest.TestCase):

    def test_break_sig_name(self):
        names = (
            ('CamelCase', r'Camel\-Case'),
            ('snake\_case', r'snake\_\-case'),
            ('UPPER\_CASE', r'UPPER\_\-CASE'),
            ('CANMessage', r'CAN\-Message'),
            ('CommandID', r'Command\-ID'),
            ('i-i', r'i-\-i'),
            ('', r''),
        )
        for inp, expected in names:
            out = sut.Converter.break_sig_name(inp)
            self.assertEqual(expected, out)


if __name__ == '__main__':
    unittest.main()
