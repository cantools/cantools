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

#include "multiplex_2.h"

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

static inline uint8_t pack_left_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint32_t unpack_left_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) << shift);
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

static inline uint32_t unpack_right_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) >> shift);
}

int multiplex_2_shared_pack(
    uint8_t *dst_p,
    const struct multiplex_2_shared_t *src_p,
    size_t size)
{
    uint8_t s0;
    uint8_t s1;
    uint8_t s2;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s0 = (uint8_t)src_p->s0;
    dst_p[0] |= pack_left_shift_u8(s0, 0u, 0x0fu);

    switch (src_p->s0) {

    case 1:
        s1 = (uint8_t)src_p->s1;
        dst_p[0] |= pack_left_shift_u8(s1, 4u, 0xf0u);
        break;

    case 2:
        s2 = (uint8_t)src_p->s2;
        dst_p[1] |= pack_left_shift_u8(s2, 0u, 0xffu);
        break;

    case 3:
        s1 = (uint8_t)src_p->s1;
        dst_p[0] |= pack_left_shift_u8(s1, 4u, 0xf0u);
        s2 = (uint8_t)src_p->s2;
        dst_p[1] |= pack_left_shift_u8(s2, 0u, 0xffu);
        break;

    case 4:
        s2 = (uint8_t)src_p->s2;
        dst_p[1] |= pack_left_shift_u8(s2, 0u, 0xffu);
        break;

    case 5:
        s2 = (uint8_t)src_p->s2;
        dst_p[1] |= pack_left_shift_u8(s2, 0u, 0xffu);
        break;

    default:
        break;
    }

    return (8);
}

int multiplex_2_shared_unpack(
    struct multiplex_2_shared_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint8_t s0;
    uint8_t s1;
    uint8_t s2;

    if (size < 8u) {
        return (-EINVAL);
    }

    s0 = unpack_right_shift_u8(src_p[0], 0u, 0x0fu);

    if ((s0 & (1u << 3)) != 0u) {
        s0 |= 0xf0u;
    }

    dst_p->s0 = (int8_t)s0;

    switch (dst_p->s0) {

    case 1:
        s1 = unpack_right_shift_u8(src_p[0], 4u, 0xf0u);

        if ((s1 & (1u << 3)) != 0u) {
            s1 |= 0xf0u;
        }

        dst_p->s1 = (int8_t)s1;
        break;

    case 2:
        s2 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->s2 = (int8_t)s2;
        break;

    case 3:
        s1 = unpack_right_shift_u8(src_p[0], 4u, 0xf0u);

        if ((s1 & (1u << 3)) != 0u) {
            s1 |= 0xf0u;
        }

        dst_p->s1 = (int8_t)s1;
        s2 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->s2 = (int8_t)s2;
        break;

    case 4:
        s2 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->s2 = (int8_t)s2;
        break;

    case 5:
        s2 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->s2 = (int8_t)s2;
        break;

    default:
        break;
    }

    return (0);
}

static int multiplex_2_shared_check_ranges(struct multiplex_2_shared_t *msg)
{
    int idx = 1;

    if (!multiplex_2_shared_s0_is_in_range(msg->s0))
        return idx;

    idx++;

    if (!multiplex_2_shared_s1_is_in_range(msg->s1))
        return idx;

    idx++;

    if (!multiplex_2_shared_s2_is_in_range(msg->s2))
        return idx;

    idx++;

    return 0;
}

