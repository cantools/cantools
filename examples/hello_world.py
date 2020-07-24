#!/usr/bin/env python3
#
# > python3 hello_world.py
# Message: {'Temperature': 250.1, 'AverageRadius': 3.2, 'Enable': 'Enabled'}
# Encoded: c001400000000000
# Decoded: {'Enable': 'Enabled', 'AverageRadius': 3.2, 'Temperature': 250.1}
#

import os
from binascii import hexlify
import cantools


SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
MOTOHAWK_PATH = os.path.join(SCRIPT_DIR,
                             '..',
                             'tests',
                             'files',
                             'dbc',
                             'motohawk.dbc')

database = cantools.db.load_file(MOTOHAWK_PATH)

message = {
    'Temperature': 250.1,
    'AverageRadius': 3.2,
    'Enable': 'Enabled'
}

encoded = database.encode_message('ExampleMessage', message)
decoded = database.decode_message('ExampleMessage', encoded)

print('Message:', message)
print('Encoded:', hexlify(encoded).decode('ascii'))
print('Decoded:', decoded)
