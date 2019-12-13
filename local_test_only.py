# -*- coding: utf-8 -*-
"""
Created on Mon Nov 25 20:19:14 2019

@author: Wir
"""

import cantools
import os
import sys

local_repo_path = r"C:\Users\Wir\Documents\Programmieren\repos\cantools"


if local_repo_path not in sys.path:
    sys.path.insert(0, local_repo_path)

try:
    sys.modules.pop("cantools")
except KeyError:
    pass



def test_long_names_from_scratch():
    LONG_MSG_NAME = "MSG456789_123456789_123456789_ABC"
    LONG_NODE_NAME = "NODE56789_abcdefghi_ABCDEFGHI_XYZ"
    node = cantools.database.can.node.Node(LONG_NODE_NAME, None)

    msg = cantools.database.can.message.Message(
            frame_id=1,
            name=LONG_MSG_NAME,
            length=8,
            signals=[],
            senders=[LONG_NODE_NAME])
    
    db = cantools.database.Database(
            nodes=[node],
            messages=[msg],
            version='')


    content = db.as_dbc_string()
    print(content)

test_long_names_from_scratch()