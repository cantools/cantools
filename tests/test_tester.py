import os
import time
import unittest

try:
    from queue import Queue
except ImportError:
    from Queue import Queue

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
        self._periodic_queue.put(message)

        return self

    def wait_for_send_periodic(self):
        return self._periodic_queue.get()

    def recv(self, message, timeout=None):
        while True:
            time.sleep(1)


def setup_tester():
    filename = os.path.join('tests', 'files', 'tester.kcd')
    database = cantools.db.load_file(filename)
    can_bus = CanBus()
    tester = cantools.tester.Tester('PeriodicConsumer',
                                    'PeriodicBus',
                                    database,
                                    can_bus)
    return tester, can_bus


class CanToolsTesterTest(unittest.TestCase):

    def test_periodic_message(self):
        """Test that the periodic message 'Message1' is received by the
        'PeriodicConsumer' node on the 'PeriodicBus' bus.

        """

        tester, can_bus = setup_tester()
        tester.messages['PeriodicMessage1']['Signal1'] = 3
        tester.start()

        message = can_bus.wait_for_send_periodic()
        self.assertEqual(message.arbitration_id, 1)
        self.assertEqual(message.data, b'\x03\x00')

        tester.stop()

    def test_modify_signals(self):
        """Modify signals.

        """

        tester, _ = setup_tester()

        # Message default data.
        self.assertEqual(tester.messages['PeriodicMessage1'],
                         {'Signal1': 0, 'Signal2': 0})

        # Set a single signal.
        tester.messages['PeriodicMessage1']['Signal1'] = 1
        self.assertEqual(tester.messages['PeriodicMessage1']['Signal1'], 1)

        # Set multiple signals at once.
        tester.messages['PeriodicMessage1'].update({'Signal2': 3})
        self.assertEqual(tester.messages['PeriodicMessage1'],
                         {'Signal1': 1, 'Signal2': 3})


if __name__ == '__main__':
    unittest.main()
