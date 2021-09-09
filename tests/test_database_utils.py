# -*- coding: utf-8 -*-

from parameterized import parameterized
import unittest

from cantools.database.utils import sawtooth_to_network_bitnum, cdd_offset_to_dbc_start_bit

class CanToolsDatabaseUtilsTest(unittest.TestCase):

    @parameterized.expand((
        ("0", 0, 7),
        ("1", 1, 6),
        ("2", 2, 5),
        ("3", 3, 4),
        ("4", 4, 3),
        ("5", 5, 2),
        ("6", 6, 1),
        ("7", 7, 0),
        ("8", 8, 15),
        ("15", 15, 8),
        ("32", 32, 39),
        ("64", 64, 71),
        ("65", 65, 70),
    ))
    def test_sawtooth_to_network_bitnum(self, _name, sawtooth_bitnum, expected_network_bitnum):

        network_bitnum = sawtooth_to_network_bitnum(sawtooth_bitnum)
        self.assertEqual(network_bitnum, expected_network_bitnum)

    @parameterized.expand((
        ("BE-0-8", "big_endian", 0, 8, 7),
        ("LE-0-8", "little_endian", 0, 8, 0),

        ("BE-0-4", "big_endian", 0, 4, 3),
        ("LE-0-4", "little_endian", 0, 4, 0),

        ("BE-4-4", "big_endian", 4, 4, 7),
        ("LE-4-4", "little_endian", 4, 4, 4),

        ("BE-0-16", "big_endian", 0, 16, 7),
        ("LE-0-16", "little_endian", 0, 16, 0),

        ("BE-0-32", "big_endian", 0, 32, 7),
        ("LE-0-32", "little_endian", 0, 32, 0),

        ("BE-32-16", "big_endian", 32, 16, 39),
        ("LE-32-16", "little_endian", 32, 16, 32),
    ))
    def test_cdd_offset_to_dbc_start_bit(self, _name, byte_order, offset, bit_length, expected_dbc_start_bit):

        dbc_start_bit = cdd_offset_to_dbc_start_bit(offset, bit_length, byte_order)
        self.assertEqual(dbc_start_bit, expected_dbc_start_bit)


if __name__ == '__main__':
    unittest.main()
