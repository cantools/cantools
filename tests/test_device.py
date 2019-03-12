import unittest
from  cantools.database.can import device
import time

class CanToolsDeviceTest(unittest.TestCase):

    def test_vehicle(self):
        filename = 'tests/files/dbc/vehicle.dbc'
        dev = device.Device(filename, 'virtual', 1, 250000)
        dev1 = device.Device(filename, 'virtual', 1, 250000)
        dev1.send_signal_by_message("RT_SB_Speed", {"Validity_Speed":1})
        time.sleep(0.01)
        value = dev.get_signal_by_message("RT_SB_Speed", "Validity_Speed")
        self.assertEqual(value, 1, "")
        dev1.send_signal_by_message("RT_SB_Speed", {"Validity_Speed":0})
        time.sleep(0.01)
        value = dev.get_signal_by_message("RT_SB_Speed", "Validity_Speed")
        self.assertEqual(value, 0, "")
        
        try:
            value = dev.get_signal_by_message("RT_SB_Speed", "Validity_Speed1")
        except KeyError:
            pass
        
        try:
            dev.send_signal_by_message("RT_SB_Speed1", {"Validity_Speed":0})
        except KeyError:
            pass
        
        try:
            dev = device.Device(filename, 'Vector', 1, 250000) 
        except :
            pass       
if __name__ == '__main__':
    unittest.main()
