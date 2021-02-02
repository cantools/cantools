from .. import database


def _do_convert(dbase, args):
    database.dump_file(dbase,
                       args.outfile,
                       database_format=None,
                       encoding=args.encoding)


def add_subparser(subparsers):
    convert_parser = subparsers.add_parser(
        'convert',
        description='Convert given database from one format to another.')
    convert_parser.add_argument(
        'outfile',
        help='Output database file.')
    convert_parser.set_defaults(func=_do_convert)