int multiplex_2_shared_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s0,
    double s1,
    double s2)
{
    struct multiplex_2_shared_t msg;

    msg.s0 = multiplex_2_shared_s0_encode(s0);
    msg.s1 = multiplex_2_shared_s1_encode(s1);
    msg.s2 = multiplex_2_shared_s2_encode(s2);

    int ret = multiplex_2_shared_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = multiplex_2_shared_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int multiplex_2_shared_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s0,
    double *s1,
    double *s2)
{
    struct multiplex_2_shared_t msg;
    memset(&msg, 0, sizeof(msg));

    if (multiplex_2_shared_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = multiplex_2_shared_check_ranges(&msg);

    if (s0)
        *s0 = multiplex_2_shared_s0_decode(msg.s0);

    if (s1)
        *s1 = multiplex_2_shared_s1_decode(msg.s1);

    if (s2)
        *s2 = multiplex_2_shared_s2_decode(msg.s2);

    return ret;
}

int8_t multiplex_2_shared_s0_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_shared_s0_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_shared_s0_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_shared_s0_is_in_range(int8_t value)
{
    return ((value >= -8) && (value <= 7));
}

int8_t multiplex_2_shared_s1_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_shared_s1_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_shared_s1_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_shared_s1_is_in_range(int8_t value)
{
    return ((value >= -8) && (value <= 7));
}

int8_t multiplex_2_shared_s2_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_shared_s2_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_shared_s2_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_shared_s2_is_in_range(int8_t value)
{
    (void)value;

    return (true);
}

int multiplex_2_normal_pack(
    uint8_t *dst_p,
    const struct multiplex_2_normal_t *src_p,
    size_t size)
{
    uint8_t s0;
    uint8_t s1;
    uint8_t s2;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s0 = (uint8_t)src_p->s0;
    dst_p[0] |= pack_left_shift_u8(s0, 0u, 0x0fu);

    switch (src_p->s0) {

    case 0:
        s1 = (uint8_t)src_p->s1;
        dst_p[0] |= pack_left_shift_u8(s1, 4u, 0xf0u);
        break;

    case 1:
        s2 = (uint8_t)src_p->s2;
        dst_p[1] |= pack_left_shift_u8(s2, 0u, 0xffu);
        break;

    default:
        break;
    }

    return (8);
}

int multiplex_2_normal_unpack(
    struct multiplex_2_normal_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint8_t s0;
    uint8_t s1;
    uint8_t s2;

    if (size < 8u) {
        return (-EINVAL);
    }

    s0 = unpack_right_shift_u8(src_p[0], 0u, 0x0fu);

    if ((s0 & (1u << 3)) != 0u) {
        s0 |= 0xf0u;
    }

    dst_p->s0 = (int8_t)s0;

    switch (dst_p->s0) {

    case 0:
        s1 = unpack_right_shift_u8(src_p[0], 4u, 0xf0u);

        if ((s1 & (1u << 3)) != 0u) {
            s1 |= 0xf0u;
        }

        dst_p->s1 = (int8_t)s1;
        break;

    case 1:
        s2 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->s2 = (int8_t)s2;
        break;

    default:
        break;
    }

    return (0);
}

static int multiplex_2_normal_check_ranges(struct multiplex_2_normal_t *msg)
{
    int idx = 1;

    if (!multiplex_2_normal_s0_is_in_range(msg->s0))
        return idx;

    idx++;

    if (!multiplex_2_normal_s1_is_in_range(msg->s1))
        return idx;

    idx++;

    if (!multiplex_2_normal_s2_is_in_range(msg->s2))
        return idx;

    idx++;

    return 0;
}

int multiplex_2_normal_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s0,
    double s1,
    double s2)
{
    struct multiplex_2_normal_t msg;

    msg.s0 = multiplex_2_normal_s0_encode(s0);
    msg.s1 = multiplex_2_normal_s1_encode(s1);
    msg.s2 = multiplex_2_normal_s2_encode(s2);

    int ret = multiplex_2_normal_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = multiplex_2_normal_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int multiplex_2_normal_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s0,
    double *s1,
    double *s2)
{
    struct multiplex_2_normal_t msg;
    memset(&msg, 0, sizeof(msg));

    if (multiplex_2_normal_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = multiplex_2_normal_check_ranges(&msg);

    if (s0)
        *s0 = multiplex_2_normal_s0_decode(msg.s0);

    if (s1)
        *s1 = multiplex_2_normal_s1_decode(msg.s1);

    if (s2)
        *s2 = multiplex_2_normal_s2_decode(msg.s2);

    return ret;
}

int8_t multiplex_2_normal_s0_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_normal_s0_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_normal_s0_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_normal_s0_is_in_range(int8_t value)
{
    return ((value >= -8) && (value <= 7));
}

int8_t multiplex_2_normal_s1_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_normal_s1_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_normal_s1_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_normal_s1_is_in_range(int8_t value)
{
    return ((value >= -8) && (value <= 7));
}

int8_t multiplex_2_normal_s2_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_normal_s2_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_normal_s2_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_normal_s2_is_in_range(int8_t value)
{
    (void)value;

    return (true);
}

int multiplex_2_extended_pack(
    uint8_t *dst_p,
    const struct multiplex_2_extended_t *src_p,
    size_t size)
{
    uint16_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s7;
    uint8_t s0;
    uint8_t s1;
    uint8_t s2;
    uint8_t s6;
    uint8_t s8;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s0 = (uint8_t)src_p->s0;
    dst_p[0] |= pack_left_shift_u8(s0, 0u, 0x0fu);
    s6 = (uint8_t)src_p->s6;
    dst_p[4] |= pack_left_shift_u8(s6, 0u, 0xffu);

    switch (src_p->s0) {

    case 0:
        s1 = (uint8_t)src_p->s1;
        dst_p[0] |= pack_left_shift_u8(s1, 4u, 0xf0u);

        switch (src_p->s1) {

        case 0:
            s2 = (uint8_t)src_p->s2;
            dst_p[1] |= pack_left_shift_u8(s2, 0u, 0xffu);
            s3 = (uint16_t)src_p->s3;
            dst_p[2] |= pack_left_shift_u16(s3, 0u, 0xffu);
            dst_p[3] |= pack_right_shift_u16(s3, 8u, 0xffu);
            break;

        case 2:
            s4 = (uint32_t)src_p->s4;
            dst_p[1] |= pack_left_shift_u32(s4, 0u, 0xffu);
            dst_p[2] |= pack_right_shift_u32(s4, 8u, 0xffu);
            dst_p[3] |= pack_right_shift_u32(s4, 16u, 0xffu);
            break;

        default:
            break;
        }
        break;

    case 1:
        s5 = (uint32_t)src_p->s5;
        dst_p[0] |= pack_left_shift_u32(s5, 4u, 0xf0u);
        dst_p[1] |= pack_right_shift_u32(s5, 4u, 0xffu);
        dst_p[2] |= pack_right_shift_u32(s5, 12u, 0xffu);
        dst_p[3] |= pack_right_shift_u32(s5, 20u, 0xffu);
        break;

    default:
        break;
    }

    switch (src_p->s6) {

    case 1:
        s7 = (uint32_t)src_p->s7;
        dst_p[5] |= pack_left_shift_u32(s7, 0u, 0xffu);
        dst_p[6] |= pack_right_shift_u32(s7, 8u, 0xffu);
        dst_p[7] |= pack_right_shift_u32(s7, 16u, 0xffu);
        break;

    case 2:
        s8 = (uint8_t)src_p->s8;
        dst_p[5] |= pack_left_shift_u8(s8, 0u, 0xffu);
        break;

    default:
        break;
    }

    return (8);
}

int multiplex_2_extended_unpack(
    struct multiplex_2_extended_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s7;
    uint8_t s0;
    uint8_t s1;
    uint8_t s2;
    uint8_t s6;
    uint8_t s8;

    if (size < 8u) {
        return (-EINVAL);
    }

    s0 = unpack_right_shift_u8(src_p[0], 0u, 0x0fu);

    if ((s0 & (1u << 3)) != 0u) {
        s0 |= 0xf0u;
    }

    dst_p->s0 = (int8_t)s0;
    s6 = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    dst_p->s6 = (int8_t)s6;

    switch (dst_p->s0) {

    case 0:
        s1 = unpack_right_shift_u8(src_p[0], 4u, 0xf0u);

        if ((s1 & (1u << 3)) != 0u) {
            s1 |= 0xf0u;
        }

        dst_p->s1 = (int8_t)s1;

        switch (dst_p->s1) {

        case 0:
            s2 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
            dst_p->s2 = (int8_t)s2;
            s3 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
            s3 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
            dst_p->s3 = (int16_t)s3;
            break;

        case 2:
            s4 = unpack_right_shift_u32(src_p[1], 0u, 0xffu);
            s4 |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
            s4 |= unpack_left_shift_u32(src_p[3], 16u, 0xffu);

            if ((s4 & (1u << 23)) != 0u) {
                s4 |= 0xff000000u;
            }

            dst_p->s4 = (int32_t)s4;
            break;

        default:
            break;
        }
        break;

    case 1:
        s5 = unpack_right_shift_u32(src_p[0], 4u, 0xf0u);
        s5 |= unpack_left_shift_u32(src_p[1], 4u, 0xffu);
        s5 |= unpack_left_shift_u32(src_p[2], 12u, 0xffu);
        s5 |= unpack_left_shift_u32(src_p[3], 20u, 0xffu);

        if ((s5 & (1u << 27)) != 0u) {
            s5 |= 0xf0000000u;
        }

        dst_p->s5 = (int32_t)s5;
        break;

    default:
        break;
    }

    switch (dst_p->s6) {

    case 1:
        s7 = unpack_right_shift_u32(src_p[5], 0u, 0xffu);
        s7 |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
        s7 |= unpack_left_shift_u32(src_p[7], 16u, 0xffu);

        if ((s7 & (1u << 23)) != 0u) {
            s7 |= 0xff000000u;
        }

        dst_p->s7 = (int32_t)s7;
        break;

    case 2:
        s8 = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
        dst_p->s8 = (int8_t)s8;
        break;

    default:
        break;
    }

    return (0);
}

static int multiplex_2_extended_check_ranges(struct multiplex_2_extended_t *msg)
{
    int idx = 1;

    if (!multiplex_2_extended_s0_is_in_range(msg->s0))
        return idx;

    idx++;

    if (!multiplex_2_extended_s5_is_in_range(msg->s5))
        return idx;

    idx++;

    if (!multiplex_2_extended_s1_is_in_range(msg->s1))
        return idx;

    idx++;

    if (!multiplex_2_extended_s4_is_in_range(msg->s4))
        return idx;

    idx++;

    if (!multiplex_2_extended_s2_is_in_range(msg->s2))
        return idx;

    idx++;

    if (!multiplex_2_extended_s3_is_in_range(msg->s3))
        return idx;

    idx++;

    if (!multiplex_2_extended_s6_is_in_range(msg->s6))
        return idx;

    idx++;

    if (!multiplex_2_extended_s8_is_in_range(msg->s8))
        return idx;

    idx++;

    if (!multiplex_2_extended_s7_is_in_range(msg->s7))
        return idx;

    idx++;

    return 0;
}

int multiplex_2_extended_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s0,
    double s5,
    double s1,
    double s4,
    double s2,
    double s3,
    double s6,
    double s8,
    double s7)
{
    struct multiplex_2_extended_t msg;

    msg.s0 = multiplex_2_extended_s0_encode(s0);
    msg.s5 = multiplex_2_extended_s5_encode(s5);
    msg.s1 = multiplex_2_extended_s1_encode(s1);
    msg.s4 = multiplex_2_extended_s4_encode(s4);
    msg.s2 = multiplex_2_extended_s2_encode(s2);
    msg.s3 = multiplex_2_extended_s3_encode(s3);
    msg.s6 = multiplex_2_extended_s6_encode(s6);
    msg.s8 = multiplex_2_extended_s8_encode(s8);
    msg.s7 = multiplex_2_extended_s7_encode(s7);

    int ret = multiplex_2_extended_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = multiplex_2_extended_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int multiplex_2_extended_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s0,
    double *s5,
    double *s1,
    double *s4,
    double *s2,
    double *s3,
    double *s6,
    double *s8,
    double *s7)
{
    struct multiplex_2_extended_t msg;
    memset(&msg, 0, sizeof(msg));

    if (multiplex_2_extended_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = multiplex_2_extended_check_ranges(&msg);

    if (s0)
        *s0 = multiplex_2_extended_s0_decode(msg.s0);

    if (s5)
        *s5 = multiplex_2_extended_s5_decode(msg.s5);

    if (s1)
        *s1 = multiplex_2_extended_s1_decode(msg.s1);

    if (s4)
        *s4 = multiplex_2_extended_s4_decode(msg.s4);

    if (s2)
        *s2 = multiplex_2_extended_s2_decode(msg.s2);

    if (s3)
        *s3 = multiplex_2_extended_s3_decode(msg.s3);

    if (s6)
        *s6 = multiplex_2_extended_s6_decode(msg.s6);

    if (s8)
        *s8 = multiplex_2_extended_s8_decode(msg.s8);

    if (s7)
        *s7 = multiplex_2_extended_s7_decode(msg.s7);

    return ret;
}

int8_t multiplex_2_extended_s0_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_extended_s0_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s0_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s0_is_in_range(int8_t value)
{
    return ((value >= -8) && (value <= 7));
}

int32_t multiplex_2_extended_s5_encode(double value)
{
    return (int32_t)(value);
}

double multiplex_2_extended_s5_decode(int32_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s5_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s5_is_in_range(int32_t value)
{
    return ((value >= -134217728) && (value <= 134217727));
}

int8_t multiplex_2_extended_s1_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_extended_s1_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s1_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s1_is_in_range(int8_t value)
{
    return ((value >= -8) && (value <= 7));
}

int32_t multiplex_2_extended_s4_encode(double value)
{
    return (int32_t)(value);
}

double multiplex_2_extended_s4_decode(int32_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s4_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s4_is_in_range(int32_t value)
{
    return ((value >= -8388608) && (value <= 8388607));
}

int8_t multiplex_2_extended_s2_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_extended_s2_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s2_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s2_is_in_range(int8_t value)
{
    (void)value;

    return (true);
}

int16_t multiplex_2_extended_s3_encode(double value)
{
    return (int16_t)(value);
}

double multiplex_2_extended_s3_decode(int16_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s3_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s3_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int8_t multiplex_2_extended_s6_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_extended_s6_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s6_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s6_is_in_range(int8_t value)
{
    (void)value;

    return (true);
}

int8_t multiplex_2_extended_s8_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_extended_s8_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s8_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s8_is_in_range(int8_t value)
{
    (void)value;

    return (true);
}

int32_t multiplex_2_extended_s7_encode(double value)
{
    return (int32_t)(value);
}

double multiplex_2_extended_s7_decode(int32_t value)
{
    return ((double)value);
}

double multiplex_2_extended_s7_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_s7_is_in_range(int32_t value)
{
    return ((value >= -8388608) && (value <= 8388607));
}

int multiplex_2_extended_types_pack(
    uint8_t *dst_p,
    const struct multiplex_2_extended_types_t *src_p,
    size_t size)
{
    uint32_t s10;
    uint32_t s9;
    uint8_t s0;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->s11, 0u, 0x1fu);

    switch (src_p->s11) {

    case 3:
        s0 = (uint8_t)src_p->s0;
        dst_p[1] |= pack_left_shift_u8(s0, 0u, 0x0fu);

        switch (src_p->s0) {

        case 0:
            memcpy(&s10, &src_p->s10, sizeof(s10));
            dst_p[2] |= pack_left_shift_u32(s10, 0u, 0xffu);
            dst_p[3] |= pack_right_shift_u32(s10, 8u, 0xffu);
            dst_p[4] |= pack_right_shift_u32(s10, 16u, 0xffu);
            dst_p[5] |= pack_right_shift_u32(s10, 24u, 0xffu);
            break;

        default:
            break;
        }
        break;

    case 5:
        memcpy(&s9, &src_p->s9, sizeof(s9));
        dst_p[3] |= pack_left_shift_u32(s9, 0u, 0xffu);
        dst_p[4] |= pack_right_shift_u32(s9, 8u, 0xffu);
        dst_p[5] |= pack_right_shift_u32(s9, 16u, 0xffu);
        dst_p[6] |= pack_right_shift_u32(s9, 24u, 0xffu);
        break;

    default:
        break;
    }

    return (8);
}

int multiplex_2_extended_types_unpack(
    struct multiplex_2_extended_types_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t s10;
    uint32_t s9;
    uint8_t s0;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->s11 = unpack_right_shift_u8(src_p[0], 0u, 0x1fu);

    switch (dst_p->s11) {

    case 3:
        s0 = unpack_right_shift_u8(src_p[1], 0u, 0x0fu);

        if ((s0 & (1u << 3)) != 0u) {
            s0 |= 0xf0u;
        }

        dst_p->s0 = (int8_t)s0;

        switch (dst_p->s0) {

        case 0:
            s10 = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
            s10 |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
            s10 |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);
            s10 |= unpack_left_shift_u32(src_p[5], 24u, 0xffu);
            memcpy(&dst_p->s10, &s10, sizeof(dst_p->s10));
            break;

        default:
            break;
        }
        break;

    case 5:
        s9 = unpack_right_shift_u32(src_p[3], 0u, 0xffu);
        s9 |= unpack_left_shift_u32(src_p[4], 8u, 0xffu);
        s9 |= unpack_left_shift_u32(src_p[5], 16u, 0xffu);
        s9 |= unpack_left_shift_u32(src_p[6], 24u, 0xffu);
        memcpy(&dst_p->s9, &s9, sizeof(dst_p->s9));
        break;

    default:
        break;
    }

    return (0);
}

static int multiplex_2_extended_types_check_ranges(struct multiplex_2_extended_types_t *msg)
{
    int idx = 1;

    if (!multiplex_2_extended_types_s11_is_in_range(msg->s11))
        return idx;

    idx++;

    if (!multiplex_2_extended_types_s0_is_in_range(msg->s0))
        return idx;

    idx++;

    if (!multiplex_2_extended_types_s10_is_in_range(msg->s10))
        return idx;

    idx++;

    if (!multiplex_2_extended_types_s9_is_in_range(msg->s9))
        return idx;

    idx++;

    return 0;
}

int multiplex_2_extended_types_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s11,
    double s0,
    double s10,
    double s9)
{
    struct multiplex_2_extended_types_t msg;

    msg.s11 = multiplex_2_extended_types_s11_encode(s11);
    msg.s0 = multiplex_2_extended_types_s0_encode(s0);
    msg.s10 = multiplex_2_extended_types_s10_encode(s10);
    msg.s9 = multiplex_2_extended_types_s9_encode(s9);

    int ret = multiplex_2_extended_types_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = multiplex_2_extended_types_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int multiplex_2_extended_types_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s11,
    double *s0,
    double *s10,
    double *s9)
{
    struct multiplex_2_extended_types_t msg;
    memset(&msg, 0, sizeof(msg));

    if (multiplex_2_extended_types_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = multiplex_2_extended_types_check_ranges(&msg);

    if (s11)
        *s11 = multiplex_2_extended_types_s11_decode(msg.s11);

    if (s0)
        *s0 = multiplex_2_extended_types_s0_decode(msg.s0);

    if (s10)
        *s10 = multiplex_2_extended_types_s10_decode(msg.s10);

    if (s9)
        *s9 = multiplex_2_extended_types_s9_decode(msg.s9);

    return ret;
}

uint8_t multiplex_2_extended_types_s11_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_2_extended_types_s11_decode(uint8_t value)
{
    return ((double)value);
}

double multiplex_2_extended_types_s11_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 2.0);
    ret = CTOOLS_MIN(ret, 6.0);
    return ret;
}

