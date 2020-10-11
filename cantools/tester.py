# The tester module.

import time

try:
    from collections import UserDict
except ImportError:
    from UserDict import UserDict

try:
    import queue
except ImportError:
    import Queue as queue

import can

from .errors import Error


class DecodedMessage(object):
    """A decoded message.

    """

    def __init__(self, name, signals):
        self.name = name
        self.signals = signals


class Messages(dict):

    def __missing__(self, key):
        raise Error("invalid message name '{}'".format(key))


class Listener(can.Listener):

    def __init__(self, database, messages, input_queue, on_message):
        self._database = database
        self._messages = messages
        self._input_queue = input_queue
        self._on_message = on_message

    def on_message_received(self, msg):
        if msg.is_error_frame or msg.is_remote_frame:
            return

        try:
            database_message = self._database.get_message_by_frame_id(
                msg.arbitration_id)
        except KeyError:
            return

        if database_message.name not in self._messages:
            return

        message = self._messages[database_message.name]

        if not message.enabled:
            return

        decoded = DecodedMessage(database_message.name,
                                 database_message.decode(msg.data,
                                                         message.decode_choices,
                                                         message.scaling))

        if self._on_message:
            self._on_message(decoded)

        self._input_queue.put(decoded)


class Message(UserDict, object):

    def __init__(self,
                 database,
                 can_bus,
                 input_list,
                 input_queue,
                 decode_choices,
                 scaling,
                 padding):
        super(Message, self).__init__()
        self.database = database
        self._can_bus = can_bus
        self._input_queue = input_queue
        self.decode_choices = decode_choices
        self.scaling = scaling
        self.padding = padding
        self._input_list = input_list
        self.enabled = True
        self._can_message = None
        self._periodic_task = None
        self.update(self._prepare_initial_signal_values())

    @property
    def periodic(self):
        return self.database.cycle_time is not None

    def __getitem__(self, signal_name):
        return self.data[signal_name]

    def __setitem__(self, signal_name, value):
        self.data[signal_name] = value
        self._update_can_message()

    def update(self, signals):
        self.data.update(signals)
        self._update_can_message()

    def send(self, signals=None):
        if signals is not None:
            self.update(signals)

        self._can_bus.send(self._can_message)

    def expect(self, signals=None, timeout=None, discard_other_messages=True):
        if signals is None:
            signals = {}

        decoded = self._expect_input_list(signals, discard_other_messages)

        if decoded is None:
            decoded = self._expect_input_queue(signals,
                                               timeout,
                                               discard_other_messages)

        return decoded

    def _expect_input_list(self, signals, discard_other_messages):
        other_messages = []

        while len(self._input_list) > 0:
            message = self._input_list.pop(0)
            decoded = self._filter_expected_message(message, signals)

            if decoded is not None:
                break

            other_messages.append(message)
        else:
            decoded = None

        if not discard_other_messages:
            other_messages += self._input_list
            del self._input_list[:]
            self._input_list.extend(other_messages)

        return decoded

    def _expect_input_queue(self, signals, timeout, discard_other_messages):
        if timeout is not None:
            end_time = time.time() + timeout
            remaining_time = timeout
        else:
            remaining_time = None

        while True:
            try:
                message = self._input_queue.get(timeout=remaining_time)
            except queue.Empty:
                return

            decoded = self._filter_expected_message(message, signals)

            if decoded is not None:
                return decoded

            if not discard_other_messages:
                self._input_list.append(message)

            if timeout is not None:
                remaining_time = end_time - time.time()

                if remaining_time <= 0:
                    return

    def _filter_expected_message(self, message, signals):
        if message.name == self.database.name:
            if all([message.signals[name] == signals[name] for name in signals]):
                return message.signals

    def send_periodic_start(self):
        if not self.enabled:
            return

        self._periodic_task = self._can_bus.send_periodic(
            self._can_message,
            self.database.cycle_time / 1000.0)

    def send_periodic_stop(self):
        if self._periodic_task is not None:
            self._periodic_task.stop()
            self._periodic_task = None

    def _update_can_message(self):
        arbitration_id = self.database.frame_id
        extended_id = self.database.is_extended_frame
        data = self.database.encode(self.data,
                                    self.scaling,
                                    self.padding)
        self._can_message = can.Message(arbitration_id=arbitration_id,
                                        extended_id=extended_id,
                                        data=data)

        if self._periodic_task is not None:
            self._periodic_task.modify_data(self._can_message)

    def _prepare_initial_signal_values(self):
        initial_sig_values = dict()
        for signal in self.database.signals:
            minimum = 0 if not signal.minimum else signal.minimum
            maximum = 0 if not signal.maximum else signal.maximum
            if signal.initial:
                # use initial signal value (if set)
                initial_sig_values[signal.name] = signal.initial
            elif minimum <= 0 <= maximum:
                # use 0 if in allowed range
                initial_sig_values[signal.name] = 0
            else:
                # set at least some default value
                initial_sig_values[signal.name] = minimum
        return initial_sig_values


