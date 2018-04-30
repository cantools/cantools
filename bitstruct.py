from __future__ import print_function

import re
import struct
from io import BytesIO
import binascii


__version__ = "3.8.0"


class Error(Exception):
    pass


def _parse_format(fmt):
    if fmt and fmt[-1] in '><':
        byte_order = fmt[-1]
        fmt = fmt[:-1]
    else:
        byte_order = ''

    parsed_infos = re.findall(r'([<>]?)([a-zA-Z])(\d+)', fmt)

    if ''.join([''.join(info) for info in parsed_infos]) != fmt:
        raise Error("bad format '{}'".format(fmt + byte_order))

    # Use big endian as default and use the endianness of the previous
    # value if none is given for the current value.
    infos = []
    endianness = ">"

    for info in parsed_infos:
        if info[0] != "":
            endianness = info[0]

        if info[1] not in 'supPfbtr':
            raise Error("bad char '{}' in format".format(info[1]))

        infos.append((info[1], int(info[2]), endianness))

    return infos, byte_order or '>'


def _pack_integer(size, arg):
    value = int(arg)

    if value < 0:
        value += (1 << size)

    value += (1 << size)

    return bin(value)[3:]


def _pack_boolean(size, arg):
    value = bool(arg)

    return _pack_integer(size, int(value))


def _pack_float(size, arg):
    value = float(arg)

    if size == 16:
        value = struct.pack('>e', value)
    elif size == 32:
        value = struct.pack('>f', value)
    elif size == 64:
        value = struct.pack('>d', value)
    else:
        raise Error('expected float size of 16, 32, or 64 bits (got {})'.format(
            size))

    return bin(int(b'01' + binascii.hexlify(value), 16))[3:]


def _pack_bytearray(size, arg):
    return bin(int(b'01' + binascii.hexlify(arg), 16))[3:size + 3]


def _pack_text(size, arg):
    value = arg.encode('utf-8')

    return _pack_bytearray(size, bytearray(value))


def _unpack_signed_integer(bits):
    value = int(bits, 2)

    if bits[0] == '1':
        value -= (1 << len(bits))

    return value


def _unpack_unsigned_integer(bits):
    return int(bits, 2)


def _unpack_boolean(bits):
    return bool(int(bits, 2))


def _unpack_float(size, bits):
    packed = _unpack_bytearray(size, bits)

    if size == 16:
        value = struct.unpack('>e', packed)[0]
    elif size == 32:
        value = struct.unpack('>f', packed)[0]
    elif size == 64:
        value = struct.unpack('>d', packed)[0]
    else:
        raise Error('expected float size of 16, 32, or 64 bits (got {})'.format(
            size))

    return value


def _unpack_bytearray(size, bits):
    rest = size % 8

    if rest > 0:
        bits += (8 - rest) * '0'

    return binascii.unhexlify(hex(int('10000000' + bits, 2))[4:].rstrip('L'))


def _unpack_text(size, bits):
    return _unpack_bytearray(size, bits).decode('utf-8')


