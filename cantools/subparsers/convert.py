from .. import database


def _do_convert(args):
    dbase = database.load_file(args.infile,
                               encoding=args.encoding,
                               strict=not args.no_strict)

    database.dump_file(dbase,
                       args.outfile,
                       database_format=None,
                       encoding=args.encoding)


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
