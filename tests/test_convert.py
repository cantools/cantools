#!/usr/bin/env python3

import os
import shutil
import tempfile
import re
import datetime
import subprocess

import unittest
from unittest import mock

import cantools


class CanToolsConvertFullTest(unittest.TestCase):

    DBC_FILE_CHOICES = os.path.join(os.path.split(__file__)[0], 'files/dbc/choices.dbc')
    DBC_FILE_MULTIPLEX = os.path.join(os.path.split(__file__)[0], 'files/dbc/multiplex_2.dbc')
    DBC_FILE_SIGNED = os.path.join(os.path.split(__file__)[0], 'files/dbc/signed.dbc')
    DBC_FILE_COMMENTS = os.path.join(os.path.split(__file__)[0], 'files/dbc/motohawk_with_comments.dbc')
    DBC_FILE_MSG_WITHOUT_SIG = os.path.join(os.path.split(__file__)[0], 'files/dbc/add_two_dbc_files_2.dbc')
    DBC_FILE_LONG_SIGNAL_NAMES = os.path.join(os.path.split(__file__)[0], 'files/dbc/long_names_multiple_relations.dbc')

    reo_lines_that_are_expected_to_differ = re.compile(
            r'^\\newcommand{\\(?P<key0>infile|outfile)}{(?P<val0>.*?)}|'
            r'^\\(?P<key1>title){(?P<val1>.*?)}|'
            r'^\\(?P<key2>date){(?P<val2>.*?)}')

    maxDiff = None

    # ------- aux functions -------

    def get_out_file_name(self, fn_dbc, ext='.tex'):
        fn = fn_dbc
        fn = os.path.splitext(fn)[0] + ext
        fn = os.path.split(fn)[1]
        fn = os.path.join(tempfile.mkdtemp(), fn)
        return fn

    def remove_out_file(self, fn_out):
        path = os.path.split(fn_out)[0]
        shutil.rmtree(path)

    def get_golden_master_file_name(self, fn_dbc):
        return os.path.splitext(fn_dbc)[0] + '.tex'

    def assert_file_content_equal(self, content_expected, fn_is, *, fixed_date=False):
        self._fixed_date = fixed_date
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
            if not self._fixed_date:
                expected_date = datetime.datetime.now().strftime('%d.%m.%Y')
                self.assertEqual(expected_date, val)
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


    # ------- test run -------

    def test_run_twice(self):
        dbc = self.DBC_FILE_LONG_SIGNAL_NAMES
        fn_out = self.get_out_file_name(dbc, '.pdf')
        argv = ['cantools', 'convert', dbc, fn_out]

        self.mock_run_counter = 0
        self.mock_run_returncode = 0
        with mock.patch('sys.argv', argv):
            with mock.patch('subprocess.run', self.mock_run):
                cantools._main()

        self.assertEqual(self.mock_run_counter, 2)
        self.assertTrue(os.path.exists(fn_out))

    def test_no_rerun_on_error(self):
        dbc = self.DBC_FILE_LONG_SIGNAL_NAMES
        fn_out = self.get_out_file_name(dbc, '.pdf')
        argv = ['cantools', 'convert', dbc, fn_out]

        self.mock_run_counter = 0
        self.mock_run_returncode = 1
        with mock.patch('sys.argv', argv):
            with mock.patch('subprocess.run', self.mock_run):
                cantools._main()

        self.assertEqual(self.mock_run_counter, 1)
        self.assertTrue(os.path.exists(fn_out))

    def mock_run(self, cmd, **kw):
        self.mock_run_counter += 1

        fn = cmd[-1]
        fn = os.path.splitext(fn)[0] + '.pdf'
        with open(fn, 'wt') as f:
            f.write('')

        p = mock.Mock()
        p.returncode = self.mock_run_returncode
        return p


    # ------- test choices -------

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


    # ------- test multiplex -------

    def test_multiplex(self):
        dbc = self.DBC_FILE_MULTIPLEX
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

\title{Multiplex 2}
\date{09.09.2021}
\newcommand{\infile}{multiplex\_2.dbc}
\newcommand{\outfile}{multiplex\_2.tex}

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

\section{0xC00FEFE Extended}
Extended frame \\
DLC = 8

