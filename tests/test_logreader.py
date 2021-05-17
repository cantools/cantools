import unittest
import io

import cantools


class TestLogreaderFormats(unittest.TestCase):
    def test_empty_line(self):
        parser = cantools.logreader.Parser()
        outp = parser.parse("")
        self.assertIsNone(outp)

    def test_candump(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("vcan0  0C8   [8]  F0 00 00 00 00 00 00 00")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0xc8)
        self.assertEqual(outp.data, b'\xF0\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.MISSING)

        outp = parser.parse("  vcan1  064   [10]  F0 01 FF FF FF FF FF FF FF FF")
        self.assertEqual(outp.channel, 'vcan1')
        self.assertEqual(outp.frame_id, 0x64)
        self.assertEqual(outp.data, b'\xF0\x01\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.MISSING)

        outp = parser.parse("  vcan0  ERROR")
        self.assertIsNone(outp)

        outp = parser.parse("  vcan0  1F4   [4]  01 02 03 04")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0x1f4)
        self.assertEqual(outp.data, b'\x01\x02\x03\x04')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.MISSING)

        outp = parser.parse("  vcan0  1F3   [3]  01 02 03")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0x1f3)
        self.assertEqual(outp.data, b'\x01\x02\x03')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.MISSING)

    def test_timestamped_candump(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("(000.000000)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0xc8)
        self.assertEqual(outp.data, b'\xF0\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 0.0)

        outp = parser.parse("(002.047817)  vcan0  064   [8]  F0 01 FF FF FF FF FF FF")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0x64)
        self.assertEqual(outp.data, b'\xF0\x01\xFF\xFF\xFF\xFF\xFF\xFF')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 2)
        self.assertEqual(outp.timestamp.microseconds, 47817)

        outp = parser.parse("(012.831664)  vcan0  1F4   [4]  01 02 03 04")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0x1f4)
        self.assertEqual(outp.data, b'\x01\x02\x03\x04')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 12)
        self.assertEqual(outp.timestamp.microseconds, 831664)

        outp = parser.parse("(015.679614)  vcan0  1F3   [3]  01 02 03")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0x1f3)
        self.assertEqual(outp.data, b'\x01\x02\x03')
        self.assertEqual(outp.timestamp.seconds, 15)
        self.assertEqual(outp.timestamp.microseconds, 679614)
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)

        outp = parser.parse("(1613749650.388103)  can1       0AD  [08]  A6 55 3B CF 3F 1A F5 2A")
        self.assertEqual(outp.channel, 'can1')
        self.assertEqual(outp.frame_id, 0xad)
        self.assertEqual(outp.data, b'\xA6\x55\x3B\xCF\x3F\x1A\xF5\x2A')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.ABSOLUTE)
        self.assertEqual(outp.timestamp.year, 2021)
        self.assertEqual(outp.timestamp.month, 2)
        self.assertEqual(outp.timestamp.day, 19)
        self.assertEqual(outp.timestamp.hour, 15)
        self.assertEqual(outp.timestamp.minute, 47)
        self.assertEqual(outp.timestamp.second, 30)
        self.assertEqual(outp.timestamp.microsecond, 388103)

    def test_candump_log(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("(1579857014.345944) can2 486#82967A6B006B07F8")
        self.assertEqual(outp.channel, 'can2')
        self.assertEqual(outp.frame_id, 0x486)
        self.assertEqual(outp.data, b'\x82\x96\x7A\x6B\x00\x6B\x07\xF8')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.ABSOLUTE)
        self.assertEqual(outp.timestamp.year, 2020)
        self.assertEqual(outp.timestamp.month, 1)
        self.assertEqual(outp.timestamp.day, 24)
        self.assertEqual(outp.timestamp.hour, 9)
        self.assertEqual(outp.timestamp.minute, 10)
        self.assertEqual(outp.timestamp.second, 14)
        self.assertEqual(outp.timestamp.microsecond, 345944)

        outp = parser.parse(
            "(1613656104.501098) can3 14C##155B53476F7B82EEEB8E97236AC252B8BBB5B80A6A7734B2F675C6D2CEEC869D3")
        self.assertEqual(outp.channel, 'can3')
        self.assertEqual(outp.frame_id, 0x14c)
        self.assertEqual(
            outp.data, b'\x55\xB5\x34\x76\xF7\xB8\x2E\xEE\xB8\xE9\x72\x36\xAC\x25\x2B\x8B\xBB\x5B\x80\xA6\xA7\x73\x4B\x2F\x67\x5C\x6D\x2C\xEE\xC8\x69\xD3')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.ABSOLUTE)
        self.assertEqual(outp.timestamp.year, 2021)
        self.assertEqual(outp.timestamp.month, 2)
        self.assertEqual(outp.timestamp.day, 18)
        self.assertEqual(outp.timestamp.hour, 13)
        self.assertEqual(outp.timestamp.minute, 48)
        self.assertEqual(outp.timestamp.second, 24)
        self.assertEqual(outp.timestamp.microsecond, 501098)

    def test_candump_log_absolute_timestamp(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("(2020-12-19 12:04:45.485261)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00")
        self.assertEqual(outp.channel, 'vcan0')
        self.assertEqual(outp.frame_id, 0xc8)
        self.assertEqual(outp.data, b'\xF0\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.ABSOLUTE)
        self.assertEqual(outp.timestamp.year, 2020)
        self.assertEqual(outp.timestamp.month, 12)
        self.assertEqual(outp.timestamp.day, 19)
        self.assertEqual(outp.timestamp.hour, 12)
        self.assertEqual(outp.timestamp.minute, 4)
        self.assertEqual(outp.timestamp.second, 45)
        self.assertEqual(outp.timestamp.microsecond, 485261)

    def test_candump_log_ascii(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse(" can1  123   [8]  31 30 30 2E 35 20 46 4D   '100.5 FM'")
        self.assertEqual(outp.channel, 'can1')
        self.assertEqual(outp.frame_id, 0x123)
        self.assertEqual(outp.data, b'\x31\x30\x30\x2E\x35\x20\x46\x4D')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.MISSING)

    def test_candump_log_ascii_timestamped(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("  (1621271100.919019)  can1  123   [8]  31 30 30 2E 35 20 46 4D   '100.5 FM'")
        self.assertEqual(outp.channel, 'can1')
        self.assertEqual(outp.frame_id, 0x123)
        self.assertEqual(outp.data, b'\x31\x30\x30\x2E\x35\x20\x46\x4D')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.ABSOLUTE)

    def test_candump_log_ascii_absolute(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("(2020-12-19 12:04:45.485261)  can1  123   [8]  31 30 30 2E 35 20 46 4D   '100.5 FM'")
        self.assertEqual(outp.channel, 'can1')
        self.assertEqual(outp.frame_id, 0x123)
        self.assertEqual(outp.data, b'\x31\x30\x30\x2E\x35\x20\x46\x4D')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.ABSOLUTE)
        self.assertEqual(outp.timestamp.year, 2020)
        self.assertEqual(outp.timestamp.month, 12)
        self.assertEqual(outp.timestamp.day, 19)
        self.assertEqual(outp.timestamp.hour, 12)
        self.assertEqual(outp.timestamp.minute, 4)
        self.assertEqual(outp.timestamp.second, 45)
        self.assertEqual(outp.timestamp.microsecond, 485261)


class TestLogreaderStreams(unittest.TestCase):
    def test_candump(self):
        testvec = io.StringIO("""\
  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
  vcan0  064   [10]  F0 01 FF FF FF FF FF FF FF FF
  vcan0  ERROR

  vcan0  1F4   [4]  01 02 03 04
  vcan0  1F3   [3]  01 02 03
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0xc8)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x64)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x1f4)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x1f3)

    def test_candump_absolute_timestamp(self):
        testvec = io.StringIO("""\
 (2020-12-19 12:04:45.485261)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
 (2020-12-19 12:04:48.597222)  vcan0  064   [8]  F0 01 FF FF FF FF FF FF
 (2020-12-19 12:04:56.805087)  vcan0  1F4   [4]  01 02 03 04
 (2020-12-19 12:04:59.085517)  vcan0  1F3   [3]  01 02 03
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0xc8)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x64)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x1f4)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x1f3)

    def test_candump_time_since_start(self):
        testvec = io.StringIO("""\
 (000.000000)  vcan0  0C8   [8]  F0 00 00 00 00 00 00 00
 (002.047817)  vcan0  064   [8]  F0 01 FF FF FF FF FF FF
 (012.831664)  vcan0  1F4   [4]  01 02 03 04
 (015.679614)  vcan0  1F3   [3]  01 02 03
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0xc8)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x64)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x1f4)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x1f3)

    def test_candump_log_fd_absolute_time(self):
        testvec = io.StringIO("""\
  (1613656104.493702) can2 102##1150B7F0102010010000064A0020000100000000000E41F000000000090D1FF000020A600000000210100000000000000
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0x102)

    def test_candump_log(self):
        testvec = io.StringIO("""\
(1594172461.968006) vcan0 0C8#F000000000000000
(1594172462.126542) vcan0 064#F001FFFFFFFFFFFFFFFF
(1594172462.127684) vcan0 ERROR

(1594172462.356874) vcan0 1F4#01020304
(1594172462.688432) vcan0 1F3#010203
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0xc8)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x64)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x1f4)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x1f3)
