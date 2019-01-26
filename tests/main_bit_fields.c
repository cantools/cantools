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

static void test_motohawk_bit_fields_example_message(void)
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
        assert(motohawk_bit_fields_example_message_pack(&buf[0],
                                                        &datas[i].unpacked,
                                                        sizeof(buf)) == 8);
        assert(memcmp(&buf[0],
                      &datas[i].packed[0],
                      sizeof(buf)) == 0);
        memset(&unpacked, 0, sizeof(unpacked));
        assert(motohawk_bit_fields_example_message_unpack(&unpacked,
                                                          &buf[0],
                                                          sizeof(buf)) == 0);
        assert(unpacked.temperature == datas[i].unpacked.temperature);
        assert(unpacked.average_radius == datas[i].unpacked.average_radius);
        assert(unpacked.enable == datas[i].unpacked.enable);

        assert(motohawk_bit_fields_example_message_enable_is_in_range(
                   unpacked.enable));
        assert(motohawk_bit_fields_example_message_average_radius_is_in_range(
                   unpacked.average_radius));
        assert(motohawk_bit_fields_example_message_temperature_is_in_range(
                   unpacked.temperature));
    }
}

static void test_floating_point_bit_fields_message1(void)
{
    double signal1 = -129.448;
    struct floating_point_bit_fields_message1_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;

    memset(&buf[0], 0, sizeof(buf));
    assert(floating_point_bit_fields_message1_pack(&buf[0],
                                                   &unpacked,
                                                   sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x75\x93\x18\x04\x56\x2e\x60\xc0",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(floating_point_bit_fields_message1_unpack(&unpacked,
                                                     &buf[0],
                                                     sizeof(buf)) == 0);
    assert(memcmp(&unpacked.signal1, &signal1, sizeof(unpacked.signal1)) == 0);
}

static void test_floating_point_bit_fields_message2(void)
{
    float signal1 = 129.5f;
    float signal2 = 1234500.5f;
    struct floating_point_bit_fields_message2_t unpacked;
    uint8_t buf[8];

    unpacked.signal1 = signal1;
    unpacked.signal2 = signal2;

    memset(&buf[0], 0, sizeof(buf));
    assert(floating_point_bit_fields_message2_pack(&buf[0],
                                                   &unpacked,
                                                   sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\x00\x80\x01\x43\x24\xb2\x96\x49",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(floating_point_bit_fields_message2_unpack(&unpacked,
                                                     &buf[0],
                                                     sizeof(buf)) == 0);

    assert(memcmp(&unpacked.signal1, &signal1, sizeof(unpacked.signal1)) == 0);
    assert(memcmp(&unpacked.signal2, &signal2, sizeof(unpacked.signal2)) == 0);
}

static void test_floating_point_bit_fields(void)
{
    test_floating_point_bit_fields_message1();
    test_floating_point_bit_fields_message2();
}

static void test_signed_bit_fields_message64(void)
{
    struct signed_bit_fields_message64_t unpacked;
    uint8_t buf[8];

    unpacked.s64 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_bit_fields_message64_pack(&buf[0],
                                            &unpacked,
                                            sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\xff\xff\xff\xff",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_bit_fields_message64_unpack(&unpacked,
                                              &buf[0],
                                              sizeof(buf)) == 0);
    assert(unpacked.s64 == -5);
}

static void test_signed_bit_fields_message33(void)
{
    struct signed_bit_fields_message33_t unpacked;
    uint8_t buf[8];

    unpacked.s33 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_bit_fields_message33_pack(&buf[0],
                                            &unpacked,
                                            sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\x01\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_bit_fields_message33_unpack(&unpacked,
                                              &buf[0],
                                              sizeof(buf)) == 0);
    assert(unpacked.s33 == -5);
}

static void test_signed_bit_fields_message32(void)
{
    struct signed_bit_fields_message32_t unpacked;
    uint8_t buf[8];

    unpacked.s32 = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_bit_fields_message32_pack(&buf[0],
                                            &unpacked,
                                            sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xfb\xff\xff\xff\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_bit_fields_message32_unpack(&unpacked,
                                              &buf[0],
                                              sizeof(buf)) == 0);
    assert(unpacked.s32 == -5);
}

static void test_signed_bit_fields_message64big(void)
{
    struct signed_bit_fields_message64big_t unpacked;
    uint8_t buf[8];

    unpacked.s64big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_bit_fields_message64big_pack(&buf[0],
                                               &unpacked,
                                               sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xff\xff\xff\xff\xfb",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_bit_fields_message64big_unpack(&unpacked,
                                                 &buf[0],
                                                 sizeof(buf)) == 0);
    assert(unpacked.s64big == -5);
}

