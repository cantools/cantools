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
#include <string.h>
#include <math.h>
#include "nala.h"

static bool fequal(double v1, double v2)
{
    return (fabs(v1 - v2) < 0.000001);
}

TEST(motohawk_example_message)
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
        ASSERT_EQ(motohawk_example_message_pack(&buf[0],
                                                &datas[i].unpacked,
                                                sizeof(buf)), 8);
        ASSERT_MEMORY_EQ(&buf[0], &datas[i].packed[0], sizeof(buf));
        memset(&unpacked, 0, sizeof(unpacked));
        ASSERT_EQ(motohawk_example_message_unpack(&unpacked,
                                                  &buf[0],
                                                  sizeof(buf)), 0);
        ASSERT_TRUE(unpacked.temperature == datas[i].unpacked.temperature);
        ASSERT_TRUE(unpacked.average_radius == datas[i].unpacked.average_radius);
        ASSERT_TRUE(unpacked.enable == datas[i].unpacked.enable);

        ASSERT_TRUE(motohawk_example_message_enable_is_in_range(
                   unpacked.enable));
        ASSERT_TRUE(motohawk_example_message_average_radius_is_in_range(
                        unpacked.average_radius));
        ASSERT_TRUE(motohawk_example_message_temperature_is_in_range(
                        unpacked.temperature));
    }
}