class CompiledFormat(object):
    """A compiled format string that can be used to pack and/or unpack
    data multiple times.

    Instances of this class are created by the factory function
    :func:`~bitstruct.compile()`.

    :param fmt: Bitstruct format string. See :func:`~bitstruct.pack()`
                for details.

    """

    def __init__(self, fmt):
        infos, byte_order = _parse_format(fmt)
        self._infos = infos
        self._byte_order = byte_order
        self._number_of_bits_to_unpack = sum([info[1] for info in infos])
        self._number_of_arguments = 0

        for info in infos:
            if info[0] not in 'pP':
                self._number_of_arguments += 1

    def _pack_value(self, type_, size, value, endianness, bits):
        if type_ == 's':
            value_bits = _pack_integer(size, value)
        elif type_ == 'u':
            value_bits = _pack_integer(size, value)
        elif type_ == 'f':
            value_bits = _pack_float(size, value)
        elif type_ == 'b':
            value_bits = _pack_boolean(size, value)
        elif type_ == 't':
            value_bits = _pack_text(size, value)
        elif type_ == 'r':
            value_bits = _pack_bytearray(size, bytearray(value))
        else:
            raise Error("bad type '{}' in format".format(type_))

        # reverse the bit order in little endian values
        if endianness == "<":
            value_bits = value_bits[::-1]

        # reverse bytes order for least significant byte first
        if self._byte_order == ">":
            bits += value_bits
        else:
            aligned_offset = len(value_bits) - (8 - (len(bits) % 8))

            while aligned_offset > 0:
                bits += value_bits[aligned_offset:]
                value_bits = value_bits[:aligned_offset]
                aligned_offset -= 8

            bits += value_bits

        return bits

    def pack(self, *args):
        """Return a byte string containing the values v1, v2, ... packed
        according to the compiled format string. If the total number
        of bits are not a multiple of 8, padding will be added at the
        end of the last byte.

        :param args: Variable argument list of values to pack.
        :returns: A byte string of the packed values.

        """

        bits = ''
        i = 0

        # Sanity check of the number of arguments.
        if len(args) < self._number_of_arguments:
            raise Error(
                "pack expected {} item(s) for packing (got {})".format(
                    self._number_of_arguments,
                    len(args)))

        for type_, size, endianness in self._infos:
            if type_ == 'p':
                bits += size * '0'
            elif type_ == 'P':
                bits += size * '1'
            else:
                bits = self._pack_value(type_, size, args[i], endianness, bits)
                i += 1

        # padding of last byte
        tail = len(bits) % 8

        if tail != 0:
            bits += (8 - tail) * '0'

        return bytes(_unpack_bytearray(len(bits), bits))

    def unpack(self, data):
        """Unpack `data` (byte string, bytearray or list of integers)
        according to the compiled format string. The result is a tuple
        even if it contains exactly one item.

        :param data: Byte string of values to unpack.
        :returns: A tuple of the unpacked values.

        """

        return self.unpack_from(data)

    def pack_into(self, buf, offset, *args, **kwargs):
        """Pack given values v1, v2, ... into `buf`, starting at given bit
        offset `offset`. Give `fill_padding` as ``False`` to leave
        padding bits in `buf` unmodified.

        :param buf: Buffer to pack data into.
        :param offset: Bit offset to start at.
        :param args: Variable argument list of values to pack.
        :param fill_padding: A boolean to control if padding should
                             overwrite bits in `buf`.

        """

        # Sanity check of the number of arguments.
        if len(args) < self._number_of_arguments:
            raise Error(
                "pack expected {} item(s) for packing (got {})".format(
                    self._number_of_arguments,
                    len(args)))

        fill_padding = kwargs.get('fill_padding', True)
        buf_bits = _pack_bytearray(8 * len(buf), buf)
        bits = buf_bits[0:offset]
        i = 0

        for type_, size, endianness in self._infos:
            if type_ in 'pP':
                if fill_padding:
                    if type_ == 'p':
                        bits += size * '0'
                    else:
                        bits += size * '1'
                else:
                    bits += buf_bits[len(bits):len(bits) + size]
            else:
                bits = self._pack_value(type_, size, args[i], endianness, bits)
                i += 1

        bits += buf_bits[len(bits):]

        if len(bits) > len(buf_bits):
            raise Error(
                'pack requires a buffer of at least {} bits'.format(
                    len(bits)))

        buf[:] = _unpack_bytearray(len(bits), bits)

    def unpack_from(self, data, offset=0):
        """Unpack `data` (byte string, bytearray or list of integers)
        according to given format string `fmt`, starting at given bit
        offset `offset`. The result is a tuple even if it contains
        exactly one item.

        :param data: Byte string of values to unpack.
        :param offset: Bit offset to start unpack from.
        :returns: A tuple of the unpacked values.

        """

        bits = bin(int(b'01' + binascii.hexlify(bytearray(data)), 16))[3 + offset:]

        # Sanity check.
        if self._number_of_bits_to_unpack > len(bits):
            raise Error(
                "unpack requires at least {} bits to unpack (got {})".format(
                    self._number_of_bits_to_unpack,
                    len(bits)))

        res = []
        offset = 0

        for type_, size, endianness in self._infos:
            if type_ in 'pP':
                pass
            else:
                # reverse bytes order for least significant byte first
                if self._byte_order == ">":
                    value_bits = bits[offset:offset + size]
                else:
                    value_bits_tmp = bits[offset:offset + size]
                    aligned_offset = (size - ((offset + size) % 8))
                    value_bits = ''

                    while aligned_offset > 0:
                        value_bits += value_bits_tmp[aligned_offset:aligned_offset + 8]
                        value_bits_tmp = value_bits_tmp[:aligned_offset]
                        aligned_offset -= 8

                    value_bits += value_bits_tmp

                # reverse the bit order in little endian values
                if endianness == "<":
                    value_bits = value_bits[::-1]

                if type_ == 's':
                    value = _unpack_signed_integer(value_bits)
                elif type_ == 'u':
                    value = _unpack_unsigned_integer(value_bits)
                elif type_ == 'f':
                    value = _unpack_float(size, value_bits)
                elif type_ == 'b':
                    value = _unpack_boolean(value_bits)
                elif type_ == 't':
                    value = _unpack_text(size, value_bits)
                elif type_ == 'r':
                    value = bytes(_unpack_bytearray(size, value_bits))
                else:
                    raise Error("bad type '{}' in format".format(type_))

                res.append(value)

            offset += size

        return tuple(res)

    def calcsize(self):
        """Calculate the number of bits in the compiled format string.

        :returns: Number of bits in the format string.

        """

        return sum([size for _, size, _ in self._infos])


