from __future__ import print_function
import re
import time
import curses
import bisect

try:
    from queue import Queue
    from queue import Empty as QueueEmpty
except ImportError:
    from Queue import Queue
    from Queue import Empty as QueueEmpty

import can
from .. import database
from .utils import format_message


class QuitError(Exception):
    pass


class Monitor(can.Listener):

    def __init__(self, stdscr, args):
        self._stdscr = stdscr
        self._dbase = database.load_file(args.database,
                                         encoding=args.encoding,
                                         frame_id_mask=args.frame_id_mask,
                                         strict=not args.no_strict)
        self._filtered_sorted_message_names = []
        self._filter = ''
        self._compiled_filter = None
        self._formatted_messages = {}
        self._playing = True
        self._modified = True
        self._show_filter = False
        self._queue = Queue()
        self._nrows, self._ncols = stdscr.getmaxyx()
        self._received = 0
        self._discarded = 0
        self._basetime = None

        stdscr.nodelay(True)
        curses.use_default_colors()
        curses.curs_set(False)
        curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_GREEN)
        curses.init_pair(2, curses.COLOR_BLACK, curses.COLOR_CYAN)

        bus = self.create_bus(args)
        self._notifier = can.Notifier(bus, [self])

    def create_bus(self, args):
        kwargs = {}

        if args.bit_rate is not None:
            kwargs['bitrate'] = int(args.bit_rate)

        try:
            return can.Bus(bustype=args.bus_type,
                           channel=args.channel,
                           **kwargs)
        except:
            raise Exception(
                "Failed to create CAN bus with bustype='{}' and "
                "channel='{}'.".format(args.bus_type,
                                       args.channel))

    def run(self):
        while True:
            modified = self.update()

            if modified:
                self.redraw()

            try:
                self.process_user_input()
            except QuitError:
                break

            time.sleep(0.05)

    def redraw(self):
        # Clear the screen.
        self._stdscr.clear()

        # Draw everything.
        self.draw_stats(0)
        self.draw_title(1)

        row = 2

        for name in self._filtered_sorted_message_names:
            for line in self._formatted_messages[name]:
                self.addstr(row, 0, line)
                row += 1

                if row > self._nrows - 2:
                    break

        self.draw_menu(self._nrows - 1)

        # Refresh the screen.
        self._stdscr.refresh()

    def draw_stats(self, row):
        self.addstr(row, 0, 'Received: {}, Discarded: {}, Errors: 0'.format(
            self._received,
            self._discarded))

    def draw_title(self, row):
        self.addstr_color(row,
                          0,
                          self.stretch('   TIMESTAMP  MESSAGE'),
                          curses.color_pair(1))

    def draw_menu(self, row):
        if self._show_filter:
            text = 'Filter: ' + self._filter
        else:
            text = 'q: Quit, f: Filter, p: Play/Pause, r: Reset'

        self.addstr_color(row,
                          0,
                          self.stretch(text),
                          curses.color_pair(2))

        if self._show_filter:
            self._stdscr.move(row, len(text))

    def addstr(self, row, col, text):
        try:
            self._stdscr.addstr(row, col, text)
        except curses.error:
            pass

    def addstr_color(self, row, col, text, color):
        try:
            self._stdscr.addstr(row, col, text, color)
        except curses.error:
            pass

    def stretch(self, text):
        return text + ' ' * (self._ncols - len(text))

    def process_user_input(self):
        try:
            key = self._stdscr.getkey()
        except curses.error:
            return

        if self._show_filter:
            self.process_user_input_filter(key)
        else:
            self.process_user_input_menu(key)

    def process_user_input_menu(self, key):
        if key == 'q':
            raise QuitError()
        elif key == 'p':
            self._playing = not self._playing
        elif key == 'r':
            self._playing = True
            self._filtered_sorted_message_names = []
            self._formatted_messages = {}
            self._received = 0
            self._discarded = 0
            self._basetime = None
            self._filter = ''
            self._compiled_filter = None
            self._modified = True

            while not self._queue.empty():
                self._queue.get()
        elif key in ['f', '/']:
            self._show_filter = True
            self._modified = True
            curses.curs_set(True)

    def compile_filter(self):
        try:
            self._compiled_filter = re.compile(self._filter)
        except:
            self._compiled_filter = None

    def process_user_input_filter(self, key):
        if key == '\n':
            self._show_filter = False
            curses.curs_set(False)
        elif key in ['KEY_BACKSPACE', '\b']:
            self._filter = self._filter[:-1]
        else:
            self._filter += key

        self.compile_filter()
        self._filtered_sorted_message_names = []

        for name in self._formatted_messages:
            self.insort_filtered(name)

        self._modified = True

    def try_update_message(self):
        message = self._queue.get_nowait()
        frame_id = message.arbitration_id
        data = message.data
        timestamp = message.timestamp

        if self._basetime is None:
            self._basetime = timestamp

        timestamp -= self._basetime
        self._received += 1

        try:
            message = self._dbase.get_message_by_frame_id(frame_id)
        except KeyError:
            self._discarded += 1
            return

        if len(data) != message.length:
            self._discarded += 1
            return

        formatted = format_message(message, data, True, False)
        lines = formatted.splitlines()
        formatted = ['{:12.3f}  {}'.format(timestamp, lines[1])]
        formatted += [14 * ' ' + line for line in lines[2:]]
        self._formatted_messages[message.name] = formatted

        if message.name not in self._filtered_sorted_message_names:
            self.insort_filtered(message.name)

    def update_messages(self):
        modified = False

        try:
            while True:
                self.try_update_message()
                modified = True
        except QueueEmpty:
            pass

        return modified

    def update(self):
        if self._playing:
            modified = self.update_messages()
        else:
            modified = False

        if self._modified:
            self._modified = False
            modified = True

        if curses.is_term_resized(self._nrows, self._ncols):
            self._nrows, self._ncols = self._stdscr.getmaxyx()
            modified = True

        return modified

    def insort_filtered(self, name):
        if self._compiled_filter is None or self._compiled_filter.search(name):
            bisect.insort(self._filtered_sorted_message_names,
                          name)

    def on_message_received(self, msg):
        self._queue.put(msg)


def _do_monitor(args):
    def monitor(stdscr):
        Monitor(stdscr, args).run()

    try:
        curses.wrapper(monitor)
    except KeyboardInterrupt:
        pass


def add_subparser(subparsers):
    monitor_parser = subparsers.add_parser(
        'monitor',
        description='Monitor CAN bus traffic in a text based user interface.')
    monitor_parser.add_argument(
        '-e', '--encoding',
        help='File encoding.')
    monitor_parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    monitor_parser.add_argument(
        '-m', '--frame-id-mask',
        type=lambda x: int(x, 0),
        help=('Only compare selected frame id bits to find the message in the '
              'database. By default the received and database frame ids must '
              'be equal for a match.'))
    monitor_parser.add_argument(
        '-b', '--bus-type',
        default='socketcan',
        help='Python CAN bus type (default: socketcan).')
    monitor_parser.add_argument(
        '-c', '--channel',
        default='vcan0',
        help='Python CAN bus channel (default: vcan0).')
    monitor_parser.add_argument(
        '-B', '--bit-rate',
        help='Python CAN bus bit rate.')
    monitor_parser.add_argument(
        'database',
        help='Database file.')
    monitor_parser.set_defaults(func=_do_monitor)