static void test_signed_bit_fields_message33big(void)
{
    struct signed_bit_fields_message33big_t unpacked;
    uint8_t buf[8];

    unpacked.s33big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_bit_fields_message33big_pack(&buf[0],
                                               &unpacked,
                                               sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xfd\x80\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_bit_fields_message33big_unpack(&unpacked,
                                                 &buf[0],
                                                 sizeof(buf)) == 0);
    assert(unpacked.s33big == -5);
}

static void test_signed_bit_fields_message32big(void)
{
    struct signed_bit_fields_message32big_t unpacked;
    uint8_t buf[8];

    unpacked.s32big = -5;

    memset(&buf[0], 0, sizeof(buf));
    assert(signed_bit_fields_message32big_pack(&buf[0],
                                               &unpacked,
                                               sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xff\xff\xff\xfb\x00\x00\x00\x00",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_bit_fields_message32big_unpack(&unpacked,
                                                 &buf[0],
                                                 sizeof(buf)) == 0);
    assert(unpacked.s32big == -5);
}

static void test_signed_bit_fields_message378910(void)
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
    assert(signed_bit_fields_message378910_pack(&buf[0],
                                                &unpacked,
                                                sizeof(buf)) == 8);
    assert(memcmp(&buf[0],
                  "\xb0\xb4\x4a\x55\x87\x01\x81\xf7",
                  sizeof(buf)) == 0);
    memset(&unpacked, 0, sizeof(unpacked));
    assert(signed_bit_fields_message378910_unpack(&unpacked,
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

static void test_signed_bit_fields(void)
{
    test_signed_bit_fields_message64();
    test_signed_bit_fields_message33();
    test_signed_bit_fields_message32();
    test_signed_bit_fields_message64big();
    test_signed_bit_fields_message33big();
    test_signed_bit_fields_message32big();
    test_signed_bit_fields_message378910();
}

static void test_is_in_range(void)
{
    /* Missing limits. Anything allowed, byt should probably check
       that it fits in its number of bits. */
    assert(motohawk_bit_fields_example_message_enable_is_in_range(0));
    assert(motohawk_bit_fields_example_message_enable_is_in_range(1));

    /* Unsigned_bit_fields value with 0 as lower limit. */
    assert(motohawk_bit_fields_example_message_average_radius_is_in_range(0));
    assert(motohawk_bit_fields_example_message_average_radius_is_in_range(50));
    assert(motohawk_bit_fields_example_message_average_radius_is_in_range(51) == false);

    /* Signed_Bit_Fields value with upper and lower limits. */
    assert(motohawk_bit_fields_example_message_temperature_is_in_range(-2049) == false);
    assert(motohawk_bit_fields_example_message_temperature_is_in_range(-2048));
    assert(motohawk_bit_fields_example_message_temperature_is_in_range(2047));
    assert(motohawk_bit_fields_example_message_temperature_is_in_range(2048) == false);
}

static void test_sizeof(void)
{
    assert(sizeof(struct motohawk_example_message_t) == 4);
    assert(sizeof(struct motohawk_bit_fields_example_message_t) == 3);

    assert(sizeof(struct floating_point_message1_t) == 8);
    assert(sizeof(struct floating_point_bit_fields_message1_t) == 8);

    assert(sizeof(struct floating_point_message2_t) == 8);
    assert(sizeof(struct floating_point_bit_fields_message2_t) == 8);

    assert(sizeof(struct signed_message64_t) == 8);
    assert(sizeof(struct signed_bit_fields_message64_t) == 8);

    assert(sizeof(struct signed_message33_t) == 8);
    assert(sizeof(struct signed_bit_fields_message33_t) == 5);

    assert(sizeof(struct signed_message32_t) == 4);
    assert(sizeof(struct signed_bit_fields_message32_t) == 4);

    assert(sizeof(struct signed_message64big_t) == 8);
    assert(sizeof(struct signed_bit_fields_message64big_t) == 8);

    assert(sizeof(struct signed_message33big_t) == 8);
    assert(sizeof(struct signed_bit_fields_message33big_t) == 5);

    assert(sizeof(struct signed_message32big_t) == 4);
    assert(sizeof(struct signed_bit_fields_message32big_t) == 4);

    assert(sizeof(struct signed_message378910_t) == 10);
    assert(sizeof(struct signed_bit_fields_message378910_t) == 7);
}

int main(void)
{
    test_motohawk_bit_fields_example_message();
    test_floating_point_bit_fields();
    test_signed_bit_fields();
    test_is_in_range();
    test_sizeof();

    return (0);
}
