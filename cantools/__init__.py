import sys
import argparse

from . import database
from . import tester
from . import j1939
from .errors import Error

# Remove once less users are using the old package structure.
from . import database as db

from .database.can.c_source import camel_to_snake_case

from .version import __version__

__author__ = 'Erik Moqvist'


def _main():
    parser = argparse.ArgumentParser(
        description='Various CAN utilities.')

    parser.add_argument('--version',
                        action='version',
                        version=__version__,
                        help='Print version information and exit.')

    parser.add_argument(
        '-e', '--encoding',
        help='File encoding.')
    parser.add_argument(
        '-m', '--frame-id-mask',
        type=lambda x: int(x, 0),
        help=('Only compare selected frame id bits to find the message in the '
              'database. By default the candump and database frame ids must '
              'be equal for a match.'))
    parser.add_argument(
        '-r', '--msg-pattern', default=None,
        help='Regex pattern for filtering message names. Takes precedence over anti-pattern')
    parser.add_argument(
        '-x', '--msg-antipattern', default=None,
        help='Regex anti-pattern for filtering message names.')
    parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    parser.add_argument(
        '--database',
        help='Database file.')
    parser.add_argument(
        '--database-name',
        default=None,
        help='Database name. If not given, base the name on the database filename.')

    # Workaround to make the subparser required in Python 3.
    subparsers = parser.add_subparsers(title='subcommands',
                                       dest='subcommand')
    subparsers.required = True

    # Import when used for less dependencies. For example, curses is
    # not part of all Python builds.
    from .subparsers import decode
    from .subparsers import monitor
    from .subparsers import dump
    from .subparsers import convert
    from .subparsers import generate_c_source
    from .subparsers import plot

    decode.add_subparser(subparsers)
    monitor.add_subparser(subparsers)
    dump.add_subparser(subparsers)
    convert.add_subparser(subparsers)
    generate_c_source.add_subparser(subparsers)
    plot.add_subparser(subparsers)

    args = parser.parse_args()

    if args.database_name is None:
        import os.path
        basename = os.path.basename(args.database)
        database_name = os.path.splitext(basename)[0]
        args.database_name = camel_to_snake_case(database_name)

    dbase = database.load_file(args.database,
                               encoding=args.encoding,
                               frame_id_mask=args.frame_id_mask,
                               msg_pattern=args.msg_pattern,
                               msg_antipattern=args.msg_antipattern,
                               strict=not args.no_strict)
    args.func(dbase, args)
