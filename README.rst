|buildstatus|_

About
=====

This module is intended to have a similar interface as the python
struct module, but working on bits instead of primitive data types
(char, int, ...).

Documentation: http://bitstruct.readthedocs.org/en/latest


Installation
============

.. code-block:: python

    pip install bitstruct


Example usage
=============

See the test suite: https://github.com/eerimoq/bitstruct/blob/master/tests/test_bitstruct.py

A basic example of packing/unpacking four integers:

.. code-block:: python

    >>> from bitstruct import *
    >>> pack('u1u3u4s16', 1, 2, 3, -4)
    b'\xa3\xff\xfc'
    >>> unpack('u1u3u4s16', b'\xa3\xff\xfc')
    (1, 2, 3, -4)
    >>> calcsize('u1u3u4s16')
    24

The unpacked fields can be named by assigning them to variables or by
wrapping the result in a named tuple:

.. code-block:: python

    >>> from bitstruct import *
    >>> from collections import namedtuple
    >>> MyName = namedtuple('myname', [ 'a', 'b', 'c', 'd' ])
    >>> unpacked = unpack('u1u3u4s16', b'\xa3\xff\xfc')
    >>> myname = MyName(*unpacked)
    >>> myname
    myname(a=1, b=2, c=3, d=-4)
    >>> myname.c
    3

An example of packing/unpacking a unsinged integer, a signed integer,
a float, a boolean and a byte string:

.. code-block:: python

    >>> from bitstruct import *
    >>> pack('u5s5f32b1r13', 1, -1, 3.75, True, b'\xff\xff')
    b'\x0f\xd0\x1c\x00\x00?\xff'
    >>> unpack('u5s5f32b1r13', b'\x0f\xd0\x1c\x00\x00?\xff')
    (1, -1, 3.75, True, b'\xff\xf8')
    >>> calcsize('u5s5f32b1r13')
    56

The same format and values as in the previous example, but using LSB
(Least Significant Bit) first instead of the default MSB (Most
Significant Bit) first:

.. code-block:: python

    >>> from bitstruct import *
    >>> pack('<u5s5f32b1r13', 1, -1, 3.75, True, b'\xff\xff')
    b'\x87\xc0\x00\x03\x80\xbf\xff'
    >>> unpack('<u5s5f32b1r13', b'\x87\xc0\x00\x03\x80\xbf\xff')
    (1, -1, 3.75, True, b'\xff\xf8')
    >>> calcsize('<u5s5f32b1r13')
    56

An example of unpacking values from a hexstring and a binary file:

.. code-block:: python

    >>> from bitstruct import *
    >>> unpack('s17s13r24', '0123456789abcdef'.decode('hex'))
    (582, -3751, b'\xe2j\xf3')
    >>> with open("test.bin", "rb") as fin:
    ...     unpack('s17s13r24', fin.read(8))
    ...     
    ... 
    (582, -3751, b'\xe2j\xf3')

Change endianness of the data with byteswap(), and then unpack the
values:

.. code-block:: python

    >>> from bitstruct import *
    >>> packed = pack('u1u3u4s16', 1, 2, 3, 1)
    >>> unpack('u1u3u4s16', byteswap('12', packed))
    (1, 2, 3, 256)

.. |buildstatus| image:: https://travis-ci.org/eerimoq/bitstruct.svg
.. _buildstatus: https://travis-ci.org/eerimoq/bitstruct
