"""Receive multiplexed frames through actual python-can VirtualBus peers."""

from queue import Queue
from time import monotonic, sleep
from uuid import uuid4

import can
import pytest

import cantools

DBC = '''VERSION ""
NS_ :
BS_:
BU_: ECU TESTER
BO_ 100 M: 8 ECU
 SG_ Mux M : 0|8@1+ (1,0) [0|255] "" TESTER
 SG_ A m0 : 8|8@1+ (1,0) [0|255] "" TESTER
 SG_ B m1 : 8|8@1+ (1,0) [0|255] "" TESTER
BO_ 101 Other: 1 ECU
 SG_ Value : 0|8@1+ (1,0) [0|255] "" TESTER
VAL_ 100 B 5 "five";
'''


@pytest.fixture
def receiver():
    database = cantools.database.load_string(DBC, database_format='dbc')
    channel = str(uuid4())
    with can.Bus(interface='virtual', channel=channel) as tester_bus, can.Bus(
        interface='virtual', channel=channel
    ) as ecu_bus:
        testers = []
        received = Queue()

        def start(decode_choices=False):
            tester = cantools.tester.Tester(
                'TESTER', database, tester_bus, strict=False,
                decode_choices=decode_choices, on_message=received.put,
            )
            testers.append(tester)
            tester.start()
            return tester

        def send(name, signals):
            input_queue = testers[0]._input_queue
            queued = input_queue.qsize()
            message = database.get_message_by_name(name)
            ecu_bus.send(can.Message(
                arbitration_id=message.frame_id, is_extended_id=False,
                data=message.encode(signals),
            ))
            decoded = received.get(timeout=2)
            # The callback runs before Listener enqueues the decoded frame.
            # No consumer runs during send(), so observe that enqueue before
            # making zero-timeout assertions about retained/remaining frames.
            deadline = monotonic() + 2
            while input_queue.qsize() <= queued:
                if monotonic() >= deadline:
                    pytest.fail('decoded frame was not enqueued')
                sleep(0.001)
            return decoded

        try:
            yield start, send
        finally:
            for tester in testers:
                tester.stop()
                tester._notifier.stop()


@pytest.mark.parametrize('buffered', [False, True])
@pytest.mark.parametrize('discard', [False, True])
@pytest.mark.parametrize('decode_choices', [False, True])
def test_expect_skips_missing_multiplexed_signal(
    receiver, buffered, discard, decode_choices,
):
    start, send = receiver
    tester = start(decode_choices)
    send('M', {'Mux': 0, 'A': 7})
    send('M', {'Mux': 1, 'B': 4})
    send('M', {'Mux': 1, 'B': 5})
    send('M', {'Mux': 0, 'A': 8})
    if buffered:
        send('Other', {'Value': 9})
        assert tester.expect('Other', timeout=2, discard_other_messages=False) == {
            'Value': 9,
        }
    expected = 'five' if decode_choices else 5
    result = tester.expect(
        'M', {'B': expected}, timeout=2, discard_other_messages=discard,
    )
    assert result == {'Mux': 1, 'B': expected}
    if not discard:
        assert tester.expect('M', timeout=0) == {'Mux': 0, 'A': 7}
        assert tester.expect('M', timeout=0) == {'Mux': 1, 'B': 4}
    assert tester.expect('M', timeout=0) == {'Mux': 0, 'A': 8}
    assert tester.expect('M', timeout=0) is None


@pytest.mark.parametrize('buffered', [False, True])
def test_missing_signal_does_not_match_none(receiver, buffered):
    start, send = receiver
    tester = start()
    send('M', {'Mux': 0, 'A': 7})
    if buffered:
        send('Other', {'Value': 9})
        assert tester.expect('Other', timeout=2, discard_other_messages=False)
    assert tester.expect(
        'M', {'B': None}, timeout=0.05, discard_other_messages=False,
    ) is None
    # An empty signal filter still accepts a frame from either branch.
    assert tester.expect('M', timeout=0) == {'Mux': 0, 'A': 7}


@pytest.mark.parametrize('branch', [0, 1])
def test_empty_signal_filter_accepts_either_branch(receiver, branch):
    start, send = receiver
    tester = start()
    signals = {'Mux': 0, 'A': 7} if branch == 0 else {'Mux': 1, 'B': 5}
    send('M', signals)
    assert tester.expect('M', timeout=2) == signals
