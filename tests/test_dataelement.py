import unittest

from cantools.database.dataelement import DataElement


class CanToolsDataElementTest(unittest.TestCase):
    def test_invalid_params(self):
        with self.assertRaises(ValueError):
            DataElement("Test", 0, 1, segment_boundaries=[(0, 1), (2, 3)])

    def test_no_piecewise(self):
        exp_scale = 2.0
        exp_offset = 1.0
        d = DataElement("Test", 0, 1, scale=2.0, offset=1.0)

        # expect same offset and scaling for every input
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_scaled())
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_scaled(1.0))
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_raw())
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_raw(1.0))

    def test_piecewise_linear(self):
        exp_scale = [0.5, 2.0]
        exp_offset = [0, 1.0]
        boundaries = [(0, 5), (6, 10)]
        d = DataElement(
            "Test",
            0,
            1,
            scale=exp_scale,
            offset=exp_offset,
            segment_boundaries=boundaries,
        )

        self.assertEqual(
            (exp_offset[0], exp_scale[0]), d.get_offset_scaling_from_raw(2)
        )
        self.assertEqual(
            (exp_offset[1], exp_scale[1]), d.get_offset_scaling_from_raw(8)
        )
        self.assertEqual(
            (exp_offset[0], exp_scale[0]), d.get_offset_scaling_from_scaled(1)
        )
        self.assertEqual(
            (exp_offset[1], exp_scale[1]), d.get_offset_scaling_from_scaled(18)
        )

        with self.assertRaises(ValueError):
            # out of range
            d.get_offset_scaling_from_raw(20)
