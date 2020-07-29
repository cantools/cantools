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

#include "min_max_only_6_0.h"

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t unpack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) << shift);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

static inline uint16_t unpack_right_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) >> shift);
}

int min_max_only_6_0_symbol1_pack(
    uint8_t *dst_p,
    const struct min_max_only_6_0_symbol1_t *src_p,
    size_t size)
{
    uint16_t signal3;
    uint8_t signal4;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->signal1, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->signal2, 3u, 0xf8u);
    dst_p[2] |= pack_right_shift_u8(src_p->signal2, 5u, 0x03u);
    signal4 = (uint8_t)src_p->signal4;
    dst_p[2] |= pack_left_shift_u8(signal4, 2u, 0xfcu);
    signal3 = (uint16_t)src_p->signal3;
    dst_p[3] |= pack_right_shift_u16(signal3, 1u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(signal3, 7u, 0x80u);

    return (8);
}

int min_max_only_6_0_symbol1_unpack(
    struct min_max_only_6_0_symbol1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t signal3;
    uint8_t signal4;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->signal1 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->signal2 = unpack_right_shift_u8(src_p[1], 3u, 0xf8u);
    dst_p->signal2 |= unpack_left_shift_u8(src_p[2], 5u, 0x03u);
    signal4 = unpack_right_shift_u8(src_p[2], 2u, 0xfcu);

    if ((signal4 & (1u << 5)) != 0u) {
        signal4 |= 0xc0u;
    }

    dst_p->signal4 = (int8_t)signal4;
    signal3 = unpack_left_shift_u16(src_p[3], 1u, 0xffu);
    signal3 |= unpack_right_shift_u16(src_p[4], 7u, 0x80u);

    if ((signal3 & (1u << 8)) != 0u) {
        signal3 |= 0xfe00u;
    }

    dst_p->signal3 = (int16_t)signal3;

    return (0);
}

uint8_t min_max_only_6_0_symbol1_signal1_encode(double value)
{
    return (uint8_t)(value);
}

double min_max_only_6_0_symbol1_signal1_decode(uint8_t value)
{
    return ((double)value);
}

bool min_max_only_6_0_symbol1_signal1_is_in_range(uint8_t value)
{
    return (value <= 254u);
}

uint8_t min_max_only_6_0_symbol1_signal2_encode(double value)
{
    return (uint8_t)(value);
}

double min_max_only_6_0_symbol1_signal2_decode(uint8_t value)
{
    return ((double)value);
}

bool min_max_only_6_0_symbol1_signal2_is_in_range(uint8_t value)
{
    return ((value >= 5u) && (value <= 127u));
}

int8_t min_max_only_6_0_symbol1_signal4_encode(double value)
{
    return (int8_t)(value);
}

double min_max_only_6_0_symbol1_signal4_decode(int8_t value)
{
    return ((double)value);
}

bool min_max_only_6_0_symbol1_signal4_is_in_range(int8_t value)
{
    return ((value >= -32) && (value <= 5));
}

int16_t min_max_only_6_0_symbol1_signal3_encode(double value)
{
    return (int16_t)(value);
}

double min_max_only_6_0_symbol1_signal3_decode(int16_t value)
{
    return ((double)value);
}

bool min_max_only_6_0_symbol1_signal3_is_in_range(int16_t value)
{
    return ((value >= -2) && (value <= 255));
}
