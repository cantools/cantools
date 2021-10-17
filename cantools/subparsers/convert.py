import argparse

from .. import database
from . import convert_to_tex


def _do_convert(args):
    dbase = database.load_file(args.infile,
                               encoding=args.encoding,
                               strict=not args.no_strict)

    for ext in convert_to_tex.Converter.supported_extensions:
        if args.outfile.endswith(ext):
            convert_to_tex.Converter().save(args.outfile, dbase, args)
            return

    database.dump_file(dbase,
                       args.outfile,
                       database_format=None,
                       encoding=args.encoding)


def add_subparser(subparsers):
    convert_parser = subparsers.add_parser(
        'convert',
        description='Convert given database from one format to another.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
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
    convert_to_tex.add_argument_group(convert_parser)
