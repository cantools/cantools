.. cantools documentation master file, created by
   sphinx-quickstart on Sat Apr 25 11:54:09 2015.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. toctree::
   :maxdepth: 2

CAN BUS tools
=============

.. include:: ../README.rst

Functions and classes
=====================

.. autofunction:: cantools.db.load_file

.. code-block:: python

   >>> db = cantools.db.load_file('foo.dbc')
   >>> db.version
   '1.0'

.. autofunction:: cantools.db.load

.. code-block:: python

   >>> with open('foo.dbc') as fin:
   ...    db = cantools.db.load(fin.read())
   >>> db.version
   '1.0'

.. autoclass:: cantools.db.File
    :members:

.. autoclass:: cantools.db.Message
    :members:

.. autoclass:: cantools.db.Signal
    :members:
