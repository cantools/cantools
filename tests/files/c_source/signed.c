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

#include "signed.h"

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

static inline uint8_t pack_left_shift_u64(
    uint64_t value,
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

static inline uint64_t unpack_right_shift_u64(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint64_t)((uint64_t)(value & mask) >> shift);
}

int signed_message378910_pack(
    uint8_t *dst_p,
    const struct signed_message378910_t *src_p,
    size_t size)
{
    uint16_t s10big;
    uint16_t s9;
    uint8_t s3;
    uint8_t s3big;
    uint8_t s7;
    uint8_t s7big;
    uint8_t s8;
    uint8_t s8big;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s7 = (uint8_t)src_p->s7;
    dst_p[0] |= pack_left_shift_u8(s7, 1u, 0xfeu);
    s8big = (uint8_t)src_p->s8big;
    dst_p[0] |= pack_right_shift_u8(s8big, 7u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(s8big, 1u, 0xfeu);
    s9 = (uint16_t)src_p->s9;
    dst_p[2] |= pack_left_shift_u16(s9, 1u, 0xfeu);
    dst_p[3] |= pack_right_shift_u16(s9, 7u, 0x03u);
    s8 = (uint8_t)src_p->s8;
    dst_p[3] |= pack_left_shift_u8(s8, 2u, 0xfcu);
    dst_p[4] |= pack_right_shift_u8(s8, 6u, 0x03u);
    s3big = (uint8_t)src_p->s3big;
    dst_p[4] |= pack_left_shift_u8(s3big, 5u, 0xe0u);
    s3 = (uint8_t)src_p->s3;
    dst_p[4] |= pack_left_shift_u8(s3, 2u, 0x1cu);
    s10big = (uint16_t)src_p->s10big;
    dst_p[5] |= pack_right_shift_u16(s10big, 9u, 0x01u);
    dst_p[6] |= pack_right_shift_u16(s10big, 1u, 0xffu);
    dst_p[7] |= pack_left_shift_u16(s10big, 7u, 0x80u);
    s7big = (uint8_t)src_p->s7big;
    dst_p[7] |= pack_left_shift_u8(s7big, 0u, 0x7fu);

    return (8);
}

int signed_message378910_unpack(
    struct signed_message378910_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t s10big;
    uint16_t s9;
    uint8_t s3;
    uint8_t s3big;
    uint8_t s7;
    uint8_t s7big;
    uint8_t s8;
    uint8_t s8big;

    if (size < 8u) {
        return (-EINVAL);
    }

    s7 = unpack_right_shift_u8(src_p[0], 1u, 0xfeu);

    if ((s7 & (1u << 6)) != 0u) {
        s7 |= 0x80u;
    }

    dst_p->s7 = (int8_t)s7;
    s8big = unpack_left_shift_u8(src_p[0], 7u, 0x01u);
    s8big |= unpack_right_shift_u8(src_p[1], 1u, 0xfeu);
    dst_p->s8big = (int8_t)s8big;
    s9 = unpack_right_shift_u16(src_p[2], 1u, 0xfeu);
    s9 |= unpack_left_shift_u16(src_p[3], 7u, 0x03u);

    if ((s9 & (1u << 8)) != 0u) {
        s9 |= 0xfe00u;
    }

    dst_p->s9 = (int16_t)s9;
    s8 = unpack_right_shift_u8(src_p[3], 2u, 0xfcu);
    s8 |= unpack_left_shift_u8(src_p[4], 6u, 0x03u);
    dst_p->s8 = (int8_t)s8;
    s3big = unpack_right_shift_u8(src_p[4], 5u, 0xe0u);

    if ((s3big & (1u << 2)) != 0u) {
        s3big |= 0xf8u;
    }

    dst_p->s3big = (int8_t)s3big;
    s3 = unpack_right_shift_u8(src_p[4], 2u, 0x1cu);

    if ((s3 & (1u << 2)) != 0u) {
        s3 |= 0xf8u;
    }

    dst_p->s3 = (int8_t)s3;
    s10big = unpack_left_shift_u16(src_p[5], 9u, 0x01u);
    s10big |= unpack_left_shift_u16(src_p[6], 1u, 0xffu);
    s10big |= unpack_right_shift_u16(src_p[7], 7u, 0x80u);

    if ((s10big & (1u << 9)) != 0u) {
        s10big |= 0xfc00u;
    }

    dst_p->s10big = (int16_t)s10big;
    s7big = unpack_right_shift_u8(src_p[7], 0u, 0x7fu);

    if ((s7big & (1u << 6)) != 0u) {
        s7big |= 0x80u;
    }

    dst_p->s7big = (int8_t)s7big;

    return (0);
}

static int signed_message378910_check_ranges(struct signed_message378910_t *msg)
{
    int idx = 1;

    if (!signed_message378910_s7_is_in_range(msg->s7))
        return idx;

    idx++;

    if (!signed_message378910_s8big_is_in_range(msg->s8big))
        return idx;

    idx++;

    if (!signed_message378910_s9_is_in_range(msg->s9))
        return idx;

    idx++;

    if (!signed_message378910_s8_is_in_range(msg->s8))
        return idx;

    idx++;

    if (!signed_message378910_s3big_is_in_range(msg->s3big))
        return idx;

    idx++;

    if (!signed_message378910_s3_is_in_range(msg->s3))
        return idx;

    idx++;

    if (!signed_message378910_s10big_is_in_range(msg->s10big))
        return idx;

    idx++;

    if (!signed_message378910_s7big_is_in_range(msg->s7big))
        return idx;

    idx++;

    return 0;
}

int signed_message378910_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s7,
    double s8big,
    double s9,
    double s8,
    double s3big,
    double s3,
    double s10big,
    double s7big)
{
    struct signed_message378910_t msg;

    msg.s7 = signed_message378910_s7_encode(s7);
    msg.s8big = signed_message378910_s8big_encode(s8big);
    msg.s9 = signed_message378910_s9_encode(s9);
    msg.s8 = signed_message378910_s8_encode(s8);
    msg.s3big = signed_message378910_s3big_encode(s3big);
    msg.s3 = signed_message378910_s3_encode(s3);
    msg.s10big = signed_message378910_s10big_encode(s10big);
    msg.s7big = signed_message378910_s7big_encode(s7big);

    int ret = signed_message378910_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message378910_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message378910_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s7,
    double *s8big,
    double *s9,
    double *s8,
    double *s3big,
    double *s3,
    double *s10big,
    double *s7big)
{
    struct signed_message378910_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message378910_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message378910_check_ranges(&msg);

    if (s7)
        *s7 = signed_message378910_s7_decode(msg.s7);

    if (s8big)
        *s8big = signed_message378910_s8big_decode(msg.s8big);

    if (s9)
        *s9 = signed_message378910_s9_decode(msg.s9);

    if (s8)
        *s8 = signed_message378910_s8_decode(msg.s8);

    if (s3big)
        *s3big = signed_message378910_s3big_decode(msg.s3big);

    if (s3)
        *s3 = signed_message378910_s3_decode(msg.s3);

    if (s10big)
        *s10big = signed_message378910_s10big_decode(msg.s10big);

    if (s7big)
        *s7big = signed_message378910_s7big_decode(msg.s7big);

    return ret;
}

