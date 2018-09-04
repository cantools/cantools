#!/usr/bin/env python3

"""An example using a database cache to speed up database loading for
projects using the same database over and over again.

> python3 database_cache.py
First:
  Loaded database using cantools.
  Number of messages: 217
  Execution time: 0.09 seconds
Second:
  Loaded cached database.
  Number of messages: 217
  Execution time: 0.0 seconds

The database is found in the cantools-database-cache folder.

> du -sh cantools-database-cache/
308K	cantools-database-cache/

"""

from __future__ import print_function
import time
import os
import cantools
import diskcache


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
DBC_PATH = os.path.join(SCRIPT_DIR,
                        '..',
                        'tests',
                        'files',
                        'vehicle.dbc')
DATABASE_CACHE = diskcache.Cache('cantools-database-cache')


def load_database(filename):
    try:
        # Try to load the database for given file from the cache.
        with open(filename, 'rb') as fin:
            database = DATABASE_CACHE[fin.read()]

        print('  Loaded cached database.')
    except KeyError:
        # The database for given file was not found in the cache. Load
        # it using cantools.
        database = cantools.database.load_file(filename)

        with open(filename, 'rb') as fin:
            DATABASE_CACHE[fin.read()] = database

        print('  Loaded database using cantools.')

    return database


def timed_database_load(title):
    print(title)

    start_time = time.time()
    database = load_database(DBC_PATH)
    end_time = time.time()

    print('  Number of messages:', len(database.messages))
    print('  Execution time: {} seconds'.format(round(end_time - start_time, 2)))


if __name__ == '__main__':
    timed_database_load('First:')
    timed_database_load('Second:')
