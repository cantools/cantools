# The tester module.

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

        if value and self.periodic:
            self.send_periodic_start()
        else:
            self.send_periodic_stop()

    def send(self, signals=None):
        if signals is not None:
            self.update(signals)

        self._can_bus.send(self._can_message)

    def expect(self, signals=None, timeout=None):
        while True:
            message = self._input_queue.get(timeout=timeout)

            if message is None:
                continue

            if message.arbitration_id != self.database.frame_id:
                continue

            decoded = self.database.decode(message.data)

            if signals is None:
                return decoded

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
        data = self.database.encode(self.data)
        extended_id = self.database.is_extended_frame
        self._can_message = can.Message(arbitration_id=arbitration_id,
                                        extended_id=extended_id,
                                        data=data)

        if self._periodic_task is not None:
            self._periodic_task.modify_data(self._can_message)


class Tester(object):
    """Test the node named `dut_name` on bus `bus_name`. `database` is a
    database object, and `can_bus` a CAN bus object, normally created
    using the python-can package.

    """

    def __init__(self, dut_name, bus_name, database, can_bus):
        self._dut_name = dut_name
        self._bus_name = bus_name
        self._database = database
        self._can_bus = can_bus
        self._input_queue = Queue()
        self._notifier = can.Notifier(can_bus,
                                      [Listener(self._input_queue)])
        self._messages = Messages()

        for message in database.messages:
            if message.bus_name == bus_name:
                self._messages[message.name] = Message(message,
                                                       can_bus,
                                                       self._input_queue)

    def start(self):
        """Start the tester. Starts sending enabled periodic messages.

        """

        for message in self._messages.values():
            if self._dut_name in message.database.senders:
                continue

            if not message.periodic:
                continue

            message.send_periodic_start()

    def stop(self):
        """Stop the tester.

        """

        for message in self._messages.values():
            message.send_periodic_stop()

    @property
    def messages(self):
        """A dictionary-like object of all messages.

        """

        return self._messages

    def enable(self, message_name):
        """Enable given message.

        """

        self._messages[message_name].enabled = True

    def disable(self, message_name):
        """Disable given message.

        """

        self._messages[message_name].enabled = False

    def is_enabled(self, message_name):
        """``True`` is given message is enabled, ``False`` otherwise.

        """

        return self._messages[message_name].enabled

    def send(self, message_name, signals=None):
        """Send a message with given name `message_name` and optional signals
        `signals`.

        >>> tester.send('Foo', {'Bar': 5})

        """

        self._messages[message_name].send(signals)

    def expect(self, message_name, signals=None, timeout=None):
        """Expect a message with name `message_name` and signal values
        `signals`. Give `signals` as ``None`` to expect any signal
        values.

        >>> tester.expect('Foo', {'Bar': 5})

        """

        return self._messages[message_name].expect(signals, timeout)

    def flush_input(self):
        """Flush, or discard, all messages in the message input queue.

        """

        while not self._input_queue.empty():
            self._input_queue.get()
