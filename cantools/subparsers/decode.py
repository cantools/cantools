import sys
import re
import binascii
import struct

from .. import database
from .utils import format_message_by_frame_id


# Matches 'candump' output, i.e. "vcan0  1F0   [8]  00 00 00 00 00 00 1B C1".
RE_CANDUMP = re.compile(r'^.*?(?P<can_id>[0-9A-F]+)\s+\[\d+\]\s*(?P<can_data>[0-9A-F ]*)$')
RE_CANDUMP_LOG = re.compile(
    r'^.*?(?P<can_id>[0-9A-F]+)##?(?P<can_data>[0-9A-F]*)$')


def _mo_unpack(mo):
    frame_id = int(mo.group('can_id'), 16)
    data = mo.group('can_data')
    data = data.replace(' ', '')
    data = binascii.unhexlify(data)

    return frame_id, data


def _do_decode(dbase, args):
    decode_choices = not args.no_decode_choices
    pattern = None
    while True:
        line = sys.stdin.readline()

        # Break at EOF.
        if not line:
            break

        line = line.strip('\r\n')

        if pattern is None:
            for p in [RE_CANDUMP, RE_CANDUMP_LOG]:
                mo = p.match(line)

                if mo:
                    pattern = p
                    break
        if pattern is None:
            continue

        mo = pattern.match(line)
        if mo:
            frame_id, data = _mo_unpack(mo)
            line += ' ::'
            line += format_message_by_frame_id(dbase,
                                               frame_id,
                                               data,
                                               decode_choices,
                                               args.single_line)

        print(line)


def add_subparser(subparsers):
    decode_parser = subparsers.add_parser(
        'decode',
        description=('Decode "candump" CAN frames read from standard input '
                     'and print them in a human readable format.'))
    decode_parser.add_argument(
        '-c', '--no-decode-choices',
        action='store_true',
        help='Do not convert scaled values to choice strings.')
    decode_parser.add_argument(
        '-s', '--single-line',
        action='store_true',
        help='Print the decoded message on a single line.')
    decode_parser.set_defaults(func=_do_decode)
