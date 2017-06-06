|buildstatus|_
|coverage|_

About
=====

CAN BUS tools.

- DBC file mangling.

Installation
============

.. code-block:: python

    pip install cantools

Example usage
=============

See the test suite: https://github.com/eerimoq/cantools/blob/master/tests/test_cantools.py

An example parsing and using a `small DBC-file`_:

.. code-block:: python

   >>> import cantools
   >>> from pprint import pprint
   >>> db = cantools.db.File()
   >>> db.add_dbc_file('tests/files/motohawk.dbc')
   >>> pprint(db.messages)
   [message("ExampleMessage", 0x1f0, 8, "Example message used as template in MotoHawk models.")]
   >>> example_message = db.messages[0]
   >>> example_message.comment
   'Example message used as template in MotoHawk models.'
   >>> pprint(example_message.signals)
   [signal("Temperature", 7, 12, "big_endian", "signed", 0.01, 250, 229.53, 270.47, "degK", None, None),
    signal("AverageRadius", 1, 6, "big_endian", "unsigned", 0.1, 0, 0.0, 5.0, "m", None, ""),
    signal("Enable", 0, 1, "big_endian", "unsigned", 1.0, 0, 0.0, 0.0, "-", None, None)]
   >>> pprint(db.ecus)
   [ecu("PCM1", None)]
   >>> db.version
   '1.0'
   >>>

The example continues encoding a message and sending it on a CAN bus
using the `python-can`_ package.

   >>> import can
   >>> can_bus = can.interface.Bus()
   >>> data = example_message.encode({'Temperature': 250.1, 'AverageRadius': 3.2, 'Enable': 1})
   >>> message = can.Message(arbitration_id=example_message.frame_id, data=data)
   >>> can_bus.send(message)
   >>>

Contributing
============

#. Fork the repository.

#. Implement the new feature or bug fix.

#. Implement test case(s) to ensure that future changes do not break
   legacy.

#. Run the test suite for `Python 2` and `Python 3`. Make sure all
   tests pass.

   .. code-block:: text

      python setup.py test
      python3 setup.py test

#. Create a pull request.

.. |buildstatus| image:: https://travis-ci.org/eerimoq/cantools.svg
.. _buildstatus: https://travis-ci.org/eerimoq/cantools

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/cantools/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/cantools

.. _small DBC-file: https://github.com/eerimoq/cantools/blob/master/tests/files/motohawk.dbc

.. _python-can: https://python-can.readthedocs.io/en/latest/
