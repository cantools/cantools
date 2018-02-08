|buildstatus|_
|coverage|_

About
=====

CAN BUS tools.

- `DBC`_, `KCD`_ and SYM file mangling.

- CAN message encoding and decoding.

- Simple and extended signal multiplexing.

- ``candump`` output decoder.

Project homepage: https://github.com/eerimoq/cantools

Documentation: http://cantools.readthedocs.org/en/latest

Installation
============

.. code-block:: python

    pip install cantools

Example usage
=============

Scripting
---------

The example starts by parsing a `small DBC-file`_ and printing its
messages and signals.

.. code-block:: python

   >>> import cantools
   >>> from pprint import pprint
   >>> db = cantools.db.load_file('tests/files/motohawk.dbc')
   >>> db.messages
   [message('ExampleMessage', 0x1f0, False, 8, 'Example message used as template in MotoHawk models.')]
   >>> example_message = db.get_message_by_name('ExampleMessage')
   >>> pprint(example_message.signals)
   [signal('Enable', 7, 1, 'big_endian', False, 1.0, 0, 0.0, 0.0, '-', False, None, {0: 'Disabled', 1: 'Enabled'}, None),
    signal('AverageRadius', 6, 6, 'big_endian', False, 0.1, 0, 0.0, 5.0, 'm', False, None, None, ''),
    signal('Temperature', 0, 12, 'big_endian', True, 0.01, 250, 229.53, 270.47, 'degK', False, None, None, None)]

The example continues `encoding`_ a message and sending it on a CAN
bus using the `python-can`_ package.

.. code-block:: python

   >>> import can
   >>> can.rc['interface'] = 'socketcan_ctypes'
   >>> can.rc['channel'] = 'vcan0'
   >>> can_bus = can.interface.Bus()
   >>> data = example_message.encode({'Temperature': 250.1, 'AverageRadius': 3.2, 'Enable': 1})
   >>> message = can.Message(arbitration_id=example_message.frame_id, data=data)
   >>> can_bus.send(message)

Alternatively, a message can be encoded using the `encode_message()`_
method on the database object.

The last part of the example receives and `decodes`_ a CAN message.

.. code-block:: python

   >>> message = can_bus.recv()
   >>> db.decode_message(message.arbitration_id, message.data)
   {'AverageRadius': 3.2, 'Enable': 'Enabled', 'Temperature': 250.09}

See the test suite for additional examples: https://github.com/eerimoq/cantools/blob/master/tests/test_cantools.py

Command line tool
-----------------

Decode CAN frames captured with the Linux program ``candump``.

.. code-block:: text

   $ candump vcan0 | cantools decode motohawk.dbc
     vcan0  1F0   [7]  80 4A 0F 00 00 00 00 :: ExampleMessage(Enable: 'Enabled' -, AverageRadius: 0.0 m, Temperature: 255.92 degK)
     vcan0  1F0   [7]  80 4A 0F 00 00 00 00 :: ExampleMessage(Enable: 'Enabled' -, AverageRadius: 0.0 m, Temperature: 255.92 degK)
     vcan0  1F0   [7]  80 4A 0F 00 00 00 00 :: ExampleMessage(Enable: 'Enabled' -, AverageRadius: 0.0 m, Temperature: 255.92 degK)

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

.. |buildstatus| image:: https://travis-ci.org/eerimoq/cantools.svg?branch=master
.. _buildstatus: https://travis-ci.org/eerimoq/cantools

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/cantools/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/cantools

.. _small DBC-file: https://github.com/eerimoq/cantools/blob/master/tests/files/motohawk.dbc

.. _python-can: https://python-can.readthedocs.io/en/latest/

.. _DBC: http://www.socialledge.com/sjsu/index.php?title=DBC_Format

.. _KCD: https://github.com/julietkilo/kcd

.. _encoding: http://cantools.readthedocs.io/en/latest/#cantools.db.Message.encode

.. _encode_message(): http://cantools.readthedocs.io/en/latest/#cantools.db.File.encode_message

.. _decodes: http://cantools.readthedocs.io/en/latest/#cantools.db.File.decode_message
