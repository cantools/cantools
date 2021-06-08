#!/usr/bin/env python3

import os
import shutil
import tempfile
import subprocess


class Converter:

    MSG_SORT_KEY_ID = "id"
    MSG_SORT_KEY_NAME = "name"
    MSG_SORT_KEYS = (MSG_SORT_KEY_ID, MSG_SORT_KEY_NAME)

    SIG_SORT_KEY_START_BIT = "start"
    SIG_SORT_KEY_NAME = "name"
    SIG_SORT_KEYS = (SIG_SORT_KEY_START_BIT, SIG_SORT_KEY_NAME)

    ext_tex = ".tex"
    ext_pdf = ".pdf"
    supported_extensions = (ext_tex, ext_pdf)

    cmd_tex = ["pdflatex", "-interaction=nonstop"]

    preamble = r"""
% !TeX program = pdflatex

\documentclass[a4paper]{article}
\usepackage[DIV=12]{typearea}
\usepackage{booktabs}
\usepackage{siunitx}
\usepackage{hyperref}

\hypersetup{hidelinks}
\setcounter{secnumdepth}{0}

\providecommand{\degree}{\ensuremath{^\circ}}
\newcommand{\thead}[1]{#1}

\iffalse
	\usepackage[table]{xcolor}
	\catcode`*=\active
	\def*{\rowcolor{green!20}}
\fi
"""
    begin_document = r"\begin{document}"
    table_of_contents = r"\tableofcontents"
    end_document = r"\end{document}"

    msg_pattern = r"""
\section{{0x{frame_id:03X} {name}}}
\begin{{tabular}}{{{colspec}}}
\toprule
{header}
\midrule
{signals}
\bottomrule
\end{{tabular}}
"""
    sig_pattern = "\t{name} && {start} & {length} && {datatype} & {scale} & {offset} && {minimum} & {maximum} & {unit} \\\\"


    def save(self, fn, db, args):
        if fn.endswith(self.ext_pdf):
            self.save_pdf(fn, db, args)
        else:
            self.save_tex(fn, db, args)

    def save_pdf(self, fn, db, args):
        with tempfile.TemporaryDirectory() as path:
            tmpfn = "main"
            cwd = os.getcwd()
            os.chdir(path)
            fn_in = os.path.join(path, tmpfn + self.ext_tex)
            fn_out = os.path.join(path, tmpfn + self.ext_pdf)
            self.save_tex(fn_in, db, args)
            cmd = self.cmd_tex + [fn_in]
            for i in range(self.get_number_runs(args)):
                subprocess.run(cmd)
            os.chdir(cwd)
            shutil.move(fn_out, fn)

    def get_number_runs(self, args):
        n = 1
        if args.toc:
            n = 2
        return n

    def save_tex(self, fn, db, args):
        with open(fn, 'wt') as f:
            f.write(self.create_tex_document(db, args))

    def create_tex_document(self, db, args):
        out = []
        out.append(self.preamble)
        out.append(self.begin_document)
        if args.toc:
            out.append(self.table_of_contents)
        out.append(self.format_db(db, args))
        out.append(self.end_document)
        return "\n".join(out)

    def format_db(self, db, args):
        self.none = "{%s}" % args.none
        out = []
        for msg in sorted(db.messages, key=self.msg_sort_key(args)):
            out.append(self.format_message(msg, args))
        return "\n".join(out)

    def msg_sort_key(self, args):
        key = args.msg_sort_key
        if key == self.MSG_SORT_KEY_NAME:
            return lambda msg: msg.id
        else:
            return lambda msg: msg.name

    def sig_sort_key(self, args):
        key = args.sig_sort_key
        if key == self.SIG_SORT_KEY_NAME:
            return lambda sig: sig.name
        else:
            return lambda sig: sig.start

    def format_message(self, msg, args):
        return self.msg_pattern.format(colspec=self.get_colspec(msg.signals), header=self.format_header(), signals=self.format_signals(msg.signals, args), **self.message_format_dict(msg))

    def message_format_dict(self, msg):
        out = {
            'name' : msg.name,
            'frame_id' : msg.frame_id,
            'is_extended_frame' : msg.is_extended_frame,
            'length' : msg.length,
        }
        out = {key:self.texify(val) for key,val in out.items()}
        return out

    def get_colspec(self, signals):
        coltypes = self.colspec_dict()
        out = []
        for col in self.sig_pattern.rstrip("\\").split("&"):
            col = col.strip()
            col = col[1:][:-1]
            alignment = coltypes.get(col, "l")
            if alignment == "S":
                alignment = self.measure_s_column(col, signals)
            out.append(alignment)
        return "".join(out)

    def measure_s_column(self, col, signals):
        max_left = 0
        max_right = 0
        for sig in signals:
            val = getattr(sig, col)
            if val is None:
                val = 0
            val = "{:0f}".format(val).rstrip("0")
            if "." in val:
                val = val.split(".", 1)
                left, right = len(val[0]), len(val[1])
            else:
                left = len(val)
                right = 0
            if left > max_left:
                max_left = left
            if right > max_right:
                max_right = right
        return "S[table-format=%s.%s]" % (max_left, max_right)

    def format_header(self):
        return self.sig_pattern.format(**self.header_format_dict())

    def format_signals(self, signals, sort_key):
        out = []
        for sig in sorted(signals, key=self.sig_sort_key(sort_key)):
            out.append(self.sig_pattern.format(**self.signal_format_dict(sig)))
        return "\n".join(out)

    def colspec_dict(self):
        text = 'c'
        num = 'S'
        out = {
            'name' : 'l',
            'start' : num,
            'length' : num,
            'byte_order' : text,

            'datatype' : text,
            'is_float' : text,
            'is_signed' : text,

            'initial' : num,
            'scale' : num,
            'offset' : num,
            'minimum' : num,
            'maximum' : num,
            'unit' : text,

            'choices' : text,
            'comment' : text,
            'comments' : text,

            'is_multiplexer' : text,
            'multiplexer_ids' : text,
            'multiplexer_signal' : text,
        }
        return out

    def header_format_dict(self):
        out = {
            'name' : 'Name',
            'start' : 'Start',
            'length' : 'Length',
            'byte_order' : 'Byte order',

            'datatype' : 'Type',
            'is_float' : 'Float?',
            'is_signed' : 'Digned?',

            'initial' : 'Initial',
            'scale' : 'Scale',
            'offset' : 'Offset',
            'minimum' : 'Minimum',
            'maximum' : 'Maximum',
            'unit' : 'Unit',

            'choices' : 'Choices',
            'comment' : 'Comment',
            'comments' : 'Comments',

            'is_multiplexer' : 'Multiplexer?',
            'multiplexer_ids' : 'Multiplexer IDs',
            'multiplexer_signal' : 'Multiplexer signal',
        }
        out = {key:self.texify(val) for key,val in out.items()}
        out = {key: r"{\thead{%s}}" % val for key,val in out.items()}
        return out

    def signal_format_dict(self, sig):
        out = {
            'name' : sig.name,
            'start' : sig.start,
            'length' : sig.length,
            'byte_order' : sig.byte_order,

            'datatype' : self.get_datatype(sig),
            'is_float' : sig.is_float,
            'is_signed' : sig.is_signed,

            'initial' : sig.initial,
            'scale' : sig.decimal.scale,
            'offset' : sig.decimal.offset,
            'minimum' : sig.decimal.minimum,
            'maximum' : sig.decimal.maximum,
            'unit' : sig.unit,

            'choices' : sig.choices,
            'comment' : sig.comment,
            'comments' : sig.comments,

            'is_multiplexer' : sig.is_multiplexer,
            'multiplexer_ids' : sig.multiplexer_ids,
            'multiplexer_signal' : sig.multiplexer_signal,
        }
        out = {key:self.texify(val) for key,val in out.items()}
        return out

    def texify(self, val):
        if val is None:
            return self.none
        if isinstance(val, int):
            return val

        val = str(val)
        val = val.replace("\\", r"\textbackslash ")
        val = val.replace("{", r"\{")
        val = val.replace("}", r"\}")
        val = val.replace("$", r"\$")
        val = val.replace("&", r"\&")
        val = val.replace("#", r"\#")
        val = val.replace("^", r"\string^")
        val = val.replace("_", r"\_")
        val = val.replace("~", r"\string~")
        val = val.replace("%", r"\%")

        val = val.replace("°", r"\degree ")
        return val

    def get_datatype(self, sig):
        if sig.is_float:
            return "float"
        elif sig.is_signed:
            return "int"
        else:
            return "uint"


def add_argument_group(parser):
    group = parser.add_argument_group("TeX converter options")
    group.add_argument("--msg-sort", dest="msg_sort_key", choices=Converter.MSG_SORT_KEYS, default=Converter.MSG_SORT_KEY_ID)
    group.add_argument("--sig-sort", dest="sig_sort_key", choices=Converter.SIG_SORT_KEYS, default=Converter.SIG_SORT_KEY_START_BIT)
    group.add_argument("--none", default="--", help="a symbol to print if a value is None")

    group.add_argument("--toc", action="store_true", help="insert a table of contents")
