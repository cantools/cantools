#!/usr/bin/env python3
#
# > python3 dump_messages.py ../tests/files/motohawk.dbc
# ------------------------------------------------
# 
# Name:   ExampleMessage
# Id:     0x1f0
# Length: 8
# 
#                       Bit
# 
#          7   6   5   4   3   2   1   0
#        +---+---+---+---+---+---+---+---+
#      0 |<-x|<---------------------x|<--|
#        +---+---+---+---+---+---+---+---+
#          |   |                       +-- Temperature
#          |   +-- AverageRadius
#          +-- Enable
#        +---+---+---+---+---+---+---+---+
#      1 |-------------------------------|
#  B     +---+---+---+---+---+---+---+---+
#  y   2 |----------x|   |   |   |   |   |
#  t     +---+---+---+---+---+---+---+---+
#  e   3 |   |   |   |   |   |   |   |   |
#        +---+---+---+---+---+---+---+---+
#      4 |   |   |   |   |   |   |   |   |
#        +---+---+---+---+---+---+---+---+
#      5 |   |   |   |   |   |   |   |   |
#        +---+---+---+---+---+---+---+---+
#      6 |   |   |   |   |   |   |   |   |
#        +---+---+---+---+---+---+---+---+
#      7 |   |   |   |   |   |   |   |   |
#        +---+---+---+---+---+---+---+---+
# 
# ------------------------------------------------
#

from __future__ import print_function
import argparse
import cantools


parser = argparse.ArgumentParser()
parser.add_argument('filename', help='Database filename.')
args = parser.parse_args()

database = cantools.db.load_file(args.filename)

print('------------------------------------------------')

for message in database.messages:
    print()
    print('Name:   {}'.format(message.name))
    print('Id:     0x{:x}'.format(message.frame_id))
    print('Length: {}'.format(message.length))
    print()
    print(message.layout())
    print()
    print('------------------------------------------------')
