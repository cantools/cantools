import struct
import time
import unittest
from queue import Empty, Queue

import can
from parameterized import parameterized

import cantools
from cantools.database.can import Message, Signal
from cantools.database.conversion import BaseConversion


class CanBus(can.BusABC):

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
    database = cantools.database.load_file('tests/files/kcd/tester.kcd')
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
        can_bus.input_message(can.Message(arbitration_id=0x101, is_extended_id=False, data=b'\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x101, is_extended_id=False, data=b'\x00\x01'))
        can_bus.input_message(can.Message(arbitration_id=0x101, is_extended_id=False, data=b'\x02\x03'))

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
        can_bus.input_message(can.Message(arbitration_id=0x102, is_extended_id=False, data=b'\x00\x00\x00'))
        message = tester.expect('Message1', timeout=0.5)
        self.assertIsNone(message)
        message = tester.expect('Message2', timeout=0.0)
        self.assertIsNone(message)

        # Expect with timeout 0.0 with wrong message in queue.
        can_bus.input_message(can.Message(arbitration_id=0x102, is_extended_id=False, data=b'\x00\x00\x00'))
        time.sleep(0.1)
        message = tester.expect('Message1', timeout=0.0)
        self.assertIsNone(message)

        # Expect with discard_other_messages set to False.
        can_bus.input_message(can.Message(arbitration_id=0x102, is_extended_id=False, data=b'\x03\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x102, is_extended_id=False, data=b'\x04\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x101, is_extended_id=False, data=b'\x05\x00'))
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

        can_bus.input_message(can.Message(arbitration_id=0x101, is_extended_id=False, data=b'\x00\x00'))
        can_bus.input_message(can.Message(arbitration_id=0x102, is_extended_id=False, data=b'\x00\x00\x00'))
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

        database = cantools.database.load_file('tests/files/kcd/tester.kcd')
        can_bus = CanBus()

        with self.assertRaises(cantools.tester.Error) as cm:
            cantools.tester.Tester('BadDut', database, can_bus, 'Bus1')

        self.assertEqual(str(cm.exception),
                         "expected DUT name in ['Node1', 'Node2'], but got 'BadDut'")

    def test_bad_bus_name(self):
        """The bus must exist in the database.

        """

        database = cantools.database.load_file('tests/files/kcd/tester.kcd')
        can_bus = CanBus()

        with self.assertRaises(cantools.tester.Error) as cm:
            cantools.tester.Tester('Node1', database, can_bus, 'BadBus')

        self.assertEqual(str(cm.exception),
                         "expected bus name in ['Bus1'], but got 'BadBus'")

    def test_bus_name_none(self):
        """The bus name should be None if no bus is given in the database.

        """

        database = cantools.database.load_file('tests/files/dbc/foobar.dbc')
        can_bus = CanBus()

        with self.assertRaises(cantools.tester.Error) as cm:
            cantools.tester.Tester('FOO', database, can_bus, 'BadBus')

        self.assertEqual(str(cm.exception),
                         "expected bus name in ['TheBusName'], but got 'BadBus'")

    def test_signal_init_out_range(self):
        """The bus name should be None if no bus is given in the database.

        """

        database = cantools.database.load_file('tests/files/dbc/issue_725.dbc')
        can_bus = CanBus()

        with self.assertRaises(cantools.tester.Error) as cm:
            cantools.tester.Tester("Node1",
                                    database,
                                    can_bus,
                                    'test',
                                    on_message=None,
                                    decode_choices=False,
                                    scaling=False)

        self.assertEqual(str(cm.exception),
                         'Expected signal "Signal1" value smaller than or equal to 250 in message "TestMessage", but got 254.')
        try:
            cantools.tester.Tester("Node1",
                                    database,
                                    can_bus,
                                    'test',
                                    on_message=None,
                                    decode_choices=False,
                                    scaling=False,
                                    strict=False)
        except Exception as e:
            self.fail(f"Unexpected exception raised: {e}")

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
        can_bus.input_message(can.Message(arbitration_id=0x7ff, is_extended_id=False, data=b'\x00\x00'))

        # Disabled message.
        can_bus.input_message(can.Message(arbitration_id=0x102, is_extended_id=False, data=b'\x00\x00\x00'))

        # Good message
        can_bus.input_message(can.Message(arbitration_id=0x101, is_extended_id=False, data=b'\x00\x00'))

        # Check that only the good message was passed to on_message().
        decoded_message = message_queue.get()
        self.assertEqual(decoded_message.name, 'Message1')
        self.assertEqual(decoded_message.signals, {'Signal1': 0, 'Signal2': 0})
        self.assertTrue(message_queue.empty())

        tester.stop()


class InitialValuesTest(unittest.TestCase):

    def make_message(self, signal, scaling, decode_choices=False):
        database = Message(0x123, 'Initial', signal.length // 8, [signal])
        bus = CanBus()
        message = cantools.tester.Message(
            database, bus, [], Queue(), decode_choices, scaling, False)
        return message, bus

    @parameterized.expand([
        (name + str(scaling), raw, scale, offset, signed, physical,
         expected, minimum, scaling)
        for name, raw, scale, offset, signed, physical, expected, minimum in [
            ('positive', 3, 2, 10, False, 16, b'\x03', None),
            ('raw_zero', 0, 2, 10, False, 10, b'\x00', 10),
            ('physical_zero', 3, 2, -6, False, 0, b'\x03', None),
            ('signed', -2, 2, 10, True, 6, b'\xfe', None),
            ('fractional', 3, 0.5, 0, False, 1.5, b'\x03', None),
        ]
        for scaling in (False, True)
    ])
    def test_declared_initial(self, name, raw, scale, offset, signed,
                              physical, expected, minimum, scaling):
        signal = Signal(
            's', 0, 8, is_signed=signed, raw_initial=raw,
            minimum=minimum,
            conversion=BaseConversion.factory(scale=scale, offset=offset))
        message, bus = self.make_message(signal, scaling)
        self.assertEqual(message['s'], physical if scaling else raw, name)
        message.send()
        self.assertEqual(bytes(bus.wait_for_send().data), expected, name)

    @parameterized.expand([
        (scaling, decode_choices)
        for scaling in (False, True)
        for decode_choices in (False, True)
    ])
    def test_choice_initial(self, scaling, decode_choices):
        signal = Signal(
            's', 0, 8, raw_initial=0,
            conversion=BaseConversion.factory(
                scale=2, offset=10, choices={0: 'Off', 3: 'On'}))
        message, bus = self.make_message(signal, scaling, decode_choices)
        self.assertEqual(message['s'], signal.initial if scaling else 0)
        message.send()
        self.assertEqual(bytes(bus.wait_for_send().data), b'\x00')

    def test_raw_initial_keeps_large_integer_precision(self):
        raw = (1 << 60) + 7
        signal = Signal(
            's', 0, 64, raw_initial=raw,
            conversion=BaseConversion.factory(scale=0.5))
        message, bus = self.make_message(signal, scaling=False)
        self.assertEqual(message['s'], raw)
        message.send()
        self.assertEqual(bytes(bus.wait_for_send().data), struct.pack('<Q', raw))

    def test_raw_float_initial(self):
        signal = Signal(
            's', 0, 32, raw_initial=1.5,
            conversion=BaseConversion.factory(scale=2, offset=4, is_float=True))
        message, bus = self.make_message(signal, scaling=False)
        self.assertEqual(message['s'], 1.5)
        message.send()
        self.assertEqual(bytes(bus.wait_for_send().data), struct.pack('<f', 1.5))

    @parameterized.expand([
        (scaling, minimum)
        for scaling in (False, True)
        for minimum in (None, 10)
    ])
    def test_missing_initial(self, scaling, minimum):
        signal = Signal('s', 0, 8, minimum=minimum, maximum=255)
        message, bus = self.make_message(signal, scaling)
        expected = 0 if minimum is None else minimum
        self.assertEqual(message['s'], expected)
        message.send()
        self.assertEqual(bytes(bus.wait_for_send().data), bytes([expected]))


if __name__ == '__main__':
    unittest.main()
