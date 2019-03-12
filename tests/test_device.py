import unittest
from  cantools.database.can import device
import time

class CanToolsDeviceTest(unittest.TestCase):

    def test_vehicle(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        dev = device.Device(filename, 'virtual', 1, 250000)
        dev1 = device.Device(filename, 'virtual', 1, 250000)
        value = dev1.send_signal_by_message("RT_SB_Speed", {"Validity_Speed":1})
        self.assertEqual(value, 1, "valid signal")
        
        time.sleep(0.01)
        value = dev.get_signal_by_message("RT_SB_Speed", "Validity_Speed")
        self.assertEqual(value, 1, "")
        dev1.send_signal_by_message("RT_SB_Speed", {"Validity_Speed":0})
        time.sleep(0.01)
        value = dev.get_signal_by_message("RT_SB_Speed", "Validity_Speed")
        self.assertEqual(value, 0, "")
        
        
        value = dev.get_signal_by_message("RT_SB_Speed", "Validity_Speed1")
        self.assertEqual(value, None, "invalid signal")
        
        value = dev.send_signal_by_message("RT_SB_Speed1", {"Validity_Speed":0})
        self.assertEqual(value, None, "invalid signal")
        
        with self.assertRaises(Exception) as cm:
            dev = device.Device(filename, 'Vector', 1, 250000) 
        self.assertEqual(
            str(cm.exception),
            "Failed to create CAN bus with bustype='Vector' and channel='1'.")
            
if __name__ == '__main__':
    unittest.main()
