import os

from mako.template import Template

from .. import database
from ..database.can.c_source import Context
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

    for template in args.templates:
        template_basename = os.path.basename(template)
        output_filename = '{}_{}'.format(database_name, template_basename)

        render_context = Context(
            dbase,
            database_name,
            os.path.splitext(output_filename)[0],
            not args.no_floating_point_numbers,
            args.bit_fields)

        with open(os.path.join(args.outdir, output_filename), 'wb') as fout:
            with open(template,'rb') as fd_template:
                template_str = fd_template.read()

            fout.write(Template(template_str,
                input_encoding='utf-8',  # Default encoding for templates
                default_filters=['decode.cp1252'],  # Encoding of the DBC file, or eq.
                output_encoding='utf-8', encoding_errors='ignore'  # Encoding of the output
            ).render(ctx=render_context))

            print('Successfully generated {}.'.format(output_filename))


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
        '--infile',
        help='Input database file.')
    generate_c_source_parser.add_argument(
        '--outdir',
        default='.',
        help='Output directory (default: current working directory).')
    generate_c_source_parser.add_argument(
        'templates',
        metavar='TEMPLATE',
        nargs='+',
        help='Mako template file(s) to generate code from.')
    generate_c_source_parser.set_defaults(func=_do_generate_c_source)
