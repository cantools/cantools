#!/usr/bin/env python3
#
# Script testing dbc manipulation and io.
#

import os
#import cantools
import sys
sys.path.append('/mnt/d/JQU/dbc/cantools')
import cantools

SCRIPT_DIR = os.path.dirname(os.path.realpath(__file__))
INPUT_DBC_PATH = os.path.join(SCRIPT_DIR, 'dbc_input.dbc')
OUTPUT_DBC_PATH = os.path.join(SCRIPT_DIR, 'dbc_output.dbc')

# Read the dbc
db = cantools.db.load_file(INPUT_DBC_PATH)
# Get a message to manipulate
message = db.get_message_by_name('TheMessage')
# Manipulate a message attribute. Try/except will catch accesses to non existent attributes
try:
    print("Input cycle time:", message.dbc.attributes['GenMsgCycleTime'].value)
    message.dbc.attributes['GenMsgCycleTime'].value = 2000
    print("Output cycle time:", message.dbc.attributes['GenMsgCycleTime'].value)
except KeyError as e:
    raise e
# Manipulate the message frame id
print("Input frame id: " + hex(message.frame_id))
message.frame_id = 0x234
print("Output frame id: " + hex(message.frame_id))

with open(OUTPUT_DBC_PATH, 'w', newline="\r\n") as f:
     f.write(db.as_dbc_string())