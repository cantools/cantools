/* Include the generated files first to test that all required header
   files are included. */
#include "files/c_source/motohawk.h"
#include "files/c_source/padding_bit_order.h"
#include "files/c_source/vehicle.h"
#include "files/c_source/multiplex.h"
#include "files/c_source/multiplex_2.h"
#include "files/c_source/floating_point.h"
#include "files/c_source/no_signals.h"
#include "files/c_source/signed.h"
#include "files/c_source/my_database_name.h"
#include "files/c_source/min_max_only_6_0.h"
#include "files/c_source/abs.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static bool fequal(double v1, double v2)
{
    return (fabs(v1 - v2) < 0.000001);
}

static void test_motohawk_example_message(void)
{
    struct {
        struct motohawk_example_message_t unpacked;
        uint8_t packed[8];
    } datas[3] = {
        {
            .unpacked = {
                .temperature = 55,
                .average_radius = 32,
                .enable = 1
            },
            .packed = "\xc0\x06\xe0\x00\x00\x00\x00\x00"
        },
        {
            .unpacked = {
                .temperature = -2047,
                .average_radius = 0,
                .enable = 0
            },
            .packed = "\x01\x00\x20\x00\x00\x00\x00\x00"
        },
        {
            .unpacked = {
                .temperature = -2048,
                .average_radius = 0,
                .enable = 0
            },
            .packed = "\x01\x00\x00\x00\x00\x00\x00\x00"
        }
    };
    struct motohawk_example_message_t unpacked;
    uint8_t buf[8];
    int i;

    for (i = 0; i < 3; i++) {
        memset(&buf[0], 0, sizeof(buf));
        assert(motohawk_example_message_pack(&buf[0],
                                             &datas[i].unpacked,
                                             sizeof(buf)) == 8);
        assert(memcmp(&buf[0],
                      &datas[i].packed[0],
                      sizeof(buf)) == 0);
        memset(&unpacked, 0, sizeof(unpacked));
        assert(motohawk_example_message_unpack(&unpacked,
                                               &buf[0],
                                               sizeof(buf)) == 0);
        assert(unpacked.temperature == datas[i].unpacked.temperature);
        assert(unpacked.average_radius == datas[i].unpacked.average_radius);
        assert(unpacked.enable == datas[i].unpacked.enable);

        assert(motohawk_example_message_enable_is_in_range(
                   unpacked.enable));
        assert(motohawk_example_message_average_radius_is_in_range(
                   unpacked.average_radius));
        assert(motohawk_example_message_temperature_is_in_range(
                   unpacked.temperature));
    }
}

