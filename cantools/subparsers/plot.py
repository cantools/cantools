
'''
Decode "candump" CAN frames or the output of "cantools decode"
read from standard input and plot them using matplotlib.
You can select which signals to plot by specifying them on the command line.
Each signal is one argument and has the pattern "[bo.]sg[:fmt]"
where bo is the name of the message, sg is the name of the signal
and fmt is the format of the graph.
The wildcards * (any number of any character)
and ? (exactly one arbitrary character)
can be used inside of sg and bo.
If bo is omitted it defaults to *.

fmt is passed to matplotlib and can be used to specify
the line style, markers and color.
For example the following values can be combined:
Line style:
    '-'  solid line style,
    '--' dashed line style,
    '-.' dash-dot line style and
    ':'  dotted line style.
Markers:
    '.' point marker,
    ',' pixel marker,
    'o' circle marker,
    's' square marker,
    'D' diamond marker,
    'x' x marker
    and many more.
Colors:
    'b' blue,
    'g' green,
    'r' red,
    'c' cyan,
    'm' magenta,
    'y' yellow,
    'k' black and
    'w' white.
https://matplotlib.org/2.1.2/api/_as_gen/matplotlib.pyplot.plot.html.

If the first character of fmt is a '|' stem is used instead of plot.
'''

import sys
import re
import binascii
import struct
import datetime
import argparse
from argparse_addons import Integer
try:
    from matplotlib import pyplot as plt
except ImportError:
    print("matplotlib package not installed. Required for producing plots.")

from .. import database


plt.rcParams["date.autoformatter.hour"] = "%H:%M"
plt.rcParams["date.autoformatter.minute"] = "%H:%M"
plt.rcParams["date.autoformatter.microsecond"] = "%H:%M:%S.%f"


# Matches 'candump' output, i.e. "vcan0  1F0   [8]  00 00 00 00 00 00 1B C1".
RE_CANDUMP = re.compile(r'^\s*(?:\((?P<time>.*?)\))?\s*\S+\s+(?P<frameid>[0-9A-F]+)\s*\[\d+\]\s*(?P<data>[0-9A-F ]*)(?:\s*::.*)?$')
# Matches 'cantools decode' output, i.e. ")" or "   voltage: 0 V,".
RE_DECODE = re.compile(r'\w+\(|\s+\w+:\s+[0-9.+-]+(\s+.*)?,?|\)')
# Matches 'candump -l' (or -L) output, i.e. "(1594172461.968006) vcan0 1F0#0000000000001BC1"
RE_CANDUMP_LOG = re.compile(r'^\((?P<time>\d+\.\d+)\)\s+\S+\s+(?P<frameid>[\dA-F]+)#(?P<data>[\dA-F]*)$')


def _mo_unpack(mo):
    '''extract the data from a re match object'''
    timestamp = mo.group('time')
    frame_id = mo.group('frameid')
    frame_id = '0' * (8 - len(frame_id)) + frame_id
    frame_id = binascii.unhexlify(frame_id)
    frame_id = struct.unpack('>I', frame_id)[0]
    data = mo.group('data')
    data = data.replace(' ', '')
    data = binascii.unhexlify(data)

    return timestamp, frame_id, data

