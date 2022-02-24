import argparse
import re
import time
import curses
import bisect
import queue

import can
from argparse_addons import Integer
from .. import database
from .__utils__ import format_message
from .__utils__ import format_multiplexed_name


class QuitError(Exception):
    pass


class Monitor(can.Listener):

    def __init__(self, stdscr, args):
        self._stdscr = stdscr
        print(f'Reading bus description file "{args.database}"...\r')
        self._dbase = database.load_file(args.database,
                                         encoding=args.encoding,
                                         frame_id_mask=args.frame_id_mask,
                                         prune_choices=args.prune,
                                         strict=not args.no_strict)
        self._single_line = args.single_line
        self._filtered_sorted_message_names = []
        self._filter = ''
        self._filter_cursor_pos = 0
        self._compiled_filter = None
        self._formatted_messages = {}
        self._playing = True
        self._modified = True
        self._show_filter = False
        self._queue = queue.Queue()
        self._nrows, self._ncols = stdscr.getmaxyx()
        self._received = 0
        self._discarded = 0
        self._basetime = None
        self._page_first_row = 0

        stdscr.keypad(True)
        stdscr.nodelay(True)
        curses.use_default_colors()
        curses.curs_set(False)
        curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_GREEN)
        curses.init_pair(2, curses.COLOR_BLACK, curses.COLOR_CYAN)
        curses.init_pair(3, curses.COLOR_CYAN, curses.COLOR_BLACK)

        bus = self.create_bus(args)
        self._notifier = can.Notifier(bus, [self])

    def create_bus(self, args):
        kwargs = {}

        if args.bit_rate is not None:
            kwargs['bitrate'] = int(args.bit_rate)

        if args.fd:
            kwargs['fd'] = True

        try:
            return can.Bus(bustype=args.bus_type,
                           channel=args.channel,
                           **kwargs)
        except:
            raise Exception(
                "Failed to create CAN bus with bustype='{}' and "
                "channel='{}'.".format(args.bus_type,
                                       args.channel))

    def run(self, max_num_keys_per_tick=-1):
        while True:
            try:
                self.tick(max_num_keys_per_tick)
            except QuitError:
                break

            time.sleep(0.05)

    def tick(self, max_num_keys=-1):
        modified = self.update()

        if modified:
            self.redraw()

        self.process_user_input(max_num_keys)

    def redraw(self):
        # Clear the screen.
        self._stdscr.clear()

        # Draw everything.
        self.draw_stats(0)
        self.draw_title(1)

        lines = []

        for name in self._filtered_sorted_message_names:
            for line in self._formatted_messages[name]:
                lines.append(line)

        # Only render the visible screen. We only have (self._nrows - 3)
        # available rows to draw on, due to the persistent TUI features that
        # are drawn:
        #
        # - line 0: stats
        # - line 1: title
        # - line (n - 1): menu
        num_actual_usable_rows = self._nrows - 2 - 1
        row = 2

        # make sure that we don't overshoot the last line of
        # content. this is a bit of a hack, because manipulation of
        # the controls is not supposed to happen within this method
        if len(lines) < self._page_first_row + num_actual_usable_rows:
            self._page_first_row = max(0, len(lines) - num_actual_usable_rows)

        for line in lines[self._page_first_row:self._page_first_row + num_actual_usable_rows]:
            self.addstr(row, 0, line)
            row += 1

        self.draw_menu(self._nrows - 1)

        # Refresh the screen.
        self._stdscr.refresh()

    def draw_stats(self, row):
        status_text = \
            f'Received: {self._received}, Discarded: {self._discarded}, Errors: 0'
        if self._filter:
            status_text += f', Filter: {self._filter}'
        self.addstr(row, 0, status_text)

    def draw_title(self, row):
        self.addstr_color(row,
                          0,
                          self.stretch('   TIMESTAMP  MESSAGE'),
                          curses.color_pair(1))

    def draw_menu(self, row):
        if self._show_filter:
            col = 0

            # text before cursor
            text = 'Filter regex: ' + self._filter[:self._filter_cursor_pos]
            self.addstr_color(row,
                              col,
                              text,
                              curses.color_pair(2))

            col = len(text)

            # cursor
            if self._filter_cursor_pos >= len(self._filter):
                c = " "
            else:
                c = self._filter[self._filter_cursor_pos]
            self.addstr_color(row,
                              col,
                              c,
                              curses.color_pair(3))
            col += 1

            # text after cursor
            text = self._filter[self._filter_cursor_pos + 1:]
            if len(text) > 0:
                self.addstr_color(row,
                                  col,
                                  text,
                                  curses.color_pair(2))
                col += len(text)

            # fill rest of line
            self.addstr_color(row,
                              col,
                              ' '*(self._ncols - col),
                              curses.color_pair(2))
        else:
            text = 'q: Quit, f: Filter, p: Play/Pause, r: Reset'

            self.addstr_color(row,
                              0,
                              self.stretch(text),
                              curses.color_pair(2))

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

    def process_user_input(self, max_num_keys=-1):
        while max_num_keys < 0 or max_num_keys > 0:
            max_num_keys -= 1
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
            self._page = 0

            while not self._queue.empty():
                self._queue.get()
        elif key in ['f', '/']:
            self._old_filter = self._filter
            self._show_filter = True
            self._filter_cursor_pos = len(self._filter)
            self._modified = True
            curses.curs_set(True)
        elif key in ['KEY_UP']:
            self.line_up()
        elif key in ['KEY_DOWN']:
            self.line_down()
        elif key in ['KEY_PPAGE']:
            self.page_up()
        elif key in ['KEY_NPAGE']:
            self.page_down()

    def line_down(self):
        # Increment line
        self._page_first_row += 1

        self._modified = True

    def line_up(self):
        # Decrement line
        if self._page_first_row > 0:
            self._page_first_row -= 1
        else:
            self._page_first_row = 0

        self._modified = True

    def page_up(self):
        num_actual_usable_rows = self._nrows - 2 - 1

        # Decrement page
        if self._page_first_row > num_actual_usable_rows:
            self._page_first_row -= num_actual_usable_rows
        else:
            self._page_first_row = 0

        self._modified = True

    def page_down(self):
        num_actual_usable_rows = self._nrows - 2 - 1

        # Increment page
        self._page_first_row += num_actual_usable_rows

        self._modified = True

    def compile_filter(self):
        try:
            self._compiled_filter = re.compile(self._filter)
        except:
            self._compiled_filter = None

    def process_user_input_filter(self, key):
        if key == '\n':
            self._show_filter = False
            curses.curs_set(False)
        elif key == chr(27):
            # Escape
            self._show_filter = False
            self._filter = self._old_filter
            del self._old_filter
            curses.curs_set(False)
        elif key in ['KEY_BACKSPACE', '\b']:
            if self._filter_cursor_pos > 0:
                self._filter = \
                    self._filter[:self._filter_cursor_pos - 1] + \
                    self._filter[self._filter_cursor_pos:]
                self._filter_cursor_pos -= 1
        elif key == 'KEY_DC':
            # delete key
            if self._filter_cursor_pos < len(self._filter):
                self._filter = \
                    self._filter[:self._filter_cursor_pos] + \
                    self._filter[self._filter_cursor_pos + 1:]
        elif key == 'KEY_LEFT':
            if self._filter_cursor_pos > 0:
                self._filter_cursor_pos -= 1
        elif key == 'KEY_RIGHT':
            if self._filter_cursor_pos < len(self._filter):
                self._filter_cursor_pos += 1
        elif key in ['KEY_UP']:
            self.line_up()
        elif key in ['KEY_DOWN']:
            self.line_down()
        elif key in ['KEY_PPAGE']:
            self.page_up()
        elif key in ['KEY_NPAGE']:
            self.page_down()
        else:
            # we ignore keys with more than one character here. These
            # (mostly?) are control keys like KEY_UP, KEY_DOWN, etc.
            if len(key) == 1:
                self._filter = \
                    self._filter[:self._filter_cursor_pos] + \
                    key + \
                    self._filter[self._filter_cursor_pos:]
                self._filter_cursor_pos += 1

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

        if message.is_container:
            self._try_update_container(message, timestamp, data)
            return

        if len(data) < message.length:
            formatted = [
                f'{timestamp:12.3f} {message.name} '
                f'( undecoded, {message.length - len(data)} bytes '
                f'too short: 0x{data.hex()} )'
            ]
            self._update_formatted_message(message.name, formatted)
            self._discarded += 1
            return

        name = message.name

        if message.is_multiplexed():
            # Handle the case where a multiplexer index is used that isn't
            # specified in the DBC file (ie. outside of the range). In this
            # case, we just discard the message, like we do when the CAN
            # message ID or length doesn't match what's specified in the DBC.
            try:
                name = format_multiplexed_name(message, data, True)
            except database.DecodeError:
                formatted = [
                    f'{timestamp:12.3f} {message.name} '
                    f'( undecoded: 0x{data.hex()} )'
                ]
                self._update_formatted_message(message.name, formatted)
                self._discarded += 1
                return

        if self._single_line:
            formatted = [
                '{:12.3f} {}'.format(timestamp,
                                     format_message(message, data, True, True))
            ]
        else:
            formatted = format_message(message, data, True, False)
            lines = formatted.splitlines()
            formatted = ['{:12.3f}  {}'.format(timestamp, lines[1])]
            formatted += [14 * ' ' + line for line in lines[2:]]

        self._update_formatted_message(name, formatted)

    def _try_update_container(self, dbmsg, timestamp, data):
        try:
            decoded = dbmsg.decode(data, decode_containers=True)
        except:
            formatted = None
            if self._single_line:
                formatted = [
                    f'{timestamp:12.3f} {dbmsg.name} '
                    f'( undecodable: 0x{data.hex()} )'
                ]
            else:
                formatted = [
                    f'{timestamp:12.3f} {dbmsg.name} (',
                    ' '*14+f'    undecodable: 0x{data.hex()}',
                    ' '*14+f')'
                ]

            self._update_formatted_message(dbmsg.name, formatted)

            self._discarded += 1
            return

        # handle the "table of contents" of the container message. To
        # avoid too much visual turmoil and the resulting usability issues,
        # we always put the contained messages on a single line
        contained_names = []
        for cmsg, _ in decoded:
            if isinstance(cmsg, int):
                tmp = dbmsg.get_contained_message_by_header_id(cmsg)
                cmsg_name = f'0x{cmsg:x}' if tmp is None else tmp.name
            else:
                cmsg_name = cmsg.name

            contained_names.append(cmsg_name)

        formatted = None
        if self._single_line:
            formatted = [
                f'{timestamp:12.3f} {dbmsg.name} (' \
                + ', '.join(contained_names) \
                + ')'
            ]
        else:
            formatted = \
                [ f'{timestamp:12.3f} {dbmsg.name} (' ] + \
                [ 14*' ' +          f'    {x}' for x in contained_names ] + \
                [ 14*' ' +          f')' ]

        self._update_formatted_message(dbmsg.name, formatted)

        # handle the contained messages just as normal messages but
        # prefix their names with the name of the container followed
        # by '.'
        for cmsg, cdata in decoded:
            if isinstance(cmsg, int):
                tmp = dbmsg.get_contained_message_by_header_id(cmsg)
                cmsg_name = f'0x{cmsg:x}' if tmp is None else tmp.name
                full_name = f'{dbmsg.name} :: {cmsg_name}'

                if len(cdata) == 0:
                    cdata_str = f'<empty>'
                else:
                    cdata_str = f'0x{cdata.hex()}'

                formatted = []
                if self._single_line:
                    formatted = [
                        f'{timestamp:12.3f}  {full_name}('
                        f' undecoded: {cdata_str} '
                        f')'
                    ]
                else:
                    formatted = [
                        f'{timestamp:12.3f}  {full_name}(',
                        ' '*14 +            f'    undecoded: {cdata_str}',
                        ' '*14 +            f')',
                    ]

            else:
                full_name = f'{dbmsg.name} :: {cmsg.name}'
                formatted = format_message(cmsg,
                                           data,
                                           True,
                                           False)
                lines = formatted.splitlines()
                formatted = [f'{timestamp:12.3f}  {full_name}(']
                formatted += [14 * ' ' + line for line in lines[2:]]

            self._update_formatted_message(full_name, formatted)

    def _update_formatted_message(self, msg_name, formatted):
        old_formatted = self._formatted_messages.get(msg_name, [])

        # make sure never to decrease the number of lines occupied by
        # a message to avoid jittering
        if len(formatted) < len(old_formatted):
            formatted.extend(['']*(len(old_formatted) - len(formatted)))

        self._formatted_messages[msg_name] = formatted

        if msg_name not in self._filtered_sorted_message_names:
            self.insort_filtered(msg_name)

    def update_messages(self):
        modified = False

        try:
            while True:
                self.try_update_message()
                modified = True
        except queue.Empty:
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
        description='Monitor CAN bus traffic in a text based user interface.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    monitor_parser.add_argument(
        '-s', '--single-line',
        action='store_true',
        help='Print the decoded message on a single line.')
    monitor_parser.add_argument(
        '-e', '--encoding',
        help='File encoding.')
    monitor_parser.add_argument(
        '-m', '--frame-id-mask',
        type=Integer(0),
        help=('Only compare selected frame id bits to find the message in the '
              'database. By default the received and database frame ids must '
              'be equal for a match.'))
    monitor_parser.add_argument(
        '-b', '--bus-type',
        default='socketcan',
        help='Python CAN bus type.')
    monitor_parser.add_argument(
        '-c', '--channel',
        default='vcan0',
        help='Python CAN bus channel.')
    monitor_parser.add_argument(
        '-B', '--bit-rate',
        help='Python CAN bus bit rate.')
    monitor_parser.add_argument(
        '-f', '--fd',
        action='store_true',
        help='Python CAN CAN-FD bus.')
    monitor_parser.add_argument(
        '--prune',
        action='store_true',
        help='Refrain from shortening the names of named signal values.')
    monitor_parser.add_argument(
        '--no-strict',
        action='store_true',
        help='Skip database consistency checks.')
    monitor_parser.add_argument(
        'database',
        help='Database file.')
    monitor_parser.set_defaults(func=_do_monitor)