static void test_padding_bit_order_msg0(void)
{
    struct padding_bit_order_msg0_t unpacked;
    uint8_t buf[8];

    unpacked.a = 0x2c9;
    unpacked.b = 1;
    unpacked.c = 0x2c9;
    unpacked.d = 0;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg0_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x82\xc9\x00\x00\x02\xc9\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(padding_bit_order_msg0_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(unpacked.a == 0x2c9);
    assert(unpacked.b == 1);
    assert(unpacked.c == 0x2c9);
    assert(unpacked.d == 0);
}

static void test_padding_bit_order_msg1(void)
{
    struct padding_bit_order_msg1_t unpacked;
    uint8_t buf[8];

    unpacked.e = 1;
    unpacked.f = 0x2c9;
    unpacked.g = 0;
    unpacked.h = 0x2c9;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg1_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x93\x05\x00\x00\x92\x05\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(padding_bit_order_msg1_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(unpacked.e == 1);
    assert(unpacked.f == 0x2c9);
    assert(unpacked.g == 0);
    assert(unpacked.h == 0x2c9);
}

static void test_padding_bit_order_msg2(void)
{
    struct padding_bit_order_msg2_t unpacked;
    uint8_t buf[8];

    unpacked.i = 1;
    unpacked.j = 2;
    unpacked.k = 3;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg2_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x21\x03\x00\x00\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(padding_bit_order_msg2_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(unpacked.i == 1);
    assert(unpacked.j == 2);
    assert(unpacked.k == 3);
}

static void test_padding_bit_order_msg3(void)
{
    struct padding_bit_order_msg3_t unpacked;
    uint8_t buf[8];

    unpacked.l = 0x0123456789abcdef;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg3_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x01\x23\x45\x67\x89\xab\xcd\xef",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(padding_bit_order_msg3_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(unpacked.l == 0x0123456789abcdef);
}

static void test_padding_bit_order_msg4(void)
{
    struct padding_bit_order_msg4_t unpacked;
    uint8_t buf[8];

    unpacked.m = 0x0123456789abcdef;

    memset(&buf[0], 0, sizeof(buf));
    assert(padding_bit_order_msg4_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xef\xcd\xab\x89\x67\x45\x23\x01",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(padding_bit_order_msg4_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)) == 0);
    assert(unpacked.m == 0x0123456789abcdef);
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
        struct multiplex_message1_t unpacked;
        uint8_t packed[8];
    } datas[3] = {
        {
            .unpacked = {
                .multiplexor = 8,
                .bit_c = 1,
                .bit_g = 1,
                .bit_j = 1,
                .bit_l = 1
            },
            .packed = "\x20\x00\x8c\x01\x00\x00\x00\x00"
        },
        {
            .unpacked = {
                .multiplexor = 16,
                .bit_c = 1,
                .bit_g = 1,
                .bit_j = 1,
                .bit_l = 1
            },
            .packed = "\x40\x00\x8c\x01\x00\x00\x00\x00"
        },
        {
            .unpacked = {
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
            .packed = "\x60\x00\x8c\x35\xc3\x00\x00\x00"
        }
    };
    struct multiplex_message1_t unpacked;
    uint8_t buf[8];
    int i;

    for (i = 0; i < 3; i++) {
        memset(&buf[0], 0, sizeof(buf));
        assert(multiplex_message1_pack(&buf[0],
                                       &datas[i].unpacked,
                                       sizeof(buf)) == 8);
        assert(memcmp(&buf[0],
                      &datas[i].packed[0],
                      sizeof(buf)) == 0);
        memset(&unpacked, 0, sizeof(unpacked));
        assert(multiplex_message1_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)) == 0);
        assert(unpacked.multiplexor == datas[i].unpacked.multiplexor);
        assert(unpacked.bit_a == datas[i].unpacked.bit_a);
        assert(unpacked.bit_b == datas[i].unpacked.bit_b);
        assert(unpacked.bit_c == datas[i].unpacked.bit_c);
        assert(unpacked.bit_d == datas[i].unpacked.bit_d);
        assert(unpacked.bit_e == datas[i].unpacked.bit_e);
        assert(unpacked.bit_f == datas[i].unpacked.bit_f);
        assert(unpacked.bit_g == datas[i].unpacked.bit_g);
        assert(unpacked.bit_h == datas[i].unpacked.bit_h);
        assert(unpacked.bit_j == datas[i].unpacked.bit_j);
        assert(unpacked.bit_k == datas[i].unpacked.bit_k);
        assert(unpacked.bit_l == datas[i].unpacked.bit_l);
    }
}

static void test_multiplex_2_extended(void)
{
    struct {
        struct multiplex_2_extended_t unpacked;
        uint8_t packed[8];
    } datas[3] = {
        {
            .unpacked = {
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
            .packed = "\x20\x10\x27\x00\x01\x21\x00\x00"
        },
        {
            .unpacked = {
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
            .packed = "\x00\x64\x88\x13\x02\x16\x00\x00"
        },
        {
            .unpacked = {
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
            .packed = "\x31\x00\x00\x00\x01\x04\x03\x00"
        },
    };
    struct multiplex_2_extended_t unpacked;
    uint8_t buf[8];
    int i;

    for (i = 0; i < 3; i++) {
        memset(&buf[0], 0, sizeof(buf));
        assert(multiplex_2_extended_pack(&buf[0],
                                         &datas[i].unpacked,
                                         sizeof(buf)) == 8);
        assert(memcmp(&buf[0],
                      &datas[i].packed[0],
                      sizeof(buf)) == 0);
        memset(&unpacked, 0, sizeof(unpacked));
        assert(multiplex_2_extended_unpack(&unpacked,
                                           &buf[0],
                                           sizeof(buf)) == 0);
        assert(unpacked.s0 == datas[i].unpacked.s0);
        assert(unpacked.s1 == datas[i].unpacked.s1);
        assert(unpacked.s2 == datas[i].unpacked.s2);
        assert(unpacked.s3 == datas[i].unpacked.s3);
        assert(unpacked.s4 == datas[i].unpacked.s4);
        assert(unpacked.s5 == datas[i].unpacked.s5);
        assert(unpacked.s6 == datas[i].unpacked.s6);
        assert(unpacked.s7 == datas[i].unpacked.s7);
        assert(unpacked.s8 == datas[i].unpacked.s8);
    }
}

static void test_floating_point_message1(void)
{
    double signal1 = -129.448;
    struct floating_point_message1_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;

    memset(&buf[0], 0, sizeof(buf));
    assert(floating_point_message1_pack(&buf[0],
                                        &unpacked,
                                        sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x75\x93\x18\x04\x56\x2e\x60\xc0",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(floating_point_message1_unpack(&unpacked,
                                          &buf[0],
                                          sizeof(buf)) == 0);
    assert(memcmp(&unpacked.signal1, &signal1, sizeof(unpacked.signal1)) == 0);
}

static void test_floating_point_message2(void)
{
    float signal1 = 129.5f;
    float signal2 = 1234500.5f;
    struct floating_point_message2_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;
    unpacked.signal2 = signal2;

    memset(&buf[0], 0, sizeof(buf));
    assert(floating_point_message2_pack(&buf[0],
                                        &unpacked,
                                        sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x00\x80\x01\x43\x24\xb2\x96\x49",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(floating_point_message2_unpack(&unpacked,
                                          &buf[0],
                                          sizeof(buf)) == 0);

    assert(memcmp(&unpacked.signal1, &signal1, sizeof(unpacked.signal1)) == 0);
    assert(memcmp(&unpacked.signal2, &signal2, sizeof(unpacked.signal2)) == 0);
}

static void test_floating_point(void)
{
    test_floating_point_message1();
    test_floating_point_message2();
}

static void test_signed_message64(void)
{
    struct signed_message64_t unpacked;
    uint8_t buf[8];

    unpacked.s64 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message64_pack(&buf[0],
                                 &unpacked,
                                 sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\xff\xff\xff\xff",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_message64_unpack(&unpacked,
                                   &buf[0],
                                   sizeof(buf)) == 0);
    assert(unpacked.s64 == -5);
}

static void test_signed_message33(void)
{
    struct signed_message33_t unpacked;
    uint8_t buf[8];

    unpacked.s33 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message33_pack(&buf[0],
                                 &unpacked,
                                 sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\x01\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_message33_unpack(&unpacked,
                                   &buf[0],
                                   sizeof(buf)) == 0);
    assert(unpacked.s33 == -5);
}

static void test_signed_message32(void)
{
    struct signed_message32_t unpacked;
    uint8_t buf[8];

    unpacked.s32 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message32_pack(&buf[0],
                                 &unpacked,
                                 sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_message32_unpack(&unpacked,
                                   &buf[0],
                                   sizeof(buf)) == 0);
    assert(unpacked.s32 == -5);
}

static void test_signed_message64big(void)
{
    struct signed_message64big_t unpacked;
    uint8_t buf[8];

    unpacked.s64big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message64big_pack(&buf[0],
                                    &unpacked,
                                    sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xff\xff\xff\xff\xfb",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_message64big_unpack(&unpacked,
                                      &buf[0],
                                      sizeof(buf)) == 0);
    assert(unpacked.s64big == -5);
}

static void test_signed_message33big(void)
{
    struct signed_message33big_t unpacked;
    uint8_t buf[8];

    unpacked.s33big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message33big_pack(&buf[0],
                                    &unpacked,
                                    sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xfd\x80\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_message33big_unpack(&unpacked,
                                      &buf[0],
                                      sizeof(buf)) == 0);
    assert(unpacked.s33big == -5);
}

static void test_signed_message32big(void)
{
    struct signed_message32big_t unpacked;
    uint8_t buf[8];

    unpacked.s32big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message32big_pack(&buf[0],
                                    &unpacked,
                                    sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xfb\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_message32big_unpack(&unpacked,
                                      &buf[0],
                                      sizeof(buf)) == 0);
    assert(unpacked.s32big == -5);
}

static void test_signed_message378910(void)
{
    struct signed_message378910_t unpacked;
    uint8_t buf[8];

    unpacked.s7 = -40;
    unpacked.s8big = 0x5a;
    unpacked.s9 = 0xa5;
    unpacked.s8 = -43;
    unpacked.s3big = -4;
    unpacked.s3 = 1;
    unpacked.s10big = -253;
    unpacked.s7big = -9;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_message378910_pack(&buf[0],
                                     &unpacked,
                                     sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xb0\xb4\x4a\x55\x87\x01\x81\xf7",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_message378910_unpack(&unpacked,
                                       &buf[0],
                                       sizeof(buf)) == 0);
    assert(unpacked.s7 == -40);
    assert(unpacked.s8big == 0x5a);
    assert(unpacked.s9 == 0xa5);
    assert(unpacked.s8 == -43);
    assert(unpacked.s3big == -4);
    assert(unpacked.s3 == 1);
    assert(unpacked.s10big == -253);
    assert(unpacked.s7big == -9);
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
    /* Missing limits. Check that it fits in its number of bits. */
    assert(motohawk_example_message_enable_is_in_range(0));
    assert(motohawk_example_message_enable_is_in_range(1));
    assert(motohawk_example_message_enable_is_in_range(2) == false);

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

static void test_encode_decode(void)
{
    /* Scale=1 and offset=0. */
    assert(motohawk_example_message_enable_encode(5.0) == 5);
    assert(fequal(motohawk_example_message_enable_decode(5), 5.0));

    /* Scale=0.1 and offset=0. */
    assert(motohawk_example_message_average_radius_encode(5.0) == 50);
    assert(fequal(motohawk_example_message_average_radius_decode(50), 5.0));

    /* Scale=0.01 and offset=250. */
    assert(motohawk_example_message_temperature_encode(251.0) == 100);
    assert(fequal(motohawk_example_message_temperature_decode(100), 251.0));
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
    test_encode_decode();

    return (0);
}
