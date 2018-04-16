# The tester module.

import time

try:
    from collections import UserDict
except ImportError:
    from UserDict import UserDict

try:
    from queue import Queue
except ImportError:
    from Queue import Queue

import can


class Error(Exception):
    pass


class TimeoutError(Error):
    pass


class Messages(dict):

    def __missing__(self, key):
        raise Error("invalid message name '{}'".format(key))


class Listener(can.Listener):

    def __init__(self, input_queue):
        self._input_queue = input_queue

    def on_message_received(self, msg):
        if msg.is_error_frame or msg.is_remote_frame:
            return

        self._input_queue.put(msg)


class Message(UserDict, object):

    def __init__(self, database, can_bus, input_queue):
        super(Message, self).__init__()
        self.database = database
        self._can_bus = can_bus
        self._input_queue = input_queue
        self._enabled = True
        self._can_message = None
        self._periodic_task = None
        self.update({signal.name: 0 for signal in database.signals})

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

    @property
    def enabled(self):
        return self._enabled

    @enabled.setter
    def enabled(self, value):
        self._enabled = value

    def send(self, signals=None):
        if signals is not None:
            self.update(signals)

        self._can_bus.send(self._can_message)

    def expect(self, signals=None, timeout=None):
        if signals is None:
            signals = {}

        if timeout is not None:
            end_time = time.time() + timeout

        while True:
            if timeout is not None:
                remaining_time = end_time - time.time()

                if remaining_time <= 0:
                    raise TimeoutError()
            else:
                remaining_time = None

            message = self._input_queue.get(timeout=remaining_time)

            if message is None:
                continue

            if message.arbitration_id != self.database.frame_id:
                continue

            decoded = self.database.decode(message.data)

            if all([decoded[name] == signals[name] for name in signals]):
                return decoded

    def send_periodic_start(self):
        if not self._enabled:
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
        data = self.database.encode(self.data)
        self._can_message = can.Message(arbitration_id=arbitration_id,
                                        extended_id=extended_id,
                                        data=data)

        if self._periodic_task is not None:
            self._periodic_task.modify_data(self._can_message)


class Tester(object):
    """Test the node named `dut_name` on bus `bus_name`. `database` is a
    database object, and `can_bus` a CAN bus object, normally created
    using the python-can package.

    >>> import can
    >>> import cantools
    >>> can.rc['interface'] = 'socketcan_native'
    >>> can.rc['channel'] = 'vcan0'
    >>> can_bus = can.interface.Bus()
    >>> database = cantools.db.load_file('tests/files/tester.kcd')
    >>> tester = cantools.tester.Tester('PeriodicConsumer', database, can_bus, 'PeriodicBus')

    """

    def __init__(self, dut_name, database, can_bus, bus_name=None):
        self._dut_name = dut_name
        self._bus_name = bus_name
        self._database = database
        self._can_bus = can_bus
        self._input_queue = Queue()
        self._notifier = can.Notifier(can_bus,
                                      [Listener(self._input_queue)])
        self._messages = Messages()
        self._is_running = False

        for message in database.messages:
            if message.bus_name == bus_name:
                self._messages[message.name] = Message(message,
                                                       can_bus,
                                                       self._input_queue)

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
        """Stop the tester.

        >>> tester.stop()

        """

        for message in self._messages.values():
            message.send_periodic_stop()

        self._is_running = False

    @property
    def messages(self):
        """Set and get signals in messages. Changed signal values takes effect
        immediately for started periodic messages. Call
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
        """Enable given message `message_name`.

        >>> tester.enable('PeriodicMessage1')

        """

        message = self._messages[message_name]
        message.enabled = True

        if self._is_running and message.periodic:
            message.send_periodic_start()

    def disable(self, message_name):
        """Disable given message `message_name`.

        >>> tester.disable('PeriodicMessage1')

        """

        message = self._messages[message_name]
        message.enabled = False

        if self._is_running and message.periodic:
            message.send_periodic_stop()

    def send(self, message_name, signals=None):
        """Send a message with given name `message_name` and optional signals
        `signals`.

        >>> tester.send('Message1', {'Signal2': 10})
        >>> tester.send('Message1')

        """

        self._messages[message_name].send(signals)

    def expect(self, message_name, signals=None, timeout=None):
        """Expect a message with name `message_name` and signal values
        `signals` within `timeout` seconds. Give `signals` as ``None``
        to expect any signal values. Give `timeout` as ``None`` to
        wait forever.

        >>> tester.expect('Message2', {'Signal1': 13})
        {'Signal1': 13, 'Signal2': 9}

        """

        return self._messages[message_name].expect(signals, timeout)

    def flush_input(self):
        """Flush, or discard, all messages in the input queue.

        """

        while not self._input_queue.empty():
            self._input_queue.get()
