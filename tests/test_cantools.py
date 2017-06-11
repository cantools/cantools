import os
import unittest
import bitstruct
import cantools


class CanToolsTest(unittest.TestCase):

    def test_vehicle(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'vehicle.dbc')
        db.add_dbc_file(filename)
        self.assertEqual(len(db.ecus), 1)
        self.assertEqual(db.ecus[0].name, 'Vector__XXX')
        self.assertEqual(len(db.messages), 217)
        self.assertEqual(db.messages[216].frame_id, 155872546)
        self.assertEqual(str(db.messages[0]),
                         'message("RT_SB_INS_Vel_Body_Axes", 0x9588322, 8, None)')
        self.assertEqual(repr(db.messages[0].signals[0]),
                         'signal("INS_Vel_Sideways_2D", 40, 24, "little_endian", '
                         '"True", 0.0001, 0, -838, 838, "m/s", None, '
                         '"Sideways Velocity in the vehicle body axes, 2D (no '
                         'vertical component) .  +ve for motion to the vehicle '
                         'RHS.")')
        self.assertEqual(repr(db.ecus[0]), 'ecu("Vector__XXX", None)')
        i = 0

        for message in db.messages:
            for signal in message.signals:
                if signal.choices is not None:
                    i += 1

        self.assertEqual(i, 15)

        with open(filename, 'r') as fin:
            self.assertEqual(db.as_dbc(), fin.read())

    def test_motohawk(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')

        with open(filename, 'r') as fin:
            db.add_dbc(fin)

        self.assertEqual(len(db.ecus), 2)
        self.assertEqual(db.ecus[0].name, 'PCM1')
        self.assertEqual(db.ecus[1].name, 'FOO')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(len(db.messages[0].signals[0].ecus), 2)
        self.assertEqual(db.messages[0].signals[0].ecus[0], 'Vector__XXX')
        self.assertEqual(db.messages[0].signals[0].ecus[1], 'FOO')
        self.assertEqual(db.messages[0].signals[1].ecus[0], 'Vector__XXX')
        
        with open(filename, 'r') as fin:
            self.assertEqual(db.as_dbc(), fin.read())

    def test_emc32(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'emc32.dbc')

        with open(filename, 'r') as fin:
            db.add_dbc(fin)

        self.assertEqual(len(db.ecus), 1)
        self.assertEqual(db.ecus[0].name, 'EMV_Statusmeldungen')
        self.assertEqual(len(db.messages), 1)
        self.assertEqual(len(db.messages[0].signals[0].ecus), 1)

    def test_motohawk_encode(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
        db.add_dbc_file(filename)

        # Encode signals into an ExampleMessage frame.
        example_message_frame_id = 496

        temperature = 250.55
        average_radius = 3.2
        enable = 1

        data = bitstruct.pack('p45u12u6u1',
                              int((temperature - 250) / 0.01),
                              int(average_radius / 0.1),
                              enable)[::-1]

        message = db.lookup_message(example_message_frame_id)
        encoded = db.encode_message(example_message_frame_id,
                                    message.Signals(message,
                                                    temperature,
                                                    average_radius,
                                                    enable))

        self.assertEqual(encoded, data)

        encoded = db.encode_message(example_message_frame_id,
                                    {'Temperature': temperature,
                                     'AverageRadius': average_radius,
                                     'Enable': enable})

        self.assertEqual(encoded, data)

    def test_motohawk_decode(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
        db.add_dbc_file(filename)

        # Encode and decode the signals in an ExampleMessage frame.
        example_message_frame_id = 496

        temperature = 250.55
        average_radius = 3.2
        enable = 1

        data = bitstruct.pack('p45u12u6u1',
                              int((temperature - 250) / 0.01),
                              int(average_radius / 0.1),
                              enable)[::-1]

        decoded = db.decode_message(example_message_frame_id, data)

        self.assertEqual(decoded.Temperature, temperature)
        self.assertEqual(decoded.AverageRadius, average_radius)
        self.assertEqual(decoded.Enable, enable)
        self.assertEqual(str(decoded),
                         'ExampleMessage(Temperature: 250.55 degK, '
                         'AverageRadius: 3.2 m, Enable: 1.0 -)')

    def test_motohawk_encode_decode(self):
        db = cantools.db.File()
        filename = os.path.join('tests', 'files', 'motohawk.dbc')
        db.add_dbc_file(filename)

        # Decode the signals in an ExampleMessage frame.
        example_message_frame_id = 496

        temperature = 250.55
        average_radius = 3.2
        enable = 1

        encoded = db.encode_message(example_message_frame_id,
                                    {'Temperature': temperature,
                                     'AverageRadius': average_radius,
                                     'Enable': enable})

        decoded = db.decode_message(example_message_frame_id, encoded)

        self.assertEqual(decoded.Temperature, temperature)
        self.assertEqual(decoded.AverageRadius, average_radius)
        self.assertEqual(decoded.Enable, enable)

    def test_add_message(self):
        db = cantools.db.File()
        signals = [cantools.db.Signal(name='signal',
                                      start=0,
                                      length=4,
                                      ecus=['foo'],
                                      byte_order='big_endian',
                                      is_signed=False,
                                      scale=1.0,
                                      offset=10,
                                      minimum=10.0,
                                      maximum=100.0,
                                      unit='m/s',
                                      choices=None,
                                      comment=None)]
        message = cantools.db.Message(frame_id=37,
                                      name='message',
                                      length=8,
                                      ecus=['bar'],
                                      signals=signals,
                                      comment='')
        db.add_message(message)
        self.assertEqual(len(db.messages), 1)


if __name__ == '__main__':
    unittest.main()
