#!/usr/bin/env python3

import os
import shutil
import tempfile
import re
import datetime

import unittest
from unittest import mock

import cantools


class CanToolsConvertFullTest(unittest.TestCase):

    DBC_FILE_CHOICES = os.path.join(os.path.split(__file__)[0], 'files/dbc/choices.dbc')
    DBC_FILE_MULTIPLEX = os.path.join(os.path.split(__file__)[0], 'files/dbc/multiplex_2.dbc')
    DBC_FILE_SIGNED = os.path.join(os.path.split(__file__)[0], 'files/dbc/signed.dbc')

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


if __name__ == '__main__':
    unittest.main()