class TimestampParser:

    '''
    Parses the values for the horizontal axis
    and generates the corresponding axis label.
    Preferably timestamps are used but if none
    are given it falls back to line numbers.
    '''

    # candump -ta, -tz and -td have the same timestamp syntax: a floating number in seconds.
    # In case of -td using timestamps does not seem useful and a user should use --line-numbers.
    # The following constant shall distinguish between -ta and -tz.
    # If the first timestamp is bigger than THRESHOLD_ABSOLUTE_SECONDS I am assuming -ta is used
    # and convert timestamps to datetime objects which will print a date.
    # Otherwise I'll assume -tz is used and format them using timedelta objects.
    # I am not using zero to compare against in case the beginning of the log file is stripped.
    THRESHOLD_ABSOLUTE_SECONDS = 60*60*24*7

    FORMAT_ABSOLUTE_TIMESTAMP = "%Y-%m-%d %H:%M:%S.%f"

    def __init__(self):
        self.use_timestamp = None
        self.relative = None
        self._parse_timestamp = None
        self.first_timestamp = None

    def parse_timestamp(self, timestamp, linenumber):
        if self.use_timestamp is None:
            if timestamp is None:
                self.use_timestamp = False
                return linenumber

            try:
                out = self.parse_absolute_timestamp(timestamp)
                self.use_timestamp = True
                self.relative = False
                self.first_timestamp = out
                self._parse_timestamp = self.parse_absolute_timestamp
                return out
            except ValueError:
                pass

            try:
                if float(timestamp) > self.THRESHOLD_ABSOLUTE_SECONDS:
                    out = self.parse_absolute_seconds(timestamp)
                    self.relative = False
                    self.first_timestamp = out
                    self._parse_timestamp = self.parse_absolute_seconds
                else:
                    out = self.parse_seconds(timestamp)
                    self.relative = True
                    self._parse_timestamp = self.parse_seconds

                self.use_timestamp = True
                return out
            except ValueError:
                pass

            self.use_timestamp = False

        if self.use_timestamp:
            return self._parse_timestamp(timestamp)
        else:
            return linenumber

    def parse_absolute_timestamp(self, timestamp):
        return datetime.datetime.strptime(timestamp, self.FORMAT_ABSOLUTE_TIMESTAMP)

    @staticmethod
    def parse_absolute_seconds(timestamp):
        return datetime.datetime.fromtimestamp(float(timestamp))

    @staticmethod
    def parse_seconds(timestamp):
        return float(timestamp)

    def get_label(self):
        if self.use_timestamp:
            if self.relative:
                label = "relative time"
            else:
                label = "absolute time"
        else:
            label = "line number"

        if isinstance(self.first_timestamp, datetime.datetime):
            label += self.first_timestamp.strftime(" (start: %d.%m.%Y)")

        return label

def _do_decode(args):
    '''
    The entry point of the program.
    It iterates over all input lines, parses them
    and passes the data to a Plotter object.
    '''
    if args.show_errors:
        args.show_invalid_syntax = True
        args.show_unknown_frames = True
        args.show_invalid_data = True
    if args.quiet:
        args.ignore_invalid_syntax = True
        args.ignore_unknown_frames = True
        args.ignore_invalid_data = True

    dbase = database.load_file(args.database,
                               encoding=args.encoding,
                               frame_id_mask=args.frame_id_mask,
                               strict=not args.no_strict)
    re_format = None
    timestamp_parser = TimestampParser()
    if args.show_invalid_syntax:
        # we cannot use a timestamp if we have failed to parse the line
        timestamp_parser.use_timestamp = False
    if args.line_numbers:
        timestamp_parser.use_timestamp = False

    plotter = Plotter(dbase, args)

    line_number = 1
    while True:
        line = sys.stdin.readline()

        # Break at EOF.
        if not line:
            break

        line = line.strip('\r\n')
        if not line:
            continue

        # Auto-detect on first valid line.
        if re_format is None:
            mo = RE_CANDUMP.match(line)

            if mo:
                re_format = RE_CANDUMP
            else:
                mo = RE_CANDUMP_LOG.match(line)

                if mo:
                    re_format = RE_CANDUMP_LOG
        else:
            mo = re_format.match(line)

        if mo:
            timestamp, frame_id, data = _mo_unpack(mo)
            timestamp = timestamp_parser.parse_timestamp(timestamp, line_number)
            plotter.add_msg(timestamp, frame_id, data)
        elif RE_DECODE.match(line):
            continue
        else:
            plotter.failed_to_parse_line(line_number, line)

        line_number += 1

    plotter.plot(timestamp_parser.get_label())