bool multiplex_2_extended_types_s11_is_in_range(uint8_t value)
{
    return ((value >= 2u) && (value <= 6u));
}

int8_t multiplex_2_extended_types_s0_encode(double value)
{
    return (int8_t)(value);
}

double multiplex_2_extended_types_s0_decode(int8_t value)
{
    return ((double)value);
}

double multiplex_2_extended_types_s0_clamp(double val)
{
    double ret = val;


    return ret;
}

bool multiplex_2_extended_types_s0_is_in_range(int8_t value)
{
    return ((value >= -8) && (value <= 7));
}

float multiplex_2_extended_types_s10_encode(double value)
{
    return (float)(value);
}

double multiplex_2_extended_types_s10_decode(float value)
{
    return ((double)value);
}

double multiplex_2_extended_types_s10_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -3.4e+38);
    ret = CTOOLS_MIN(ret, 3.4e+38);
    return ret;
}

bool multiplex_2_extended_types_s10_is_in_range(float value)
{
    return ((value >= -340000000000000000000000000000000000000.0f) && (value <= 340000000000000000000000000000000000000.0f));
}

float multiplex_2_extended_types_s9_encode(double value)
{
    return (float)(value);
}

double multiplex_2_extended_types_s9_decode(float value)
{
    return ((double)value);
}

double multiplex_2_extended_types_s9_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -1.34);
    ret = CTOOLS_MIN(ret, 1235.0);
    return ret;
}

bool multiplex_2_extended_types_s9_is_in_range(float value)
{
    return ((value >= -1.34f) && (value <= 1235.0f));
}

#undef CTOOLS_MAX
#undef CTOOLS_MIN
