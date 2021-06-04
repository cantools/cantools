#!/usr/bin/env python3


class Converter:

    MSG_SORT_KEY_ID = "id"
    MSG_SORT_KEY_NAME = "name"
    MSG_SORT_KEYS = (MSG_SORT_KEY_ID, MSG_SORT_KEY_NAME)

    SIG_SORT_KEY_START_BIT = "start"
    SIG_SORT_KEY_NAME = "name"
    SIG_SORT_KEYS = (SIG_SORT_KEY_START_BIT, SIG_SORT_KEY_NAME)

    ext = ".tex"

    before_document = r"""
% !TeX program = pdflatex

\documentclass[a4paper]{article}
\usepackage{booktabs}
\usepackage{hyperref}

\providecommand{\degree}{\ensuremath{^\circ}}

\begin{document}
""".lstrip()
    after_document = r"""
\end{document}
"""

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
    sig_pattern = "\t{name} & {start} & {scale} & {offset} & {minimum} & {maximum} & {unit} \\\\"


    def save(self, fn, db, args):
        with open(fn, 'wt') as f:
            f.write(self.create_tex_document(db, args))

    def create_tex_document(self, db, args):
        return self.before_document + self.format_db(db, args) + self.after_document

    def format_db(self, db, args):
        out = []
        for msg in sorted(db.messages, key=self.msg_sort_key(args.msg_sort_key)):
            out.append(self.format_message(msg, args.sig_sort_key))
        return "\n".join(out)

    def msg_sort_key(self, key):
        if key == self.MSG_SORT_KEY_NAME:
            return lambda msg: msg.id
        else:
            return lambda msg: msg.name

    def sig_sort_key(self, key):
        if key == self.SIG_SORT_KEY_NAME:
            return lambda sig: sig.name
        else:
            return lambda sig: sig.start

    def format_message(self, msg, sig_sort_key):
        return self.msg_pattern.format(colspec=self.get_colspec(), header=self.format_header(), signals=self.format_signals(msg.signals, sig_sort_key), **self.message_format_dict(msg))

    def message_format_dict(self, msg):
        out = {
            'name' : msg.name,
            'frame_id' : msg.frame_id,
            'is_extended_frame' : msg.is_extended_frame,
            'length' : msg.length,
        }
        out = {key:self.texify(val) for key,val in out.items()}
        return out

    def get_colspec(self):
        return "*{%s}{l}" % (self.sig_pattern.count("&")+1)

    def format_header(self):
        return self.sig_pattern.format(**self.header_format_dict())

    def format_signals(self, signals, sort_key):
        out = []
        for sig in sorted(signals, key=self.sig_sort_key(sort_key)):
            out.append(self.sig_pattern.format(**self.signal_format_dict(sig)))
        return "\n".join(out)

    def header_format_dict(self):
        out = {
            'name' : 'Name',
            'start' : 'Start',
            'length' : 'Length',
            'byte_order' : 'Byte order',

            'datatype' : 'Datatype',
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
            return ""
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
            return "signed int"
        else:
            return "unsigned int"


def add_argument_group(parser):
    group = parser.add_argument_group("TeX converter options")
    group.add_argument("--msg-sort", dest="msg_sort_key", choices=Converter.MSG_SORT_KEYS, default=Converter.MSG_SORT_KEY_ID)
    group.add_argument("--sig-sort", dest="sig_sort_key", choices=Converter.SIG_SORT_KEYS, default=Converter.SIG_SORT_KEY_START_BIT)
