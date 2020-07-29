/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Erik Moqvist
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**

 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "multiplex_2.h"

static void assert_first_pack(int res)
{
    if (res < 0) {
        printf("First pack failed with %ld.\n", res);
        __builtin_trap();
    }
}

static void assert_second_unpack(int res)
{
    if (res < 0) {
        printf("Second unpack failed with %ld.\n", res);
        __builtin_trap();
    }
}

static void assert_second_unpack_data(const void *unpacked_p,
                                      const void *unpacked2_p,
                                      size_t size)
{
    if (memcmp(unpacked_p, unpacked2_p, size) != 0) {
        printf("Second unpacked data does not match first unpacked data.\n");
        __builtin_trap();
    }
}

static void assert_second_pack(int res, int res2)
{
    if (res != res2) {
        printf("Second pack result %ld does not match first pack "
               "result %ld.\n",
               res,
               res2);
        __builtin_trap();
    }
}

static void assert_second_pack_data(const uint8_t *packed_p,
                                    const uint8_t *packed2_p,
                                    int size)
{
    int i;

    if (memcmp(packed_p, packed2_p, size) != 0) {
        for (i = 0; i < size; i++) {
            printf("[%04ld]: 0x%02x 0x%02x\n", i, packed_p[i], packed2_p[i]);
        }

        __builtin_trap();
    }
}


static void test_multiplex_2_shared(
    const uint8_t *packed_p,
    size_t size)
{
    int res;
    int res2;
    uint8_t packed[size];
    uint8_t packed2[size];
    struct multiplex_2_shared_t unpacked;
    struct multiplex_2_shared_t unpacked2;

    memset(&unpacked, 0, sizeof(unpacked));

    res = multiplex_2_shared_unpack(
        &unpacked,
        packed_p,
        size);

    if (res >= 0) {
        res = multiplex_2_shared_pack(
            &packed[0],
            &unpacked,
            sizeof(packed));

        assert_first_pack(res);

        memset(&unpacked2, 0, sizeof(unpacked2));

        res2 = multiplex_2_shared_unpack(
            &unpacked2,
            &packed[0],
            res);

        assert_second_unpack(res2);
        assert_second_unpack_data(&unpacked,
                                  &unpacked2,
                                  sizeof(unpacked));

        res2 = multiplex_2_shared_pack(
            &packed2[0],
            &unpacked,
            sizeof(packed2));

        assert_second_pack(res, res2);
        assert_second_pack_data(&packed[0], &packed2[0], res);
    }
}

static void test_multiplex_2_normal(
    const uint8_t *packed_p,
    size_t size)
{
    int res;
    int res2;
    uint8_t packed[size];
    uint8_t packed2[size];
    struct multiplex_2_normal_t unpacked;
    struct multiplex_2_normal_t unpacked2;

    memset(&unpacked, 0, sizeof(unpacked));

    res = multiplex_2_normal_unpack(
        &unpacked,
        packed_p,
        size);

    if (res >= 0) {
        res = multiplex_2_normal_pack(
            &packed[0],
            &unpacked,
            sizeof(packed));

        assert_first_pack(res);

        memset(&unpacked2, 0, sizeof(unpacked2));

        res2 = multiplex_2_normal_unpack(
            &unpacked2,
            &packed[0],
            res);

        assert_second_unpack(res2);
        assert_second_unpack_data(&unpacked,
                                  &unpacked2,
                                  sizeof(unpacked));

        res2 = multiplex_2_normal_pack(
            &packed2[0],
            &unpacked,
            sizeof(packed2));

        assert_second_pack(res, res2);
        assert_second_pack_data(&packed[0], &packed2[0], res);
    }
}

static void test_multiplex_2_extended(
    const uint8_t *packed_p,
    size_t size)
{
    int res;
    int res2;
    uint8_t packed[size];
    uint8_t packed2[size];
    struct multiplex_2_extended_t unpacked;
    struct multiplex_2_extended_t unpacked2;

    memset(&unpacked, 0, sizeof(unpacked));

    res = multiplex_2_extended_unpack(
        &unpacked,
        packed_p,
        size);

    if (res >= 0) {
        res = multiplex_2_extended_pack(
            &packed[0],
            &unpacked,
            sizeof(packed));

        assert_first_pack(res);

        memset(&unpacked2, 0, sizeof(unpacked2));

        res2 = multiplex_2_extended_unpack(
            &unpacked2,
            &packed[0],
            res);

        assert_second_unpack(res2);
        assert_second_unpack_data(&unpacked,
                                  &unpacked2,
                                  sizeof(unpacked));

        res2 = multiplex_2_extended_pack(
            &packed2[0],
            &unpacked,
            sizeof(packed2));

        assert_second_pack(res, res2);
        assert_second_pack_data(&packed[0], &packed2[0], res);
    }
}

static void test_multiplex_2_extended_types(
    const uint8_t *packed_p,
    size_t size)
{
    int res;
    int res2;
    uint8_t packed[size];
    uint8_t packed2[size];
    struct multiplex_2_extended_types_t unpacked;
    struct multiplex_2_extended_types_t unpacked2;

    memset(&unpacked, 0, sizeof(unpacked));

    res = multiplex_2_extended_types_unpack(
        &unpacked,
        packed_p,
        size);

    if (res >= 0) {
        res = multiplex_2_extended_types_pack(
            &packed[0],
            &unpacked,
            sizeof(packed));

        assert_first_pack(res);

        memset(&unpacked2, 0, sizeof(unpacked2));

        res2 = multiplex_2_extended_types_unpack(
            &unpacked2,
            &packed[0],
            res);

        assert_second_unpack(res2);
        assert_second_unpack_data(&unpacked,
                                  &unpacked2,
                                  sizeof(unpacked));

        res2 = multiplex_2_extended_types_pack(
            &packed2[0],
            &unpacked,
            sizeof(packed2));

        assert_second_pack(res, res2);
        assert_second_pack_data(&packed[0], &packed2[0], res);
    }
}

int LLVMFuzzerTestOneInput(const uint8_t *data_p, size_t size)
{
    test_multiplex_2_shared(data_p, size);
    test_multiplex_2_normal(data_p, size);
    test_multiplex_2_extended(data_p, size);
    test_multiplex_2_extended_types(data_p, size);

    return (0);
}