def pack(fmt, *args):
    """Return a byte string containing the values v1, v2, ... packed
    according to given format string `fmt`. If the total number of
    bits are not a multiple of 8, padding will be added at the end of
    the last byte.

    :param fmt: Bitstruct format string. See format description below.
    :param args: Variable argument list of values to pack.
    :returns: A byte string of the packed values.

    `fmt` is a string of bitorder-type-length groups, and optionally a
    byteorder identifier after the groups. Bitorder and byteorder may
    be omitted.

    Bitorder is either ``>`` or ``<``, where ``>`` means MSB first and
    ``<`` means LSB first. If bitorder is omitted, the previous
    values' bitorder is used for the current value. For example, in
    the format string ``'u1<u2u3'``, ``u1`` is MSB first and both
    ``u2`` and ``u3`` are LSB first.

    Byteorder is either ``>`` or ``<``, where ``>`` means most
    significant byte first and ``<`` means least significant byte
    first. If byteorder is omitted, most significant byte first is
    used.

    There are seven types; ``u``, ``s``, ``f``, ``b``, ``t``, ``r``
    and ``p``.

    - ``u`` -- unsigned integer
    - ``s`` -- signed integer
    - ``f`` -- floating point number of 16, 32, or 64 bits
    - ``b`` -- boolean
    - ``t`` -- text (ascii or utf-8)
    - ``r`` -- raw, bytes
    - ``p`` -- padding with zeros, ignore
    - ``P`` -- padding with ones, ignore

    Length is the number of bits to pack the value into.

    Example format string with default bit and byte ordering:
    ``'u1u3p7s16'``

    Same format string, but with least significant byte first:
    ``'u1u3p7s16<'``

    Same format string, but with LSB first (``<`` prefix) and least
    significant byte first (``<`` suffix): ``'<u1u3p7s16<'``

    """

    return CompiledFormat(fmt).pack(*args)


def unpack(fmt, data):
    """Unpack `data` (byte string, bytearray or list of integers)
    according to given format string `fmt`. The result is a tuple even
    if it contains exactly one item.

    :param fmt: Bitstruct format string. See :func:`~bitstruct.pack()`
                for details.
    :param data: Byte string of values to unpack.
    :returns: A tuple of the unpacked values.

    """

    return CompiledFormat(fmt).unpack(data)


def pack_into(fmt, buf, offset, *args, **kwargs):
    """Pack given values v1, v2, ... into `buf`, starting at given bit
    offset `offset`. Pack according to given format string `fmt`. Give
    `fill_padding` as ``False`` to leave padding bits in `buf`
    unmodified.

    :param fmt: Bitstruct format string. See format description below.
    :param buf: Buffer to pack data into.
    :param offset: Bit offset to start at.
    :param args: Variable argument list of values to pack.
    :param fill_padding: A boolean to control if padding should
                         overwrite bits in `buf`.

    """

    return CompiledFormat(fmt).pack_into(buf,
                                         offset,
                                         *args,
                                         **kwargs)


def unpack_from(fmt, data, offset=0):
    """Unpack `data` (byte string, bytearray or list of integers)
    according to given format string `fmt`, starting at given bit
    offset `offset`. The result is a tuple even if it contains exactly
    one item.

    :param fmt: Bitstruct format string. See :func:`~bitstruct.pack()`
                for details.
    :param data: Byte string of values to unpack.
    :param offset: Bit offset to start unpack from.
    :returns: A tuple of the unpacked values.

    """

    return CompiledFormat(fmt).unpack_from(data, offset)


def calcsize(fmt):
    """Calculate the number of bits in given format string `fmt`.

    :param fmt: Bitstruct format string. See :func:`~bitstruct.pack()`
                for details.
    :returns: Number of bits in format string.

    """

    return CompiledFormat(fmt).calcsize()


def byteswap(fmt, data, offset=0):
    """Swap bytes in `data` according to `fmt`, starting at byte
    `offset`. `fmt` must be an iterable, iterating over number of
    bytes to swap. For example, the format string ``'24'`` applied to
    the byte string ``b'\\x00\\x11\\x22\\x33\\x44\\x55'`` will produce
    the result ``b'\\x11\\x00\\x55\\x44\\x33\\x22'``.

    :param fmt: Swap format string.
    :param data: Byte string of data to swap.
    :param offset: Start offset into `data`.
    :returns: Byte string of swapped bytes.

    """

    data = BytesIO(data)
    data.seek(offset)
    data_swapped = BytesIO()

    for f in fmt:
        swapped = data.read(int(f))[::-1]
        data_swapped.write(swapped)

    return data_swapped.getvalue()


def compile(fmt):
    """Compile given format string `fmt` and return a
    :class:`~bitstruct.CompiledFormat` object that can be used to pack
    and/or unpack data multiple times.

    :param fmt: Bitstruct format string. See :func:`~bitstruct.pack()`
                for details.

    """

    return CompiledFormat(fmt)
