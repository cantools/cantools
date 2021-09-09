#!/usr/bin/env python3

import os
import shutil
import tempfile
import re

import unittest
from unittest import mock

import cantools


class CanToolsConvertFullTest(unittest.TestCase):

    DBC_FILE_CHOICES = os.path.join(os.path.split(__file__)[0], 'files/dbc/choices.dbc')

    reo_lines_that_are_expected_to_differ = re.compile(
            r'^\\newcommand{\\(?P<key0>infile|outfile)}{(?P<val0>.*?)}|'
            r'^\\(?P<key1>title){(?P<val1>.*?)}|'
            r'^\\(?P<key2>date){(?P<val2>.*?)}')

    maxDiff = None

    # ------- aux functions -------

    def get_out_file_name(self, fn_dbc):
        fn = fn_dbc
        fn = os.path.splitext(fn)[0] + '.tex'
        fn = os.path.split(fn)[1]
        fn = os.path.join(tempfile.mkdtemp(), fn)
        return fn

    def remove_out_file(self, fn_out):
        path = os.path.split(fn_out)[0]
        shutil.rmtree(path)

    def get_golden_master_file_name(self, fn_dbc):
        return os.path.splitext(fn_dbc)[0] + '.tex'

    def assert_file_content_equal(self, content_expected, fn_is):
        read_is = lambda f: [self.reo_lines_that_are_expected_to_differ.sub(self.sub_line, ln).rstrip('\n') for ln in f]

        with open(fn_is, 'rt') as f:
            content_is = read_is(f)
        content_expected = content_expected.splitlines()
        self.assertEqual(content_expected, content_is)

    def sub_line(self, m):
        key, val, group = self.find_group(m)
        if key in ('infile', 'outfile'):
            val = os.path.split(val)[1]
        elif key == 'date':
            val = '09.09.2021'
        elif key == 'title':
            val = os.path.split(val)[1]
            val = val[0].upper() + val[1:]
        else:
            raise ValueError('invalid key: %s' % key)

        return self.replace(m, group, val)

    def find_group(self, m):
        for i in range(10):
            key = m.group('key%s' % i)
            if key:
                val = m.group('val%s' % i)
                return key, val, i

        raise ValueError('no key found in match object')

    def replace(self, m, i, val):
        group = 'val%s' % i
        s = m.group()
        im0, im1 = m.span()
        iv0, iv1 = m.span(group)
        i0 = iv0 - im0
        i1 = iv1 - im0
        return s[:i0] + val + s[i1:]


    # ------- test cases -------

    def test_choices(self):
        dbc = self.DBC_FILE_CHOICES
        fn_out = self.get_out_file_name(dbc)
        argv = ['cantools', 'convert', dbc, fn_out]

        expected_content = r'''
% !TeX program = pdflatex
\documentclass[a4paper]{article}

\usepackage{typearea}
\usepackage{parskip}
\usepackage{booktabs}
\usepackage{siunitx}
\usepackage{fancyhdr}
\usepackage{lastpage}
\usepackage{hyperref}

\hypersetup{hidelinks}
\setcounter{secnumdepth}{0}

\providecommand{\degree}{\ensuremath{^\circ}}
\newcommand{\thead}[1]{#1}

\makeatletter
    \newcommand{\thetitle}{\@title}
    \newcommand{\thedate}{\@date}
    \newcommand{\theauthor}{\@author}
\makeatother
\renewcommand{\maketitle}{%
    \begin{center}%
        \Large
        \thetitle
    \end{center}%
}

\iffalse
	\usepackage[table]{xcolor}
	\catcode`*=\active
	\def*{\rowcolor{green!20}}
\fi

\usepackage{xltabular}

\KOMAoption{DIV}{12}

\title{Choices}
\date{09.09.2021}
\newcommand{\infile}{choices.dbc}
\newcommand{\outfile}{choices.tex}

\fancyhead{}
\fancyhead[ol,er]{}
\fancyhead[or,el]{}
\renewcommand{\headrulewidth}{0pt}
\fancyfoot{}
\fancyfoot[ol,er]{Created from \infile, \thedate}
\fancyfoot[or,el]{Page \thepage\ of \pageref{LastPage}}
\pagestyle{fancy}


\newcommand{\sig}[1]{{\def\-{\discretionary{-}{\hbox{\footnotesize$\hookrightarrow$ }}{}}#1}}

\begin{document}
\maketitle

\section{0x000 Foo}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{xltabular}{1\linewidth}{XlS[table-format=1.0]S[table-format=1.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=3.0]c}
	\toprule
		{\thead{Name}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endfirsthead
		{\thead{Name}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endhead
	\midrule
		\multicolumn{\expandafter\the\csname LT@cols\endcsname}{r}{(continued on next page)} \\
\endfoot
	\bottomrule
\endlastfoot

	\sig{Foo} && 0 & 8 & LE && int & 1 & 0 && -128 & 127 & {--} \\
\end{xltabular}
\par
\endgroup


%\subsection{Comments, allowed values and multiplexing details}
\begin{description}
\item[\sig{Foo}] ~

	\begin{tabular}{@{}l@{ }l}
	\multicolumn{2}{@{}l}{Allowed values:} \\
	\quad\textbullet~6 & reserved \\
	\quad\textbullet~5 & reserved \\
	\quad\textbullet~4 & unused 2 \\
	\quad\textbullet~3 & unused \\
	\quad\textbullet~2 & unused \\
	\quad\textbullet~1 & \#\%=*ä' \\
	\quad\textbullet~0 & With space \\
	\quad\textbullet~-5 & A negative value \\
	\end{tabular}

\end{description}

\end{document}
'''.lstrip('\n')

        with mock.patch('sys.argv', argv):
            cantools._main()

        self.assert_file_content_equal(expected_content, fn_out)


if __name__ == '__main__':
    unittest.main()
