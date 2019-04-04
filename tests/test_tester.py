import time
import os
import unittest
import can

try:
    from queue import Queue
    from queue import Empty
except ImportError:
    from Queue import Queue
    from Queue import Empty

import cantools


class CanBus(object):

    def __init__(self):
        self.channel_info = None
        self._queue = Queue()
        self._periodic_queue = Queue()
        self._input_queue = Queue()
        self._periodic_stop_queue = Queue()

    def stop(self):
        self._periodic_stop_queue.put(None)

    def wait_for_periodic_stop(self):
        return self._periodic_stop_queue.get()

    def send(self, message):
        self._queue.put(message)

    def wait_for_send(self):
        return self._queue.get()

    def send_periodic(self, message, period=None):
        self._periodic_queue.put((message, period))

        return self

    def wait_for_send_periodic(self, timeout=None):
        return self._periodic_queue.get(timeout=timeout)

    def recv(self, timeout=None):
        try:
            return self._input_queue.get(timeout=timeout)
        except Empty:
            return None

    def input_message(self, message):
        self._input_queue.put(message)


def setup_tester(dut_name,
                 on_message=None,
                 decode_choices=False,
                 scaling=False):
    database = cantools.db.load_file('tests/files/kcd/tester.kcd')
    can_bus = CanBus()
    tester = cantools.tester.Tester(dut_name,
                                    database,
                                    can_bus,
                                    'Bus1',
                                    on_message=on_message,
                                    decode_choices=decode_choices,
                                    scaling=scaling)

    return tester, can_bus


