import os
import time
import unittest

try:
    from queue import Queue
    from queue import Empty
except ImportError:
    from Queue import Queue
    from Queue import Empty

import cantools


class CanBus(object):

    def __init__(self):
        self._queue = Queue()
        self._periodic_queue = Queue()

    def stop(self):
        pass

    def send(self, message):
        self._queue.put(message)

    def wait_for_send(self):
        return self._queue.get()

    def send_periodic(self, message, period=None):
        self._periodic_queue.put((message, period))

        return self

    def wait_for_send_periodic(self, timeout=None):
        return self._periodic_queue.get(timeout=timeout)

    def recv(self, message, timeout=None):
        while True:
            time.sleep(1)


def setup_tester():
    filename = os.path.join('tests', 'files', 'tester.kcd')
    database = cantools.db.load_file(filename)
    can_bus = CanBus()
    tester = cantools.tester.Tester('PeriodicConsumer',
                                    'Bus1',
                                    database,
                                    can_bus)

    return tester, can_bus


class CanToolsTesterTest(unittest.TestCase):

    def test_periodic_message_modify_signal_before_start(self):
        """Test that send_periodic() is called after start(), even if signals
        are modified before start().

        """

        tester, can_bus = setup_tester()

        tester.messages['PeriodicMessage1']['Signal1'] = 3

        # Periodic messages should not be sent before start().
        with self.assertRaises(Empty):
            can_bus.wait_for_send_periodic(timeout=0.5)

        tester.start()

        message, period = can_bus.wait_for_send_periodic()
        self.assertEqual(message.arbitration_id, 1)
        self.assertEqual(message.data, b'\x03\x00')
        self.assertEqual(period, 0.05)

        tester.stop()

    def test_set_and_get_signals(self):
        """Set and get signals.

        """

        tester, _ = setup_tester()

        # Check message default data.
        self.assertEqual(tester.messages['Message2'],
                         {'Signal1': 0, 'Signal2': 0, 'Signal3': 0})

        # Set a single signal.
        tester.messages['Message2']['Signal1'] = 1
        self.assertEqual(tester.messages['Message2'],
                         {'Signal1': 1, 'Signal2': 0, 'Signal3': 0})

        # Set multiple signals at once.
        tester.messages['Message2'].update({'Signal2': 3, 'Signal3': 7})
        self.assertEqual(tester.messages['Message2'],
                         {'Signal1': 1, 'Signal2': 3, 'Signal3': 7})

    def test_enable_disable(self):
        """Test the enable and disable methods for a periodic message.

        """

        tester, can_bus = setup_tester()

        tester.enable('PeriodicMessage1')
        tester.disable('PeriodicMessage1')
        tester.start()

        with self.assertRaises(Empty):
            can_bus.wait_for_send_periodic(timeout=0.5)

        tester.enable('PeriodicMessage1')

        message, period = can_bus.wait_for_send_periodic()
        self.assertEqual(message.arbitration_id, 1)
        self.assertEqual(message.data, b'\x00\x00')
        self.assertEqual(period, 0.05)

        tester.stop()


if __name__ == '__main__':
    unittest.main()
