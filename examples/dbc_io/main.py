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
DBC_IO_DBC_PATH = os.path.join(SCRIPT_DIR, 'dbc_input.dbc')

infile = DBC_IO_DBC_PATH
outfile = infile.replace('_input', '_output')

# Read the dbc
db = cantools.db.load_file(infile)
# Get a message to manipulate
msg = db.get_message_by_name('TheMessage')
# Manipulate a message attribute. Try/except will catch accesses to non existent attributes
try:
    print("Input cycle time: " + str(msg.attributes['GenMsgCycleTime'].value))
    msg.attributes['GenMsgCycleTime'].value = 2000
    print("Output cycle time: " + str(msg.attributes['GenMsgCycleTime'].value))
except KeyError:
    pass
# Manipulate the message frame id
print("Input frame id: " + hex(msg.frame_id))
msg.frame_id = 0x234
print("Output frame id: " + hex(msg.frame_id))

with open(outfile, 'w', newline="\r\n") as f:
     f.write(db.as_dbc_string())