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
   >>> db = cantools.db.File()
   >>> with open('tests/files/motohawk.dbc') as fin:
           db.add_dbc(fin)
   >>> db.messages
   [ExampleMessage]
   >>> db.messages[0].comment
   'Example message used as template in MotoHawk models.'
   >>> db.messages[0].signals
   [Temperature, AverageRadius, Enable]
   >>> db.ecus
   [PCM1]
   >>> db.version
   '1.0'
   >>>

.. |buildstatus| image:: https://travis-ci.org/eerimoq/cantools.svg
.. _buildstatus: https://travis-ci.org/eerimoq/cantools

.. |coverage| image:: https://coveralls.io/repos/github/eerimoq/cantools/badge.svg?branch=master
.. _coverage: https://coveralls.io/github/eerimoq/cantools

.. _small DBC-file: https://github.com/eerimoq/cantools/blob/master/tests/files/motohawk.dbc
