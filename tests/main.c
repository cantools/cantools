#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "files/motohawk.h"
#include "files/padding_bit_order.h"

static void test_motohawk_example_message(void)
{
    struct motohawk_example_message_t decoded;
    uint8_t buf[8];

    decoded.temperature = 55;
    decoded.average_radius = 32;
    decoded.enable = 1;

    memset(&buf[0], 0, sizeof(buf));
    assert(motohawk_example_message_encode(&buf[0],
                                           &decoded,
                                           sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xc0\x06\xe0\x00\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
}

static void test_padding_bit_order_msg0(void)
{
    struct padding_bit_order_msg0_t decoded;
    uint8_t buf[8];

    decoded.a = 0x2c9;
    decoded.b = 1;
    decoded.c = 0x2c9;
    decoded.d = 0;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg0_encode(&buf[0],
                                         &decoded,
                                         sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x82\xc9\x00\x00\x02\xc9\x00\x00",
                  sizeof(buf)) == 0);
}

static void test_padding_bit_order_msg1(void)
{
    struct padding_bit_order_msg1_t decoded;
    uint8_t buf[8];

    decoded.e = 1;
    decoded.f = 0x2c9;
    decoded.g = 0;
    decoded.h = 0x2c9;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg1_encode(&buf[0],
                                         &decoded,
                                         sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x93\x05\x00\x00\x92\x05\x00\x00",
                  sizeof(buf)) == 0);
}

static void test_padding_bit_order_msg2(void)
{
    struct padding_bit_order_msg2_t decoded;
    uint8_t buf[8];

    decoded.i = 1;
    decoded.j = 2;
    decoded.k = 3;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg2_encode(&buf[0],
                                         &decoded,
                                         sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x21\x03\x00\x00\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
}

static void test_padding_bit_order_msg3(void)
{
    struct padding_bit_order_msg3_t decoded;
    uint8_t buf[8];

    decoded.l = 0x0123456789abcdef;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg3_encode(&buf[0],
                                         &decoded,
                                         sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x01\x23\x45\x67\x89\xab\xcd\xef",
                  sizeof(buf)) == 0);
}

static void test_padding_bit_order_msg4(void)
{
    struct padding_bit_order_msg4_t decoded;
    uint8_t buf[8];

    decoded.m = 0x0123456789abcdef;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg4_encode(&buf[0],
                                         &decoded,
                                         sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xef\xcd\xab\x89\x67\x45\x23\x01",
                  sizeof(buf)) == 0);
}

static void test_padding_bit_order(void)
{
    test_padding_bit_order_msg0();
    test_padding_bit_order_msg1();
    test_padding_bit_order_msg2();
    test_padding_bit_order_msg3();
    test_padding_bit_order_msg4();
}

int main()
{
    test_motohawk_example_message();
    test_padding_bit_order();

    return (0);
}