int8_t signed_message378910_s7_encode(double value)
{
    return (int8_t)(value);
}

double signed_message378910_s7_decode(int8_t value)
{
    return ((double)value);
}

double signed_message378910_s7_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s7_is_in_range(int8_t value)
{
    return ((value >= -64) && (value <= 63));
}

int8_t signed_message378910_s8big_encode(double value)
{
    return (int8_t)(value);
}

double signed_message378910_s8big_decode(int8_t value)
{
    return ((double)value);
}

double signed_message378910_s8big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s8big_is_in_range(int8_t value)
{
    (void)value;

    return (true);
}

int16_t signed_message378910_s9_encode(double value)
{
    return (int16_t)(value);
}

double signed_message378910_s9_decode(int16_t value)
{
    return ((double)value);
}

double signed_message378910_s9_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s9_is_in_range(int16_t value)
{
    return ((value >= -256) && (value <= 255));
}

int8_t signed_message378910_s8_encode(double value)
{
    return (int8_t)(value);
}

double signed_message378910_s8_decode(int8_t value)
{
    return ((double)value);
}

double signed_message378910_s8_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s8_is_in_range(int8_t value)
{
    (void)value;

    return (true);
}

int8_t signed_message378910_s3big_encode(double value)
{
    return (int8_t)(value);
}

