# unit tests for the autosar specifics of message and database objects
# (message.autosar and db.autosar)
import unittest
import traceback

import cantools
import cantools.autosar

class CanToolsAutosarTest(unittest.TestCase):
    def test_autosar3_e2e_profile2(self):
        db = cantools.db.load_file('tests/files/arxml/system-3.2.3.arxml')

        msg = db.get_message_by_name('Status')

        # verify the parameters
        self.assertTrue(msg.autosar is not None)
        self.assertTrue(msg.autosar.e2e is not None)
        self.assertEqual(msg.autosar.e2e.category, 'p2')
        self.assertEqual(msg.autosar.e2e.data_ids, list(range(0,16)))

        # test the crc calculation
        did = msg.autosar.e2e.data_ids
        self.assertEqual(cantools.autosar.compute_profile2_crc(b'\x00\x51\x22',
                                                               did[5]),
                         173)
        self.assertEqual(cantools.autosar.compute_profile2_crc(b'\x00\x51\x22',
                                                               msg),
                         173)


        # test the apply function
        self.assertEqual(cantools.autosar.apply_profile2_crc(b'\xff',
                                                             msg),
                         None)
        self.assertEqual(cantools.autosar.apply_profile2_crc(b'\xff\x51\x22',
                                                             msg),
                         b'\xad\x51\x22')

        # test the check function
        self.assertFalse(cantools.autosar.check_profile2_crc(b'\xff',
                                                             msg))
        self.assertFalse(cantools.autosar.check_profile2_crc(b'\x00\x51\x22',
                                                             msg))
        self.assertTrue(cantools.autosar.check_profile2_crc(b'\xad\x51\x22',
                                                            msg))

    def test_autosar4_e2e_profile5(self):
        db = cantools.db.load_file('tests/files/arxml/system-4.2.arxml')

        msg = db.get_message_by_name('Message3')

        # verify the parameters
        self.assertTrue(msg.autosar is not None)
        self.assertTrue(msg.autosar.is_secured)
        self.assertEqual(msg.autosar.secured_payload_length, 4)
        self.assertTrue(msg.autosar.e2e is not None)
        self.assertEqual(msg.autosar.e2e.category, 'Profile5')
        self.assertEqual(msg.autosar.e2e.data_ids, [321])

        # test the crc calculation
        self.assertEqual(cantools.autosar.compute_profile5_crc(
            b'\x00\x00\x11\x22', msg.autosar.e2e.data_ids[0]),
                         12201)
        self.assertEqual(cantools.autosar.compute_profile5_crc(
            b'\xff\xff\x11\x22\x33\x44\x55', msg),
                         12201
                         )

        # test  the apply function
        self.assertEqual(cantools.autosar.apply_profile5_crc(b'\xff\x51\x22',
                                                             msg),
                         None)
        self.assertEqual(cantools.autosar.apply_profile5_crc(b'\xff\xee\x22\x33',
                                                             msg),
                         b'\xcf\xec"3')

        # test the check function
        self.assertTrue(cantools.autosar.check_profile5_crc(b'\xcf\xec"3',
                                                             msg))
        self.assertFalse(cantools.autosar.check_profile5_crc(b'\xcf\xec"4',
                                                             msg))
        self.assertFalse(cantools.autosar.check_profile5_crc(b'\xcf\xec"',
                                                             msg))

    def test_autosar4_e2e(self):
        db = cantools.db.load_file('tests/files/arxml/system-4.2.arxml')

        # verify the E2E parameters for a message that uses profile 5
        msg = db.get_message_by_name('Message3')
        self.assertTrue(msg.autosar is not None)
        self.assertTrue(msg.autosar.e2e is not None)
        self.assertEqual(msg.autosar.e2e.category, 'Profile5')
        self.assertEqual(msg.autosar.e2e.data_ids, [321])

        # verify the E2E parameters for a message that uses profile 2
        msg = db.get_message_by_name('Message1')
        self.assertTrue(msg.autosar is not None)
        self.assertTrue(msg.autosar.e2e is not None)
        self.assertEqual(msg.autosar.e2e.category, 'Profile2')
        self.assertEqual(msg.autosar.e2e.data_ids, list(range(123, 123+16)))

        # verify the E2E parameters for a contained message
        cmsg = db.get_message_by_name('OneToContainThemAll')
        msg = cmsg.get_contained_message_by_name('message1')
        self.assertTrue(msg.autosar is not None)
        self.assertFalse(msg.autosar.is_secured)
        self.assertEqual(msg.autosar.secured_payload_length, 0)
        self.assertTrue(msg.autosar.e2e is not None)
        self.assertEqual(msg.autosar.e2e.category, 'Profile2')
        self.assertEqual(msg.autosar.e2e.data_ids, list(range(123, 123+16)))

        # verify the E2E parameters for a contained secured message
        cmsg = db.get_message_by_name('OneToContainThemAll')
        msg = cmsg.get_contained_message_by_name('message3_secured')
        self.assertTrue(msg.autosar is not None)
        self.assertTrue(msg.autosar.is_secured)
        self.assertEqual(msg.autosar.secured_payload_length, 4)
        self.assertTrue(msg.autosar.e2e is not None)
        self.assertEqual(msg.autosar.e2e.category, 'Profile5')
        self.assertEqual(msg.autosar.e2e.data_ids, [321])

if __name__ == '__main__':
    unittest.main()