class CanToolsTesterTest(unittest.TestCase):

    def test_periodic_message_modify_signal_before_start(self):
        """Test that send_periodic() is called after start(), even if signals
        are modified before start().

        """

        tester, can_bus = setup_tester('Node2')

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
        can_bus.wait_for_periodic_stop()

    def test_set_and_get_signals(self):
        """Set and get signals.

        """

        tester, _ = setup_tester('Node1')

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
        """Test enable and disable functionality for a periodic message.

        """

        tester, can_bus = setup_tester('Node2')

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

        tester.disable('PeriodicMessage1')
        can_bus.wait_for_periodic_stop()

        tester.stop()

    def test_expect(self):
        """Test the expect method.

        """

        tester, can_bus = setup_tester('Node1')
        tester.start()

        # Input the three messages.
        can_bus.input_message(can.Message(arbitration_id=0x101, data=b'\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x101, data=b'\x00\x01'))
        can_bus.input_message(can.Message(arbitration_id=0x101, data=b'\x02\x03'))

        # Expect Message1 with no filtering.
        message = tester.expect('Message1')
        self.assertEqual(message, {'Signal1': 0, 'Signal2': 0})

        # Expect Message1 with Signal1 filtering.
        message = tester.expect('Message1', {'Signal1': 0})
        self.assertEqual(message, {'Signal1': 0, 'Signal2': 1})

        # Expect Message1 with Signal1 and Signal2 filtering.
        message = tester.expect('Message1', {'Signal1': 2, 'Signal2': 3})
        self.assertEqual(message, {'Signal1': 2, 'Signal2': 3})

        # Expect Message1 with timeout.
        message = tester.expect('Message1', timeout=0.5)
        self.assertIsNone(message)

        # Expect with timeout, with Message2 discarded when expecting Message1.
        can_bus.input_message(can.Message(arbitration_id=0x102, data=b'\x00\x00\x00'))
        message = tester.expect('Message1', timeout=0.5)
        self.assertIsNone(message)
        message = tester.expect('Message2', timeout=0.0)
        self.assertIsNone(message)

        # Expect with timeout 0.0 with wrong message in queue.
        can_bus.input_message(can.Message(arbitration_id=0x102, data=b'\x00\x00\x00'))
        time.sleep(0.1)
        message = tester.expect('Message1', timeout=0.0)
        self.assertIsNone(message)

        # Expect with discard_other_messages set to False.
        can_bus.input_message(can.Message(arbitration_id=0x102, data=b'\x03\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x102, data=b'\x04\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x101, data=b'\x05\x00'))
        message = tester.expect('Message1', discard_other_messages=False)
        self.assertEqual(message, {'Signal1': 5, 'Signal2': 0})
        message = tester.expect('Message1', timeout=0.0, discard_other_messages=False)
        self.assertIsNone(message)
        message = tester.expect('Message2', discard_other_messages=False)
        self.assertEqual(message, {'Signal1': 3, 'Signal2': 0, 'Signal3': 0})
        message = tester.expect('Message2', discard_other_messages=False)
        self.assertEqual(message, {'Signal1': 4, 'Signal2': 0, 'Signal3': 0})
        message = tester.expect('Message1', timeout=0.0)
        self.assertIsNone(message)
        message = tester.expect('Message2', timeout=0.0)
        self.assertIsNone(message)

        tester.stop()

    def test_flush_input(self):
        """Test the flush_input method.

        """

        tester, can_bus = setup_tester('Node1')
        tester.start()

        can_bus.input_message(can.Message(arbitration_id=0x101, data=b'\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x102, data=b'\x00\x00\x00'))
        time.sleep(0.1)
        self.assertIsNone(tester.flush_input())
        message = tester.expect('Message1', timeout=0.0)
        self.assertIsNone(message)

        tester.stop()

    def test_send(self):
        """Test the send method.

        """

        tester, can_bus = setup_tester('Node1')
        tester.start()

        # Without signals.
        tester.send('Message1')
        message = can_bus.wait_for_send()
        self.assertEqual(message.arbitration_id, 0x101)
        self.assertEqual(message.data, b'\x00\x00')

        # With a signal.
        tester.send('Message1', {'Signal1': 1})
        message = can_bus.wait_for_send()
        self.assertEqual(message.arbitration_id, 0x101)
        self.assertEqual(message.data, b'\x01\x00')

        # Without signals again.
        tester.send('Message1')
        message = can_bus.wait_for_send()
        self.assertEqual(message.arbitration_id, 0x101)
        self.assertEqual(message.data, b'\x01\x00')

        tester.stop()

    def test_send_with_scaling(self):
        """Test the send method with scaling.

        """

        tester, can_bus = setup_tester('Node1', scaling=True)
        tester.start()

        tester.send('Message1')
        message = can_bus.wait_for_send()
        self.assertEqual(message.arbitration_id, 0x101)
        self.assertEqual(message.data, b'\x0a\x00')

        tester.stop()

    def test_bad_message_name(self):
        """Try to send an unknown message.

        """

        tester, _ = setup_tester('Node1')
        tester.start()

        with self.assertRaises(cantools.tester.Error) as cm:
            tester.send('MessageMissing')

        self.assertEqual(str(cm.exception),
                         "invalid message name 'MessageMissing'")

        tester.stop()

    def test_bad_dut_name(self):
        """The DUT must exist in the database.

        """

        database = cantools.db.load_file('tests/files/kcd/tester.kcd')
        can_bus = CanBus()

        with self.assertRaises(cantools.tester.Error) as cm:
            cantools.tester.Tester('BadDut', database, can_bus, 'Bus1')

        self.assertEqual(str(cm.exception),
                         "expected DUT name in ['Node1', 'Node2'], but got 'BadDut'")

    def test_bad_bus_name(self):
        """The bus must exist in the database.

        """

        database = cantools.db.load_file('tests/files/kcd/tester.kcd')
        can_bus = CanBus()

        with self.assertRaises(cantools.tester.Error) as cm:
            cantools.tester.Tester('Node1', database, can_bus, 'BadBus')

        self.assertEqual(str(cm.exception),
                         "expected bus name in ['Bus1'], but got 'BadBus'")

    def test_bus_name_none(self):
        """The bus name should be None if no bus is given in the database.

        """

        database = cantools.db.load_file('tests/files/dbc/foobar.dbc')
        can_bus = CanBus()

        with self.assertRaises(cantools.tester.Error) as cm:
            cantools.tester.Tester('FOO', database, can_bus, 'BadBus')

        self.assertEqual(str(cm.exception),
                         "expected bus name in ['TheBusName'], but got 'BadBus'")

    def test_on_message(self):
        """Test the on_message callback.

        """

        message_queue = Queue()

        def on_message(decoded_message):
            message_queue.put(decoded_message)

        tester, can_bus = setup_tester('Node1', on_message)
        tester.disable('Message2')
        tester.start()

        # Bad message id.
        can_bus.input_message(can.Message(arbitration_id=0x7ff, data=b'\x00\x00'))

        # Disabled message.
        can_bus.input_message(can.Message(arbitration_id=0x102, data=b'\x00\x00\x00'))

        # Good message
        can_bus.input_message(can.Message(arbitration_id=0x101, data=b'\x00\x00'))

        # Check that only the good message was passed to on_message().
        decoded_message = message_queue.get()
        self.assertEqual(decoded_message.name, 'Message1')
        self.assertEqual(decoded_message.signals, {'Signal1': 0, 'Signal2': 0})
        self.assertTrue(message_queue.empty())

        tester.stop()


if __name__ == '__main__':
    unittest.main()
