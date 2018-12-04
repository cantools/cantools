#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "files/c_source/motohawk.h"
#include "files/c_source/padding_bit_order.h"
#include "files/c_source/vehicle.h"
#include "files/c_source/multiplex.h"
#include "files/c_source/multiplex_2.h"
#include "files/c_source/floating_point.h"
#include "files/c_source/no_signals.h"
#include "files/c_source/signed.h"

static void test_motohawk_example_message(void)
{
    struct {
        struct motohawk_example_message_t decoded;
        uint8_t encoded[8];
    } datas[3] = {
        {
            .decoded = {
                .temperature = 55,
                .average_radius = 32,
                .enable = 1
            },
            .encoded = "\xc0\x06\xe0\x00\x00\x00\x00\x00"
        },
        {
            .decoded = {
                .temperature = -2047,
                .average_radius = 0,
                .enable = 0
            },
            .encoded = "\x01\x00\x20\x00\x00\x00\x00\x00"
        },
        {
            .decoded = {
                .temperature = -2048,
                .average_radius = 0,
                .enable = 0
            },
            .encoded = "\x01\x00\x00\x00\x00\x00\x00\x00"
        }
    };
    struct motohawk_example_message_t decoded;
    uint8_t buf[8];
    int i;

    for (i = 0; i < 3; i++) {
        memset(&buf[0], 0, sizeof(buf));
        assert(motohawk_example_message_encode(&buf[0],
                                               &datas[i].decoded,
                                               sizeof(buf)) == 8);
        assert(memcmp(&buf[0],
                      &datas[i].encoded[0],
                      sizeof(buf)) == 0);
        memset(&decoded, 0, sizeof(decoded));
        assert(motohawk_example_message_decode(&decoded,
                                               &buf[0],
                                               sizeof(buf)) == 0);
        assert(decoded.temperature == datas[i].decoded.temperature);
        assert(decoded.average_radius == datas[i].decoded.average_radius);
        assert(decoded.enable == datas[i].decoded.enable);

        assert(motohawk_example_message_enable_is_in_range(
                   decoded.enable));
        assert(motohawk_example_message_average_radius_is_in_range(
                   decoded.average_radius));
        assert(motohawk_example_message_temperature_is_in_range(
                   decoded.temperature));
    }
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
    memset(&decoded, 0, sizeof(decoded));
    assert(padding_bit_order_msg0_decode(&decoded,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(decoded.a == 0x2c9);
    assert(decoded.b == 1);
    assert(decoded.c == 0x2c9);
    assert(decoded.d == 0);
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
    memset(&decoded, 0, sizeof(decoded));
    assert(padding_bit_order_msg1_decode(&decoded,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(decoded.e == 1);
    assert(decoded.f == 0x2c9);
    assert(decoded.g == 0);
    assert(decoded.h == 0x2c9);
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
    memset(&decoded, 0, sizeof(decoded));
    assert(padding_bit_order_msg2_decode(&decoded,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(decoded.i == 1);
    assert(decoded.j == 2);
    assert(decoded.k == 3);
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
    memset(&decoded, 0, sizeof(decoded));
    assert(padding_bit_order_msg3_decode(&decoded,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(decoded.l == 0x0123456789abcdef);
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
    memset(&decoded, 0, sizeof(decoded));
    assert(padding_bit_order_msg4_decode(&decoded,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(decoded.m == 0x0123456789abcdef);
}

static void test_padding_bit_order(void)
{
    test_padding_bit_order_msg0();
    test_padding_bit_order_msg1();
    test_padding_bit_order_msg2();
    test_padding_bit_order_msg3();
    test_padding_bit_order_msg4();
}

static void test_multiplex(void)
{
    struct {
        struct multiplex_message1_t decoded;
        uint8_t encoded[8];
    } datas[3] = {
        {
            .decoded = {
                .multiplexor = 8,
                .bit_c = 1,
                .bit_g = 1,
                .bit_j = 1,
                .bit_l = 1
            },
            .encoded = "\x20\x00\x8c\x01\x00\x00\x00\x00"
        },
        {
            .decoded = {
                .multiplexor = 16,
                .bit_c = 1,
                .bit_g = 1,
                .bit_j = 1,
                .bit_l = 1
            },
            .encoded = "\x40\x00\x8c\x01\x00\x00\x00\x00"
        },
        {
            .decoded = {
                .multiplexor = 24,
                .bit_a = 1,
                .bit_b = 1,
                .bit_c = 1,
                .bit_d = 1,
                .bit_e = 1,
                .bit_f = 1,
                .bit_g = 1,
                .bit_h = 1,
                .bit_j = 1,
                .bit_k = 1,
                .bit_l = 1
            },
            .encoded = "\x60\x00\x8c\x35\xc3\x00\x00\x00"
        }
    };
    struct multiplex_message1_t decoded;
    uint8_t buf[8];
    int i;

    for (i = 0; i < 3; i++) {
        memset(&buf[0], 0, sizeof(buf));
        assert(multiplex_message1_encode(&buf[0],
                                         &datas[i].decoded,
                                         sizeof(buf)) == 8);
        assert(memcmp(&buf[0],
                      &datas[i].encoded[0],
                      sizeof(buf)) == 0);
        memset(&decoded, 0, sizeof(decoded));
        assert(multiplex_message1_decode(&decoded,
                                         &buf[0],
                                         sizeof(buf)) == 0);
        assert(decoded.multiplexor == datas[i].decoded.multiplexor);
        assert(decoded.bit_a == datas[i].decoded.bit_a);
        assert(decoded.bit_b == datas[i].decoded.bit_b);
        assert(decoded.bit_c == datas[i].decoded.bit_c);
        assert(decoded.bit_d == datas[i].decoded.bit_d);
        assert(decoded.bit_e == datas[i].decoded.bit_e);
        assert(decoded.bit_f == datas[i].decoded.bit_f);
        assert(decoded.bit_g == datas[i].decoded.bit_g);
        assert(decoded.bit_h == datas[i].decoded.bit_h);
        assert(decoded.bit_j == datas[i].decoded.bit_j);
        assert(decoded.bit_k == datas[i].decoded.bit_k);
        assert(decoded.bit_l == datas[i].decoded.bit_l);
    }
}

static void test_multiplex_2_extended(void)
{
    struct {
        struct multiplex_2_extended_t decoded;
        uint8_t encoded[8];
    } datas[3] = {
        {
            .decoded = {
                .s0 = 0,
                .s1 = 2,
                .s2 = 0,
                .s3 = 0,
                .s4 = 10000,
                .s5 = 0,
                .s6 = 1,
                .s7 = 33,
                .s8 = 0
            },
            .encoded = "\x20\x10\x27\x00\x01\x21\x00\x00"
        },
        {
            .decoded = {
                .s0 = 0,
                .s1 = 0,
                .s2 = 100,
                .s3 = 5000,
                .s4 = 0,
                .s5 = 0,
                .s6 = 2,
                .s7 = 0,
                .s8 = 22
            },
            .encoded = "\x00\x64\x88\x13\x02\x16\x00\x00"
        },
        {
            .decoded = {
                .s0 = 1,
                .s1 = 0,
                .s2 = 0,
                .s3 = 0,
                .s4 = 0,
                .s5 = 3,
                .s6 = 1,
                .s7 = 772,
                .s8 = 0
            },
            .encoded = "\x31\x00\x00\x00\x01\x04\x03\x00"
        },
    };
    struct multiplex_2_extended_t decoded;
    uint8_t buf[8];
    int i;

    for (i = 0; i < 3; i++) {
        memset(&buf[0], 0, sizeof(buf));
        assert(multiplex_2_extended_encode(&buf[0],
                                           &datas[i].decoded,
                                           sizeof(buf)) == 8);
        assert(memcmp(&buf[0],
                      &datas[i].encoded[0],
                      sizeof(buf)) == 0);
        memset(&decoded, 0, sizeof(decoded));
        assert(multiplex_2_extended_decode(&decoded,
                                           &buf[0],
                                           sizeof(buf)) == 0);
        assert(decoded.s0 == datas[i].decoded.s0);
        assert(decoded.s1 == datas[i].decoded.s1);
        assert(decoded.s2 == datas[i].decoded.s2);
        assert(decoded.s3 == datas[i].decoded.s3);
        assert(decoded.s4 == datas[i].decoded.s4);
        assert(decoded.s5 == datas[i].decoded.s5);
        assert(decoded.s6 == datas[i].decoded.s6);
        assert(decoded.s7 == datas[i].decoded.s7);
        assert(decoded.s8 == datas[i].decoded.s8);
    }
}

static void test_floating_point_message1(void)
{
    double signal1 = -129.448;
    struct floating_point_message1_t decoded;
    uint8_t buf[8];

    decoded.signal1 = signal1;

    memset(&buf[0], 0, sizeof(buf));
    assert(floating_point_message1_encode(&buf[0],
                                          &decoded,
                                          sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x75\x93\x18\x04\x56\x2e\x60\xc0",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(floating_point_message1_decode(&decoded,
                                          &buf[0],
                                          sizeof(buf)) == 0);
    assert(memcmp(&decoded.signal1, &signal1, sizeof(decoded.signal1)) == 0);
}

static void test_floating_point_message2(void)
{
    float signal1 = 129.5f;
    float signal2 = 1234500.5f;
    struct floating_point_message2_t decoded;
    uint8_t buf[8];

    decoded.signal1 = signal1;
    decoded.signal2 = signal2;

    memset(&buf[0], 0, sizeof(buf));
    assert(floating_point_message2_encode(&buf[0],
                                          &decoded,
                                          sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x00\x80\x01\x43\x24\xb2\x96\x49",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(floating_point_message2_decode(&decoded,
                                          &buf[0],
                                          sizeof(buf)) == 0);

    assert(memcmp(&decoded.signal1, &signal1, sizeof(decoded.signal1)) == 0);
    assert(memcmp(&decoded.signal2, &signal2, sizeof(decoded.signal2)) == 0);
}

static void test_floating_point(void)
{
    test_floating_point_message1();
    test_floating_point_message2();
}

static void test_signed_message64(void)
{
    struct signed_message64_t decoded;
    uint8_t buf[8];

    decoded.s64 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message64_encode(&buf[0],
                                   &decoded,
                                   sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\xff\xff\xff\xff",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(signed_message64_decode(&decoded,
                                   &buf[0],
                                   sizeof(buf)) == 0);
    assert(decoded.s64 == -5);
}

static void test_signed_message33(void)
{
    struct signed_message33_t decoded;
    uint8_t buf[8];

    decoded.s33 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message33_encode(&buf[0],
                                   &decoded,
                                   sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\x01\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(signed_message33_decode(&decoded,
                                   &buf[0],
                                   sizeof(buf)) == 0);
    assert(decoded.s33 == -5);
}

static void test_signed_message32(void)
{
    struct signed_message32_t decoded;
    uint8_t buf[8];

    decoded.s32 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message32_encode(&buf[0],
                                   &decoded,
                                   sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(signed_message32_decode(&decoded,
                                   &buf[0],
                                   sizeof(buf)) == 0);
    assert(decoded.s32 == -5);
}

static void test_signed_message64big(void)
{
    struct signed_message64big_t decoded;
    uint8_t buf[8];

    decoded.s64big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message64big_encode(&buf[0],
                                      &decoded,
                                      sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xff\xff\xff\xff\xfb",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(signed_message64big_decode(&decoded,
                                      &buf[0],
                                      sizeof(buf)) == 0);
    assert(decoded.s64big == -5);
}

static void test_signed_message33big(void)
{
    struct signed_message33big_t decoded;
    uint8_t buf[8];

    decoded.s33big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message33big_encode(&buf[0],
                                      &decoded,
                                      sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xfd\x80\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(signed_message33big_decode(&decoded,
                                      &buf[0],
                                      sizeof(buf)) == 0);
    assert(decoded.s33big == -5);
}

static void test_signed_message32big(void)
{
    struct signed_message32big_t decoded;
    uint8_t buf[8];

    decoded.s32big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message32big_encode(&buf[0],
                                      &decoded,
                                      sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xfb\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(signed_message32big_decode(&decoded,
                                      &buf[0],
                                      sizeof(buf)) == 0);
    assert(decoded.s32big == -5);
}

static void test_signed_message378910(void)
{
    struct signed_message378910_t decoded;
    uint8_t buf[8];

    decoded.s7 = -40;
    decoded.s8big = 0x5a;
    decoded.s9 = 0xa5;
    decoded.s8 = -43;
    decoded.s3big = -4;
    decoded.s3 = 1;
    decoded.s10big = -253;
    decoded.s7big = -9;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message378910_encode(&buf[0],
                                       &decoded,
                                       sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xb0\xb4\x4a\x55\x87\x01\x81\xf7",
                  sizeof(buf)) == 0);
    memset(&decoded, 0, sizeof(decoded));
    assert(signed_message378910_decode(&decoded,
                                       &buf[0],
                                       sizeof(buf)) == 0);
    assert(decoded.s7 == -40);
    assert(decoded.s8big == 0x5a);
    assert(decoded.s9 == 0xa5);
    assert(decoded.s8 == -43);
    assert(decoded.s3big == -4);
    assert(decoded.s3 == 1);
    assert(decoded.s10big == -253);
    assert(decoded.s7big == -9);
}

static void test_signed(void)
{
    test_signed_message64();
    test_signed_message33();
    test_signed_message32();
    test_signed_message64big();
    test_signed_message33big();
    test_signed_message32big();
    test_signed_message378910();
}

static void test_is_in_range(void)
{
    /* Missing limits. Anything allowed, byt should probably check
       that it fits in its number of bits. */
    assert(motohawk_example_message_enable_is_in_range(0));
    assert(motohawk_example_message_enable_is_in_range(1));

    /* Unsigned value with 0 as lower limit. */
    assert(motohawk_example_message_average_radius_is_in_range(0));
    assert(motohawk_example_message_average_radius_is_in_range(50));
    assert(motohawk_example_message_average_radius_is_in_range(51) == false);

    /* Signed value with upper and lower limits. */
    assert(motohawk_example_message_temperature_is_in_range(-2049) == false);
    assert(motohawk_example_message_temperature_is_in_range(-2048));
    assert(motohawk_example_message_temperature_is_in_range(2047));
    assert(motohawk_example_message_temperature_is_in_range(2048) == false);
}

int main(void)
{
    test_motohawk_example_message();
    test_padding_bit_order();
    test_multiplex();
    test_multiplex_2_extended();
    test_floating_point();
    test_signed();
    test_is_in_range();

    return (0);
}