class Plotter:

    '''
    Decodes the data received from _do_decode further
    and stores them in a Signals object.
    Shows or exports the data plotted by Signals.
    '''

    # ------- initialization -------

    def __init__(self, dbase, args):
        self.dbase = dbase
        self.decode_choices = not args.no_decode_choices
        self.show_invalid_syntax = args.show_invalid_syntax
        self.show_unknown_frames = args.show_unknown_frames
        self.show_invalid_data = args.show_invalid_data
        self.ignore_invalid_syntax = args.ignore_invalid_syntax
        self.ignore_unknown_frames = args.ignore_unknown_frames
        self.ignore_invalid_data = args.ignore_invalid_data
        self.output_filename = args.output_file
        self.signals = Signals(args.signals, args.case_sensitive, args.break_time)

        self.x_invalid_syntax = []
        self.x_unknown_frames = []
        self.x_invalid_data = []

    # ------- while reading data -------

    def add_msg(self, timestamp, frame_id, data):
        try:
            message = self.dbase.get_message_by_frame_id(frame_id)
        except KeyError:
            if self.show_unknown_frames:
                self.x_unknown_frames.append(timestamp)
            if not self.ignore_unknown_frames:
                print('Unknown frame id {0} (0x{0:x})'.format(frame_id))
            return

        try:
            decoded_signals = message.decode(data, self.decode_choices)
        except Exception as e:
            if self.show_invalid_data:
                self.x_invalid_data.append(timestamp)
            if not self.ignore_invalid_data:
                print('Failed to parse data of frame id {0} (0x{0:x}): {1}'.format(frame_id, e))
            return

        for signal in decoded_signals:
            x = timestamp
            y = decoded_signals[signal]
            signal = message.name + '.' + signal
            self.signals.add_value(signal, x, y)

    def failed_to_parse_line(self, timestamp, line):
        if self.show_invalid_syntax:
            self.x_invalid_syntax.append(timestamp)
        if not self.ignore_invalid_syntax:
            print("Failed to parse line: %r" % line)

    # ------- at end -------

    def plot(self, xlabel):
        self.signals.plot(xlabel, self.x_invalid_syntax, self.x_unknown_frames, self.x_invalid_data)
        if self.output_filename:
            plt.savefig(self.output_filename)
            print("Result written to %s" % self.output_filename)
        else:
            plt.show()