\begingroup
\centering
\begin{xltabular}{1\linewidth}{XclS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
	\toprule
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endfirsthead
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endhead
	\midrule
		\multicolumn{\expandafter\the\csname LT@cols\endcsname}{r}{(continued on next page)} \\
\endfoot
	\bottomrule
\endlastfoot

	\sig{S0} & M && 0 & 4 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S1} & m0, M && 4 & 4 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S5} & m1 && 4 & 28 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S2} & m0 && 8 & 8 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S4} & m2 && 8 & 24 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S3} & m0 && 16 & 16 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S6} & M && 32 & 8 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S7} & m1 && 40 & 24 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S8} & m2 && 40 & 8 & LE && int & 1 & 0 && {--} & {--} & {--} \\
\end{xltabular}
\par
\endgroup


%\subsection{Comments, allowed values and multiplexing details}
\begin{description}
\item[\sig{S1}] This signal is available only if S0 has the value 0x0.

\item[\sig{S5}] This signal is available only if S0 has the value 0x1.

\item[\sig{S2}] This signal is available only if S1 has the value 0x0.

\item[\sig{S4}] This signal is available only if S1 has the value 0x2.

\item[\sig{S3}] This signal is available only if S1 has the value 0x0.

\item[\sig{S7}] This signal is available only if S6 has the value 0x1.

\item[\sig{S8}] This signal is available only if S6 has the value 0x2.

\end{description}


\section{0xC01FEFE Normal}
Extended frame \\
DLC = 8

\begingroup
\centering
\begin{xltabular}{1\linewidth}{XclS[table-format=1.0]S[table-format=1.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
	\toprule
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endfirsthead
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endhead
	\midrule
		\multicolumn{\expandafter\the\csname LT@cols\endcsname}{r}{(continued on next page)} \\
\endfoot
	\bottomrule
\endlastfoot

	\sig{S0} & M && 0 & 4 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S1} & m0 && 4 & 4 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S2} & m1 && 8 & 8 & LE && int & 1 & 0 && {--} & {--} & {--} \\
\end{xltabular}
\par
\endgroup


%\subsection{Comments, allowed values and multiplexing details}
\begin{description}
\item[\sig{S1}] This signal is available only if S0 has the value 0x0.

\item[\sig{S2}] This signal is available only if S0 has the value 0x1.

\end{description}


\section{0xC02FEFE Shared}
Extended frame \\
DLC = 8

\begingroup
\centering
\begin{xltabular}{1\linewidth}{XclS[table-format=1.0]S[table-format=1.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
	\toprule
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endfirsthead
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endhead
	\midrule
		\multicolumn{\expandafter\the\csname LT@cols\endcsname}{r}{(continued on next page)} \\
\endfoot
	\bottomrule
\endlastfoot

	\sig{S0} & M && 0 & 4 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S1} & mx && 4 & 4 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S2} & mx && 8 & 8 & LE && int & 1 & 0 && {--} & {--} & {--} \\
\end{xltabular}
\par
\endgroup


%\subsection{Comments, allowed values and multiplexing details}
\begin{description}
\item[\sig{S1}] This signal is available only if S0 has one of the values 0x1 or 0x3.

\item[\sig{S2}] This signal is available only if S0 has one of the values 0x2, 0x3, 0x4 or 0x5.

\end{description}


\section{0xC03FEFE ExtendedTypes}
Extended frame \\
DLC = 8

\begingroup
\centering
\begin{xltabular}{1\linewidth}{XclS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.2E3]S[table-format=4.1E3]c}
	\toprule
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endfirsthead
		{\thead{Name}} & {\thead{Mux}} && {\thead{Start}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
	\midrule
\endhead
	\midrule
		\multicolumn{\expandafter\the\csname LT@cols\endcsname}{r}{(continued on next page)} \\
\endfoot
	\bottomrule
\endlastfoot

	\sig{S11} & M && 0 & 5 & LE && uint & 1 & 0 && {\texttt{0x02}} & {\texttt{0x06}} & {--} \\
	\sig{S0} & m3, M && 8 & 4 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{S10} & m0 && 16 & 32 & LE && float & 1 & 0 && -3.4E+38 & 3.4E+38 & {--} \\
	\sig{S9} & m5 && 24 & 32 & LE && float & 1 & 0 && -1.34 & 1235 & {--} \\
\end{xltabular}
\par
\endgroup


%\subsection{Comments, allowed values and multiplexing details}
\begin{description}
\item[\sig{S0}] This signal is available only if S11 has the value 0x3.

\item[\sig{S10}] This signal is available only if S0 has the value 0x0.

\item[\sig{S9}] This signal is available only if S11 has the value 0x5.

\end{description}

\end{document}
'''.lstrip('\n')

        with mock.patch('sys.argv', argv):
            cantools._main()

        self.assert_file_content_equal(expected_content, fn_out)


    # ------- test vector -------

    def test_vector(self):
        dbc = self.DBC_FILE_SIGNED
        fn_out = self.get_out_file_name(dbc)
        argv = ['cantools', 'convert', '--vector', '--landscape', '--env', 'tabular', dbc, fn_out]

        expected_content = r'''
% !TeX program = pdflatex
\documentclass[a4paper, landscape]{article}

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


\KOMAoption{DIV}{12}

\title{Signed}
\date{09.09.2021}
\newcommand{\infile}{signed.dbc}
\newcommand{\outfile}{signed.tex}

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

\section{0x000 Message32}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=1.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s32} && 0 & 32 & LE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x001 Message33}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=1.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=11.0]S[table-format=10.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s33} && 0 & 33 & LE && int & 1 & 0 && -4294967296 & 4294967295 & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x002 Message64}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=1.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=2.14E3]S[table-format=1.14E3]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s64} && 0 & 64 & LE && int & 1 & 0 && -9.22337203685478E+18 & 9.22337203685478E+18 & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x003 Message64big}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s64\-big} && 56 & 64 & BE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x004 Message33big}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s33\-big} && 39 & 33 & BE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x005 Message32big}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s32\-big} && 24 & 32 & BE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x006 Message63}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=1.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s63} && 0 & 63 & LE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x007 Message63big}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s63\-big} && 57 & 63 & BE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x008 Message63\_1}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=1.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s63} && 1 & 63 & LE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x009 Message63big\_1}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s63\-big} && 56 & 63 & BE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup


