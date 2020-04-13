/* Include the generated files first to test that all required header
   files are included. */
#include "files/c_source/motohawk.h"
#include "files/c_source/motohawk_bit_fields.h"
#include "files/c_source/floating_point.h"
#include "files/c_source/floating_point_bit_fields.h"
#include "files/c_source/signed.h"
#include "files/c_source/signed_bit_fields.h"

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "nala.h"

TEST(motohawk_bit_fields_example_message)
{
    struct {
        struct motohawk_bit_fields_example_message_t unpacked;
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
    struct motohawk_bit_fields_example_message_t unpacked;
    uint8_t buf[8];
    int i;

    for (i = 0; i < 3; i++) {
        memset(&buf[0], 0, sizeof(buf));
        ASSERT_EQ(motohawk_bit_fields_example_message_pack(&buf[0],
                                                           &datas[i].unpacked,
                                                           sizeof(buf)), 8);
        ASSERT_MEMORY_EQ(&buf[0], &datas[i].packed[0], sizeof(buf));
        memset(&unpacked, 0, sizeof(unpacked));
        ASSERT_EQ(motohawk_bit_fields_example_message_unpack(&unpacked,
                                                             &buf[0],
                                                             sizeof(buf)), 0);
        ASSERT_TRUE(unpacked.temperature == datas[i].unpacked.temperature);
        ASSERT_TRUE(unpacked.average_radius == datas[i].unpacked.average_radius);
        ASSERT_TRUE(unpacked.enable == datas[i].unpacked.enable);

        ASSERT_TRUE(motohawk_bit_fields_example_message_enable_is_in_range(
                        unpacked.enable));
        ASSERT_TRUE(motohawk_bit_fields_example_message_average_radius_is_in_range(
                        unpacked.average_radius));
        ASSERT_TRUE(motohawk_bit_fields_example_message_temperature_is_in_range(
                        unpacked.temperature));
    }
}

TEST(floating_point_bit_fields_message1)
{
    double signal1 = -129.448;
    struct floating_point_bit_fields_message1_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(floating_point_bit_fields_message1_pack(&buf[0],
                                                      &unpacked,
                                                      sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x75\x93\x18\x04\x56\x2e\x60\xc0",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(floating_point_bit_fields_message1_unpack(&unpacked,
                                                        &buf[0],
                                                        sizeof(buf)), 0);
    ASSERT_MEMORY_EQ(&unpacked.signal1, &signal1, sizeof(unpacked.signal1));
}

TEST(floating_point_bit_fields_message2)
{
    float signal1 = 129.5f;
    float signal2 = 1234500.5f;
    struct floating_point_bit_fields_message2_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;
    unpacked.signal2 = signal2;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(floating_point_bit_fields_message2_pack(&buf[0],
                                                      &unpacked,
                                                      sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\x00\x80\x01\x43\x24\xb2\x96\x49",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(floating_point_bit_fields_message2_unpack(&unpacked,
                                                        &buf[0],
                                                        sizeof(buf)), 0);

    ASSERT_MEMORY_EQ(&unpacked.signal1, &signal1, sizeof(unpacked.signal1));
    ASSERT_MEMORY_EQ(&unpacked.signal2, &signal2, sizeof(unpacked.signal2));
}

TEST(signed_bit_fields_message64)
{
    struct signed_bit_fields_message64_t unpacked;
    uint8_t buf[8];

    unpacked.s64 = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_bit_fields_message64_pack(&buf[0],
                                               &unpacked,
                                               sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xfb\xff\xff\xff\xff\xff\xff\xff",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_bit_fields_message64_unpack(&unpacked,
                                                 &buf[0],
                                                 sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s64 == -5);
}

TEST(signed_bit_fields_message33)
{
    struct signed_bit_fields_message33_t unpacked;
    uint8_t buf[8];

    unpacked.s33 = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_bit_fields_message33_pack(&buf[0],
                                               &unpacked,
                                               sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xfb\xff\xff\xff\x01\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_bit_fields_message33_unpack(&unpacked,
                                                 &buf[0],
                                                 sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s33 == -5);
}

TEST(signed_bit_fields_message32)
{
    struct signed_bit_fields_message32_t unpacked;
    uint8_t buf[8];

    unpacked.s32 = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_bit_fields_message32_pack(&buf[0],
                                               &unpacked,
                                               sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xfb\xff\xff\xff\x00\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_bit_fields_message32_unpack(&unpacked,
                                                 &buf[0],
                                                 sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s32 == -5);
}

TEST(signed_bit_fields_message64big)
{
    struct signed_bit_fields_message64big_t unpacked;
    uint8_t buf[8];

    unpacked.s64big = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_bit_fields_message64big_pack(&buf[0],
                                                  &unpacked,
                                                  sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xff\xff\xff\xff\xff\xff\xff\xfb",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_bit_fields_message64big_unpack(&unpacked,
                                                    &buf[0],
                                                    sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s64big == -5);
}

TEST(signed_bit_fields_message33big)
{
    struct signed_bit_fields_message33big_t unpacked;
    uint8_t buf[8];

    unpacked.s33big = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_bit_fields_message33big_pack(&buf[0],
                                                  &unpacked,
                                                  sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xff\xff\xff\xfd\x80\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_bit_fields_message33big_unpack(&unpacked,
                                                    &buf[0],
                                                    sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s33big == -5);
}

TEST(signed_bit_fields_message32big)
{
    struct signed_bit_fields_message32big_t unpacked;
    uint8_t buf[8];

    unpacked.s32big = -5;

    memset(&buf[0], 0, sizeof(buf));
    ASSERT_EQ(signed_bit_fields_message32big_pack(&buf[0],
                                                  &unpacked,
                                                  sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xff\xff\xff\xfb\x00\x00\x00\x00",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_bit_fields_message32big_unpack(&unpacked,
                                                    &buf[0],
                                                    sizeof(buf)), 0);
    ASSERT_TRUE(unpacked.s32big == -5);
}

TEST(signed_bit_fields_message378910)
{
    struct signed_bit_fields_message378910_t unpacked;
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
    ASSERT_EQ(signed_bit_fields_message378910_pack(&buf[0],
                                                   &unpacked,
                                                   sizeof(buf)), 8);
    ASSERT_MEMORY_EQ(&buf[0],
                     "\xb0\xb4\x4a\x55\x87\x01\x81\xf7",
                     sizeof(buf));
    memset(&unpacked, 0, sizeof(unpacked));
    ASSERT_EQ(signed_bit_fields_message378910_unpack(&unpacked,
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
    /* Missing limits. Anything allowed, byt should probably check
       that it fits in its number of bits. */
    ASSERT_TRUE(motohawk_bit_fields_example_message_enable_is_in_range(0));
    ASSERT_TRUE(motohawk_bit_fields_example_message_enable_is_in_range(1));

    /* Unsigned_bit_fields value with 0 as lower limit. */
    ASSERT_TRUE(motohawk_bit_fields_example_message_average_radius_is_in_range(0));
    ASSERT_TRUE(motohawk_bit_fields_example_message_average_radius_is_in_range(50));
    ASSERT_FALSE(motohawk_bit_fields_example_message_average_radius_is_in_range(51));

    /* Signed_Bit_Fields value with upper and lower limits. */
    ASSERT_FALSE(motohawk_bit_fields_example_message_temperature_is_in_range(-2049));
    ASSERT_TRUE(motohawk_bit_fields_example_message_temperature_is_in_range(-2048));
    ASSERT_TRUE(motohawk_bit_fields_example_message_temperature_is_in_range(2047));
    ASSERT_FALSE(motohawk_bit_fields_example_message_temperature_is_in_range(2048));
}

TEST(sizeof_types)
{
    ASSERT_EQ(sizeof(struct motohawk_example_message_t), 4);
    ASSERT_EQ(sizeof(struct motohawk_bit_fields_example_message_t), 3);

    ASSERT_EQ(sizeof(struct floating_point_message1_t), 8);
    ASSERT_EQ(sizeof(struct floating_point_bit_fields_message1_t), 8);

    ASSERT_EQ(sizeof(struct floating_point_message2_t), 8);
    ASSERT_EQ(sizeof(struct floating_point_bit_fields_message2_t), 8);

    ASSERT_EQ(sizeof(struct signed_message64_t), 8);
    ASSERT_EQ(sizeof(struct signed_bit_fields_message64_t), 8);

    ASSERT_EQ(sizeof(struct signed_message33_t), 8);
    ASSERT_EQ(sizeof(struct signed_bit_fields_message33_t), 5);

    ASSERT_EQ(sizeof(struct signed_message32_t), 4);
    ASSERT_EQ(sizeof(struct signed_bit_fields_message32_t), 4);

    ASSERT_EQ(sizeof(struct signed_message64big_t), 8);
    ASSERT_EQ(sizeof(struct signed_bit_fields_message64big_t), 8);

    ASSERT_EQ(sizeof(struct signed_message33big_t), 8);
    ASSERT_EQ(sizeof(struct signed_bit_fields_message33big_t), 5);

    ASSERT_EQ(sizeof(struct signed_message32big_t), 4);
    ASSERT_EQ(sizeof(struct signed_bit_fields_message32big_t), 4);

    ASSERT_EQ(sizeof(struct signed_message378910_t), 10);
    ASSERT_EQ(sizeof(struct signed_bit_fields_message378910_t), 7);
}
