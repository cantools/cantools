import re
import struct


def _parse_format(fmt):
    types = re.findall(r'[a-zA-Z]+', fmt)
    sizes = map(lambda size: int(size),
                re.findall(r'\d+', fmt))

    return zip(types, sizes)


def _pack_integer(size, arg):
    if arg < 0:
        arg = ((1 << size) + arg)

    return '{{:0{}b}}'.format(size).format(arg)


def _pack_boolean(size, arg):
    return _pack_integer(size, int(arg))


def _pack_float(size, arg):
    if size == 32:
        value = struct.pack('>f', arg)
    elif size == 64:
        value = struct.pack('>d', arg)
    else:
        raise ValueError('Bad float size {}. Must be 32 or 64 bits.'.format(size))
    return ''.join('{:08b}'.format(b)
                   for b in bytearray(value))


def _pack_bytearray(size, arg):
    bits = ''.join('{:08b}'.format(b)
                   for b in arg)

    return bits[0:size]


def _unpack_integer(type, bits):
    value = int(bits, 2)

    if type == 's':
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
        raise ValueError('Bad float size {}. Must be 32 or 64 bits.'.format(size))

    return value

def _unpack_bytearray(size, bits):
    value = bytearray()
    for i in range(size // 8):
        value.append(int(bits[8*i:8*i+8], 2))
    rest = size % 8
    if rest > 0:
        value.append(int(bits[size-rest:], 2) << (8-rest))
    return value


def pack(fmt, *args):
    """Return a bytearray containing the values v1, v2, ... packed
    according to the given format. If the total number of bits are not
    a multiple of 8, padding will be added at the end of the last
    byte.

    :param fmt: Bitstruct format string.
    :param args: Variable argument list of values to pack.
    :returns: Bytearray of packed values.
    
    `fmt` is a string of type-length pairs. There are six types; 'u',
    's', 'f', 'b', 'r' and 'p'. Length is the number of bits to pack
    the value into.
    
    - 'u' -- unsigned integer
    - 's' -- signed integer
    - 'f' -- floating point number of 32 or 64 bits
    - 'b' -- boolean
    - 'r' -- raw, bytearray
    - 'p' -- padding, ignore

    Example format string: 'u1u3p7s16'

    """

    bits = ''
    infos = _parse_format(fmt)
    i = 0

    for type, size in infos:
        if type == 'p':
            bits += size * '0'
        else:
            if type in 'us':
                bits += _pack_integer(size, args[i])
            elif type == 'f':
                bits += _pack_float(size, args[i])
            elif type == 'b':
                bits += _pack_boolean(size, args[i])
            elif type == 'r':
                bits += _pack_bytearray(size, args[i])
            else:
                raise ValueError("bad type '{}' in format".format(type))
            i += 1

    # padding of last byte
    tail = len(bits) % 8
    if tail != 0:
        bits += (8 - tail) * '0'

    return bytearray([int(''.join(bits[i:i+8]), 2)
                      for i in range(0, len(bits), 8)])


def unpack(fmt, data):
    """Unpack the bytearray (presumably packed by pack(fmt, ...))
    according to the given format. The result is a tuple even if it
    contains exactly one item.

    :param fmt: Bitstruct format string. See pack() for details.
    :param data: Bytearray of values to unpack.
    :returns: Tuple of unpacked values.

    """

    bits = ''.join(['{:08b}'.format(b) for b in data])
    infos = _parse_format(fmt)
    res = []
    i = 0

    for type, size in infos:
        if type == 'p':
            pass
        else:
            if type in 'us':
                value = _unpack_integer(type, bits[i:i+size])
            elif type == 'f':
                value = _unpack_float(size, bits[i:i+size])
            elif type == 'b':
                value = _unpack_boolean(bits[i:i+size])
            elif type == 'r':
                value = _unpack_bytearray(size, bits[i:i+size])
            else:
                raise ValueError("bad type '{}' in format".format(type))
            res.append(value)
        i += size

    return tuple(res)


def calcsize(fmt):
    """Return the size of the bitstruct (and hence of the bytearray)
    corresponding to the given format.

    :param fmt: Bitstruct format string.
    :returns: Number of bits in format string.

    """

    return sum([size for _, size in _parse_format(fmt)])


def byteswap(fmt, data, offset = 0):
    """In place swap bytes in `data` according to `fmt`, starting at byte
    `offset`. `fmt` must be an iterable, iterating over number of
    bytes to swap. For example, the format string "24" applied to the
    bytearray "\x00\x11\x22\x33\x44\x55" will produce the result
    "\x11\x00\x55\x44\x33\x22".

    :param fmt: Swap format string.
    :param data: Bytearray of data to swap.
    :param offset: Start offset into `data`.
    :returns: Bytearray of swapped bytes.

    """

    i = offset

    for f in fmt:
        length = int(f)
        value = data[i:i+length]
        value.reverse()
        data[i:i+length] = value
        i += length

    return data
