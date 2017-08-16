|buildstatus|_
|coverage|_

About
=====

CAN BUS tools.

- `DBC`_ and `KCD`_ file mangling.

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

An example parsing and using a `small DBC-file`_:

.. code-block:: python

   >>> import cantools
   >>> from pprint import pprint
   >>> db = cantools.db.load_file('tests/files/motohawk.dbc')
   >>> db
   version('1.0')

   node('PCM1', None)
   node('FOO', None)

   message('ExampleMessage', 0x1f0, False, 8, 'Example message used as template in MotoHawk models.')
     signal('Enable', 0, 1, 'big_endian', False, 1.0, 0, 0.0, 0.0, '-', False, None, {0: 'Disabled', 1: 'Enabled'}, None)
     signal('AverageRadius', 1, 6, 'big_endian', False, 0.1, 0, 0.0, 5.0, 'm', False, None, None, '')
     signal('Temperature', 7, 12, 'big_endian', True, 0.01, 250, 229.53, 270.47, 'degK', False, None, None, None)

   >>> db.messages
   [message('ExampleMessage', 0x1f0, 8, 'Example message used as template in MotoHawk models.')]
   >>> example_message = db.messages[0]
   >>> example_message.comment
   'Example message used as template in MotoHawk models.'
   >>> pprint(example_message.signals)
   [signal('Enable', 0, 1, 'big_endian', False, 1.0, 0, 0.0, 0.0, '-', False, None, {0: 'Disabled', 1: 'Enabled'}, None),
    signal('AverageRadius', 1, 6, 'big_endian', False, 0.1, 0, 0.0, 5.0, 'm', False, None, None, ''),
    signal('Temperature', 7, 12, 'big_endian', True, 0.01, 250, 229.53, 270.47, 'degK', False, None, None, None)]
   >>> db.nodes
   [node('PCM1', None), node('FOO', None)]
   >>> db.version
   '1.0'

The example continues encoding a message and sending it on a CAN bus
using the `python-can`_ package.

.. code-block:: python

   >>> import can
   >>> can.rc['interface'] = 'socketcan_ctypes'
   >>> can.rc['channel'] = 'vcan0'
   >>> can_bus = can.interface.Bus()
   >>> data = example_message.encode({'Temperature': 250.1, 'AverageRadius': 3.2, 'Enable': 1})
   >>> message = can.Message(arbitration_id=example_message.frame_id, data=data)
   >>> can_bus.send(message)

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

#. Implement the new feature or bug fix.

#. Implement test case(s) to ensure that future changes do not break
   legacy.

#. Run the test suite for `Python 2` and `Python 3`. Make sure all
   tests pass.

   .. code-block:: text

      python2 setup.py test
      python3 setup.py test

#. Create a pull request.

.. |buildstatus| image:: https://travis-ci.org/eerimoq/cantools.svg?branch=master
.. _buildstatus: https://travis-ci.org/eerimoq/cantools

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/cantools/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/cantools

.. _small DBC-file: https://github.com/eerimoq/cantools/blob/master/tests/files/motohawk.dbc

.. _python-can: https://python-can.readthedocs.io/en/latest/

.. _DBC: http://www.socialledge.com/sjsu/index.php?title=DBC_Format

.. _KCD: https://github.com/julietkilo/kcd
