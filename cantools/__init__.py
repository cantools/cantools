import sys
import argparse

from . import tester
from . import j1939
from .errors import Error

# Remove once less users are using the old package structure.
from . import database as db

from .version import __version__

__author__ = 'Erik Moqvist'


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

    # Import when used for less dependencies. For example, curses is
    # not part of all Python builds.
    from .subparsers import decode
    from .subparsers import monitor
    from .subparsers import dump
    from .subparsers import convert
    from .subparsers import generate_c_source

    decode.add_subparser(subparsers)
    monitor.add_subparser(subparsers)
    dump.add_subparser(subparsers)
    convert.add_subparser(subparsers)
    generate_c_source.add_subparser(subparsers)

    args = parser.parse_args()

    if args.debug:
        args.func(args)
    else:
        try:
            args.func(args)
        except BaseException as e:
            sys.exit('error: ' + str(e))