class Signals:

    '''
    Parses the command line options which signals should be plotted
    and saves the corresponding values in Graph objects.
    Automatically inserts None values as specified by break_time.
    Plots the values using matplotlib.pyplot.
    '''

    # before re.escape
    SEP_SUBPLOT = '-'

    SEP_FMT = ':'
    FMT_STEM = '|'

    # after re.escape
    SEP_SG = re.escape('.')

    WILDCARD_MANY = re.escape('*')
    WILDCARD_ONE  = re.escape('?')

    COLOR_INVALID_SYNTAX = '#ff0000'
    COLOR_UNKNOWN_FRAMES = '#ffab00'
    COLOR_INVALID_DATA   = '#ff00ff'
    ERROR_LINEWIDTH = 1

    # ------- initialization -------

    def __init__(self, signals, case_sensitive, break_time):
        self.args = signals
        self.signals = []
        self.values = {}
        self.re_flags = 0 if case_sensitive else re.I
        self.break_time = break_time
        self.break_time_uninit = True
        self.subplot = 1

        if signals:
            for sg in signals:
                self.add_signal(sg)
        else:
            self.add_signal('*')
        self.compile_reo()

    def init_break_time(self, datatype):
        if self.break_time <= 0:
            self.break_time = None
        elif datatype == datetime.datetime:
            self.half_break_time = datetime.timedelta(seconds=self.break_time/2)
            self.break_time = datetime.timedelta(seconds=self.break_time)
        else:
            self.half_break_time = self.break_time / 2
        self.break_time_uninit = False

    def add_signal(self, signal):
        if signal == self.SEP_SUBPLOT:
            self.subplot += 1
            return

        if self.SEP_FMT in signal:
            signal, fmt = signal.split(self.SEP_FMT, 1)
            if fmt.startswith(self.FMT_STEM):
                fmt = fmt[len(self.FMT_STEM):]
                plt_func = 'stem'
            else:
                plt_func = 'plot'
        else:
            fmt = ''
            plt_func = 'plot'

        signal = re.escape(signal)
        if self.SEP_SG not in signal:
            signal = self.WILDCARD_MANY + self.SEP_SG + signal
        signal = signal.replace(self.WILDCARD_MANY, '.*')
        signal = signal.replace(self.WILDCARD_ONE, '.')
        signal += '$'
        reo = re.compile(signal, self.re_flags)

        sgo = Signal(reo, self.subplot, plt_func, fmt)
        self.signals.append(sgo)

    def compile_reo(self):
        self.reo = re.compile('|'.join(sg.reo.pattern for sg in self.signals), re.I)

    # ------- while reading data -------

    def add_value(self, signal, x, y):
        if not self.is_displayed_signal(signal):
            return

        if signal not in self.values:
            graph = Graph()
            self.values[signal] = graph
        else:
            graph = self.values[signal]
            last_x = graph.x[-1]
            if self.break_time_uninit:
                self.init_break_time(type(x))
            if self.break_time and last_x + self.break_time < x:
                x_break = last_x + self.half_break_time
                graph.x.append(x_break)
                graph.y.append(None)
        graph.x.append(x)
        graph.y.append(y)

    def is_displayed_signal(self, signal):
        return self.reo.match(signal)

    # ------- at end -------

    def plot(self, xlabel, x_invalid_syntax, x_unknown_frames, x_invalid_data):
        splot = None
        last_subplot = 0
        axis_format_uninitialized = True
        sorted_signal_names = sorted(self.values.keys())
        for sgo in self.signals:
            if sgo.subplot > last_subplot:
                last_subplot = sgo.subplot
                if splot is None:
                    axes = None
                else:
                    axes = splot.axes
                    splot.legend()
                    splot.set_xlabel(xlabel)
                splot = plt.subplot(self.subplot, 1, sgo.subplot, sharex=axes)

            plotted = False
            for signal_name in sorted_signal_names:
                graph = self.values[signal_name]
                if not sgo.match(signal_name):
                    continue
                if graph.plotted_signal:
                    if not self.is_replotting_desired(sgo, graph.plotted_signal):
                        continue
                else:
                    graph.plotted_signal = sgo

                x = graph.x
                y = graph.y
                if axis_format_uninitialized and x:
                    if isinstance(x[0], float):
                        splot.axes.xaxis.set_major_formatter(lambda x,pos: str(datetime.timedelta(seconds=x)))
                    axis_format_uninitialized = False
                getattr(splot, sgo.plt_func)(x, y, sgo.fmt, label=signal_name)
                plotted = True

            if not plotted:
                print("WARNING: signal %r with format %r was not plotted." % (sgo.reo.pattern, sgo.fmt))

        self.plot_error(splot, x_invalid_syntax, 'invalid syntax', self.COLOR_INVALID_SYNTAX)
        self.plot_error(splot, x_unknown_frames, 'unknown frames', self.COLOR_UNKNOWN_FRAMES)
        self.plot_error(splot, x_invalid_data, 'invalid data', self.COLOR_INVALID_DATA)

        splot.legend()
        splot.set_xlabel(xlabel)

    def plot_error(self, splot, xs, label, color):
        if xs:
            label += " (%s)" % len(xs)
            xs = iter(xs)
            splot.axvline(next(xs), color=color, linewidth=self.ERROR_LINEWIDTH, label=label)
            for x in xs:
                splot.axvline(x, color=color, linewidth=self.ERROR_LINEWIDTH)

    def is_replotting_desired(self, current_signal, previously_plotted_signal):
        if current_signal.reo.pattern == previously_plotted_signal.reo.pattern:
            # if the user bothers to type out the same regex twice
            # it is probably intended to be plotted twice
            return True
        if '.' not in current_signal.reo.pattern:
            # if the user bothers to type out a complete signal name without wildcards
            # he/she probably means to plot this signal even if it has been plotted already
            return True

        return False


