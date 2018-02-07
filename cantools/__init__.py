import sys
import argparse
import re
import binascii
import struct
from . import db

__author__ = 'Erik Moqvist'
__version__ = '16.3.0'


# Matches 'candump' output, i.e. "vcan0  1F0   [8]  00 00 00 00 00 00 1B C1".
RE_CANDUMP = re.compile(r'^.*  ([0-9A-F]+)   \[\d+\]\s*([0-9A-F ]*)$')


def _mo_unpack(mo):
    frame_id = mo.group(1)
    frame_id = '0' * (8 - len(frame_id)) + frame_id
    frame_id = binascii.unhexlify(frame_id)
    frame_id = struct.unpack('>I', frame_id)[0]
    data = mo.group(2)
    data = data.replace(' ', '')
    data = binascii.unhexlify(data)

    return frame_id, data


def _format_message(dbf, frame_id, data, decode_choices):
    try:
        message = dbf.get_message_by_frame_id(frame_id)
    except KeyError:
        return 'Unknown frame id {}'.format(frame_id)

    try:
        decoded_signals = message.decode(data, decode_choices)
    except ValueError as e:
        return str(e)

    formatted_signals = []

    for signal in message.signals:
        try:
            value = decoded_signals[signal.name]
        except KeyError:
            continue

        if isinstance(value, str):
            value = "'{}'".format(value)

        formatted_signals.append(
            '{}: {}{}'.format(signal.name,
                               value,
                              ''
                              if signal.unit is None
                              else ' ' + signal.unit))

    return '{}({})'.format(message.name,
                           ', '.join(formatted_signals))


def _do_decode(args):
    dbf = db.load_file(args.dbfile)
    decode_choices = not args.no_decode_choices

    while True:
        line = sys.stdin.readline()

        # Break at EOF.
        if not line:
            break

        line = line.strip('\r\n')
        mo = RE_CANDUMP.match(line)

        if mo:
            frame_id, data = _mo_unpack(mo)
            line += ' :: '
            line += _format_message(dbf,
                                    frame_id,
                                    data,
                                    decode_choices)

        print(line)


def _main():
    parser = argparse.ArgumentParser(
        description='Various CAN utilities.')

    parser.add_argument('-d', '--debug', action='store_true')
    parser.add_argument('--version',
                        action='version',
                        version=__version__,
                        help='Print version information and exit.')

    # Workaround to make the subparser required in Python 3.
    subparsers = parser.add_subparsers(title='subcommands',
                                       dest='subcommand')
    subparsers.required = True

    # The 'decode' subparser.
    decode_parser = subparsers.add_parser(
        'decode',
        description=('Decode "candump" CAN frames read from standard input '
                     'and print them in a human readable format.'))
    decode_parser.add_argument('-c', '--no-decode-choices',
                               action='store_true',
                               help='Do not convert scaled values to choice strings.')
    decode_parser.add_argument('dbfile', help='Database file (.dbc).')
    decode_parser.set_defaults(func=_do_decode)

    args = parser.parse_args()

    if args.debug:
        args.func(args)
    else:
        try:
            args.func(args)
        except BaseException as e:
            sys.exit(str(e))
