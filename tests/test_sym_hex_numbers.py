"""SYM hexadecimal values must remain distinct from hex-shaped identifiers."""

import unittest

from parameterized import parameterized

import cantools.database
from cantools.database.errors import UnsupportedDatabaseFormatError


class SymHexNumbersTest(unittest.TestCase):
    @parameterized.expand([
        ('a', 'Ah', 10),
        ('a0', 'A0h', 160),
        ('c0', 'C0h', 192),
        ('aaaa', 'AAAAh', 0xAAAA),
        ('negative_alpha', '-Ah', -10),
        ('digit_hex', '09h', 9),
        ('decimal', '9', 9),
        ('negative_digit_hex', '-0Ah', -10),
    ])
    def test_mux_value(self, name, value, expected):
        database = cantools.database.load_string(
            'FormatVersion=6.0 // generated test\n'
            'Title="test"\n'
            '{SENDRECEIVE}\n'
            '[Message]\nID=123h\nLen=8\n'
            f'Mux=Mux 0,16 {value} // numeric mux value\n'
            'Var=Payload unsigned 16,8\n', 'sym')
        message = database.get_message_by_frame_id(0x123)
        self.assertTrue(message.get_signal_by_name('Mux').is_multiplexer)
        self.assertEqual(message.get_signal_by_name('Payload').multiplexer_ids,
                         [expected])

    def test_alpha_hex_id_range_and_color(self):
        database = cantools.database.load_string(
            'FormatVersion=6.0 // generated test\n'
            '{SENDRECEIVE}\n'
            '[Message]\nID=AAh-ACh\nLen=1\nColor=FF0000h\n'
            'Var=Payload unsigned 0,8\n', 'sym')
        self.assertEqual([message.frame_id for message in database.messages],
                         [0xAA, 0xAB, 0xAC])

    def test_hex_shaped_names_and_char_type(self):
        database = cantools.database.load_string(
            'FormatVersion=6.0 // generated test\n'
            '{ENUMS}\nEnum=Ah(0="zero",1="one")\n'
            '{SIGNALS}\nSig=Char char\nSig=Bh Ah 8 /d:Ah\n'
            '{SENDRECEIVE}\n'
            '[Ch]\nID=123h\nLen=2\n'
            'Sig=Char 0\nSig=Bh 8\n', 'sym')
        message = database.get_message_by_name('Ch')
        self.assertEqual(message.get_signal_by_name('Char').length, 8)
        self.assertEqual(str(message.get_signal_by_name('Bh').choices[1]), 'one')

    @parameterized.expand(['char', 'A0hello', 'A0h_more', 'GGh'])
    def test_non_hex_word_is_not_a_mux_value(self, value):
        with self.assertRaises(UnsupportedDatabaseFormatError):
            cantools.database.load_string(
                'FormatVersion=6.0 // generated test\n'
                '{SENDRECEIVE}\n'
                '[Message]\nID=123h\nLen=8\n'
                f'Mux=Mux 0,8 {value}\n'
                'Var=Payload unsigned 8,8\n', 'sym')


if __name__ == '__main__':
    unittest.main()
