import unittest
from collections import OrderedDict

from parameterized import parameterized

import cantools.database
from cantools.database import Database, Message, Signal
from cantools.database.can.formats.dbc.dbc_specifics import DbcSpecifics

VALUES = [
    ('empty', ''),
    ('plain', 'inches'),
    ('quotes', 'say "hello"'),
    ('only_quotes', '""'),
    ('backslashes', r'C:\tools\logs'),
    ('backslash_before_quote', r'path\"name'),
    ('two_backslashes_before_quote', r'path\\"name'),
    ('unicode', 'länge "µm"'),
]


class DbcDisplayTextTest(unittest.TestCase):
    @parameterized.expand(VALUES)
    def test_unit_round_trip(self, _name, value):
        signal = Signal('Value', 0, 8, unit=value)
        database = Database([Message(1, 'Frame', 1, [signal])])
        for _round in range(2):
            database = cantools.database.load_string(
                database.as_dbc_string(), database_format='dbc')
            self.assertEqual(database.messages[0].signals[0].unit, value or None)

    @parameterized.expand(VALUES)
    def test_choice_round_trip(self, _name, value):
        signal = Signal('Value', 0, 8)
        signal.choices = {1: value}
        database = Database([Message(1, 'Frame', 1, [signal])])
        for _round in range(2):
            database = cantools.database.load_string(
                database.as_dbc_string(), database_format='dbc')
            self.assertEqual(str(database.messages[0].signals[0].choices[1]), value)

    @parameterized.expand(VALUES)
    def test_value_table_round_trip(self, _name, value):
        specifics = DbcSpecifics(value_tables=OrderedDict(
            [('Labels', {1: value})]))
        database = Database(dbc_specifics=specifics)
        for _round in range(2):
            database = cantools.database.load_string(
                database.as_dbc_string(), database_format='dbc')
            self.assertEqual(str(database.dbc.value_tables['Labels'][1]), value)
