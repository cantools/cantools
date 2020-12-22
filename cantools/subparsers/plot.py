import sys
import re
import binascii
import struct
import datetime
from argparse_addons import Integer
from matplotlib import pyplot as plt

from .. import database

#TODO: implement --show-* arguments
#TODO: optionally write result to output file
#TODO: customizable line formats


# Matches 'candump' output, i.e. "vcan0  1F0   [8]  00 00 00 00 00 00 1B C1".
RE_CANDUMP = re.compile(r'^\s*(?:\((?P<time>.*?)\))?\s*\S+\s+(?P<frameid>[0-9A-F]+)\s*\[\d+\]\s*(?P<data>[0-9A-F ]*)(?:\s*::.*)?$')
# Matches 'cantools decode' output, i.e. ")" or "   voltage: 0 V,".
RE_DECODE = re.compile(r'\w+\(|\s+\w+:\s+[0-9.+-]+(\s+.*)?,?|\)')
# Matches 'candump -l' (or -L) output, i.e. "(1594172461.968006) vcan0 1F0#0000000000001BC1"
RE_CANDUMP_LOG = re.compile(r'^\((?P<time>\d+\.\d+)\)\s+\S+\s+(?P<frameid>[\dA-F]+)#(?P<data>[\dA-F]*)$')


def _mo_unpack(mo):
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

    def __init__(self):
        self.use_timestamp = None
        self.unit = None
        self._parse_timestamp = None

    def parse_timestamp(self, timestamp, linenumber):
        if self.use_timestamp is None:
            if timestamp is None:
                self.use_timestamp = False
                return linenumber

            try:
                out = self.parse_iso_timestamp(timestamp)
                self.use_timestamp = True
                self._parse_timestamp = self.parse_iso_timestamp
                return out
            except ValueError:
                pass

            try:
                out = self.parse_seconds(timestamp)
                self.use_timestamp = True
                self.unit = "s"
                self._parse_timestamp = self.parse_seconds
                return out
            except ValueError:
                pass

            self.use_timestamp = False

        if self.use_timestamp:
            return self._parse_timestamp(timestamp)
        else:
            return linenumber

    @staticmethod
    def parse_iso_timestamp(timestamp):
        return datetime.datetime.fromisoformat(timestamp)

    @staticmethod
    def parse_seconds(timestamp):
        return float(timestamp)

    def get_label(self):
        if self.use_timestamp:
            label = "time"
        else:
            label = "line number"

        if self.unit:
            label += " / " + self.unit

        return label

def _do_decode(args):
    dbase = database.load_file(args.database,
                               encoding=args.encoding,
                               frame_id_mask=args.frame_id_mask,
                               strict=not args.no_strict)
    re_format = None
    timestamp_parser = TimestampParser()
    if args.show_invalid_syntax:
        # we cannot use a timestamp if we have failed to parse the line
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

    def __init__(self, dbase, args):
        self.dbase = dbase
        self.decode_choices = not args.no_decode_choices
        self.show_invalid_syntax = args.show_invalid_syntax
        self.show_unknown_frames = args.show_unknown_frames
        self.show_invalid_data = args.show_invalid_data
        self.ignore_invalid_syntax = args.ignore_invalid_syntax
        self.ignore_unknown_frames = args.ignore_unknown_frames
        self.ignore_invalid_data = args.ignore_invalid_data
        self.signals = Signals(args.signals, args.case_sensitive, args.break_time)

        self.x_invalid_syntax = []
        self.x_unknown_frames = []
        self.x_invalid_data = []

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
            print("failed to parse line: %r" % line)

    def plot(self, xlabel):
        self.signals.plot()
        plt.figlegend()
        plt.xlabel(xlabel)
        plt.show()

class Signals:

    SEP_SG = re.escape('.')

    WILDCARD_MANY = re.escape('*')
    WILDCARD_ONE  = re.escape('?')

    def __init__(self, signals, case_sensitive, break_time):
        self.args = signals
        self.signals = []
        self.values = {}
        self.re_flags = 0 if case_sensitive else re.I
        self.break_time = break_time
        self.break_time_uninit = True

        if signals:
            for sg in signals:
                self.add_signal(sg)
        else:
            self.add_signal('*')

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
        signal = re.escape(signal)
        if self.SEP_SG not in signal:
            signal = self.WILDCARD_MANY + self.SEP_SG + signal
        signal = signal.replace(self.WILDCARD_MANY, '.*')
        signal = signal.replace(self.WILDCARD_ONE, '.')
        signal += '$'
        reo = re.compile(signal, self.re_flags)
        self.signals.append(reo)

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
        for reo in self.signals:
            if reo.match(signal):
                return True
        return False

    def plot(self):
        for sg in self.values:
            x = self.values[sg].x
            y = self.values[sg].y
            plt.plot(x, y, label=sg)

class Graph:

    __slots__ = ('x', 'y')

    def __init__(self):
        self.x = []
        self.y = []


def add_subparser(subparsers):
    decode_parser = subparsers.add_parser(
        'plot',
        description=('Decode "candump" CAN frames or the output of cantools decode '
                     'read from standard input and plot them using matplotlib.'))
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
        help='Show a marker for lines which could not be parsed.')
    decode_parser.add_argument(
        '--show-unknown-frames',
        action='store_true',
        help='Show a marker for messages which are not contained in the database file.')
    decode_parser.add_argument(
        '--show-invalid-data',
        action='store_true',
        help='Show a marker for messages with data which could not be parsed.')

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
        'database',
        help='Database file.')
    decode_parser.add_argument(
        'signals',
        nargs='*',
        help='The signals to be plotted.')
    decode_parser.set_defaults(func=_do_decode)