TEST(padding_bit_order_msg0)
{
    struct padding_bit_order_msg0_t unpacked;
    uint8_t buf[8];

    unpacked.a = 0x2c9;
    unpacked.b = 1;
    unpacked.c = 0x2c9;
    unpacked.d = 0;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(padding_bit_order_msg0_pack(&buf[0],
                                          &unpacked,
                                          sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x82\xc9\x00\x00\x02\xc9\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(padding_bit_order_msg0_unpack(&unpacked,
                                            &buf[0],
                                            sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.a == 0x2c9);
    ASSERT_TRUE(unpacked.b == 1);
    ASSERT_TRUE(unpacked.c == 0x2c9);
    ASSERT_TRUE(unpacked.d == 0);
}

TEST(padding_bit_order_msg1)
{
    struct padding_bit_order_msg1_t unpacked;
    uint8_t buf[8];

    unpacked.e = 1;
    unpacked.f = 0x2c9;
    unpacked.g = 0;
    unpacked.h = 0x2c9;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(padding_bit_order_msg1_pack(&buf[0],
                                          &unpacked,
                                          sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x93\x05\x00\x00\x92\x05\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(padding_bit_order_msg1_unpack(&unpacked,
                                            &buf[0],
                                            sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.e == 1);
    ASSERT_TRUE(unpacked.f == 0x2c9);
    ASSERT_TRUE(unpacked.g == 0);
    ASSERT_TRUE(unpacked.h == 0x2c9);
}

TEST(padding_bit_order_msg2)
{
    struct padding_bit_order_msg2_t unpacked;
    uint8_t buf[8];

    unpacked.i = 1;
    unpacked.j = 2;
    unpacked.k = 3;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(padding_bit_order_msg2_pack(&buf[0],
                                          &unpacked,
                                          sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x21\x03\x00\x00\x00\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(padding_bit_order_msg2_unpack(&unpacked,
                                            &buf[0],
                                            sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.i == 1);
    ASSERT_TRUE(unpacked.j == 2);
    ASSERT_TRUE(unpacked.k == 3);
}

TEST(padding_bit_order_msg3)
{
    struct padding_bit_order_msg3_t unpacked;
    uint8_t buf[8];

    unpacked.l = 0x0123456789abcdef;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(padding_bit_order_msg3_pack(&buf[0],
                                          &unpacked,
                                          sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x01\x23\x45\x67\x89\xab\xcd\xef",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(padding_bit_order_msg3_unpack(&unpacked,
                                            &buf[0],
                                            sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.l == 0x0123456789abcdef);
}

TEST(padding_bit_order_msg4)
{
    struct padding_bit_order_msg4_t unpacked;
    uint8_t buf[8];

    unpacked.m = 0x0123456789abcdef;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(padding_bit_order_msg4_pack(&buf[0],
                                          &unpacked,
                                          sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xef\xcd\xab\x89\x67\x45\x23\x01",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(padding_bit_order_msg4_unpack(&unpacked,
                                            &buf[0],
                                            sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.m == 0x0123456789abcdef);
}

TEST(multiplex)
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
        ASSERT_EQ(multiplex_message1_pack(&buf[0],
                                          &datas[i].unpacked,
                                          sizeof(buf)), 8);
        ASSERT_MEMORY_EQ(&buf[0], &datas[i].packed[0], sizeof(buf));
        memset(&unpacked, 0, sizeof(unpacked));
        ASSERT_EQ(multiplex_message1_unpack(&unpacked,
                                            &buf[0],
                                            sizeof(buf)), 0);
        ASSERT_TRUE(unpacked.multiplexor == datas[i].unpacked.multiplexor);
        ASSERT_TRUE(unpacked.bit_a == datas[i].unpacked.bit_a);
        ASSERT_TRUE(unpacked.bit_b == datas[i].unpacked.bit_b);
        ASSERT_TRUE(unpacked.bit_c == datas[i].unpacked.bit_c);
        ASSERT_TRUE(unpacked.bit_d == datas[i].unpacked.bit_d);
        ASSERT_TRUE(unpacked.bit_e == datas[i].unpacked.bit_e);
        ASSERT_TRUE(unpacked.bit_f == datas[i].unpacked.bit_f);
        ASSERT_TRUE(unpacked.bit_g == datas[i].unpacked.bit_g);
        ASSERT_TRUE(unpacked.bit_h == datas[i].unpacked.bit_h);
        ASSERT_TRUE(unpacked.bit_j == datas[i].unpacked.bit_j);
        ASSERT_TRUE(unpacked.bit_k == datas[i].unpacked.bit_k);
        ASSERT_TRUE(unpacked.bit_l == datas[i].unpacked.bit_l);
    }
}

TEST(multiplex_2_extended)
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
        ASSERT_EQ(multiplex_2_extended_pack(&buf[0],
                                            &datas[i].unpacked,
                                            sizeof(buf)), 8);
        ASSERT_MEMORY_EQ(&buf[0], &datas[i].packed[0], sizeof(buf));
        memset(&unpacked, 0, sizeof(unpacked));
        ASSERT_EQ(multiplex_2_extended_unpack(&unpacked,
                                              &buf[0],
                                              sizeof(buf)), 0);
        ASSERT_TRUE(unpacked.s0 == datas[i].unpacked.s0);
        ASSERT_TRUE(unpacked.s1 == datas[i].unpacked.s1);
        ASSERT_TRUE(unpacked.s2 == datas[i].unpacked.s2);
        ASSERT_TRUE(unpacked.s3 == datas[i].unpacked.s3);
        ASSERT_TRUE(unpacked.s4 == datas[i].unpacked.s4);
        ASSERT_TRUE(unpacked.s5 == datas[i].unpacked.s5);
        ASSERT_TRUE(unpacked.s6 == datas[i].unpacked.s6);
        ASSERT_TRUE(unpacked.s7 == datas[i].unpacked.s7);
        ASSERT_TRUE(unpacked.s8 == datas[i].unpacked.s8);
    }
}

TEST(floating_point_message1)
{
    double signal1 = -129.448;
    struct floating_point_message1_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(floating_point_message1_pack(&buf[0],
                                           &unpacked,
                                           sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x75\x93\x18\x04\x56\x2e\x60\xc0",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(floating_point_message1_unpack(&unpacked,
                                             &buf[0],
                                             sizeof(buf)), 0);
    ASSERT_MEMORY_EQ(&unpacked.signal1, &signal1, sizeof(unpacked.signal1));
}

TEST(floating_point_message2)
{
    float signal1 = 129.5f;
    float signal2 = 1234500.5f;
    struct floating_point_message2_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;
    unpacked.signal2 = signal2;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(floating_point_message2_pack(&buf[0],
                                           &unpacked,
                                           sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x00\x80\x01\x43\x24\xb2\x96\x49",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(floating_point_message2_unpack(&unpacked,
                                             &buf[0],
                                             sizeof(buf)), 0);

    ASSERT_MEMORY_EQ(&unpacked.signal1, &signal1, sizeof(unpacked.signal1));
    ASSERT_MEMORY_EQ(&unpacked.signal2, &signal2, sizeof(unpacked.signal2));
}

TEST(signed_message64)
{
    struct signed_message64_t unpacked;
    uint8_t buf[8];

    unpacked.s64 = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_message64_pack(&buf[0],
                                    &unpacked,
                                    sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xfb\xff\xff\xff\xff\xff\xff\xff",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_message64_unpack(&unpacked,
                                      &buf[0],
                                      sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s64 == -5);
}

TEST(signed_message33)
{
    struct signed_message33_t unpacked;
    uint8_t buf[8];

    unpacked.s33 = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_message33_pack(&buf[0],
                                    &unpacked,
                                    sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xfb\xff\xff\xff\x01\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_message33_unpack(&unpacked,
                                      &buf[0],
                                      sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s33 == -5);
}

TEST(signed_message32)
{
    struct signed_message32_t unpacked;
    uint8_t buf[8];

    unpacked.s32 = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_message32_pack(&buf[0],
                                    &unpacked,
                                    sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xfb\xff\xff\xff\x00\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_message32_unpack(&unpacked,
                                      &buf[0],
                                      sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s32 == -5);
}

TEST(signed_message64big)
{
    struct signed_message64big_t unpacked;
    uint8_t buf[8];

    unpacked.s64big = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_message64big_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xff\xff\xff\xff\xff\xff\xff\xfb",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_message64big_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s64big == -5);
}

TEST(signed_message33big)
{
    struct signed_message33big_t unpacked;
    uint8_t buf[8];

    unpacked.s33big = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_message33big_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xff\xff\xff\xfd\x80\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_message33big_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s33big == -5);
}

TEST(signed_message32big)
{
    struct signed_message32big_t unpacked;
    uint8_t buf[8];

    unpacked.s32big = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_message32big_pack(&buf[0],
                                       &unpacked,
                                       sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xff\xff\xff\xfb\x00\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_message32big_unpack(&unpacked,
                                         &buf[0],
                                         sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s32big == -5);
}

TEST(signed_message378910)
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
    ASSERT_EQ(signed_message378910_pack(&buf[0],
                                        &unpacked,
                                        sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xb0\xb4\x4a\x55\x87\x01\x81\xf7",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_message378910_unpack(&unpacked,
                                          &buf[0],
                                          sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s7 == -40);
    ASSERT_TRUE(unpacked.s8big == 0x5a);
    ASSERT_TRUE(unpacked.s9 == 0xa5);
    ASSERT_TRUE(unpacked.s8 == -43);
    ASSERT_TRUE(unpacked.s3big == -4);
    ASSERT_TRUE(unpacked.s3 == 1);
    ASSERT_TRUE(unpacked.s10big == -253);
    ASSERT_TRUE(unpacked.s7big == -9);
}

TEST(is_in_range)
{
    /* Missing limits. Check that it fits in its number of bits. */
    ASSERT_TRUE(motohawk_example_message_enable_is_in_range(0));
    ASSERT_TRUE(motohawk_example_message_enable_is_in_range(1));
    ASSERT_FALSE(motohawk_example_message_enable_is_in_range(2));

    /* Unsigned value with 0 as lower limit. */
    ASSERT_TRUE(motohawk_example_message_average_radius_is_in_range(0));
    ASSERT_TRUE(motohawk_example_message_average_radius_is_in_range(50));
    ASSERT_FALSE(motohawk_example_message_average_radius_is_in_range(51));

    /* Signed value with upper and lower limits. */
    ASSERT_FALSE(motohawk_example_message_temperature_is_in_range(-2049));
    ASSERT_TRUE(motohawk_example_message_temperature_is_in_range(-2048));
    ASSERT_TRUE(motohawk_example_message_temperature_is_in_range(2047));
    ASSERT_FALSE(motohawk_example_message_temperature_is_in_range(2048));
}

TEST(encode_decode)
{
    /* Scale=1 and offset=0. */
    ASSERT_EQ(motohawk_example_message_enable_encode(5.0), 5);
    ASSERT_TRUE(fequal(motohawk_example_message_enable_decode(5), 5.0));

    /* Scale=0.1 and offset=0. */
    ASSERT_EQ(motohawk_example_message_average_radius_encode(5.0), 50);
    ASSERT_TRUE(fequal(motohawk_example_message_average_radius_decode(50), 5.0));

    /* Scale=0.01 and offset=250. */
    ASSERT_EQ(motohawk_example_message_temperature_encode(251.0), 100);
    ASSERT_TRUE(fequal(motohawk_example_message_temperature_decode(100), 251.0));
}

TEST(unpack_does_not_modify_other_mux_signals)
{
    uint8_t packed_mux_24[] = "\x60\x00\x8c\x35\xc3\x00\x00\x00";
    uint8_t packed_mux_8[] = "\x20\x00\x00\x00\x00\x00\x00\x00";
    struct multiplex_message1_t unpacked;

    memset(&unpacked, 0, sizeof(unpacked));

    /* First unpack, mux 24, which includes all signals. */
    ASSERT_EQ(multiplex_message1_unpack(&unpacked,
                                        &packed_mux_24[0],
                                        sizeof(packed_mux_24)), 0);
    ASSERT_EQ(unpacked.multiplexor, 24);
    ASSERT_EQ(unpacked.bit_a, 1);
    ASSERT_EQ(unpacked.bit_b, 1);
    ASSERT_EQ(unpacked.bit_c, 1);
    ASSERT_EQ(unpacked.bit_d, 1);
    ASSERT_EQ(unpacked.bit_e, 1);
    ASSERT_EQ(unpacked.bit_f, 1);
    ASSERT_EQ(unpacked.bit_g, 1);
    ASSERT_EQ(unpacked.bit_h, 1);
    ASSERT_EQ(unpacked.bit_j, 1);
    ASSERT_EQ(unpacked.bit_k, 1);
    ASSERT_EQ(unpacked.bit_l, 1);

    /* Secon unpack, mux 8, which only includes four signals, which
       are all set to zero(0). */
    ASSERT_EQ(multiplex_message1_unpack(&unpacked,
                                        &packed_mux_8[0],
                                        sizeof(packed_mux_8)), 0);
    ASSERT_EQ(unpacked.multiplexor, 8);
    ASSERT_EQ(unpacked.bit_a, 1);
    ASSERT_EQ(unpacked.bit_b, 1);
    ASSERT_EQ(unpacked.bit_c, 0);
    ASSERT_EQ(unpacked.bit_d, 1);
    ASSERT_EQ(unpacked.bit_e, 1);
    ASSERT_EQ(unpacked.bit_f, 1);
    ASSERT_EQ(unpacked.bit_g, 0);
    ASSERT_EQ(unpacked.bit_h, 1);
    ASSERT_EQ(unpacked.bit_j, 0);
    ASSERT_EQ(unpacked.bit_k, 1);
    ASSERT_EQ(unpacked.bit_l, 0);
}
