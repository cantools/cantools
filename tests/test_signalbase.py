import logging
import unittest

try:
    from StringIO import StringIO
except ImportError:
    from io import StringIO

from cantools.database.signalbase import SignalBase


logger = logging.getLogger()
logger.level = logging.DEBUG


class CanToolsSignalBaseTest(unittest.TestCase):
    def test_invalid_params(self):
        with self.assertRaises(ValueError):
            SignalBase("Test", 0, 1, segment_boundaries=[(0, 1), (2, 3)])

    def test_no_piecewise(self):
        exp_scale = 2.0
        exp_offset = 1.0
        d = SignalBase("Test", 0, 1, scale=2.0, offset=1.0)

        # expect same offset and scaling for every input
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_scaled())
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_scaled(1.0))
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_raw())
        self.assertEqual((exp_offset, exp_scale), d.get_offset_scaling_from_raw(1.0))

    def test_piecewise_linear(self):
        exp_scale = [0.5, 2.0]
        exp_offset = [0, 1.0]
        boundaries = [(0, 5), (6, 10)]
        d = SignalBase(
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

    def test_no_choices(self):
        d = SignalBase("Test", 0, 1, scale=2.0, offset=1.0)

        with self.assertRaises(ValueError):
            d.choice_string_to_number("test")

    def test_overlapping_boundaries(self):
        exp_scale = [2.0, 1.0]
        exp_offset = [0, 1.0]
        boundaries = [(0, 5), (6, 10)]
        stdout = StringIO()
        stream_handler = logging.StreamHandler(stdout)
        logger.addHandler(stream_handler)

        _ = SignalBase(
            "Test",
            0,
            1,
            scale=exp_scale,
            offset=exp_offset,
            segment_boundaries=boundaries,
        )

        actual_out = stdout.getvalue()
        expected_out = (
            "Piecewise linear type: Test has overlapping segments! "
            "Segment 1 starts at phys val 7.0 but one of the prev segments ended at 10.0. "
            "Encoding might be ambiguous.\n"
        )
        self.assertEqual(expected_out, actual_out)


if __name__ == "__main__":
    unittest.main()
