"""Wrap `cantools generate_c_source` and `cantools generate_cpp_source` in a macro"""

GENERATE_CPP_DEPS = [
    "@com_google_absl//absl/types:span",
]

# Duplicate camel_to_snake_case function in cantools/database/can/c_source.py with re
def _camel_to_snake_case(value):
    # Replace hyphen with underscore
    value = value.replace('-', '_')
    # Replace multiple underscores = re.sub(r'(_+)', '_', value)
    for underscores in ['__', '___', '____', '_____']:
        value = value.replace(underscores, '_')

    outname = value.elems()[0]
    prev_is_upper = value.elems()[0].isupper()
    prev_is_lower = value.elems()[0].islower()
    prev_is_digit = value.elems()[0].isdigit()
    is_upper = False
    is_lower = False
    is_digit = False
    first_iter = True

    for letter in value.elems()[1:]:
        is_upper = letter.isupper()
        is_lower = letter.islower()
        is_digit = letter.isdigit()

        # Prefix camelcase with _ = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', value)
        # Prefix lower to upper or digit to upper with _ = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', value)
        if (prev_is_upper and is_lower) or (prev_is_lower and is_upper) or (prev_is_digit and is_upper):
            last_char = outname[-1]
            outname = outname.rstrip(last_char)
            # If first iteration, do not add _ in beginning
            if first_iter:
                outname = outname + last_char
            else:
                outname = outname + '_' + last_char

        outname += letter
        prev_is_upper = is_upper
        prev_is_lower = is_lower
        prev_is_digit = is_digit
        if first_iter:
            first_iter = False
    return outname.lower()

def _generate_c_impl(ctx):
    outdir = ctx.build_file_path.split('/BUILD')[0]
    all_outputs = []

    for target in ctx.attr.dbcs:
        for dbc in target.files.to_list():
            outputs = []
            args = ctx.actions.args()
    
            basename = dbc.path.split("/")[-1].split(".dbc")[0]
            basename = _camel_to_snake_case(basename)
            outputs.append(ctx.actions.declare_file("%s.c" % basename))
            outputs.append(ctx.actions.declare_file("%s.h" % basename))

            args.add("generate_c_source")
            args.add(dbc.path)
            args.add("--outdir")
            args.add("%s/%s" % (ctx.bin_dir.path, outdir))

            for arg in ctx.attr.args:
                args.add(arg)
                if arg == "--generate-fuzzer" or arg == "-f":
                    outputs.append(ctx.actions.declare_file("%s_fuzzer.c" % basename))
                    outputs.append(ctx.actions.declare_file("%s_fuzzer.mk" % basename))

            ctx.actions.run(
                inputs = [dbc],
                outputs = outputs,
                executable = ctx.attr._cantools.files_to_run,
                arguments = [args],
                progress_message = "Generating DBC C source code for %s" % ctx.label,
            )
            all_outputs.extend(outputs)

    return [
        DefaultInfo(files = depset(all_outputs))
    ]

generate_c = rule(
    implementation = _generate_c_impl,
    attrs = {
        "dbcs": attr.label_list(
            doc = "List of filegroups that contain DBC files",
            mandatory = True,
            allow_files = True,
        ),
        "args": attr.string_list(
            doc = "Additional args to pass into cantools generate_c_source command",
            allow_empty = True,
            mandatory = False,
        ),
        "_cantools": attr.label(
            doc = "py_binary cantools target",
            executable = True,
            cfg = "host",
            default = Label("//cantools:cantools"),
        ),
    }
)

def _generate_cpp_impl(ctx):
    outdir = ctx.build_file_path.split('/BUILD')[0]
    all_outputs = []

    for target in ctx.attr.dbcs:
        for dbc in target.files.to_list():
            outputs = []
            args = ctx.actions.args()

            basename = dbc.path.split("/")[-1].split(".dbc")[0]
            basename = _camel_to_snake_case(basename)
            outputs.append(ctx.actions.declare_file("%s.cc" % basename))
            outputs.append(ctx.actions.declare_file("%s.h" % basename))

            args.add("generate_cpp_source")
            args.add(dbc.path)
            args.add("--outdir")
            args.add("%s/%s" % (ctx.bin_dir.path, outdir))

            for arg in ctx.attr.args:
                args.add(arg)

            ctx.actions.run(
                inputs = [dbc],
                outputs = outputs,
                executable = ctx.attr._cantools.files_to_run,
                arguments = [args],
                progress_message = "Generating DBC C++ source code for %s" % ctx.label,
            )
            all_outputs.extend(outputs)

    # TODO a better way to copy Frame.h as an output
    out = ctx.actions.declare_file("DBC.h")
    ctx.actions.run_shell(
        outputs=[out],
        inputs=ctx.attr._dbc_header.files,
        arguments=[ctx.attr._dbc_header.files.to_list()[0].path, out.path],
        command="cp $1 $2" 
    )
    all_outputs.extend([out])

    return [
        DefaultInfo(files = depset(all_outputs))
    ]

generate_cpp = rule(
    implementation = _generate_cpp_impl,
    attrs = {
        "dbcs": attr.label_list(
            doc = "List of filegroups that contain DBC files",
            mandatory = True,
            allow_files = True,
        ),
        "args": attr.string_list(
            doc = "Additional args to pass into cantools generate_cpp_source command",
            allow_empty = True,
            mandatory = False,
        ),
        "_cantools": attr.label(
            doc = "py_binary cantools target",
            executable = True,
            cfg = "host",
            default = Label("//cantools:cantools"),
        ),
        "_dbc_header": attr.label(
            doc = "DBC.h header file",
            allow_single_file = True,
            default = Label("//cantools/database/can/templates:DBC.h"),
        ),
    }
)
