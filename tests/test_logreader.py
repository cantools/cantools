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

    def test_pcan_traceV10(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("1) 1841 0001 8 F0 00 00 00 00 00 00 00")
        self.assertEqual(outp.channel, 'pcanx')
        self.assertEqual(outp.frame_id, 0x0001)
        self.assertEqual(outp.data, b'\xF0\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 1)
        self.assertEqual(outp.timestamp.microseconds, 841000)

    def test_pcan_traceV11(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("1)      6357.2 Rx        0401  8    F0 00 00 00 00 00 00 00")
        self.assertEqual(outp.channel, 'pcanx')
        self.assertEqual(outp.frame_id, 0x0401)
        self.assertEqual(outp.data, b'\xF0\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 6)
        self.assertEqual(outp.timestamp.microseconds, 357200)

    def test_pcan_traceV12(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("1)      6357.213 1  Rx        0401  8    F0 00 00 00 00 00 00 00")
        self.assertEqual(outp.channel, 'pcan1')
        self.assertEqual(outp.frame_id, 0x0401)
        self.assertEqual(outp.data, b'\xF0\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 6)
        self.assertEqual(outp.timestamp.microseconds, 357213)

    def test_pcan_traceV13(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse("1)      6357.213 1  Rx        0401 -  8    F0 00 00 00 00 00 00 00")
        self.assertEqual(outp.channel, 'pcan1')
        self.assertEqual(outp.frame_id, 0x0401)
        self.assertEqual(outp.data, b'\xF0\x00\x00\x00\x00\x00\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 6)
        self.assertEqual(outp.timestamp.microseconds, 357213)

    def test_pcan_traceV20(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse(" 1      1059.900 DT 0300 Rx 7 00 00 00 00 04 00 00")
        self.assertEqual(outp.channel, 'pcanx')
        self.assertEqual(outp.frame_id, 0x0300)
        self.assertEqual(outp.data, b'\x00\x00\x00\x00\x04\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 1)
        self.assertEqual(outp.timestamp.microseconds, 59900)

    def test_pcan_traceV21(self):
        parser = cantools.logreader.Parser()

        outp = parser.parse(" 1      1059.900 DT 1 0300 Rx - 7 00 00 00 00 04 00 00")
        self.assertEqual(outp.channel, 'pcan1')
        self.assertEqual(outp.frame_id, 0x0300)
        self.assertEqual(outp.data, b'\x00\x00\x00\x00\x04\x00\x00')
        self.assertEqual(outp.timestamp_format, cantools.logreader.TimestampFormat.RELATIVE)
        self.assertEqual(outp.timestamp.seconds, 1)
        self.assertEqual(outp.timestamp.microseconds, 59900)

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

    def test_pcan_traceV10(self):
        testvec = io.StringIO("""\
;##########################################################################
; C:\\TraceFile.trc
;
; CAN activities recorded by PCAN Explorer
; Start time: 11.09.2002 16:00:20.682
; PCAN-Net: PCI1
;
; Columns description:
; ~~~~~~~~~~~~~~~~~~~~~
; +-current number in actual sample
; | +time offset of message (ms)
; | | +ID of message (hex)
; | | | +data length code
; | | | | +data bytes (hex) ...
; | | | | |
;----+- ---+--- ----+--- + -+ -- -- ...
1) 1841 0001 8 00 00 00 00 00 00 00 00
2) 1842 0008 4 ERROR 00 19 08 08
3) 1843 FFFFFFFF 4 00 00 00 04 -- -- -- -- BUSLIGHT
4) 1844 0100 3 RTR
5) 1845 0300 8 00 00 00 00 00 00 00 00
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0x001)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x300)

    def test_pcan_traceV11(self):
        testvec = io.StringIO("""\
;$FILEVERSION=1.1
;$STARTTIME=37704.5364870833
;
; C:\\TraceFile.trc
;
; Start time: 24.03.2003 12:52:32.484
; PCAN-Net: TestNet
;
; Columns description:
; ~~~~~~~~~~~~~~~~~~~~~
; +-Message Number
; | +Time Offset (ms)
; | | +Type
; | | | +ID (hex)
; | | | | +Data Length Code
; | | | | | +Data Bytes (hex) ...
; | | | | | |
;---+-- ----+---- --+-- ----+--- + -+ -- -- -- -- -- -- --
1) 1059.9 Rx 0300 7 00 00 00 00 04 00 00
2) 1283.2 Rx 0300 7 00 00 00 00 04 00 00
3) 1298.9 Tx 0400 2 00 00
4) 1323.0 Rx 0300 7 00 00 00 00 06 00 00
5) 1346.8 Warng FFFFFFFF 4 00 00 00 04 BUSLIGHT
6) 1349.2 Error 0008 4 00 19 08 08
7) 1352.7 Rx 0100 3 RTR
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0x300)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x300)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x400)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x300)
        f5 = next(frame_iter)
        self.assertEqual(f5.frame_id, 0x008)

    def test_pcan_traceV12(self):
        testvec = io.StringIO("""\
;$FILEVERSION=1.2
;$STARTTIME=39878.6772258947;
; C:\\TraceFile.trc
;
; Start time: 06.03.2009 16:15:12.317.3
; Connection: TestNet
;
; Columns description:
; ~~~~~~~~~~~~~~~~~~~~~
; +-Message Number
; | +Time Offset (ms)
; | | +Bus
; | | | +Type
; | | | | +ID (hex)
; | | | | | +Data Length Code
; | | | | | | +Data Bytes (hex) ...
; | | | | | | |
;---+-- ----+------ + --+-- ----+--- + -+ -- -- -- -- -- -- --
1) 1059.900 1 Rx 0300 7 00 00 00 00 04 00 00
2) 1283.231 1 Rx 0300 7 00 00 00 00 04 00 00
3) 1298.945 1 Tx 0400 2 00 00
4) 1323.201 1 Rx 0300 7 00 00 00 00 06 00 00
5) 1346.834 1 Warng FFFFFFFF 4 00 00 00 04 BUSLIGHT
6) 1349.222 1 Error 0008 4 00 19 08 08
7) 1352.743 1 Rx 0100 3 RTR
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0x300)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x300)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x400)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x300)
        f5 = next(frame_iter)
        self.assertEqual(f5.frame_id, 0x008)

    def test_pcan_traceV13(self):
        testvec = io.StringIO("""\
;$FILEVERSION=1.3
;$STARTTIME=44656.4982577909614
;
;   Start time: 05/04/2022 11:57:29.473.1
;   Generated by PCAN-Explorer v6.5.2.2576
;-------------------------------------------------------------------------------
;   Bus  Connection   Net Connection             Protocol  Bit rate
;   1    Connection1  PCANLight_USB_16@pcan_usb  CAN       250 kbit/s
;-------------------------------------------------------------------------------
;   Message   Time    Bus  Type   ID    Reserved
;   Number    Offset  |    |      [hex] |   Data Length Code
;   |         [ms]    |    |      |     |   |    Data [hex] ...
;   |         |       |    |      |     |   |    |
;---+-- ------+------ +- --+-- ---+---- +- -+-- -+ -- -- -- -- -- -- --
     1)      6357.213 1  Rx        0401 -  8    00 00 00 00 00 00 00 00
     2)      6357.682 1  Rx        04A1 -  8    00 00 00 00 00 00 00 00
     3)      1346.834 1  Warng FFFFFFFF -  4    00 00 00 04 BUSLIGHT
     4)      6377.180 1  Rx        0400 -  8    00 00 00 00 00 00 00 00
     5)      6377.692 1  Rx        04A0 -  8    00 00 00 00 00 00 00 00
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0x401)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x4A1)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x400)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x4A0)

    def test_pcan_traceV20(self):
        testvec = io.StringIO("""\
;$FILEVERSION=2.0
;$STARTTIME=42209.4075997106
;$COLUMNS=N,O,T,I,d,l,D
;
; C:\\TraceFile.trc
; Start time: 24.07.2015 09:46:56.615.0
; Generated by PCAN-View v4.0.29.426
;-------------------------------------------------------------------------------
; Connection Bit rate
; PCANLight_USB_16@pcan_usb Nominal 1 MBit/s, Data 2 Mbit/s
;-------------------------------------------------------------------------------
; Message Time Type ID Rx/Tx
; Number Offset | [hex] | Data Length
; | [ms] | | | | Data [hex] ...
; | | | | | | |
;---+-- ------+------ +- --+----- +- +- +- -- -- -- -- -- -- --
1 1059.900 DT 0300 Rx 7 00 00 00 00 04 00 00
2 1283.231 DT 0300 Rx 7 00 00 00 00 04 00 00
3 1298.945 DT 0400 Tx 2 00 00
4 1323.201 DT 0300 Rx 7 00 00 00 00 06 00 00
5 1334.416 FD 0500 Tx 12 01 02 03 04 05 06 07 08 09 0A 0B 0C
6 1334.522 ER Rx 04 00 02 00 00
7 1334.531 ST Rx 00 00 00 08
8 1334.643 EC Rx 02 02
9 1335.156 DT 18EFC034 Tx 8 01 02 03 04 05 06 07 08
10 1336.543 RR 0100 Rx 3
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0x300)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x300)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x400)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x300)
        f5 = next(frame_iter)
        self.assertEqual(f5.frame_id, 0x500)
        f6 = next(frame_iter)
        self.assertEqual(f6.frame_id, 0x18EFC034)

    def test_pcan_traceV21(self):
        testvec = io.StringIO("""\
;$FILEVERSION=2.1
;$STARTTIME=41766.4648963872
;$COLUMNS=N,O,T,B,I,d,R,L,D
;
; C:\\TraceFile.trc
; Start time: 07.05.2015 11:09:27.047.8
; Generated by PCAN-Explorer v6.0.0
;-------------------------------------------------------------------------------
; Bus Name Connection Protocol
; 1 Connection1 TestNet@pcan_usb CAN
;-------------------------------------------------------------------------------
; Message Time Type ID Rx/Tx
; Number Offset | Bus [hex] | Reserved
; | [ms] | | | | | Data Length Code
; | | | | | | | | Data [hex] ...
; | | | | | | | | |
;---+-- ------+------ +- +- --+----- +- +- +--- +- -- -- -- -- -- -- --
1 1059.900 DT 1 0300 Rx - 7 00 00 00 00 04 00 00
2 1283.231 DT 1 0300 Rx - 7 00 00 00 00 04 00 00
3 1298.945 DT 1 0400 Tx - 2 00 00
4 1323.201 DT 1 0300 Rx - 7 00 00 00 00 06 00 00
5 1334.416 FD 1 0500 Tx - 12 01 02 03 04 05 06 07 08 09 0A 0B 0C
6 1334.222 ER 1 - Rx - 5 04 00 02 00 00
7 1334.224 EV 1 User-defined event for bus 1
8 1334.225 EV - User-defined event for all busses
9 1334.231 ST 1 - Rx - 4 00 00 00 08
10 1334.268 ER 1 - Rx - 5 04 00 02 08 00
11 1334.643 EC 1 - Rx - 2 02 02
12 1335.156 DT 1 18EFC034 Tx - 8 01 02 03 04 05 06 07 08
13 1336.543 RR 1 0100 Rx - 3
""")
        parser = cantools.logreader.Parser(testvec)
        frame_iter = iter(parser)
        f1 = next(frame_iter)
        self.assertEqual(f1.frame_id, 0x300)
        f2 = next(frame_iter)
        self.assertEqual(f2.frame_id, 0x300)
        f3 = next(frame_iter)
        self.assertEqual(f3.frame_id, 0x400)
        f4 = next(frame_iter)
        self.assertEqual(f4.frame_id, 0x300)
        f5 = next(frame_iter)
        self.assertEqual(f5.frame_id, 0x500)
        f6 = next(frame_iter)
        self.assertEqual(f6.frame_id, 0x18EFC034)