double signed_message378910_s3big_decode(int8_t value)
{
    return ((double)value);
}

double signed_message378910_s3big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s3big_is_in_range(int8_t value)
{
    return ((value >= -4) && (value <= 3));
}

int8_t signed_message378910_s3_encode(double value)
{
    return (int8_t)(value);
}

double signed_message378910_s3_decode(int8_t value)
{
    return ((double)value);
}

double signed_message378910_s3_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s3_is_in_range(int8_t value)
{
    return ((value >= -4) && (value <= 3));
}

int16_t signed_message378910_s10big_encode(double value)
{
    return (int16_t)(value);
}

double signed_message378910_s10big_decode(int16_t value)
{
    return ((double)value);
}

double signed_message378910_s10big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s10big_is_in_range(int16_t value)
{
    return ((value >= -512) && (value <= 511));
}

int8_t signed_message378910_s7big_encode(double value)
{
    return (int8_t)(value);
}

double signed_message378910_s7big_decode(int8_t value)
{
    return ((double)value);
}

double signed_message378910_s7big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message378910_s7big_is_in_range(int8_t value)
{
    return ((value >= -64) && (value <= 63));
}

int signed_message63big_1_pack(
    uint8_t *dst_p,
    const struct signed_message63big_1_t *src_p,
    size_t size)
{
    uint64_t s63big;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s63big = (uint64_t)src_p->s63big;
    dst_p[0] |= pack_right_shift_u64(s63big, 56u, 0x7fu);
    dst_p[1] |= pack_right_shift_u64(s63big, 48u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s63big, 40u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s63big, 32u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(s63big, 24u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(s63big, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(s63big, 8u, 0xffu);
    dst_p[7] |= pack_left_shift_u64(s63big, 0u, 0xffu);

    return (8);
}

int signed_message63big_1_unpack(
    struct signed_message63big_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s63big;

    if (size < 8u) {
        return (-EINVAL);
    }

    s63big = unpack_left_shift_u64(src_p[0], 56u, 0x7fu);
    s63big |= unpack_left_shift_u64(src_p[1], 48u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[2], 40u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[3], 32u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[4], 24u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[5], 16u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[6], 8u, 0xffu);
    s63big |= unpack_right_shift_u64(src_p[7], 0u, 0xffu);

    if ((s63big & (1ull << 62)) != 0ull) {
        s63big |= 0x8000000000000000ull;
    }

    dst_p->s63big = (int64_t)s63big;

    return (0);
}

static int signed_message63big_1_check_ranges(struct signed_message63big_1_t *msg)
{
    int idx = 1;

    if (!signed_message63big_1_s63big_is_in_range(msg->s63big))
        return idx;

    idx++;

    return 0;
}

int signed_message63big_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s63big)
{
    struct signed_message63big_1_t msg;

    msg.s63big = signed_message63big_1_s63big_encode(s63big);

    int ret = signed_message63big_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message63big_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message63big_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s63big)
{
    struct signed_message63big_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message63big_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message63big_1_check_ranges(&msg);

    if (s63big)
        *s63big = signed_message63big_1_s63big_decode(msg.s63big);

    return ret;
}

int64_t signed_message63big_1_s63big_encode(double value)
{
    return (int64_t)(value);
}

double signed_message63big_1_s63big_decode(int64_t value)
{
    return ((double)value);
}

double signed_message63big_1_s63big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message63big_1_s63big_is_in_range(int64_t value)
{
    return ((value >= -4611686018427387904ll) && (value <= 4611686018427387903ll));
}

int signed_message63_1_pack(
    uint8_t *dst_p,
    const struct signed_message63_1_t *src_p,
    size_t size)
{
    uint64_t s63;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s63 = (uint64_t)src_p->s63;
    dst_p[0] |= pack_left_shift_u64(s63, 1u, 0xfeu);
    dst_p[1] |= pack_right_shift_u64(s63, 7u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s63, 15u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s63, 23u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(s63, 31u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(s63, 39u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(s63, 47u, 0xffu);
    dst_p[7] |= pack_right_shift_u64(s63, 55u, 0xffu);

    return (8);
}

int signed_message63_1_unpack(
    struct signed_message63_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s63;

    if (size < 8u) {
        return (-EINVAL);
    }

    s63 = unpack_right_shift_u64(src_p[0], 1u, 0xfeu);
    s63 |= unpack_left_shift_u64(src_p[1], 7u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[2], 15u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[3], 23u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[4], 31u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[5], 39u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[6], 47u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[7], 55u, 0xffu);

    if ((s63 & (1ull << 62)) != 0ull) {
        s63 |= 0x8000000000000000ull;
    }

    dst_p->s63 = (int64_t)s63;

    return (0);
}

static int signed_message63_1_check_ranges(struct signed_message63_1_t *msg)
{
    int idx = 1;

    if (!signed_message63_1_s63_is_in_range(msg->s63))
        return idx;

    idx++;

    return 0;
}

int signed_message63_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s63)
{
    struct signed_message63_1_t msg;

    msg.s63 = signed_message63_1_s63_encode(s63);

    int ret = signed_message63_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message63_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message63_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s63)
{
    struct signed_message63_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message63_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message63_1_check_ranges(&msg);

    if (s63)
        *s63 = signed_message63_1_s63_decode(msg.s63);

    return ret;
}

int64_t signed_message63_1_s63_encode(double value)
{
    return (int64_t)(value);
}

double signed_message63_1_s63_decode(int64_t value)
{
    return ((double)value);
}

double signed_message63_1_s63_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message63_1_s63_is_in_range(int64_t value)
{
    return ((value >= -4611686018427387904ll) && (value <= 4611686018427387903ll));
}

int signed_message63big_pack(
    uint8_t *dst_p,
    const struct signed_message63big_t *src_p,
    size_t size)
{
    uint64_t s63big;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s63big = (uint64_t)src_p->s63big;
    dst_p[0] |= pack_right_shift_u64(s63big, 55u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(s63big, 47u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s63big, 39u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s63big, 31u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(s63big, 23u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(s63big, 15u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(s63big, 7u, 0xffu);
    dst_p[7] |= pack_left_shift_u64(s63big, 1u, 0xfeu);

    return (8);
}

int signed_message63big_unpack(
    struct signed_message63big_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s63big;

    if (size < 8u) {
        return (-EINVAL);
    }

    s63big = unpack_left_shift_u64(src_p[0], 55u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[1], 47u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[2], 39u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[3], 31u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[4], 23u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[5], 15u, 0xffu);
    s63big |= unpack_left_shift_u64(src_p[6], 7u, 0xffu);
    s63big |= unpack_right_shift_u64(src_p[7], 1u, 0xfeu);

    if ((s63big & (1ull << 62)) != 0ull) {
        s63big |= 0x8000000000000000ull;
    }

    dst_p->s63big = (int64_t)s63big;

    return (0);
}

static int signed_message63big_check_ranges(struct signed_message63big_t *msg)
{
    int idx = 1;

    if (!signed_message63big_s63big_is_in_range(msg->s63big))
        return idx;

    idx++;

    return 0;
}

int signed_message63big_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s63big)
{
    struct signed_message63big_t msg;

    msg.s63big = signed_message63big_s63big_encode(s63big);

    int ret = signed_message63big_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message63big_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message63big_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s63big)
{
    struct signed_message63big_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message63big_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message63big_check_ranges(&msg);

    if (s63big)
        *s63big = signed_message63big_s63big_decode(msg.s63big);

    return ret;
}

int64_t signed_message63big_s63big_encode(double value)
{
    return (int64_t)(value);
}

double signed_message63big_s63big_decode(int64_t value)
{
    return ((double)value);
}

double signed_message63big_s63big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message63big_s63big_is_in_range(int64_t value)
{
    return ((value >= -4611686018427387904ll) && (value <= 4611686018427387903ll));
}

int signed_message63_pack(
    uint8_t *dst_p,
    const struct signed_message63_t *src_p,
    size_t size)
{
    uint64_t s63;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s63 = (uint64_t)src_p->s63;
    dst_p[0] |= pack_left_shift_u64(s63, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(s63, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s63, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s63, 24u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(s63, 32u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(s63, 40u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(s63, 48u, 0xffu);
    dst_p[7] |= pack_right_shift_u64(s63, 56u, 0x7fu);

    return (8);
}

int signed_message63_unpack(
    struct signed_message63_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s63;

    if (size < 8u) {
        return (-EINVAL);
    }

    s63 = unpack_right_shift_u64(src_p[0], 0u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[1], 8u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[2], 16u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[3], 24u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[4], 32u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[5], 40u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[6], 48u, 0xffu);
    s63 |= unpack_left_shift_u64(src_p[7], 56u, 0x7fu);

    if ((s63 & (1ull << 62)) != 0ull) {
        s63 |= 0x8000000000000000ull;
    }

    dst_p->s63 = (int64_t)s63;

    return (0);
}

static int signed_message63_check_ranges(struct signed_message63_t *msg)
{
    int idx = 1;

    if (!signed_message63_s63_is_in_range(msg->s63))
        return idx;

    idx++;

    return 0;
}

int signed_message63_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s63)
{
    struct signed_message63_t msg;

    msg.s63 = signed_message63_s63_encode(s63);

    int ret = signed_message63_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message63_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message63_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s63)
{
    struct signed_message63_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message63_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message63_check_ranges(&msg);

    if (s63)
        *s63 = signed_message63_s63_decode(msg.s63);

    return ret;
}

int64_t signed_message63_s63_encode(double value)
{
    return (int64_t)(value);
}

double signed_message63_s63_decode(int64_t value)
{
    return ((double)value);
}

double signed_message63_s63_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message63_s63_is_in_range(int64_t value)
{
    return ((value >= -4611686018427387904ll) && (value <= 4611686018427387903ll));
}

int signed_message32big_pack(
    uint8_t *dst_p,
    const struct signed_message32big_t *src_p,
    size_t size)
{
    uint32_t s32big;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s32big = (uint32_t)src_p->s32big;
    dst_p[0] |= pack_right_shift_u32(s32big, 24u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(s32big, 16u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(s32big, 8u, 0xffu);
    dst_p[3] |= pack_left_shift_u32(s32big, 0u, 0xffu);

    return (8);
}

int signed_message32big_unpack(
    struct signed_message32big_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t s32big;

    if (size < 8u) {
        return (-EINVAL);
    }

    s32big = unpack_left_shift_u32(src_p[0], 24u, 0xffu);
    s32big |= unpack_left_shift_u32(src_p[1], 16u, 0xffu);
    s32big |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    s32big |= unpack_right_shift_u32(src_p[3], 0u, 0xffu);
    dst_p->s32big = (int32_t)s32big;

    return (0);
}

static int signed_message32big_check_ranges(struct signed_message32big_t *msg)
{
    int idx = 1;

    if (!signed_message32big_s32big_is_in_range(msg->s32big))
        return idx;

    idx++;

    return 0;
}

int signed_message32big_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s32big)
{
    struct signed_message32big_t msg;

    msg.s32big = signed_message32big_s32big_encode(s32big);

    int ret = signed_message32big_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message32big_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message32big_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s32big)
{
    struct signed_message32big_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message32big_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message32big_check_ranges(&msg);

    if (s32big)
        *s32big = signed_message32big_s32big_decode(msg.s32big);

    return ret;
}

int32_t signed_message32big_s32big_encode(double value)
{
    return (int32_t)(value);
}

double signed_message32big_s32big_decode(int32_t value)
{
    return ((double)value);
}

double signed_message32big_s32big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message32big_s32big_is_in_range(int32_t value)
{
    (void)value;

    return (true);
}

int signed_message33big_pack(
    uint8_t *dst_p,
    const struct signed_message33big_t *src_p,
    size_t size)
{
    uint64_t s33big;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s33big = (uint64_t)src_p->s33big;
    dst_p[0] |= pack_right_shift_u64(s33big, 25u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(s33big, 17u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s33big, 9u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s33big, 1u, 0xffu);
    dst_p[4] |= pack_left_shift_u64(s33big, 7u, 0x80u);

    return (8);
}

int signed_message33big_unpack(
    struct signed_message33big_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s33big;

    if (size < 8u) {
        return (-EINVAL);
    }

    s33big = unpack_left_shift_u64(src_p[0], 25u, 0xffu);
    s33big |= unpack_left_shift_u64(src_p[1], 17u, 0xffu);
    s33big |= unpack_left_shift_u64(src_p[2], 9u, 0xffu);
    s33big |= unpack_left_shift_u64(src_p[3], 1u, 0xffu);
    s33big |= unpack_right_shift_u64(src_p[4], 7u, 0x80u);

    if ((s33big & (1ull << 32)) != 0ull) {
        s33big |= 0xfffffffe00000000ull;
    }

    dst_p->s33big = (int64_t)s33big;

    return (0);
}

static int signed_message33big_check_ranges(struct signed_message33big_t *msg)
{
    int idx = 1;

    if (!signed_message33big_s33big_is_in_range(msg->s33big))
        return idx;

    idx++;

    return 0;
}

int signed_message33big_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s33big)
{
    struct signed_message33big_t msg;

    msg.s33big = signed_message33big_s33big_encode(s33big);

    int ret = signed_message33big_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message33big_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message33big_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s33big)
{
    struct signed_message33big_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message33big_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message33big_check_ranges(&msg);

    if (s33big)
        *s33big = signed_message33big_s33big_decode(msg.s33big);

    return ret;
}

int64_t signed_message33big_s33big_encode(double value)
{
    return (int64_t)(value);
}

double signed_message33big_s33big_decode(int64_t value)
{
    return ((double)value);
}

double signed_message33big_s33big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message33big_s33big_is_in_range(int64_t value)
{
    return ((value >= -4294967296ll) && (value <= 4294967295ll));
}

int signed_message64big_pack(
    uint8_t *dst_p,
    const struct signed_message64big_t *src_p,
    size_t size)
{
    uint64_t s64big;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s64big = (uint64_t)src_p->s64big;
    dst_p[0] |= pack_right_shift_u64(s64big, 56u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(s64big, 48u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s64big, 40u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s64big, 32u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(s64big, 24u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(s64big, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(s64big, 8u, 0xffu);
    dst_p[7] |= pack_left_shift_u64(s64big, 0u, 0xffu);

    return (8);
}

int signed_message64big_unpack(
    struct signed_message64big_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s64big;

    if (size < 8u) {
        return (-EINVAL);
    }

    s64big = unpack_left_shift_u64(src_p[0], 56u, 0xffu);
    s64big |= unpack_left_shift_u64(src_p[1], 48u, 0xffu);
    s64big |= unpack_left_shift_u64(src_p[2], 40u, 0xffu);
    s64big |= unpack_left_shift_u64(src_p[3], 32u, 0xffu);
    s64big |= unpack_left_shift_u64(src_p[4], 24u, 0xffu);
    s64big |= unpack_left_shift_u64(src_p[5], 16u, 0xffu);
    s64big |= unpack_left_shift_u64(src_p[6], 8u, 0xffu);
    s64big |= unpack_right_shift_u64(src_p[7], 0u, 0xffu);
    dst_p->s64big = (int64_t)s64big;

    return (0);
}

static int signed_message64big_check_ranges(struct signed_message64big_t *msg)
{
    int idx = 1;

    if (!signed_message64big_s64big_is_in_range(msg->s64big))
        return idx;

    idx++;

    return 0;
}

int signed_message64big_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s64big)
{
    struct signed_message64big_t msg;

    msg.s64big = signed_message64big_s64big_encode(s64big);

    int ret = signed_message64big_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message64big_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message64big_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s64big)
{
    struct signed_message64big_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message64big_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message64big_check_ranges(&msg);

    if (s64big)
        *s64big = signed_message64big_s64big_decode(msg.s64big);

    return ret;
}

int64_t signed_message64big_s64big_encode(double value)
{
    return (int64_t)(value);
}

double signed_message64big_s64big_decode(int64_t value)
{
    return ((double)value);
}

double signed_message64big_s64big_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message64big_s64big_is_in_range(int64_t value)
{
    (void)value;

    return (true);
}

int signed_message64_pack(
    uint8_t *dst_p,
    const struct signed_message64_t *src_p,
    size_t size)
{
    uint64_t s64;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s64 = (uint64_t)src_p->s64;
    dst_p[0] |= pack_left_shift_u64(s64, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(s64, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s64, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s64, 24u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(s64, 32u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(s64, 40u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(s64, 48u, 0xffu);
    dst_p[7] |= pack_right_shift_u64(s64, 56u, 0xffu);

    return (8);
}

int signed_message64_unpack(
    struct signed_message64_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s64;

    if (size < 8u) {
        return (-EINVAL);
    }

    s64 = unpack_right_shift_u64(src_p[0], 0u, 0xffu);
    s64 |= unpack_left_shift_u64(src_p[1], 8u, 0xffu);
    s64 |= unpack_left_shift_u64(src_p[2], 16u, 0xffu);
    s64 |= unpack_left_shift_u64(src_p[3], 24u, 0xffu);
    s64 |= unpack_left_shift_u64(src_p[4], 32u, 0xffu);
    s64 |= unpack_left_shift_u64(src_p[5], 40u, 0xffu);
    s64 |= unpack_left_shift_u64(src_p[6], 48u, 0xffu);
    s64 |= unpack_left_shift_u64(src_p[7], 56u, 0xffu);
    dst_p->s64 = (int64_t)s64;

    return (0);
}

static int signed_message64_check_ranges(struct signed_message64_t *msg)
{
    int idx = 1;

    if (!signed_message64_s64_is_in_range(msg->s64))
        return idx;

    idx++;

    return 0;
}

int signed_message64_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s64)
{
    struct signed_message64_t msg;

    msg.s64 = signed_message64_s64_encode(s64);

    int ret = signed_message64_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message64_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message64_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s64)
{
    struct signed_message64_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message64_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message64_check_ranges(&msg);

    if (s64)
        *s64 = signed_message64_s64_decode(msg.s64);

    return ret;
}

int64_t signed_message64_s64_encode(double value)
{
    return (int64_t)(value);
}

double signed_message64_s64_decode(int64_t value)
{
    return ((double)value);
}

double signed_message64_s64_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -9.22337203685478e+18);
    ret = CTOOLS_MIN(ret, 9.22337203685478e+18);
    return ret;
}

bool signed_message64_s64_is_in_range(int64_t value)
{
    (void)value;

    return (true);
}

int signed_message33_pack(
    uint8_t *dst_p,
    const struct signed_message33_t *src_p,
    size_t size)
{
    uint64_t s33;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s33 = (uint64_t)src_p->s33;
    dst_p[0] |= pack_left_shift_u64(s33, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(s33, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(s33, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(s33, 24u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(s33, 32u, 0x01u);

    return (8);
}

int signed_message33_unpack(
    struct signed_message33_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint64_t s33;

    if (size < 8u) {
        return (-EINVAL);
    }

    s33 = unpack_right_shift_u64(src_p[0], 0u, 0xffu);
    s33 |= unpack_left_shift_u64(src_p[1], 8u, 0xffu);
    s33 |= unpack_left_shift_u64(src_p[2], 16u, 0xffu);
    s33 |= unpack_left_shift_u64(src_p[3], 24u, 0xffu);
    s33 |= unpack_left_shift_u64(src_p[4], 32u, 0x01u);

    if ((s33 & (1ull << 32)) != 0ull) {
        s33 |= 0xfffffffe00000000ull;
    }

    dst_p->s33 = (int64_t)s33;

    return (0);
}

static int signed_message33_check_ranges(struct signed_message33_t *msg)
{
    int idx = 1;

    if (!signed_message33_s33_is_in_range(msg->s33))
        return idx;

    idx++;

    return 0;
}

int signed_message33_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s33)
{
    struct signed_message33_t msg;

    msg.s33 = signed_message33_s33_encode(s33);

    int ret = signed_message33_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message33_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message33_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s33)
{
    struct signed_message33_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message33_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message33_check_ranges(&msg);

    if (s33)
        *s33 = signed_message33_s33_decode(msg.s33);

    return ret;
}

int64_t signed_message33_s33_encode(double value)
{
    return (int64_t)(value);
}

double signed_message33_s33_decode(int64_t value)
{
    return ((double)value);
}

double signed_message33_s33_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -4294967296.0);
    ret = CTOOLS_MIN(ret, 4294967295.0);
    return ret;
}

bool signed_message33_s33_is_in_range(int64_t value)
{
    return ((value >= -4294967296ll) && (value <= 4294967295ll));
}

int signed_message32_pack(
    uint8_t *dst_p,
    const struct signed_message32_t *src_p,
    size_t size)
{
    uint32_t s32;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    s32 = (uint32_t)src_p->s32;
    dst_p[0] |= pack_left_shift_u32(s32, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(s32, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(s32, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(s32, 24u, 0xffu);

    return (8);
}

int signed_message32_unpack(
    struct signed_message32_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t s32;

    if (size < 8u) {
        return (-EINVAL);
    }

    s32 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    s32 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    s32 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    s32 |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->s32 = (int32_t)s32;

    return (0);
}

static int signed_message32_check_ranges(struct signed_message32_t *msg)
{
    int idx = 1;

    if (!signed_message32_s32_is_in_range(msg->s32))
        return idx;

    idx++;

    return 0;
}

int signed_message32_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double s32)
{
    struct signed_message32_t msg;

    msg.s32 = signed_message32_s32_encode(s32);

    int ret = signed_message32_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = signed_message32_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int signed_message32_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *s32)
{
    struct signed_message32_t msg;
    memset(&msg, 0, sizeof(msg));

    if (signed_message32_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = signed_message32_check_ranges(&msg);

    if (s32)
        *s32 = signed_message32_s32_decode(msg.s32);

    return ret;
}

int32_t signed_message32_s32_encode(double value)
{
    return (int32_t)(value);
}

double signed_message32_s32_decode(int32_t value)
{
    return ((double)value);
}

double signed_message32_s32_clamp(double val)
{
    double ret = val;


    return ret;
}

bool signed_message32_s32_is_in_range(int32_t value)
{
    (void)value;

    return (true);
}

#undef CTOOLS_MAX
#undef CTOOLS_MIN
