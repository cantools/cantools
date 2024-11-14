#include "test_framework/unity.h"
#include "test_basic.h"
#include "test_bit_fields.h"
#include "test_initial_values.h"

extern void test_add(void);
extern void test_subtract(void);

void setUp(void) {
    // Set up code here
}

void tearDown(void) {
    // Clean up code here
}

int main(void) {
    UNITY_BEGIN();

    // test_basic.h
    RUN_TEST(test_motohawk_example_message);
    RUN_TEST(test_padding_bit_order_msg0);
    RUN_TEST(test_padding_bit_order_msg1);
    RUN_TEST(test_padding_bit_order_msg2);
    RUN_TEST(test_padding_bit_order_msg3);
    RUN_TEST(test_padding_bit_order_msg4);
    RUN_TEST(test_multiplex);
    RUN_TEST(test_multiplex_2_extended);
    RUN_TEST(test_floating_point_message1);
    RUN_TEST(test_floating_point_message2);
    RUN_TEST(test_signed_message64);
    RUN_TEST(test_signed_message33);
    RUN_TEST(test_signed_message32);
    RUN_TEST(test_signed_message64big);
    RUN_TEST(test_signed_message33big);
    RUN_TEST(test_signed_message32big);
    RUN_TEST(test_signed_message378910);
    RUN_TEST(test_is_in_range);
    RUN_TEST(test_encode_decode);
    RUN_TEST(test_unpack_does_not_modify_other_mux_signals);

    // test_bit_fields.h
    RUN_TEST(test_motohawk_bit_fields_example_message);
    RUN_TEST(test_floating_point_bit_fields_message1);
    RUN_TEST(test_floating_point_bit_fields_message2);
    RUN_TEST(test_signed_bit_fields_message64);
    RUN_TEST(test_signed_bit_fields_message33);
    RUN_TEST(test_signed_bit_fields_message32);
    RUN_TEST(test_signed_bit_fields_message64big);
    RUN_TEST(test_signed_bit_fields_message33big);
    RUN_TEST(test_signed_bit_fields_message32big);
    RUN_TEST(test_signed_bit_fields_message378910);
    RUN_TEST(test_is_in_range_bit_fields);

    // test_initial_values.h
    RUN_TEST(valid_initial_value);
    RUN_TEST(absence_of_initial_value);
    RUN_TEST(nullptr_ptr_initial_value);

    return UNITY_END();
}
