from __future__ import print_function
import sys
import os

from .. import database
from ..compat import fopen


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


def add_subparser(subparsers):
    convert_parser = subparsers.add_parser(
        'convert',
        description='Convert given database from one format to another.')
    convert_parser.add_argument(
        '-e', '--encoding',
        help='File encoding.')
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
