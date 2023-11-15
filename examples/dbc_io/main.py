#!/usr/bin/env python3
#
# Script testing DBC manipulation and io.
#

import os

import cantools

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
INPUT_DBC_PATH = os.path.join(SCRIPT_DIR, 'dbc_input.dbc')
OUTPUT_DBC_PATH = os.path.join(SCRIPT_DIR, 'dbc_output.dbc')

# Read the DBC.
print(f"Loading DBC database from '{INPUT_DBC_PATH}'.")
db = cantools.db.load_file(INPUT_DBC_PATH)

# Get a message to manipulate.
message = db.get_message_by_name('TheMessage')

# Manipulate a message attribute. Try/except will catch accesses to
# non existent attributes.
try:
    print("Input cycle time: ", message.cycle_time)
    message.cycle_time = 2000
    print("Output cycle time:", message.cycle_time)
except KeyError as e:
    raise e

# Manipulate the message frame id.
print("Input frame id: ", hex(message.frame_id))
message.frame_id = 0x234
print("Output frame id:", hex(message.frame_id))

print(f"Writing modified DBC database to '{OUTPUT_DBC_PATH}'.")

with open(OUTPUT_DBC_PATH, 'w', newline="\r\n") as fout:
    fout.write(db.as_dbc_string())
