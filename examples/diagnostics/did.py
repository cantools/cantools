#!/usr/bin/env python3
#
# > python3 did.py
# Data:    {'Country_variant': 'Europe', 'Vehicle_type': 'Sedan', 'Special_setting': 3}
# Encoded: 2103
# Decoded: {'Country_variant': 'Europe', 'Vehicle_type': 'Sedan', 'Special_setting': 3}
#

import os
from binascii import hexlify
import cantools


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
EXAMPLE_PATH = os.path.join(SCRIPT_DIR,
                            '..',
                            '..',
                            'tests',
                            'files',
                            'cdd',
                            'example.cdd')

database = cantools.db.load_file(EXAMPLE_PATH)

data = {
    'Country_variant': 'Europe',
    'Vehicle_type': 'Sedan',
    'Special_setting': 3
}

did = database.get_did_by_name('Coding')
encoded = did.encode(data)
decoded = did.decode(encoded)

print('Data:   ', data)
print('Encoded:', hexlify(encoded).decode('ascii'))
print('Decoded:', decoded)
