import unittest
from cantools.database import Message, Signal
from cantools.database.utils import create_encode_decode_formats, decode_data, encode_data, CompositeFormats

class CanToolsOverlappingSignalsTest(unittest.TestCase):

    def test_overlapping_signals(self):
        # Define signals that overlap
        # Signal A: bits 0-7
        # Signal B: bits 4-11 (overlaps A by 4 bits)
        
        sig_a = Signal(name='SignalA', start=0, length=8, byte_order='little_endian')
        sig_b = Signal(name='SignalB', start=4, length=8, byte_order='little_endian')
        
        # Message length: 2 bytes (16 bits)
        # Signal A uses [0, 8)
        # Signal B uses [4, 12)
        
        signals = [sig_a, sig_b]
        msg_length = 2
        
        # 1. Test Formats Creation
        formats = create_encode_decode_formats(signals, msg_length)
        
        # Expecting a CompositeFormats object because of overlap
        self.assertIsInstance(formats, CompositeFormats)
        self.assertEqual(len(formats.codecs), 2)
        
        # 2. Test Encoding
        # Signal A = 0xFF (1111 1111) -> occupies bits 0-7
        # Signal B = 0x0  (0000 0000) -> occupies bits 4-11
        
        # Overlap region: bits 4-7. 
        # Signal A sets them to 1. Signal B sets them to 0.
        # Merged result should have 1s because OR logic.
        
        # However, checking a simpler separate case first.
        # Signal A: 0x0F (0000 1111) -> bits 0-3 set. No overlap with B's significant bits if B is shifted?
        # A occupies 0-7. B occupies 4-11.
        
        # Let's try to set specific bits.
        # A = 0x01 (bit 0 set)
        # B = 0x01 (bit 4 set)
        
        data = {'SignalA': 1, 'SignalB': 1}
        encoded = encode_data(data, signals, formats, scaling=False)
        
        # Result should be:
        # bit 0 of Byte 0 = 1 (from A)
        # bit 4 of Byte 0 = 1 (from B)
        # So Byte 0 should be 0x11.
        # cantools encode_data returns an integer where MSB corresponds to Byte 0.
        # So we expect 0x1100.
        self.assertEqual(encoded, 0x1100)
        
        # Now test actual conflict
        # A = 0xF0 (1111 0000) -> bits 4-7 set
        # B = 0x00 (0000 0000) -> bits 4-11 clear
        # Result should preserve A's 1s in the overlap region
        data_conflict = {'SignalA': 0xF0, 'SignalB': 0}
        encoded_conflict = encode_data(data_conflict, signals, formats, scaling=False)
        
        # Bits 4-7 set -> 0xF0 at Byte 0.
        # Expect 0xF000.
        self.assertEqual(encoded_conflict, 0xF000)
        
        # 3. Test Decoding
        # We start with bytes that have data in both fields
        # Bytes: 0xFF 0xFF (all 1s)
        payload = b'\xff\xff'
        
        decoded = decode_data(payload, msg_length, signals, formats, decode_choices=False, scaling=False, allow_truncated=False, allow_excess=False)
        
        self.assertEqual(decoded['SignalA'], 0xFF)
        self.assertEqual(decoded['SignalB'], 0xFF)
        
        # Test distinct simple values
        # Payload: 0x11 (0001 0001)
        # Signal A (bits 0-7) should read 0x11
        # Signal B (bits 4-11) -> read bits 4-11.
        # 0x11 = 0000 0000 0001 0001
        # Bits 4-11 are 0000 0001 -> 1
        payload_simple = b'\x11\x00'
        decoded_simple = decode_data(payload_simple, msg_length, signals, formats, decode_choices=False, scaling=False, allow_truncated=False, allow_excess=False)
        
        self.assertEqual(decoded_simple['SignalA'], 0x11)
        self.assertEqual(decoded_simple['SignalB'], 0x01)

    def test_non_overlapping_behavior_preserved(self):
        # Define signals that DO NOT overlap
        sig_a = Signal(name='SignalA', start=0, length=4, byte_order='little_endian')
        sig_b = Signal(name='SignalB', start=4, length=4, byte_order='little_endian')
        
        signals = [sig_a, sig_b]
        msg_length = 1
        
        formats = create_encode_decode_formats(signals, msg_length)
        
        # Expecting Standard Formats object, NOT CompositeFormats
        # (Assuming the optimization logic still returns a single Formats object for single group)
        # Actually my implementation wraps even a single group in standard Formats, 
        # but returns it directly if len(groups) == 1.
        
        # Check type is NOT CompositeFormats by checking it does not have 'codecs' attribute 
        # or checking class name if strictly imported.
        self.assertNotIsInstance(formats, CompositeFormats)
        
        data = {'SignalA': 0xF, 'SignalB': 0xA}
        # A=1111 (0-3), B=1010 (4-7) -> 1010 1111 -> 0xAF
        encoded = encode_data(data, signals, formats, scaling=False)
        self.assertEqual(encoded, 0xAF)
        
        payload = b'\xAF'
        decoded = decode_data(payload, msg_length, signals, formats, decode_choices=False, scaling=False, allow_truncated=False, allow_excess=False)
        self.assertEqual(decoded['SignalA'], 0xF)
        self.assertEqual(decoded['SignalB'], 0xA)


if __name__ == '__main__':
    unittest.main()
