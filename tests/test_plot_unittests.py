#!/usr/bin/env python3

import unittest
import datetime

import cantools.subparsers.plot as plot

class CanToolsPlotUnittests(unittest.TestCase):

    def test_parse_user_input_relative_time(self):
        pass

    def test_parse_user_input_absolute_time(self):
        sut = plot.TimestampParser(None)
        for user_input, first_timestamp, expected in [
            ('12:43:', '2021-01-17 00:00:00', '2021-01-17 12:43:00'),
            ('12:43.', '2021-01-17 00:00:00', '2021-01-17 00:12:43'),
            (':12:43', '2021-01-17 00:00:00', '2021-01-17 00:12:43'),
            ('17.1.', '2021-01-14 12:34:56', '2021-01-17 00:00:00'),
            ('2021-01-01 2:00', '2020-12-30 12:34:56', '2021-01-01 02:00:00'),
            ('2-14', '2021-02-13 12:34:56', '2021-02-14 00:00:00'),
            ('42', '2021-03-14 15:9:26', '2021-03-14 15:9:42'),
            ('1.5', '2021-01-01 00:00:00', '2021-01-01 00:00:01.5'),
            ('2020-04-01 10:11:12.131415', '1970-1-1 1:00:00', '2020-04-01 10:11:12.131415'),
        ]:
            first_timestamp = self.parse_time(first_timestamp)
            expected = self.parse_time(expected)
            actual = sut.parse_user_input_absolute_time(user_input, first_timestamp)
            self.assertEqual(actual, expected)

    def test_parse_user_input_absolute_time_invalid_input(self):
        sut = plot.TimestampParser(None)
        with self.assertRaises(ValueError):
            sut.parse_user_input_absolute_time('12:43', None)

    # ------- auxiliary functions -------

    def parse_time(self, s):
        for pattern in ("%Y-%m-%d %H:%M:%S", "%Y-%m-%d %H:%M:%S.%f"):
            try:
                return datetime.datetime.strptime(s, pattern)
            except ValueError:
                pass
        raise ValueError('Failed to parse time %r' % s)


if __name__ == '__main__':
    unittest.main()
