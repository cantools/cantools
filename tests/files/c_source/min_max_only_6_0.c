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

#define CTOOLS_MAX(x,y) (((x) < (y)) ? (y) : (x))
#define CTOOLS_MIN(x,y) (((x) < (y)) ? (x) : (y))

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

static int min_max_only_6_0_symbol1_check_ranges(struct min_max_only_6_0_symbol1_t *msg)
{
    int idx = 1;

    if (!min_max_only_6_0_symbol1_signal1_is_in_range(msg->signal1))
        return idx;

    idx++;

    if (!min_max_only_6_0_symbol1_signal2_is_in_range(msg->signal2))
        return idx;

    idx++;

    if (!min_max_only_6_0_symbol1_signal4_is_in_range(msg->signal4))
        return idx;

    idx++;

    if (!min_max_only_6_0_symbol1_signal3_is_in_range(msg->signal3))
        return idx;

    idx++;

    return 0;
}

int min_max_only_6_0_symbol1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double signal1,
    double signal2,
    double signal4,
    double signal3)
{
    struct min_max_only_6_0_symbol1_t msg;

    msg.signal1 = min_max_only_6_0_symbol1_signal1_encode(signal1);
    msg.signal2 = min_max_only_6_0_symbol1_signal2_encode(signal2);
    msg.signal4 = min_max_only_6_0_symbol1_signal4_encode(signal4);
    msg.signal3 = min_max_only_6_0_symbol1_signal3_encode(signal3);

    int ret = min_max_only_6_0_symbol1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = min_max_only_6_0_symbol1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int min_max_only_6_0_symbol1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *signal1,
    double *signal2,
    double *signal4,
    double *signal3)
{
    struct min_max_only_6_0_symbol1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (min_max_only_6_0_symbol1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = min_max_only_6_0_symbol1_check_ranges(&msg);

    if (signal1)
        *signal1 = min_max_only_6_0_symbol1_signal1_decode(msg.signal1);

    if (signal2)
        *signal2 = min_max_only_6_0_symbol1_signal2_decode(msg.signal2);

    if (signal4)
        *signal4 = min_max_only_6_0_symbol1_signal4_decode(msg.signal4);

    if (signal3)
        *signal3 = min_max_only_6_0_symbol1_signal3_decode(msg.signal3);

    return ret;
}

uint8_t min_max_only_6_0_symbol1_signal1_encode(double value)
{
    return (uint8_t)(value);
}

double min_max_only_6_0_symbol1_signal1_decode(uint8_t value)
{
    return ((double)value);
}

double min_max_only_6_0_symbol1_signal1_clamp(double val)
{
    double ret = val;

    ret = CTOOLS_MIN(ret, 254.0);
    return ret;
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

double min_max_only_6_0_symbol1_signal2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 5.0);

    return ret;
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

double min_max_only_6_0_symbol1_signal4_clamp(double val)
{
    double ret = val;

    ret = CTOOLS_MIN(ret, 5.0);
    return ret;
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

double min_max_only_6_0_symbol1_signal3_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -2.0);

    return ret;
}

bool min_max_only_6_0_symbol1_signal3_is_in_range(int16_t value)
{
    return ((value >= -2) && (value <= 255));
}

#undef CTOOLS_MAX
#undef CTOOLS_MIN