class Signal:

    '''
    Stores meta information about signals to be plotted:
    - a regex matching all signals it refers to
    - the format how it should be plotted
    - the subplot in which to display the signal

    It does *not* store the values to be plotted.
    They are stored in Graph.
    Signal and Graph have a one-to-many-relationship.
    '''

    # ------- initialization -------

    def __init__(self, reo, subplot, plt_func, fmt):
        self.reo = reo
        self.subplot = subplot
        self.plt_func = plt_func
        self.fmt = fmt

    # ------- while reading data -------

    def match(self, signal):
        return self.reo.match(signal)

class Graph:

    '''
    A container for the values to be plotted.
    The corrsponding signal names are the keys in Signals.values.
    The format how to plot this data is stored in Signals.signals (a list of Signal objects).

    plotted_signal stores a Signal object with which this graph has been plotted already
    to avoid undesired replotting of the same data in case the user gives two regex
    matching the same signal, one more specific to match a certain signal with a special format
    and one more generic matching the rest with another format.
    '''

    __slots__ = ('x', 'y', 'plotted_signal')

    def __init__(self):
        self.x = []
        self.y = []
        self.plotted_signal = None


def add_subparser(subparsers):
    '''
    Is called from ../__init__.py.
    It adds the options for this subprogram to the argparse parser.
    It sets the entry point for this subprogram by setting a default values for func.
    '''
    decode_parser = subparsers.add_parser(
        'plot',
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    decode_parser.add_argument(
        '-c', '--no-decode-choices',
        action='store_true',
        help='Do not convert scaled values to choice strings.')
    decode_parser.add_argument(
        '-e', '--encoding',
        help='File encoding of dbc file.')
    decode_parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    decode_parser.add_argument(
        '-m', '--frame-id-mask',
        type=Integer(0),
        help=('Only compare selected frame id bits to find the message in the '
              'database. By default the candump and database frame ids must '
              'be equal for a match.'))
    decode_parser.add_argument(
        '-I', '--case-sensitive',
        action='store_true',
        help='Match the signal names case sensitive.')
    decode_parser.add_argument(
        '-l', '--line-numbers',
        action='store_true',
        help='Use line numbers instead of time stamps on the horizontal axis (useful with `candump -td`).')
    decode_parser.add_argument(
        '-t', '--break-time',
        default=100,
        type=float,
        help=('If the time distance between two consecutive signals is longer than this value '
              'the line in the plot will be interrupted. The value is given in seconds '
              '(if timestamps are used) or input lines (if line numbers are used). '
              '-1 means infinite. '))

    decode_parser.add_argument(
        '--show-invalid-syntax',
        action='store_true',
        help='Show a marker for lines which could not be parsed. This implies -l.')
    decode_parser.add_argument(
        '--show-unknown-frames',
        action='store_true',
        help='Show a marker for messages which are not contained in the database file.')
    decode_parser.add_argument(
        '--show-invalid-data',
        action='store_true',
        help='Show a marker for messages with data which could not be parsed.')
    decode_parser.add_argument(
        '-s', '--show-errors',
        action='store_true',
        help='Show all error messages in the plot. This is an abbreviation for all --show-* options. This implies -l.')

    decode_parser.add_argument(
        '--ignore-invalid-syntax',
        action='store_true',
        help='Don\'t print an error message for lines which could not be parsed.')
    decode_parser.add_argument(
        '--ignore-unknown-frames',
        action='store_true',
        help='Don\'t print an error message for messages which are not contained in the database file.')
    decode_parser.add_argument(
        '--ignore-invalid-data',
        action='store_true',
        help='Don\'t print an error message for messages with data which could not be parsed.')
    decode_parser.add_argument(
        '-q', '--quiet',
        action='store_true',
        help='Don\'t print any error messages. This is an abbreviation for all --ignore-* options.')

    decode_parser.add_argument(
        '-o', '--output-file',
        help='A file to write the plot to instead of displaying it in a window.')

    decode_parser.add_argument(
        'database',
        help='Database file.')
    decode_parser.add_argument(
        'signals',
        nargs='*',
        help='The signals to be plotted.')
    decode_parser.set_defaults(func=_do_decode)
