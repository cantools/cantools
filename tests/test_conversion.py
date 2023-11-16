import unittest
from collections import OrderedDict
from typing import TYPE_CHECKING

from cantools.database.conversion import (
    BaseConversion,
    IdentityConversion,
    LinearConversion,
    LinearIntegerConversion,
    NamedSignalConversion,
)
from cantools.database.namedsignalvalue import NamedSignalValue

if TYPE_CHECKING:
    from cantools.typechecking import Choices


class TestConversions(unittest.TestCase):
    def test_base_conversion_factory(self):
        # Test the IdentityConversion
        conversion = BaseConversion.factory()
        assert isinstance(conversion, IdentityConversion)
        assert conversion.scale == 1
        assert conversion.offset == 0
        assert not conversion.is_float
        assert conversion.choices is None

        # Test the LinearIntegerConversion
        conversion = BaseConversion.factory(scale=2, offset=3)
        assert isinstance(conversion, LinearIntegerConversion)
        assert conversion.scale == 2
        assert conversion.offset == 3
        assert not conversion.is_float
        assert conversion.choices is None

        # Test the LinearConversion
        conversion = BaseConversion.factory(scale=2.5, offset=-1.5, is_float=True)
        assert isinstance(conversion, LinearConversion)
        assert conversion.scale == 2.5
        assert conversion.offset == -1.5
        assert conversion.is_float
        assert conversion.choices is None

        # Test the NamedSignalConversion
        choices = {0: "Off", 1: "On"}
        conversion = BaseConversion.factory(choices=choices)
        assert isinstance(conversion, NamedSignalConversion)
        assert conversion.scale == 1
        assert conversion.offset == 0
        assert not conversion.is_float
        assert conversion.choices == choices

        # Test the error handling
        with self.assertRaises(TypeError):
            BaseConversion.factory(scale="2", offset="3")

    def test_identity_conversion(self):
        conversion = IdentityConversion(is_float=True)
        self.assertEqual(conversion.scale, 1)
        self.assertEqual(conversion.offset, 0)
        self.assertTrue(conversion.is_float)

        self.assertEqual(conversion.raw_to_scaled(42), 42)
        self.assertEqual(conversion.scaled_to_raw(42), 42)

        self.assertEqual(str(conversion), "IdentityConversion(is_float=True)")

    def test_linear_integer_conversion(self):
        conversion = LinearIntegerConversion(scale=2, offset=10)
        self.assertEqual(conversion.scale, 2)
        self.assertEqual(conversion.offset, 10)
        self.assertFalse(conversion.is_float)

        self.assertEqual(conversion.raw_to_scaled(0), 10)
        self.assertEqual(conversion.raw_to_scaled(1), 12)
        self.assertEqual(conversion.raw_to_scaled(2), 14)

        self.assertEqual(conversion.scaled_to_raw(10), 0)
        self.assertEqual(conversion.scaled_to_raw(12), 1)
        self.assertEqual(conversion.scaled_to_raw(14), 2)

        self.assertEqual(str(conversion), "LinearIntegerConversion(scale=2, offset=10)")

    def test_linear_conversion(self):
        conversion = LinearConversion(scale=1.5, offset=10, is_float=True)
        self.assertEqual(conversion.scale, 1.5)
        self.assertEqual(conversion.offset, 10)
        self.assertTrue(conversion.is_float)

        self.assertEqual(conversion.raw_to_scaled(0.0), 10)
        self.assertEqual(conversion.raw_to_scaled(1.0), 11.5)
        self.assertEqual(conversion.raw_to_scaled(2.0), 13)

        self.assertEqual(conversion.scaled_to_raw(10), 0.0)
        self.assertEqual(conversion.scaled_to_raw(11.5), 1.0)
        self.assertEqual(conversion.scaled_to_raw(13), 2.0)

        self.assertEqual(
            str(conversion), "LinearConversion(scale=1.5, offset=10, is_float=True)"
        )


class TestNamedSignalConversion(unittest.TestCase):
    def setUp(self) -> None:
        self.choices: Choices = OrderedDict(
            {
                0: "Low",
                1: "High",
            }
        )

        self.conversion: NamedSignalConversion = NamedSignalConversion(
            scale=2.0, offset=-1.0, choices=self.choices, is_float=False
        )

    def test_raw_to_scaled(self):
        raw_value = 5
        expected = 9.0

        self.assertEqual(
            self.conversion.raw_to_scaled(raw_value),
            expected,
        )

    def test_raw_to_scaled_with_choice(self):
        raw_value = 1
        expected = NamedSignalValue(1, "High")

        self.assertEqual(
            self.conversion.raw_to_scaled(raw_value),
            expected,
        )

    def test_raw_to_scaled_without_choice(self):
        raw_value = 1
        expected = 1.0

        self.assertEqual(
            self.conversion.raw_to_scaled(raw_value, decode_choices=False),
            expected,
        )

    def test_scaled_to_raw_with_choice(self):
        scaled_value = NamedSignalValue(1, "High")
        expected = 1

        self.assertEqual(
            self.conversion.scaled_to_raw(scaled_value),
            expected,
        )

    def test_scaled_to_raw_without_choice(self):
        scaled_value = 1.0
        expected = 1

        self.assertEqual(
            self.conversion.scaled_to_raw(scaled_value),
            expected,
        )

    def test_choice_to_number(self):
        choice = NamedSignalValue(1, "High")
        expected = 1

        self.assertEqual(
            self.conversion.choice_to_number(choice),
            expected,
        )

    def test_choice_to_number_with_invalid_choice(self):
        choice = NamedSignalValue(2, "Invalid")

        with self.assertRaises(KeyError):
            self.conversion.choice_to_number(choice)
