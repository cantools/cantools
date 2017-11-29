from __future__ import print_function
import unittest
import timeit
import sys
from bitstruct import *
import bitstruct

class BitStructTest(unittest.TestCase):

    def test_pack(self):
        """Pack values.

        """

        packed = pack('u1u1s6u7u9', 0, 0, -2, 65, 22)
        self.assertEqual(packed, b'\x3e\x82\x16')

        packed = pack('u1', 1)
        self.assertEqual(packed, b'\x80')

        packed = pack('u77', 0x100000000001000000)
        ref = b'\x00\x80\x00\x00\x00\x00\x08\x00\x00\x00'
        self.assertEqual(packed, ref)

        packed = pack('u8000', int(8000 * '1', 2))
        ref = 1000 * b'\xff'
        self.assertEqual(packed, ref)

        packed = pack('s4000', int(8000 * '0', 2))
        ref = 500 * b'\x00'
        self.assertEqual(packed, ref)

        packed = pack('p1u1s6u7u9', 0, -2, 65, 22)
        self.assertEqual(packed, b'\x3e\x82\x16')

        packed = pack('p1u1s6p7u9', 0, -2, 22)
        self.assertEqual(packed, b'\x3e\x00\x16')

        packed = pack('u1s6f32r43', 0, -2, 3.75, b'\x00\xff\x00\xff\x00\xff')
        self.assertEqual(packed, b'\x7c\x80\xe0\x00\x00\x01\xfe\x01\xfe\x01\xc0')

        packed = pack('b1', True)
        self.assertEqual(packed, b'\x80')

        packed = pack('b1p6b1', True, True)
        self.assertEqual(packed, b'\x81')

        packed = pack('u5b2u1', -1, False, 1)
        self.assertEqual(packed, b'\xf9')

        packed = pack('b1t24', False, u"Hi!")
        self.assertEqual(packed, b'$4\x90\x80')

        packed = pack('b1t24', False, "Hi!")
        self.assertEqual(packed, b'$4\x90\x80')

        packed = pack('t8000', 1000 * "7")
        self.assertEqual(packed, 1000 * b'\x37')

        # Too many values to pack.
        try:
            pack('b1t24', False)
            self.fail()
        except ValueError as e:
            self.assertEqual(
                str(e),
                'pack expected 2 item(s) for packing (got 1)')

        # Cannot convert argument to integer.
        try:
            pack('u1', "foo")
            self.fail()
        except ValueError as e:
            self.assertEqual(
                str(e),
                "invalid literal for int() with base 10: 'foo'")

        # Cannot convert argument to float.
        try:
            pack('f32', "foo")
            self.fail()
        except ValueError as e:
            if sys.version_info[0] < 3:
                self.assertEqual(
                    str(e),
                    'could not convert string to float: foo')
            else:
                self.assertEqual(
                    str(e),
                    "could not convert string to float: 'foo'")

        # Cannot convert argument to bytearray.
        try:
            pack('r5', 1.0)
            self.fail()
        except TypeError as e:
            self.assertEqual(
                str(e),
                "'float' object is not iterable")

        # Cannot encode argument as utf-8.
        try:
            pack('t8', 1.0)
            self.fail()
        except AttributeError as e:
            self.assertEqual(
                str(e),
                "'float' object has no attribute 'encode'")

    def test_unpack(self):
        """Unpack values.

        """

        unpacked = unpack('u1u1s6u7u9', b'\x3e\x82\x16')
        self.assertEqual(unpacked, (0, 0, -2, 65, 22))

        unpacked = unpack('u1', bytearray(b'\x80'))
        self.assertEqual(unpacked, (1, ))

        packed = b'\x00\x80\x00\x00\x00\x00\x08\x00\x00\x00'
        unpacked = unpack('u77', packed)
        self.assertEqual(unpacked, (0x100000000001000000,))

        packed = 1000 * b'\xff'
        unpacked = unpack('u8000', packed)
        self.assertEqual(unpacked, (int(8000 * '1', 2), ))

        packed = 500 * b'\x00'
        unpacked = unpack('s4000', packed)
        self.assertEqual(unpacked, (0, ))

        packed = b'\x3e\x82\x16'
        unpacked = unpack('p1u1s6u7u9', packed)
        self.assertEqual(unpacked, (0, -2, 65, 22))

        packed = b'\x3e\x82\x16'
        unpacked = unpack('p1u1s6p7u9', packed)
        self.assertEqual(unpacked, (0, -2, 22))

        packed = b'\x7c\x80\xe0\x00\x00\x01\xfe\x01\xfe\x01\xc0'
        unpacked = unpack('u1s6f32r43', packed)
        self.assertEqual(unpacked, (0, -2, 3.75, b'\x00\xff\x00\xff\x00\xe0'))

        packed = [0x80]
        unpacked = unpack('b1', packed)
        self.assertEqual(unpacked, (True, ))

        packed = b'\x80'
        unpacked = unpack('b1p6b1', packed)
        self.assertEqual(unpacked, (True, False))

        packed = b'\x06'
        unpacked = unpack('u5b2u1', packed)
        self.assertEqual(unpacked, (0, True, 0))

        packed = b'\x04'
        unpacked = unpack('u5b2u1', packed)
        self.assertEqual(unpacked, (0, True, 0))

        packed = b'$4\x90\x80'
        unpacked = unpack('b1t24', packed)
        self.assertEqual(unpacked, (False, u"Hi!"))

        packed = 1000 * b"7"
        unpacked = unpack('t8000', packed)
        self.assertEqual(packed, 1000 * b'\x37')

        # Bad float size.
        try:
            unpack('f33', b'\x00\x00\x00\x00\x00')
            self.fail()
        except ValueError as e:
            self.assertEqual(
                str(e),
                'expected float size of 16, 32, or 64 bits (got 33)')

        # Too many bits to unpack.
        try:
            unpack('u9', b'\x00')
            self.fail()
        except ValueError as e:
            self.assertEqual(
                str(e),
                'unpack requires at least 9 bits to unpack (got 8)')

        # gcc packed struct with bitfields
        #
        # struct foo_t {
        #     int a;
        #     char b;
        #     uint32_t c : 7;
        #     uint32_t d : 25;
        # } foo;
        #
        # foo.a = 1;
        # foo.b = 1;
        # foo.c = 0x67;
        # foo.d = 0x12345;
        unpacked = unpack('s32s8u25u7',
                          byteswap('414',
                                   b'\x01\x00\x00\x00\x01\xe7\xa2\x91\x00'))
        self.assertEqual(unpacked, (1, 1, 0x12345, 0x67))

    def test_pack_unpack(self):
        """Pack and unpack values.

        """

        packed = pack('u1u1s6u7u9', 0, 0, -2, 65, 22)
        unpacked = unpack('u1u1s6u7u9', packed)
        self.assertEqual(unpacked, (0, 0, -2, 65, 22))

        packed = pack('f64', 1.0)
        unpacked = unpack('f64', packed)
        self.assertEqual(unpacked, (1.0, ))

        if sys.version_info >= (3, 6):
            packed = pack('f16', 1.0)
            unpacked = unpack('f16', packed)
            self.assertEqual(unpacked, (1.0, ))

    def test_calcsize(self):
        """Calculate size.

        """

        size = calcsize('u1u1s6u7u9')
        self.assertEqual(size, 24)

        size = calcsize('u1')
        self.assertEqual(size, 1)

        size = calcsize('u77')
        self.assertEqual(size, 77)

        size = calcsize('u1s6u7u9')
        self.assertEqual(size, 23)

        size = calcsize('b1s6u7u9p1t8')
        self.assertEqual(size, 32)

    def test_byteswap(self):
        """Byte swap.

        """

        res = b'\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a'
        ref = b'\x01\x03\x02\x04\x08\x07\x06\x05\x0a\x09'
        self.assertEqual(byteswap('12142', ref), res)

        packed = pack('u1u5u2u16', 1, 2, 3, 4)
        unpacked = unpack('u1u5u2u16', byteswap('12', packed))
        self.assertEqual(unpacked, (1, 2, 3, 1024))

    def test_endianness(self):
        """Test pack/unpack with endianness information in the format string.

        """

        # big endian
        ref = b'\x02\x46\x9a\xfe\x00\x00\x00'
        packed = pack('>u19s3f32', 0x1234, -2, -1.0)
        self.assertEqual(packed, ref)
        unpacked = unpack('>u19s3f32', packed)
        self.assertEqual(unpacked, (0x1234, -2, -1.0))

        # little endian
        ref = b'\x2c\x48\x0c\x00\x00\x07\xf4'
        packed = pack('<u19s3f32', 0x1234, -2, -1.0)
        self.assertEqual(packed, ref)
        unpacked = unpack('<u19s3f32', packed)
        self.assertEqual(unpacked, (0x1234, -2, -1.0))

        # mixed endianness
        ref = b'\x00\x00\x2f\x3f\xf0\x00\x00\x00\x00\x00\x00\x80'
        packed = pack('>u19<s5>f64r3p4', 1, -2, 1.0, b'\x80')
        self.assertEqual(packed, ref)
        unpacked = unpack('>u19<s5>f64r3p4', packed)
        self.assertEqual(unpacked, (1, -2, 1.0, b'\x80'))

        # opposite endianness of the 'mixed endianness' test
        ref = b'\x80\x00\x1e\x00\x00\x00\x00\x00\x00\x0f\xfc\x20'
        packed = pack('<u19>s5<f64r3p4', 1, -2, 1.0, b'\x80')
        self.assertEqual(packed, ref)
        unpacked = unpack('<u19>s5<f64r3p4', packed)
        self.assertEqual(unpacked, (1, -2, 1.0, b'\x80'))

        # pack as big endian, unpack as little endian
        ref = b'\x40'
        packed = pack('u2', 1)
        self.assertEqual(packed, ref)
        unpacked = unpack('<u2', packed)
        self.assertEqual(unpacked, (2, ))

    def test_byte_order(self):
        """Test pack/unpack with byte order information in the format string.

        """

        # most significant byte first (default)
        ref = b'\x02\x46\x9a\xfe\x00\x00\x00'
        packed = pack('u19s3f32>', 0x1234, -2, -1.0)
        self.assertEqual(packed, ref)
        unpacked = unpack('u19s3f32>', packed)
        self.assertEqual(unpacked, (0x1234, -2, -1.0))

        # least significant byte first
        ref = b'\x34\x12\x18\x00\x00\xe0\xbc'
        packed = pack('u19s3f32<', 0x1234, -2, -1.0)
        self.assertEqual(packed, ref)
        unpacked = unpack('u19s3f32<', packed)
        self.assertEqual(unpacked, (0x1234, -2, -1.0))

        # least significant byte first
        ref = b'\x34\x12'
        packed = pack('u8s8<', 0x34, 0x12)
        self.assertEqual(packed, ref)
        unpacked = unpack('u8s8<', packed)
        self.assertEqual(unpacked, (0x34, 0x12))

        # least significant byte first
        ref = b'\x34\x22'
        packed = pack('u3u12<', 1, 0x234)
        self.assertEqual(packed, ref)
        unpacked = unpack('u3s12<', packed)
        self.assertEqual(unpacked, (1, 0x234))

        # least significant byte first
        ref = b'\x34\x11\x00'
        packed = pack('u3u17<', 1, 0x234)
        self.assertEqual(packed, ref)
        unpacked = unpack('u3s17<', packed)
        self.assertEqual(unpacked, (1, 0x234))

        # least significant byte first
        ref = b'\x80'
        packed = pack('u1<', 1)
        self.assertEqual(packed, ref)
        unpacked = unpack('u1<', packed)
        self.assertEqual(unpacked, (1, ))

        # least significant byte first
        ref = b'\x45\x23\x25\x82'
        packed = pack('u19u5u1u7<', 0x12345, 5, 1, 2)
        self.assertEqual(packed, ref)
        unpacked = unpack('u19u5u1u7<', packed)
        self.assertEqual(unpacked, (0x12345, 5, 1, 2))

    def test_performance_mixed_types(self):
        """Test pack/unpack performance with mixed types.

        """

        print()

        time = timeit.timeit("pack('s6u7r40b1t152', "
                             "-2, 22, b'\x01\x01\x03\x04\x05', "
                             "True, u'foo fie bar gom gum')",
                             setup="from bitstruct import pack",
                             number=50000)
        print("pack time: {} s ({} s/pack)".format(time, time / 50000))

        time = timeit.timeit(
            "fmt.pack(-2, 22, b'\x01\x01\x03\x04\x05', "
            "True, u'foo fie bar gom gum')",
            setup="import bitstruct ; fmt = bitstruct.compile('s6u7r40b1t152')",
            number=50000)
        print("pack time compiled: {} s ({} s/pack)".format(time, time / 50000))

        time = timeit.timeit("unpack('s6u7r40b1t152', "
                             "b'\\xf8\\xb0\\x08\\x08\\x18 "
                             "-\\x99\\xbd\\xbc\\x81\\x99"
                             "\\xa5\\x94\\x81\\x89\\x85"
                             "\\xc8\\x81\\x9d\\xbd\\xb4"
                             "\\x81\\x9d\\xd5\\xb4')",
                             setup="from bitstruct import unpack",
                             number=50000)
        print("unpack time: {} s ({} s/unpack)".format(time, time / 50000))

        time = timeit.timeit(
            "fmt.unpack(b'\\xf8\\xb0\\x08\\x08\\x18 "
            "-\\x99\\xbd\\xbc\\x81\\x99"
            "\\xa5\\x94\\x81\\x89\\x85"
            "\\xc8\\x81\\x9d\\xbd\\xb4"
            "\\x81\\x9d\\xd5\\xb4')",
            setup="import bitstruct ; fmt = bitstruct.compile('s6u7r40b1t152')",
            number=50000)
        print("unpack time compiled: {} s ({} s/unpack)".format(time, time / 50000))

    def test_performance_integers(self):
        """Test pack/unpack performance with integers.

        """

        print()

        time = timeit.timeit("pack('s13u7u35u1s9', "
                             "-2, 22, 44567233, 0, 33)",
                             setup="from bitstruct import pack",
                             number=50000)
        print("pack time: {} s ({} s/pack)".format(time, time / 50000))

        time = timeit.timeit(
            "fmt.pack(-2, 22, 44567233, 0, 33)",
            setup="import bitstruct ; fmt = bitstruct.compile('s13u7u35u1s9')",
            number=50000)
        print("pack time compiled: {} s ({} s/pack)".format(time, time / 50000))

        time = timeit.timeit("unpack('s13u7u35u1s9', "
                             "b'\\xff\\xf1`\\x05P\\x15\\x82\\x10\\x80')",
                             setup="from bitstruct import unpack",
                             number=50000)
        print("unpack time: {} s ({} s/unpack)".format(time, time / 50000))

        time = timeit.timeit(
            "fmt.unpack(b'\\xff\\xf1`\\x05P\\x15\\x82\\x10\\x80')",
            setup="import bitstruct ; fmt = bitstruct.compile('s13u7u35u1s9')",
            number=50000)
        print("unpack time compiled: {} s ({} s/unpack)".format(time, time / 50000))

    def test_compile(self):
        cf = bitstruct.compile('u1u1s6u7u9')

        packed = cf.pack(0, 0, -2, 65, 22)
        self.assertEqual(packed, b'\x3e\x82\x16')

        unpacked = cf.unpack(b'\x3e\x82\x16')
        self.assertEqual(unpacked, (0, 0, -2, 65, 22))

    def test_signed_integer(self):
        """Pack and unpack signed integer values.

        """

        datas = [
            ('s2', 0x01, b'\x40'),
            ('s3', 0x03, b'\x60'),
            ('s4', 0x07, b'\x70'),
            ('s5', 0x0f, b'\x78'),
            ('s6', 0x1f, b'\x7c'),
            ('s7', 0x3f, b'\x7e'),
            ('s8', 0x7f, b'\x7f'),
            ('s9', 0xff, b'\x7f\x80'),
            ('s1',   -1, b'\x80'),
            ('s2',   -1, b'\xc0')
        ]

        for fmt, value, packed in datas:
            self.assertEqual(pack(fmt, value), packed)
            self.assertEqual(unpack(fmt, packed), (value, ))

    def test_unsigned_integer(self):
        """Pack and unpack unsigned integer values.

        """

        datas = [
            ('u1', 0x001, b'\x80'),
            ('u2', 0x003, b'\xc0'),
            ('u3', 0x007, b'\xe0'),
            ('u4', 0x00f, b'\xf0'),
            ('u5', 0x01f, b'\xf8'),
            ('u6', 0x03f, b'\xfc'),
            ('u7', 0x07f, b'\xfe'),
            ('u8', 0x0ff, b'\xff'),
            ('u9', 0x1ff, b'\xff\x80')
        ]

        for fmt, value, packed in datas:
            self.assertEqual(pack(fmt, value), packed)
            self.assertEqual(unpack(fmt, packed), (value, ))

    def test_bad_float_size(self):
        """Test of bad float size.

        """

        with self.assertRaises(ValueError) as cm:
            pack('f31', 1.0)

        self.assertEqual(str(cm.exception),
                         'expected float size of 16, 32, or 64 bits (got 31)')

        with self.assertRaises(ValueError) as cm:
            unpack('f33', 8 * b'\x00')

        self.assertEqual(str(cm.exception),
                         'expected float size of 16, 32, or 64 bits (got 33)')

    def test_bad_format_type(self):
        """Test of bad format type.

        """

        cf = bitstruct.compile('g1')

        with self.assertRaises(ValueError) as cm:
            cf.pack(1.0)

        self.assertEqual(str(cm.exception), "bad type 'g' in format")

        with self.assertRaises(ValueError) as cm:
            cf.unpack(b'\x00')

        self.assertEqual(str(cm.exception), "bad type 'g' in format")


if __name__ == '__main__':
    unittest.main()
