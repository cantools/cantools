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
        self.assertEqual(len(db.ecus), 1)
        self.assertEqual(db.ecus[0].name, 'PCM1')
        self.assertEqual(len(db.messages), 1)

        with open(filename, 'r') as fin:
            self.assertEqual(db.as_dbc(), fin.read())

        # decoode a raw message
        frame_id = 496
        temperature = 250.55
        average_radius = 3.2
        enable = 1
        data = bitstruct.pack('p45u12u6u1',
                              int((temperature - 250) / 0.01),
                              int(average_radius / 0.1),
                              enable)
        decoded = db.decode_message(frame_id, data)
        self.assertEqual(decoded.Temperature, 250.55)
        self.assertEqual(decoded.AverageRadius, 3.2)
        self.assertEqual(decoded.Enable, 1.0)

    def test_add_message(self):
        db = cantools.db.File()
        signals = [cantools.db.Signal(name='signal',
                                      start=0,
                                      length=4,
                                      ecu='foo',
                                      byte_order='big_endian',
                                      _type='unsigned',
                                      scale=1.0,
                                      offset=10,
                                      _min=10.0,
                                      _max=100.0,
                                      unit='m/s',
                                      choices=None,
                                      comment=None)]
        message = cantools.db.Message(frame_id=37,
                                      name='message',
                                      length=8,
                                      ecu='bar',
                                      signals=signals,
                                      comment='')
        db.add_message(message)
        self.assertEqual(len(db.messages), 1)


if __name__ == '__main__':
    unittest.main()
