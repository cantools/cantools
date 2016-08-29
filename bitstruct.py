import re
import struct

__version__ = "3.3.1"


def _parse_format(fmt):
    parsed_infos = re.findall(r'([<>]?)([a-zA-Z])(\d+)', fmt)

    # Use big endian as default and use the endianness of the previous
    # value if none is given for the current value.
    infos = []
    endianness = ">"
    for info in parsed_infos:
        if info[0] != "":
            endianness = info[0]
        infos.append((info[1], int(info[2]), endianness))

    return infos


def _pack_integer(size, arg):
    value = int(arg)

    if value < 0:
        value = ((1 << size) + value)

    return '{{:0{}b}}'.format(size).format(value)


def _pack_boolean(size, arg):
    value = bool(arg)

    return _pack_integer(size, int(value))


def _pack_float(size, arg):
    value = float(arg)

    if size == 32:
        value = struct.pack('>f', value)
    elif size == 64:
        value = struct.pack('>d', value)
    else:
        raise ValueError('expected float size of 32 of 64 bits (got {})'.format(
            size))

    return ''.join('{:08b}'.format(b)
                   for b in bytearray(value))


def _pack_bytearray(size, arg):
    value = bytearray(arg)
    bits = ''.join('{:08b}'.format(b)
                   for b in value)

    return bits[0:size]


def _pack_text(size, arg):
    value = arg.encode('utf-8')

    return _pack_bytearray(size, bytearray(value))


def _unpack_integer(type_, bits):
    value = int(bits, 2)

    if type_ == 's':
        if bits[0] == '1':
            value -= (1 << len(bits))

    return value


def _unpack_boolean(bits):
    value = _unpack_integer('u', bits)

    return bool(value)


def _unpack_float(size, bits):
    packed = _unpack_bytearray(size, bits)

    if size == 32:
        value = struct.unpack('>f', packed)[0]
    elif size == 64:
        value = struct.unpack('>d', packed)[0]
    else:
        raise ValueError('expected float size of 32 of 64 bits (got {})'.format(
            size))

    return value


def _unpack_bytearray(size, bits):
    value = bytearray()
    for i in range(size // 8):
        value.append(int(bits[8*i:8*i+8], 2))
    rest = size % 8
    if rest > 0:
        value.append(int(bits[size-rest:], 2) << (8-rest))
    return value


def _unpack_text(size, bits):
    return _unpack_bytearray(size, bits).decode('utf-8')


def pack(fmt, *args):
    """Return a byte string containing the values v1, v2, ... packed
    according to the given format. If the total number of bits are not
    a multiple of 8, padding will be added at the end of the last
    byte.

    :param fmt: Bitstruct format string. See format description below.
    :param args: Variable argument list of values to pack.
    :returns: A byte string of the packed values.

    `fmt` is a string of bitorder-type-length groups. Bitorder may be
    omitted.

    Bitorder is either ">" or "<", where ">" means MSB first and "<"
    means LSB first. If bitorder is omitted, the previous values'
    bitorder is used for the current value. For example, in the format
    string "u1<u2u3" u1 is MSB first and both u2 and u3 are LSB first.

    There are seven types; 'u', 's', 'f', 'b', 't', 'r' and 'p'.

    - 'u' -- unsigned integer
    - 's' -- signed integer
    - 'f' -- floating point number of 32 or 64 bits
    - 'b' -- boolean
    - 't' -- text (ascii or utf-8)
    - 'r' -- raw, bytes
    - 'p' -- padding, ignore

    Length is the number of bits to pack the value into.

    Example format string: 'u1u3p7s16'

    """

    bits = ''
    infos = _parse_format(fmt)
    i = 0

    # Sanity check of the number of arguments.
    number_of_arguments = 0

    for info in infos:
        if info[0] != 'p':
            number_of_arguments += 1

    if number_of_arguments > len(args):
        raise ValueError("pack expected {} item(s) for packing "
                         "(got {})".format(number_of_arguments, len(args)))

    for type_, size, endianness in infos:
        if type_ == 'p':
            bits += size * '0'
        else:
            if type_ in 'us':
                value_bits = _pack_integer(size, args[i])
            elif type_ == 'f':
                value_bits = _pack_float(size, args[i])
            elif type_ == 'b':
                value_bits = _pack_boolean(size, args[i])
            elif type_ == 't':
                value_bits = _pack_text(size, args[i])
            elif type_ == 'r':
                value_bits = _pack_bytearray(size, bytearray(args[i]))
            else:
                raise ValueError("bad type '{}' in format".format(type_))

            # reverse the bit order in little endian values
            if endianness == "<":
                value_bits = value_bits[::-1]

            bits += value_bits
            i += 1

    # padding of last byte
    tail = len(bits) % 8
    if tail != 0:
        bits += (8 - tail) * '0'

    return bytes(bytearray([int(''.join(bits[i:i+8]), 2)
                            for i in range(0, len(bits), 8)]))


def unpack(fmt, data):
    """Unpack `data` (byte string, bytearray or list of integers)
    according to the given format. The result is a tuple even if it
    contains exactly one item.

    :param fmt: Bitstruct format string. See pack() for details.
    :param data: Byte string of values to unpack.
    :returns: A tuple of the unpacked values.

    """

    bits = ''.join(['{:08b}'.format(b) for b in bytearray(data)])
    infos = _parse_format(fmt)

    # Sanity check.
    number_of_bits_to_unpack = sum([size for _, size, _ in infos])

    if number_of_bits_to_unpack > len(bits):
        raise ValueError("unpack requires at least {} bits to unpack "
                         "(got {})".format(number_of_bits_to_unpack,
                                           len(bits)))

    res = []
    i = 0

    for type_, size, endianness in infos:
        if type_ == 'p':
            pass
        else:
            value_bits = bits[i:i+size]

            # reverse the bit order in little endian values
            if endianness == "<":
                value_bits = value_bits[::-1]

            if type_ in 'us':
                value = _unpack_integer(type_, value_bits)
            elif type_ == 'f':
                value = _unpack_float(size, value_bits)
            elif type_ == 'b':
                value = _unpack_boolean(value_bits)
            elif type_ == 't':
                value = _unpack_text(size, value_bits)
            elif type_ == 'r':
                value = bytes(_unpack_bytearray(size, value_bits))
            else:
                raise ValueError("bad type '{}' in format".format(type_))

            res.append(value)

        i += size

    return tuple(res)


def calcsize(fmt):
    """Calculate the number of bits in given format.

    :param fmt: Bitstruct format string.
    :returns: Number of bits in format string.

    """

    return sum([size for _, size, _ in _parse_format(fmt)])


def byteswap(fmt, data, offset = 0):
    """Swap bytes in `data` according to `fmt`, starting at byte
    `offset`. `fmt` must be an iterable, iterating over number of
    bytes to swap. For example, the format string "24" applied to the
    byte string b'\x00\x11\x22\x33\x44\x55' will produce the result
    b'\x11\x00\x55\x44\x33\x22'.

    :param fmt: Swap format string.
    :param data: Byte string of data to swap.
    :param offset: Start offset into `data`.
    :returns: Byte string of swapped bytes.

    """

    i = offset
    data_swapped = b''

    for f in fmt:
        length = int(f)
        value = data[i:i+length]
        data_swapped += value[::-1]
        i += length

    return data_swapped