class Tester(object):
    """Test given node `dut_name` on given CAN bus `bus_name`.

    `database` is a :class:`~cantools.database.can.Database` instance.

    `can_bus` a CAN bus object, normally created using the python-can
    package.

    The `on_message` callback is called for every successfully decoded
    received message. It is called with one argument, an
    :class:`~cantools.tester.DecodedMessage` instance.

    Here is an example of how to create a tester:

    >>> import can
    >>> import cantools
    >>> can.rc['interface'] = 'socketcan'
    >>> can.rc['channel'] = 'vcan0'
    >>> can_bus = can.interface.Bus()
    >>> database = cantools.database.load_file('tests/files/tester.kcd')
    >>> tester = cantools.tester.Tester('PeriodicConsumer', database, can_bus, 'PeriodicBus')

    """

    def __init__(self,
                 dut_name,
                 database,
                 can_bus,
                 bus_name=None,
                 on_message=None,
                 decode_choices=True,
                 scaling=True,
                 padding=False):
        self._dut_name = dut_name
        self._bus_name = bus_name
        self._database = database
        self._can_bus = can_bus
        self._input_list = []
        self._input_queue = queue.Queue()
        self._messages = Messages()
        self._is_running = False

        # DUT name validation.
        node_names = [node.name for node in database.nodes]

        if not any([name == dut_name for name in node_names]):
            raise Error(
                "expected DUT name in {}, but got '{}'".format(node_names,
                                                               dut_name))

        # BUS name validation.
        bus_names = [bus.name for bus in database.buses]

        if len(bus_names) == 0:
            if bus_name is not None:
                raise Error(
                    "expected bus name None as there are no buses defined in "
                    "the database, but got '{}'".format(bus_name))
        elif not any([name == bus_name for name in bus_names]):
            raise Error(
                "expected bus name in {}, but got '{}'".format(bus_names,
                                                               bus_name))

        for message in database.messages:
            if message.bus_name == bus_name:
                self._messages[message.name] = Message(message,
                                                       can_bus,
                                                       self._input_list,
                                                       self._input_queue,
                                                       decode_choices,
                                                       scaling,
                                                       padding)

        listener = Listener(self._database,
                            self._messages,
                            self._input_queue,
                            on_message)
        self._notifier = can.Notifier(can_bus, [listener])

    def start(self):
        """Start the tester. Starts sending enabled periodic messages.

        >>> tester.start()

        """

        for message in self._messages.values():
            if self._dut_name in message.database.senders:
                continue

            if not message.periodic:
                continue

            message.send_periodic_start()

        self._is_running = True

    def stop(self):
        """Stop the tester. Periodic messages will not be sent after this
        call. Call :meth:`~cantools.tester.Tester.start()` to resume a
        stopped tester.

        >>> tester.stop()

        """

        for message in self._messages.values():
            message.send_periodic_stop()

        self._is_running = False

    @property
    def messages(self):
        """Set and get signals in messages. Set signals takes effect
        immediately for started enabled periodic messages. Call
        :meth:`~cantools.tester.Tester.send()` for other messages.

        >>> periodic_message = tester.messages['PeriodicMessage1']
        >>> periodic_message
        {'Signal1': 0, 'Signal2': 0}
        >>> periodic_message['Signal1'] = 1
        >>> periodic_message.update({'Signal1': 2, 'Signal2': 5})
        >>> periodic_message
        {'Signal1': 2, 'Signal2': 5}

        """

        return self._messages

    def enable(self, message_name):
        """Enable given message `message_name` and start sending it if its
        periodic and the tester is running.

        >>> tester.enable('PeriodicMessage1')

        """

        message = self._messages[message_name]
        message.enabled = True

        if self._is_running and message.periodic:
            message.send_periodic_start()

    def disable(self, message_name):
        """Disable given message `message_name` and stop sending it if its
        periodic, enabled and the tester is running.

        >>> tester.disable('PeriodicMessage1')

        """

        message = self._messages[message_name]
        message.enabled = False

        if self._is_running and message.periodic:
            message.send_periodic_stop()

    def send(self, message_name, signals=None):
        """Send given message `message_name` and optional signals `signals`.

        >>> tester.send('Message1', {'Signal2': 10})
        >>> tester.send('Message1')

        """

        self._messages[message_name].send(signals)

    def expect(self,
               message_name,
               signals=None,
               timeout=None,
               discard_other_messages=True):
        """Expect given message `message_name` and signal values `signals`
        within `timeout` seconds.

        Give `signals` as ``None`` to expect any signal values.

        Give `timeout` as ``None`` to wait forever.

        Messages are read from the input queue, and those not matching
        given `message_name` and `signals` are discarded if
        `discard_other_messages` is
        ``True``. :meth:`~cantools.tester.Tester.flush_input()` may be
        called to discard all old messages in the input queue before
        calling the expect function.

        Returns the expected message, or ``None`` on timeout.

        >>> tester.expect('Message2', {'Signal1': 13})
        {'Signal1': 13, 'Signal2': 9}

        """

        return self._messages[message_name].expect(signals,
                                                   timeout,
                                                   discard_other_messages)

    def flush_input(self):
        """Flush, or discard, all messages in the input queue.

        """

        del self._input_list[:]

        while not self._input_queue.empty():
            self._input_queue.get()
