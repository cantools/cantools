from __future__ import print_function
import os

from .. import database
from ..database.can.c_source import generate
from ..database.can.c_source import camel_to_snake_case


def _do_generate_c_source(args):
    dbase = database.load_file(args.infile,
                               encoding=args.encoding,
                               strict=not args.no_strict)

    if args.database_name is None:
        basename = os.path.basename(args.infile)
        database_name = os.path.splitext(basename)[0]
        database_name = camel_to_snake_case(database_name)
    else:
        database_name = args.database_name

    filename_h = database_name + '.h'
    filename_c = database_name + '.c'
    fuzzer_filename_c = database_name + '_fuzzer.c'
    fuzzer_filename_mk = database_name + '_fuzzer.mk'

    header, source, fuzzer_source, fuzzer_makefile = generate(
        dbase,
        database_name,
        filename_h,
        filename_c,
        fuzzer_filename_c,
        not args.no_floating_point_numbers,
        args.bit_fields)

    with open(filename_h, 'w') as fout:
        fout.write(header)

    with open(filename_c, 'w') as fout:
        fout.write(source)

    print('Successfully generated {} and {}.'.format(filename_h, filename_c))

    if args.generate_fuzzer:
        with open(fuzzer_filename_c, 'w') as fout:
            fout.write(fuzzer_source)

        with open(fuzzer_filename_mk, 'w') as fout:
            fout.write(fuzzer_makefile)

        print('Successfully generated {} and {}.'.format(fuzzer_filename_c,
                                                         fuzzer_filename_mk))
        print()
        print(
            'Run "make -f {}" to build and run the fuzzer. Requires a'.format(
                fuzzer_filename_mk))
        print('recent version of clang.')


def add_subparser(subparsers):
    generate_c_source_parser = subparsers.add_parser(
        'generate_c_source',
        description='Generate C source code from given database file.')
    generate_c_source_parser.add_argument(
        '--database-name',
        help='The database name (default: input file name).')
    generate_c_source_parser.add_argument(
        '--no-floating-point-numbers',
        action='store_true',
        help='No floating point numbers in the generated code.')
    generate_c_source_parser.add_argument(
        '--bit-fields',
        action='store_true',
        help='Use bit fields to minimize struct sizes.')
    generate_c_source_parser.add_argument(
        '-e', '--encoding',
        help='File encoding.')
    generate_c_source_parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    generate_c_source_parser.add_argument(
        '-f', '--generate-fuzzer',
        action='store_true',
        help='Also generate fuzzer source code.')
    generate_c_source_parser.add_argument(
        'infile',
        help='Input database file.')
    generate_c_source_parser.set_defaults(func=_do_generate_c_source)
