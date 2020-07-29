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

#include <string.h>

#include "floating_point.h"

static inline uint8_t pack_left_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u64(
    uint64_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u64(
    uint64_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint32_t unpack_left_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) << shift);
}

static inline uint64_t unpack_left_shift_u64(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint64_t)((uint64_t)(value & mask) << shift);
}

static inline uint32_t unpack_right_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) >> shift);
}

static inline uint64_t unpack_right_shift_u64(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint64_t)((uint64_t)(value & mask) >> shift);
}

int floating_point_message1_pack(
    uint8_t *dst_p,
    const struct floating_point_message1_t *src_p,
    size_t size)
{
    uint64_t signal1;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    memcpy(&signal1, &src_p->signal1, sizeof(signal1));
    dst_p[0] |= pack_left_shift_u64(signal1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(signal1, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(signal1, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(signal1, 24u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(signal1, 32u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(signal1, 40u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(signal1, 48u, 0xffu);
    dst_p[7] |= pack_right_shift_u64(signal1, 56u, 0xffu);

    return (8);
}

int floating_point_message1_unpack(
    struct floating_point_message1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t signal1;

    if (size < 8u) {
        return (-EINVAL);
    }

    signal1 = unpack_right_shift_u64(src_p[0], 0u, 0xffu);
    signal1 |= unpack_left_shift_u64(src_p[1], 8u, 0xffu);
    signal1 |= unpack_left_shift_u64(src_p[2], 16u, 0xffu);
    signal1 |= unpack_left_shift_u64(src_p[3], 24u, 0xffu);
    signal1 |= unpack_left_shift_u64(src_p[4], 32u, 0xffu);
    signal1 |= unpack_left_shift_u64(src_p[5], 40u, 0xffu);
    signal1 |= unpack_left_shift_u64(src_p[6], 48u, 0xffu);
    signal1 |= unpack_left_shift_u64(src_p[7], 56u, 0xffu);
    memcpy(&dst_p->signal1, &signal1, sizeof(dst_p->signal1));

    return (0);
}

double floating_point_message1_signal1_encode(double value)
{
    return (double)(value);
}

double floating_point_message1_signal1_decode(double value)
{
    return ((double)value);
}

bool floating_point_message1_signal1_is_in_range(double value)
{
    (void)value;

    return (true);
}

int floating_point_message2_pack(
    uint8_t *dst_p,
    const struct floating_point_message2_t *src_p,
    size_t size)
{
    uint32_t signal1;
    uint32_t signal2;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    memcpy(&signal1, &src_p->signal1, sizeof(signal1));
    dst_p[0] |= pack_left_shift_u32(signal1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(signal1, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(signal1, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(signal1, 24u, 0xffu);
    memcpy(&signal2, &src_p->signal2, sizeof(signal2));
    dst_p[4] |= pack_left_shift_u32(signal2, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(signal2, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(signal2, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(signal2, 24u, 0xffu);

    return (8);
}

int floating_point_message2_unpack(
    struct floating_point_message2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t signal1;
    uint32_t signal2;

    if (size < 8u) {
        return (-EINVAL);
    }

    signal1 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    signal1 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    signal1 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    signal1 |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    memcpy(&dst_p->signal1, &signal1, sizeof(dst_p->signal1));
    signal2 = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    signal2 |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    signal2 |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    signal2 |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    memcpy(&dst_p->signal2, &signal2, sizeof(dst_p->signal2));

    return (0);
}

float floating_point_message2_signal1_encode(double value)
{
    return (float)(value);
}

double floating_point_message2_signal1_decode(float value)
{
    return ((double)value);
}

bool floating_point_message2_signal1_is_in_range(float value)
{
    (void)value;

    return (true);
}

float floating_point_message2_signal2_encode(double value)
{
    return (float)(value);
}

double floating_point_message2_signal2_decode(float value)
{
    return ((double)value);
}

bool floating_point_message2_signal2_is_in_range(float value)
{
    (void)value;

    return (true);
}
