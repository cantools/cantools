from __future__ import print_function
import sys
import os
import argparse
import re
import binascii
import struct
from . import database
from .database.utils import format_and
from . import tester
from .compat import fopen

# Remove once less users are using the old package structure.
from . import database as db


__author__ = 'Erik Moqvist'
__version__ = '28.6.0'


# Matches 'candump' output, i.e. "vcan0  1F0   [8]  00 00 00 00 00 00 1B C1".
RE_CANDUMP = re.compile(r'^.*  ([0-9A-F]+)   \[\d+\]\s*([0-9A-F ]*)$')

MULTI_LINE_FMT = '''
{message}(
{signals}
)\
'''


def _mo_unpack(mo):
    frame_id = mo.group(1)
    frame_id = '0' * (8 - len(frame_id)) + frame_id
    frame_id = binascii.unhexlify(frame_id)
    frame_id = struct.unpack('>I', frame_id)[0]
    data = mo.group(2)
    data = data.replace(' ', '')
    data = binascii.unhexlify(data)

    return frame_id, data


def _format_signals(message, decoded_signals):
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

    return formatted_signals


def _format_message_single_line(message, formatted_signals):
    return ' {}({})'.format(message.name,
                            ', '.join(formatted_signals))


def _format_message_multi_line(message, formatted_signals):
    indented_signals = [
        '    ' + formatted_signal
        for formatted_signal in formatted_signals
    ]

    return MULTI_LINE_FMT.format(message=message.name,
                                 signals=',\n'.join(indented_signals))


def _format_message(dbase, frame_id, data, decode_choices, single_line):
    try:
        message = dbase.get_message_by_frame_id(frame_id)
    except KeyError:
        return ' Unknown frame id {0} (0x{0:x})'.format(frame_id)

    try:
        decoded_signals = message.decode(data, decode_choices)
    except Exception as e:
        return ' ' + str(e)

    formatted_signals = _format_signals(message, decoded_signals)

    if single_line:
        return _format_message_single_line(message, formatted_signals)
    else:
        return _format_message_multi_line(message, formatted_signals)


def _do_decode(args):
    dbase = database.load_file(args.database,
                               encoding=args.encoding,
                               frame_id_mask=args.frame_id_mask,
                               strict=not args.no_strict)
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
            line += ' ::'
            line += _format_message(dbase,
                                    frame_id,
                                    data,
                                    decode_choices,
                                    args.single_line)

        print(line)


def _do_dump(args):
    dbase = database.load_file(args.database,
                               encoding=args.encoding,
                               strict=not args.no_strict)

    print('================================= Messages =================================')
    print()
    print('  ' + 72 * '-')

    for message in dbase.messages:
        cycle_time = message.cycle_time

        if cycle_time is None:
            cycle_time = '-'

        print()
        print('  Name:       {}'.format(message.name))
        print('  Id:         0x{:x}'.format(message.frame_id))
        print('  Length:     {} bytes'.format(message.length))
        print('  Cycle time: {} ms'.format(cycle_time))
        print('  Senders:    {}'.format(format_and(message.senders)))
        print('  Layout:')
        print()
        print('\n'.join([
            ('    ' + line).rstrip()
            for line in message.layout_string().splitlines()
        ]))
        print()
        print('  Signal tree:')
        print()
        print('\n'.join([
            ('    ' + line).rstrip()
            for line in message.signal_tree_string().splitlines()
        ]))
        print()
        print('  ' + 72 * '-')


def _do_convert(args):
    dbase = database.load_file(args.infile,
                               encoding=args.encoding,
                               strict=not args.no_strict)

    output_format = os.path.splitext(args.outfile)[1][1:]

    if output_format == 'dbc':
        output = dbase.as_dbc_string()
    elif output_format == 'kcd':
        output = dbase.as_kcd_string()
    else:
        sys.exit(
            "Unsupported output database format '{}'.".format(
                output_format))

    with fopen(args.outfile, 'w', encoding=args.encoding) as fout:
        fout.write(output)


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
    decode_parser.add_argument(
        '-c', '--no-decode-choices',
        action='store_true',
        help='Do not convert scaled values to choice strings.')
    decode_parser.add_argument(
        '-s', '--single-line',
        action='store_true',
        help='Print the decoded message on a single line.')
    decode_parser.add_argument(
        '-e', '--encoding',
        default='utf-8',
        help='File encoding (default: utf-8).')
    decode_parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    decode_parser.add_argument(
        '-m', '--frame-id-mask',
        type=lambda x: int(x, 0),
        help=('Only compare selected frame id bits to find the message in the '
              'database. By default the candump and database frame ids must '
              'be equal for a match.'))
    decode_parser.add_argument(
        'database',
        help='Database file.')
    decode_parser.set_defaults(func=_do_decode)

    # The 'dump' subparser.
    dump_parser = subparsers.add_parser(
        'dump',
        description='Dump given database in a human readable format.')
    dump_parser.add_argument(
        '-e', '--encoding',
        default='utf-8',
        help='File encoding (default: utf-8).')
    dump_parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    dump_parser.add_argument(
        'database',
        help='Database file.')
    dump_parser.set_defaults(func=_do_dump)

    # The 'convert' subparser.
    convert_parser = subparsers.add_parser(
        'convert',
        description='Convert given database from one format to another.')
    convert_parser.add_argument(
        '-e', '--encoding',
        default='utf-8',
        help='File encoding (default: utf-8).')
    convert_parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    convert_parser.add_argument(
        'infile',
        help='Input database file.')
    convert_parser.add_argument(
        'outfile',
        help='Output database file.')
    convert_parser.set_defaults(func=_do_convert)

    args = parser.parse_args()

    if args.debug:
        args.func(args)
    else:
        try:
            args.func(args)
        except BaseException as e:
            sys.exit('error: ' + str(e))
