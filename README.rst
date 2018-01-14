|buildstatus|_
|coverage|_

About
=====

This module is intended to have a similar interface as the python
struct module, but working on bits instead of primitive data types
(char, int, ...).

Project homepage: https://github.com/eerimoq/bitstruct

Documentation: http://bitstruct.readthedocs.org/en/latest


Installation
============

.. code-block:: python

    pip install bitstruct


Example usage
=============

A basic example of `packing`_ and `unpacking`_ four integers using the
format string ``'u1u3u4s16'``:

.. code-block:: python

    >>> from bitstruct import *
    >>> pack('u1u3u4s16', 1, 2, 3, -4)
    b'\xa3\xff\xfc'
    >>> unpack('u1u3u4s16', b'\xa3\xff\xfc')
    (1, 2, 3, -4)
    >>> calcsize('u1u3u4s16')
    24

An example `compiling`_ the format string once, and use it to `pack`_
and `unpack`_ data:

.. code-block:: python

    >>> import bitstruct
    >>> cf = bitstruct.compile('u1u3u4s16')
    >>> cf.pack(1, 2, 3, -4)
    b'\xa3\xff\xfc'
    >>> cf.unpack(b'\xa3\xff\xfc')
    (1, 2, 3, -4)

The unpacked values can be named by assigning them to variables or by
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

An example of `packing`_ and `unpacking`_ an unsinged integer, a
signed integer, a float, a boolean, a byte string and a string:

.. code-block:: python

    >>> from bitstruct import *
    >>> pack('u5s5f32b1r13t40', 1, -1, 3.75, True, b'\xff\xff', 'hello')
    b'\x0f\xd0\x1c\x00\x00?\xffhello'
    >>> unpack('u5s5f32b1r13t40', b'\x0f\xd0\x1c\x00\x00?\xffhello')
    (1, -1, 3.75, True, b'\xff\xf8', 'hello')
    >>> calcsize('u5s5f32b1r13t40')
    96

The same format string and values as in the previous example, but
using LSB (Least Significant Bit) first instead of the default MSB
(Most Significant Bit) first:

.. code-block:: python

    >>> from bitstruct import *
    >>> pack('<u5s5f32b1r13t40', 1, -1, 3.75, True, b'\xff\xff', 'hello')
    b'\x87\xc0\x00\x03\x80\xbf\xff\xf666\xa6\x16'
    >>> unpack('<u5s5f32b1r13t40', b'\x87\xc0\x00\x03\x80\xbf\xff\xf666\xa6\x16')
    (1, -1, 3.75, True, b'\xff\xf8', 'hello')
    >>> calcsize('<u5s5f32b1r13t40')
    96

An example of `unpacking`_ values from a hexstring and a binary file:

.. code-block:: python

    >>> from bitstruct import *
    >>> from binascii import unhexlify
    >>> unpack('s17s13r24', unhexlify('0123456789abcdef'))
    (582, -3751, b'\xe2j\xf3')
    >>> with open("test.bin", "rb") as fin:
    ...     unpack('s17s13r24', fin.read(8))
    ...
    ...
    (582, -3751, b'\xe2j\xf3')

Change endianness of the data with `byteswap`_, and then unpack the
values:

.. code-block:: python

    >>> from bitstruct import *
    >>> packed = pack('u1u3u4s16', 1, 2, 3, 1)
    >>> unpack('u1u3u4s16', byteswap('12', packed))
    (1, 2, 3, 256)

Contributing
============

#. Fork the repository.

#. Install prerequisites.

   .. code-block:: text

      pip install -r requirements.txt

#. Implement the new feature or bug fix.

#. Implement test case(s) to ensure that future changes do not break
   legacy.

#. Run the tests.

   .. code-block:: text

      make test

#. Create a pull request.

.. |buildstatus| image:: https://travis-ci.org/eerimoq/bitstruct.svg
.. _buildstatus: https://travis-ci.org/eerimoq/bitstruct

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/bitstruct/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/bitstruct

.. _packing: http://bitstruct.readthedocs.io/en/latest/#bitstruct.pack

.. _unpacking: http://bitstruct.readthedocs.io/en/latest/#bitstruct.unpack

.. _pack: http://bitstruct.readthedocs.io/en/latest/#bitstruct.CompiledFormat.pack

.. _unpack: http://bitstruct.readthedocs.io/en/latest/#bitstruct.CompiledFormat.unpack

.. _byteswap: http://bitstruct.readthedocs.io/en/latest/#bitstruct.byteswap

.. _compiling: http://bitstruct.readthedocs.io/en/latest/#bitstruct.compile
