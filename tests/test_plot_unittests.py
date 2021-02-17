#!/usr/bin/env python3

import unittest
import datetime

import cantools.subparsers.plot as plot

class CanToolsPlotUnittests(unittest.TestCase):

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

    def test_parse_user_input_absolute_time_output_formats(self):
        sut = plot.TimestampParser(None)
        for scope, first_timestamp, expected in [
            # if the user_input is an integer I think it's more likely to be a second than a year
            #('year', '2019-02-03 04:05:06', '2021-01-01 00:00:00'),
            ('month', '2021-02-03 04:05:06', '2021-04-01 00:00:00'),
            ('day', '2021-02-03 04:05:06', '2021-02-06 00:00:00'),
            # I am not allowing '%H:%M' as input because it could be meant as '%M:%S'.
            # That is probably why matplotlib is using '%m-%d %H' and '%d %H:%M' by default.
            # I changed that because it seems rather unintuitive.
            #('hour', '2021-02-03 04:05:06', '2021-02-03 08:00:00'),
            #('minute', '2021-02-03 04:05:06', '2021-02-03 04:30:00'),
            ('second', '2021-02-03 04:05:06', '2021-02-03 04:05:42'),
            ('microsecond', '2021-02-03 04:05:06.000007', '2021-02-03 04:05:06.0042'),
        ]:
            pattern = plot.plt.rcParams["date.autoformatter.%s" % scope]
            first_timestamp = self.parse_time(first_timestamp)
            expected = self.parse_time(expected)
            user_input = expected.strftime(pattern)
            actual = sut.parse_user_input_absolute_time(user_input, first_timestamp)
            self.assertEqual(actual, expected)

    def test_parse_user_input_absolute_time_invalid_input(self):
        sut = plot.TimestampParser(None)
        with self.assertRaises(ValueError):
            sut.parse_user_input_absolute_time('12:43', None)


    def test_parse_user_input_relative_time(self):
        sut = plot.TimestampParser(None)
        for user_input, expected in [
            ('4242', 4242),
            ('42.42', 42.42),
            ('12:34.', 12*60+34),
            (':12:34', 12*60+34),
            ('1:30:00',  1*60**2+30*60),
            ('1 day', 24*60*60),
            ('2 days', 2*24*60*60),
            ('1 day, 3:00', 1*24*60**2+3*60**2),
            ('3 days, 4:56:01.12', 3*24*60**2 + 4*60**2 + 56*60 + 1.12),
        ]:
            actual = sut.parse_user_input_relative_time(user_input, first_timestamp=0)
            self.assertEqual(actual, expected, "unexpected result for %r" % user_input)


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