\section{0x00A Message378910}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabular}{llS[table-format=2.0]S[table-format=2.0]clcS[table-format=1.0]S[table-format=1.0]lS[table-format=4.0]S[table-format=4.0]c}
\toprule
	{\thead{Name}} && {\thead{Startbit}} & {\thead{Bits}} & {\thead{E}} && {\thead{Type}} & {\thead{Scale}} & {\thead{Offset}} && {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
	\sig{s7} && 1 & 7 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{s8\-big} && 9 & 8 & BE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{s9} && 17 & 9 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{s8} && 26 & 8 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{s3} && 34 & 3 & LE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{s3\-big} && 37 & 3 & BE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{s7\-big} && 56 & 7 & BE && int & 1 & 0 && {--} & {--} & {--} \\
	\sig{s10\-big} && 63 & 10 & BE && int & 1 & 0 && {--} & {--} & {--} \\
\bottomrule
\end{tabular}
\par
\endgroup

\end{document}
'''.lstrip('\n')

        with mock.patch('sys.argv', argv):
            cantools._main()

        self.assert_file_content_equal(expected_content, fn_out)


    # ------- test comments -------

    def test_comments(self):
        dbc = self.DBC_FILE_COMMENTS
        fn_out = self.get_out_file_name(dbc)
        argv = ['cantools', 'convert', '--env', 'ltablex-keep-x', '--title', 'Testing comments', '--date', '25.02.2021', dbc, fn_out]

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

\usepackage{ltablex}
\keepXColumns

\KOMAoption{DIV}{12}

\title{Testing comments}
\date{25.02.2021}
\newcommand{\infile}{motohawk\_with\_comments.dbc}
\newcommand{\outfile}{motohawk\_with\_comments.tex}

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

\section{0x1F0 ExampleMessage}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabularx}{1\linewidth}{XlS[table-format=1.0]S[table-format=2.0]clcS[table-format=1.2]S[table-format=3.0]lS[table-format=4.2]S[table-format=4.2]c}
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

	\sig{Temperature} && 0 & 12 & BE && int & 0.01 & 250 && 229.52 & 270.47 & degK \\
	\sig{Average\-Radius} && 6 & 6 & BE && uint & 0.1 & 0 && 0 & 5 & m \\
	\sig{Enable} && 7 & 1 & BE && uint & 1 & 0 && {--} & {--} & - \\
\end{tabularx}
\par
\endgroup


%\subsection{Comments, allowed values and multiplexing details}
\begin{description}
\item[\sig{Temperature}] ~

Temperature with a really long and complicated comment that probably require many many lines in a decently wide terminal

\item[\sig{Average\-Radius}] ~

AverageRadius signal comment

\item[\sig{Enable}] ~

Enable signal comment

	\begin{tabular}{@{}l@{ }l}
	\multicolumn{2}{@{}l}{Allowed values:} \\
	\quad\textbullet~0 & Disabled \\
	\quad\textbullet~1 & Enabled \\
	\end{tabular}

\end{description}

\end{document}
'''.lstrip('\n')

        with mock.patch('sys.argv', argv):
            cantools._main()

        self.assert_file_content_equal(expected_content, fn_out, fixed_date=True)


    # ------- test no signals -------

    def test_no_signals(self):
        dbc = self.DBC_FILE_MSG_WITHOUT_SIG
        fn_out = self.get_out_file_name(dbc)
        argv = ['cantools', 'convert', '--env', 'ltablex', dbc, fn_out]

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

\usepackage{ltablex}

\KOMAoption{DIV}{12}

\title{Add Two Dbc Files 2}
\date{09.09.2021}
\newcommand{\infile}{add\_two\_dbc\_files\_2.dbc}
\newcommand{\outfile}{add\_two\_dbc\_files\_2.tex}

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

\section{0x002 M1}
Base frame \\
DLC = 8

This message has no signals.

\end{document}
'''.lstrip('\n')

        with mock.patch('sys.argv', argv):
            cantools._main()

        self.assert_file_content_equal(expected_content, fn_out)


    # ------- test break signal name anywhere -------

    def test_break_signal_name_anywhere(self):
        dbc = self.DBC_FILE_LONG_SIGNAL_NAMES
        fn_out = self.get_out_file_name(dbc)
        argv = ['cantools', 'convert', '--class-option', 'a5paper', '--toc', '--env', 'tabularx', '--sig-name-break-anywhere', '--sig-pattern', '{name} & {datatype} & {initial} & {minimum} & {maximum} & {unit}', '--hex', dbc, fn_out]

        expected_content = r'''
% !TeX program = pdflatex
\documentclass[a4paper, a5paper]{article}

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

\usepackage{tabularx}

\KOMAoption{DIV}{12}

\title{Long Names Multiple Relations}
\date{09.09.2021}
\newcommand{\infile}{long\_names\_multiple\_relations.dbc}
\newcommand{\outfile}{long\_names\_multiple\_relations.tex}

\fancyhead{}
\fancyhead[ol,er]{}
\fancyhead[or,el]{}
\renewcommand{\headrulewidth}{0pt}
\fancyfoot{}
\fancyfoot[ol,er]{Created from \infile, \thedate}
\fancyfoot[or,el]{Page \thepage\ of \pageref{LastPage}}
\pagestyle{fancy}


\usepackage{url}
\makeatletter
\expandafter\def\expandafter\UrlBreaks\expandafter{\UrlBreaks\do a\do b\do c\do d\do e\do f\do g\do h\do i\do j\do k\do l\do m\do n\do o\do p\do q\do r\do s\do t\do u\do v\do w\do x\do y\do z\do A\do B\do C\do D\do E\do F\do G\do H\do I\do J\do K\do L\do M\do N\do O\do P\do Q\do R\do S\do T\do U\do V\do W\do X\do Y\do Z\do 0\do 1\do 2\do 3\do 4\do 5\do 6\do 7\do 8\do 9}
\makeatother
\newcommand{\sig}[1]{#1}
\DeclareUrlCommand\sig{\urlstyle{rm}}

\begin{document}
\maketitle
\tableofcontents

\section{0x000 Msg\_Long\_Name\_56789\_123456789\_123456789}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabularx}{1\linewidth}{Xccccc}
\toprule
{\thead{Name}} & {\thead{Type}} & {\thead{Initial}} & {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
\sig{Sig_used_twice_efgh_abcdefghi_abcdefghi_abcdefghi} & int & {--} & {--} & {--} & {--} \\
\sig{rx_twice_short} & int & {--} & {--} & {--} & {--} \\
\bottomrule
\end{tabularx}
\par
\endgroup


\section{0x001 Msg\_Long\_Name\_56789\_123456789\_123456789\_Copy\_1}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabularx}{1\linewidth}{Xccccc}
\toprule
{\thead{Name}} & {\thead{Type}} & {\thead{Initial}} & {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
\sig{Sig_used_twice_efgh_abcdefghi_abcdefghi_abcdefghi} & int & {--} & {--} & {--} & {--} \\
\sig{rx_twice_11111111111111111111111} & int & {--} & {--} & {--} & {--} \\
\bottomrule
\end{tabularx}
\par
\endgroup


\section{0x002 Msg\_with\_value\_table\_sigs}
Base frame \\
DLC = 3

\begingroup
\centering
\begin{tabularx}{1\linewidth}{Xccccc}
\toprule
{\thead{Name}} & {\thead{Type}} & {\thead{Initial}} & {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
\sig{Sig_with_long_val_table_1} & int & {--} & {--} & {--} & {--} \\
\sig{Sig_with_long_val_table_2} & int & {--} & {--} & {--} & {--} \\
\sig{Sig_with_short_val_table} & int & {--} & {--} & {--} & {--} \\
\bottomrule
\end{tabularx}
\par
\endgroup


%\subsection{Comments, allowed values and multiplexing details}
\begin{description}
\item[\sig{Sig_with_long_val_table_1}] ~

	\begin{tabular}{@{}l@{ }l}
	\multicolumn{2}{@{}l}{Allowed values:} \\
	\quad\textbullet~\texttt{0x03} & Description for the value '0x3' \\
	\quad\textbullet~\texttt{0x02} & Description for the value '0x2' \\
	\quad\textbullet~\texttt{0x01} & Description for the value '0x1' \\
	\quad\textbullet~\texttt{0x00} & Description for the value '0x0' \\
	\end{tabular}

\item[\sig{Sig_with_long_val_table_2}] ~

	\begin{tabular}{@{}l@{ }l}
	\multicolumn{2}{@{}l}{Allowed values:} \\
	\quad\textbullet~\texttt{0x0D} & value '0xD' \\
	\quad\textbullet~\texttt{0x0C} & Dvalue '0xC' \\
	\quad\textbullet~\texttt{0x0B} & value '0xB' \\
	\quad\textbullet~\texttt{0x0A} & value '0xA' \\
	\quad\textbullet~\texttt{0x09} & value '0x9' \\
	\quad\textbullet~\texttt{0x08} & value '0x8' \\
	\quad\textbullet~\texttt{0x07} & value '0x7' \\
	\quad\textbullet~\texttt{0x06} & value '0x6' \\
	\quad\textbullet~\texttt{0x05} & value '0x5' \\
	\quad\textbullet~\texttt{0x04} & value '0x4' \\
	\quad\textbullet~\texttt{0x03} & value '0x3' \\
	\quad\textbullet~\texttt{0x02} & value '0x2' \\
	\quad\textbullet~\texttt{0x01} & value '0x1' \\
	\quad\textbullet~\texttt{0x00} & value '0x0' \\
	\end{tabular}

\item[\sig{Sig_with_short_val_table}] ~

	\begin{tabular}{@{}l@{ }l}
	\multicolumn{2}{@{}l}{Allowed values:} \\
	\quad\textbullet~\texttt{0x01} & Very long, long, long description for the value '0x1' \\
	\quad\textbullet~\texttt{0x00} & Very long, long, long description for the value '0x0' \\
	\end{tabular}

\end{description}


\section{0x003 msg\_case\_test}
Base frame \\
DLC = 8

This message has no signals.


\section{0x004 MSG\_CASE\_TEST}
Base frame \\
DLC = 8

This message has no signals.


\section{0x005 RX\_TX\_1}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabularx}{1\linewidth}{Xccccc}
\toprule
{\thead{Name}} & {\thead{Type}} & {\thead{Initial}} & {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
\sig{Sig_used_twice_efgh_abcdefghi_abcdefghi_abcdefghi} & int & {--} & {--} & {--} & {--} \\
\bottomrule
\end{tabularx}
\par
\endgroup


\section{0x006 TX\_twice}
Base frame \\
DLC = 2

\begingroup
\centering
\begin{tabularx}{1\linewidth}{Xccccc}
\toprule
{\thead{Name}} & {\thead{Type}} & {\thead{Initial}} & {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
\sig{rx_twice_short} & int & {--} & {--} & {--} & {--} \\
\sig{rx_twice_long_yyyyyyyyyyyyyyyyyyYYY} & int & {--} & {--} & {--} & {--} \\
\bottomrule
\end{tabularx}
\par
\endgroup


\section{0x055 Msg\_Long\_Name\_56789\_123456789\_123456789\_Copy\_2}
Base frame \\
DLC = 8

\begingroup
\centering
\begin{tabularx}{1\linewidth}{Xccccc}
\toprule
{\thead{Name}} & {\thead{Type}} & {\thead{Initial}} & {\thead{Min}} & {\thead{Max}} & {\thead{Unit}} \\
\midrule
\sig{Sig_used_twice_efgh_abcdefghi_abcdefghi_abcdefghi} & int & {--} & {--} & {--} & {--} \\
\sig{rx_twice_11111111111111111111111} & int & {--} & {--} & {--} & {--} \\
\sig{rx_twice_short} & int & {--} & {--} & {--} & {--} \\
\bottomrule
\end{tabularx}
\par
\endgroup

\end{document}
'''.lstrip('\n')

        with mock.patch('sys.argv', argv):
            cantools._main()

        self.assert_file_content_equal(expected_content, fn_out)


if __name__ == '__main__':
    unittest.main()
