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

.. autofunction:: cantools.db.load_string

.. autofunction:: cantools.db.load

.. autoclass:: cantools.db.Database
    :members:

.. autoclass:: cantools.db.Message
    :members:

.. autoclass:: cantools.db.Signal
    :members:

.. autoclass:: cantools.db.UnsupportedDatabaseFormatError
    :members:

.. autoclass:: cantools.tester.Tester
    :members:

Tips and tricks
===============

Virtual CAN interface setup in Ubuntu:

.. code:: text

   sudo modprobe vcan
   sudo ip link add dev vcan0 type vcan
   sudo ip link set vcan0 mtu 72         # For CAN-FD
   sudo ip link set up vcan0
