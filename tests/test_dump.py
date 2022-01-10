# -*- coding: utf-8 -*-

import unittest
import logging

import cantools
from cantools.subparsers.dump import formatting


class CanToolsDumpTest(unittest.TestCase):

    def test_the_homer(self):
        db = cantools.db.load_file('tests/files/kcd/the_homer.kcd')

        self.assertEqual(db.messages[1].signal_tree,
                         [
                             {
                                 'ABS_InfoMux': {
                                     0: ['Info0', 'Info1'],
                                     1: ['Info2', 'Info3'],
                                     2: ['Info4', 'Info5'],
                                     3: ['Info6', 'Info7']
                                 }
                             },
                             'OutsideTemp',
                             'SpeedKm',
                             'Handbrake'
        ])

        self.assertEqual(
            formatting.signal_tree_string(db.messages[1]),
            '-- {root}\n'
            '   +-- ABS_InfoMux\n'
            '   |   +-- 0\n'
            '   |   |   +-- Info0\n'
            '   |   |   +-- Info1\n'
            '   |   +-- 1\n'
            '   |   |   +-- Info2\n'
            '   |   |   +-- Info3\n'
            '   |   +-- 2\n'
            '   |   |   +-- Info4\n'
            '   |   |   +-- Info5\n'
            '   |   +-- 3\n'
            '   |       +-- Info6\n'
            '   |       +-- Info7\n'
            '   +-- OutsideTemp\n'
            '   +-- SpeedKm\n'
            '   +-- Handbrake')

    def test_multiplex(self):
        db = cantools.db.load_file('tests/files/dbc/multiplex.dbc')

        message_1 = db.messages[0]

        self.assertEqual(message_1.signal_tree,
                         [
                             {
                                 'Multiplexor': {
                                     8: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L'
                                     ],
                                     16: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L'
                                     ],
                                     24: [
                                         'BIT_J', 'BIT_C', 'BIT_G', 'BIT_L',
                                         'BIT_A', 'BIT_K', 'BIT_E', 'BIT_D',
                                         'BIT_B', 'BIT_H', 'BIT_F'
                                     ]
                                 }
                             }
                         ])

        self.assertEqual(
            formatting.signal_tree_string(message_1),
            '-- {root}\n'
            '   +-- Multiplexor\n'
            '       +-- 8\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- 16\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- 24\n'
            '           +-- BIT_J\n'
            '           +-- BIT_C\n'
            '           +-- BIT_G\n'
            '           +-- BIT_L\n'
            '           +-- BIT_A\n'
            '           +-- BIT_K\n'
            '           +-- BIT_E\n'
            '           +-- BIT_D\n'
            '           +-- BIT_B\n'
            '           +-- BIT_H\n'
            '           +-- BIT_F')

    def test_multiplex_choices(self):
        db = cantools.db.load_file('tests/files/dbc/multiplex_choices.dbc')

        message_1 = db.messages[0]

        self.assertEqual(
            formatting.signal_choices_string(message_1),
            '\n'
            'Multiplexor\n'
            '    8 MULTIPLEXOR_8\n'
            '    16 MULTIPLEXOR_16\n'
            '    24 MULTIPLEXOR_24\n'
            '\n'
            'BIT_L\n'
            '    0 Off\n'
            '    1 On'
        )

        message_2 = db.messages[1]

        self.assertEqual(
            formatting.signal_tree_string(message_2),
            '-- {root}\n'
            '   +-- Multiplexor\n'
            '       +-- MULTIPLEXOR_4_NO_SIGNALS (4)\n'
            '       +-- MULTIPLEXOR_8 (8)\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- MULTIPLEXOR_16 (16)\n'
            '       |   +-- BIT_J\n'
            '       |   +-- BIT_C\n'
            '       |   +-- BIT_G\n'
            '       |   +-- BIT_L\n'
            '       +-- MULTIPLEXOR_24 (24)\n'
            '           +-- BIT_J\n'
            '           +-- BIT_C\n'
            '           +-- BIT_G\n'
            '           +-- BIT_L\n'
            '           +-- BIT_A\n'
            '           +-- BIT_K\n'
            '           +-- BIT_E\n'
            '           +-- BIT_D\n'
            '           +-- BIT_B\n'
            '           +-- BIT_H\n'
            '           +-- BIT_F')

    def test_multiplex_extended(self):
        #            tree              |  bits
        # =============================+========
        # -- {root}                    |
        #    +-- S0                    |  0..4
        #    |   +-- 0                 |
        #    |   |   +-- S1            |  4..8
        #    |   |       +-- 0         |
        #    |   |       |   +-- S2    |  8..16
        #    |   |       |   +-- S3    | 16..32
        #    |   |       +-- 2         |
        #    |   |           +-- S4    |  8..32
        #    |   +-- 1                 |
        #    |       +-- S5            |  4..32
        #    +-- S6                    | 32..40
        #        +-- 1                 |
        #        |   +-- S7            | 40..64
        #        +-- 2                 |
        #            +-- S8            | 40..48
        #
        signals = [
            cantools.db.Signal(name='S0',
                               start=0,
                               length=4,
                               is_multiplexer=True),
            cantools.db.Signal(name='S1',
                               start=4,
                               length=4,
                               is_multiplexer=True,
                               multiplexer_ids=[0],
                               multiplexer_signal='S0'),
            cantools.db.Signal(name='S2',
                               start=8,
                               length=8,
                               multiplexer_ids=[0],
                               multiplexer_signal='S1'),
            cantools.db.Signal(name='S3',
                               start=16,
                               length=16,
                               multiplexer_ids=[0],
                               multiplexer_signal='S1'),
            cantools.db.Signal(name='S4',
                               start=8,
                               length=24,
                               multiplexer_ids=[2],
                               multiplexer_signal='S1'),
            cantools.db.Signal(name='S5',
                               start=4,
                               length=28,
                               multiplexer_ids=[1],
                               multiplexer_signal='S0'),
            cantools.db.Signal(name='S6',
                               start=32,
                               length=8,
                               is_multiplexer=True),
            cantools.db.Signal(name='S7',
                               start=40,
                               length=24,
                               multiplexer_ids=[1],
                               multiplexer_signal='S6'),
            cantools.db.Signal(name='S8',
                               start=40,
                               length=8,
                               multiplexer_ids=[2],
                               multiplexer_signal='S6')
        ]

        message = cantools.db.Message(frame_id=1,
                                      name='M0',
                                      length=8,
                                      signals=signals)

        self.assertEqual(message.signal_tree,
                         [
                             {
                                 'S0': {
                                     0: [
                                         {
                                             'S1': {
                                                 0: ['S2', 'S3'],
                                                 2: ['S4']
                                             }
                                         }
                                     ],
                                     1: ['S5']
                                 }
                             },
                             {
                                 'S6': {
                                     1: ['S7'],
                                     2: ['S8']
                                 }
                             }
                         ])

        self.assertEqual(
            formatting.signal_tree_string(message),
            '-- {root}\n'
            '   +-- S0\n'
            '   |   +-- 0\n'
            '   |   |   +-- S1\n'
            '   |   |       +-- 0\n'
            '   |   |       |   +-- S2\n'
            '   |   |       |   +-- S3\n'
            '   |   |       +-- 2\n'
            '   |   |           +-- S4\n'
            '   |   +-- 1\n'
            '   |       +-- S5\n'
            '   +-- S6\n'
            '       +-- 1\n'
            '       |   +-- S7\n'
            '       +-- 2\n'
            '           +-- S8')

    def test_message_layout(self):
        db = cantools.database.load_file('tests/files/kcd/message_layout.kcd',
                                         strict=False)

        # Message 1.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B',
            ' y',
            ' t',
            ' e'
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message1'))
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 2.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |<-----------------------------x|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t                                   +-- Signal1',
            ' e'
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message2'))
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 3.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |   |   |   |   |   |   |   |<--|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |--x|   |   |   |   |   |   |   |',
            ' e     +---+---+---+---+---+---+---+---+',
            '         +-- Signal1',
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message3'))
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 4.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |   |   |   |   |   |   |   |<--|',
            ' B     +---+---+---+---+---+---+---+---+',
            ' y   1 |--x|   |   |   |   |   |   |   |',
            ' t     +---+---+---+---+---+---+---+---+',
            ' e       +-- Signal1',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message4'))
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 5.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |<---XXXXXXX-------x|XXX|   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '                 |       |   +-- Signal7',
            '                 |       |   +-- Signal8',
            '                 |       +-- Signal1',
            ' B               +-- Signal2',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |   |   |   |   |   |<-x|XXXXXXX|',
            ' e     +---+---+---+---+---+---+---+---+',
            '                             |   +-- Signal4',
            '                             +-- Signal6',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |XXX---x|   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '         |   +-- Signal5',
            '         +-- Signal3'
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message5'))
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 6.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |   |   |   |   |   |<----------|',
            '       +---+---+---+---+---+---+---+---+',
            '     1 |------x|   |   |   |   |<-x|   |',
            '       +---+---+---+---+---+---+---+---+',
            '             |                   +-- Signal2',
            '             +-- Signal1',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            ' B     +---+---+---+---+---+---+---+---+',
            ' y   3 |--------------x|   |   |   |   |',
            ' t     +---+---+---+---+---+---+---+---+',
            ' e   4 |-------------------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '     5 |   |   |<----------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '                 +-- Signal3',
            '       +---+---+---+---+---+---+---+---+',
            '     6 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '     7 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message6'))
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 7.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            ' B    7 |-------------------------------|',
            ' y      +---+---+---+---+---+---+---+---+',
            ' t    8 |-------------------------------|',
            ' e      +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '          +-- Signal1'
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message7'))
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 8.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            ' B    7 |-------------------------------|',
            ' y      +---+---+---+---+---+---+---+---+',
            ' t    8 |-------------------------------|',
            ' e      +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+',
            '                                      +-- Signal1'
        ]

        actual = formatting.layout_string(db.get_message_by_name('Message8'))
        self.assertEqual(actual, '\n'.join(expected_lines))

    def test_message_layout_without_signal_names(self):
        db = cantools.database.load_file('tests/files/kcd/message_layout.kcd',
                                         strict=False)

        # Message 1.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B',
            ' y',
            ' t',
            ' e'
        ]

        message = db.get_message_by_name('Message1')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 2.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |<-----------------------------x|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t',
            ' e'
        ]

        message = db.get_message_by_name('Message2')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 3.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |   |   |   |   |   |   |   |<--|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |--x|   |   |   |   |   |   |   |',
            ' e     +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message3')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 4.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |   |   |   |   |   |   |   |<--|',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |--x|   |   |   |   |   |   |   |',
            ' e     +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message4')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 5.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            ' B   0 |<---XXXXXXX-------x|XXX|   |   |',
            ' y     +---+---+---+---+---+---+---+---+',
            ' t   1 |   |   |   |   |   |<-x|XXXXXXX|',
            ' e     +---+---+---+---+---+---+---+---+',
            '     2 |XXX---x|   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message5')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 6.
        expected_lines = [
            '                      Bit',
            '',
            '         7   6   5   4   3   2   1   0',
            '       +---+---+---+---+---+---+---+---+',
            '     0 |   |   |   |   |   |<----------|',
            '       +---+---+---+---+---+---+---+---+',
            '     1 |------x|   |   |   |   |<-x|   |',
            '       +---+---+---+---+---+---+---+---+',
            '     2 |   |   |   |   |   |   |   |   |',
            ' B     +---+---+---+---+---+---+---+---+',
            ' y   3 |--------------x|   |   |   |   |',
            ' t     +---+---+---+---+---+---+---+---+',
            ' e   4 |-------------------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '     5 |   |   |<----------------------|',
            '       +---+---+---+---+---+---+---+---+',
            '     6 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+',
            '     7 |   |   |   |   |   |   |   |   |',
            '       +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message6')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 7.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            ' B      +---+---+---+---+---+---+---+---+',
            ' y    7 |-------------------------------|',
            ' t      +---+---+---+---+---+---+---+---+',
            ' e    8 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message7')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

        # Message 8.
        expected_lines = [
            '                       Bit',
            '',
            '          7   6   5   4   3   2   1   0',
            '        +---+---+---+---+---+---+---+---+',
            '      0 |<------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      1 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      2 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      3 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      4 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      5 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      6 |-------------------------------|',
            ' B      +---+---+---+---+---+---+---+---+',
            ' y    7 |-------------------------------|',
            ' t      +---+---+---+---+---+---+---+---+',
            ' e    8 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '      9 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     10 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     11 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     12 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     13 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     14 |-------------------------------|',
            '        +---+---+---+---+---+---+---+---+',
            '     15 |------------------------------x|',
            '        +---+---+---+---+---+---+---+---+'
        ]

        message = db.get_message_by_name('Message8')
        actual = formatting.layout_string(message, signal_names=False)
        self.assertEqual(actual, '\n'.join(expected_lines))

    def test_issue_63(self):
        """Test issue 63.

        """

        filename = 'tests/files/dbc/issue_63.dbc'

        db = cantools.database.load_file(filename, strict=False)
        self.assertEqual(
            formatting.layout_string(db.messages[0]),
            '                      Bit\n'
            '\n'
            '         7   6   5   4   3   2   1   0\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     0 |   |   |<-----x|<-------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '                 |       +-- DetectionStatus\n'
            '                 +-- PwrSupply\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     1 |<-----------------------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '         +-- RegenFailedCount\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     2 |------------------------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            ' B   3 |<------------------------------|\n'
            ' y     +---+---+---+---+---+---+---+---+\n'
            ' t       +-- Temp\n'
            ' e     +---+---+---+---+---+---+---+---+\n'
            '     4 |------------------------------x|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     5 |XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '         +-- MaxRes\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     6 |<------------------------------|\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '         +-- HtrRes\n'
            '       +---+---+---+---+---+---+---+---+\n'
            '     7 |   |   |   |   |   |   |   |   |\n'
            '       +---+---+---+---+---+---+---+---+')


# This file is not '__main__' when executed via 'python setup.py3
# test'.
logging.basicConfig(level=logging.DEBUG)

if __name__ == '__main__':
    unittest.main()
