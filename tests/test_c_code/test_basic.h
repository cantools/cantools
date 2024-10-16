#ifndef TEST_BASIC_H
#define TEST_BASIC_H

void test_motohawk_example_message(void);
void test_padding_bit_order_msg0(void);
void test_padding_bit_order_msg1(void);
void test_padding_bit_order_msg2(void);
void test_padding_bit_order_msg3(void);
void test_padding_bit_order_msg4(void);
void test_multiplex(void);
void test_multiplex_2_extended(void);
void test_floating_point_message1(void);
void test_floating_point_message2(void);
void test_signed_message64(void);
void test_signed_message33(void);
void test_signed_message32(void);
void test_signed_message64big(void);
void test_signed_message33big(void);
void test_signed_message32big(void);
void test_signed_message378910(void);
void test_is_in_range(void);
void test_encode_decode(void);
void test_unpack_does_not_modify_other_mux_signals(void);

#endif // TEST_BASIC_H