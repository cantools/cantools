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

#include "padding_bit_order.h"

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

static inline uint8_t pack_left_shift_u64(
    uint64_t value,
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

static inline uint8_t pack_right_shift_u64(
    uint64_t value,
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

static inline uint64_t unpack_right_shift_u64(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint64_t)((uint64_t)(value & mask) >> shift);
}

int padding_bit_order_msg0_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg0_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->b, 7u, 0x80u);
    dst_p[0] |= pack_right_shift_u16(src_p->a, 8u, 0x7fu);
    dst_p[1] |= pack_left_shift_u16(src_p->a, 0u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->d, 7u, 0x80u);
    dst_p[4] |= pack_right_shift_u16(src_p->c, 8u, 0x7fu);
    dst_p[5] |= pack_left_shift_u16(src_p->c, 0u, 0xffu);

    return (8);
}

int padding_bit_order_msg0_unpack(
    struct padding_bit_order_msg0_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->b = unpack_right_shift_u8(src_p[0], 7u, 0x80u);
    dst_p->a = unpack_left_shift_u16(src_p[0], 8u, 0x7fu);
    dst_p->a |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);
    dst_p->d = unpack_right_shift_u8(src_p[4], 7u, 0x80u);
    dst_p->c = unpack_left_shift_u16(src_p[4], 8u, 0x7fu);
    dst_p->c |= unpack_right_shift_u16(src_p[5], 0u, 0xffu);

    return (0);
}

static int padding_bit_order_msg0_check_ranges(struct padding_bit_order_msg0_t *msg)
{
    int idx = 1;

    if (!padding_bit_order_msg0_b_is_in_range(msg->b))
        return idx;

    idx++;

    if (!padding_bit_order_msg0_a_is_in_range(msg->a))
        return idx;

    idx++;

    if (!padding_bit_order_msg0_d_is_in_range(msg->d))
        return idx;

    idx++;

    if (!padding_bit_order_msg0_c_is_in_range(msg->c))
        return idx;

    idx++;

    return 0;
}

int padding_bit_order_msg0_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double b,
    double a,
    double d,
    double c)
{
    struct padding_bit_order_msg0_t msg;

    msg.b = padding_bit_order_msg0_b_encode(b);
    msg.a = padding_bit_order_msg0_a_encode(a);
    msg.d = padding_bit_order_msg0_d_encode(d);
    msg.c = padding_bit_order_msg0_c_encode(c);

    int ret = padding_bit_order_msg0_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = padding_bit_order_msg0_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int padding_bit_order_msg0_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *b,
    double *a,
    double *d,
    double *c)
{
    struct padding_bit_order_msg0_t msg;
    memset(&msg, 0, sizeof(msg));

    if (padding_bit_order_msg0_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = padding_bit_order_msg0_check_ranges(&msg);

    if (b)
        *b = padding_bit_order_msg0_b_decode(msg.b);

    if (a)
        *a = padding_bit_order_msg0_a_decode(msg.a);

    if (d)
        *d = padding_bit_order_msg0_d_decode(msg.d);

    if (c)
        *c = padding_bit_order_msg0_c_decode(msg.c);

    return ret;
}

uint8_t padding_bit_order_msg0_b_encode(double value)
{
    return (uint8_t)(value);
}

double padding_bit_order_msg0_b_decode(uint8_t value)
{
    return ((double)value);
}

double padding_bit_order_msg0_b_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool padding_bit_order_msg0_b_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint16_t padding_bit_order_msg0_a_encode(double value)
{
    return (uint16_t)(value);
}

double padding_bit_order_msg0_a_decode(uint16_t value)
{
    return ((double)value);
}

double padding_bit_order_msg0_a_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32767.0);
    return ret;
}

bool padding_bit_order_msg0_a_is_in_range(uint16_t value)
{
    return (value <= 32767u);
}

uint8_t padding_bit_order_msg0_d_encode(double value)
{
    return (uint8_t)(value);
}

double padding_bit_order_msg0_d_decode(uint8_t value)
{
    return ((double)value);
}

double padding_bit_order_msg0_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool padding_bit_order_msg0_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint16_t padding_bit_order_msg0_c_encode(double value)
{
    return (uint16_t)(value);
}

double padding_bit_order_msg0_c_decode(uint16_t value)
{
    return ((double)value);
}

double padding_bit_order_msg0_c_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32767.0);
    return ret;
}

bool padding_bit_order_msg0_c_is_in_range(uint16_t value)
{
    return (value <= 32767u);
}

int padding_bit_order_msg1_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->e, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u16(src_p->f, 1u, 0xfeu);
    dst_p[1] |= pack_right_shift_u16(src_p->f, 7u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->g, 0u, 0x01u);
    dst_p[4] |= pack_left_shift_u16(src_p->h, 1u, 0xfeu);
    dst_p[5] |= pack_right_shift_u16(src_p->h, 7u, 0xffu);

    return (8);
}

int padding_bit_order_msg1_unpack(
    struct padding_bit_order_msg1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->e = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->f = unpack_right_shift_u16(src_p[0], 1u, 0xfeu);
    dst_p->f |= unpack_left_shift_u16(src_p[1], 7u, 0xffu);
    dst_p->g = unpack_right_shift_u8(src_p[4], 0u, 0x01u);
    dst_p->h = unpack_right_shift_u16(src_p[4], 1u, 0xfeu);
    dst_p->h |= unpack_left_shift_u16(src_p[5], 7u, 0xffu);

    return (0);
}

static int padding_bit_order_msg1_check_ranges(struct padding_bit_order_msg1_t *msg)
{
    int idx = 1;

    if (!padding_bit_order_msg1_e_is_in_range(msg->e))
        return idx;

    idx++;

    if (!padding_bit_order_msg1_f_is_in_range(msg->f))
        return idx;

    idx++;

    if (!padding_bit_order_msg1_g_is_in_range(msg->g))
        return idx;

    idx++;

    if (!padding_bit_order_msg1_h_is_in_range(msg->h))
        return idx;

    idx++;

    return 0;
}

int padding_bit_order_msg1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double e,
    double f,
    double g,
    double h)
{
    struct padding_bit_order_msg1_t msg;

    msg.e = padding_bit_order_msg1_e_encode(e);
    msg.f = padding_bit_order_msg1_f_encode(f);
    msg.g = padding_bit_order_msg1_g_encode(g);
    msg.h = padding_bit_order_msg1_h_encode(h);

    int ret = padding_bit_order_msg1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = padding_bit_order_msg1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int padding_bit_order_msg1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *e,
    double *f,
    double *g,
    double *h)
{
    struct padding_bit_order_msg1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (padding_bit_order_msg1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = padding_bit_order_msg1_check_ranges(&msg);

    if (e)
        *e = padding_bit_order_msg1_e_decode(msg.e);

    if (f)
        *f = padding_bit_order_msg1_f_decode(msg.f);

    if (g)
        *g = padding_bit_order_msg1_g_decode(msg.g);

    if (h)
        *h = padding_bit_order_msg1_h_decode(msg.h);

    return ret;
}

uint8_t padding_bit_order_msg1_e_encode(double value)
{
    return (uint8_t)(value);
}

double padding_bit_order_msg1_e_decode(uint8_t value)
{
    return ((double)value);
}

double padding_bit_order_msg1_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool padding_bit_order_msg1_e_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint16_t padding_bit_order_msg1_f_encode(double value)
{
    return (uint16_t)(value);
}

double padding_bit_order_msg1_f_decode(uint16_t value)
{
    return ((double)value);
}

double padding_bit_order_msg1_f_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32767.0);
    return ret;
}

bool padding_bit_order_msg1_f_is_in_range(uint16_t value)
{
    return (value <= 32767u);
}

uint8_t padding_bit_order_msg1_g_encode(double value)
{
    return (uint8_t)(value);
}

double padding_bit_order_msg1_g_decode(uint8_t value)
{
    return ((double)value);
}

double padding_bit_order_msg1_g_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool padding_bit_order_msg1_g_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint16_t padding_bit_order_msg1_h_encode(double value)
{
    return (uint16_t)(value);
}

double padding_bit_order_msg1_h_decode(uint16_t value)
{
    return ((double)value);
}

double padding_bit_order_msg1_h_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32767.0);
    return ret;
}

bool padding_bit_order_msg1_h_is_in_range(uint16_t value)
{
    return (value <= 32767u);
}

int padding_bit_order_msg2_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->i, 0u, 0x0fu);
    dst_p[0] |= pack_left_shift_u8(src_p->j, 4u, 0xf0u);
    dst_p[1] |= pack_left_shift_u8(src_p->k, 0u, 0x0fu);

    return (8);
}

int padding_bit_order_msg2_unpack(
    struct padding_bit_order_msg2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->i = unpack_right_shift_u8(src_p[0], 0u, 0x0fu);
    dst_p->j = unpack_right_shift_u8(src_p[0], 4u, 0xf0u);
    dst_p->k = unpack_right_shift_u8(src_p[1], 0u, 0x0fu);

    return (0);
}

static int padding_bit_order_msg2_check_ranges(struct padding_bit_order_msg2_t *msg)
{
    int idx = 1;

    if (!padding_bit_order_msg2_i_is_in_range(msg->i))
        return idx;

    idx++;

    if (!padding_bit_order_msg2_j_is_in_range(msg->j))
        return idx;

    idx++;

    if (!padding_bit_order_msg2_k_is_in_range(msg->k))
        return idx;

    idx++;

    return 0;
}

int padding_bit_order_msg2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double i,
    double j,
    double k)
{
    struct padding_bit_order_msg2_t msg;

    msg.i = padding_bit_order_msg2_i_encode(i);
    msg.j = padding_bit_order_msg2_j_encode(j);
    msg.k = padding_bit_order_msg2_k_encode(k);

    int ret = padding_bit_order_msg2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = padding_bit_order_msg2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int padding_bit_order_msg2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *i,
    double *j,
    double *k)
{
    struct padding_bit_order_msg2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (padding_bit_order_msg2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = padding_bit_order_msg2_check_ranges(&msg);

    if (i)
        *i = padding_bit_order_msg2_i_decode(msg.i);

    if (j)
        *j = padding_bit_order_msg2_j_decode(msg.j);

    if (k)
        *k = padding_bit_order_msg2_k_decode(msg.k);

    return ret;
}

uint8_t padding_bit_order_msg2_i_encode(double value)
{
    return (uint8_t)(value);
}

double padding_bit_order_msg2_i_decode(uint8_t value)
{
    return ((double)value);
}

double padding_bit_order_msg2_i_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 15.0);
    return ret;
}

bool padding_bit_order_msg2_i_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

uint8_t padding_bit_order_msg2_j_encode(double value)
{
    return (uint8_t)(value);
}

double padding_bit_order_msg2_j_decode(uint8_t value)
{
    return ((double)value);
}

double padding_bit_order_msg2_j_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 15.0);
    return ret;
}

bool padding_bit_order_msg2_j_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

uint8_t padding_bit_order_msg2_k_encode(double value)
{
    return (uint8_t)(value);
}

double padding_bit_order_msg2_k_decode(uint8_t value)
{
    return ((double)value);
}

double padding_bit_order_msg2_k_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 15.0);
    return ret;
}

bool padding_bit_order_msg2_k_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

int padding_bit_order_msg3_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg3_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_right_shift_u64(src_p->l, 56u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(src_p->l, 48u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(src_p->l, 40u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(src_p->l, 32u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(src_p->l, 24u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(src_p->l, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(src_p->l, 8u, 0xffu);
    dst_p[7] |= pack_left_shift_u64(src_p->l, 0u, 0xffu);

    return (8);
}

int padding_bit_order_msg3_unpack(
    struct padding_bit_order_msg3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->l = unpack_left_shift_u64(src_p[0], 56u, 0xffu);
    dst_p->l |= unpack_left_shift_u64(src_p[1], 48u, 0xffu);
    dst_p->l |= unpack_left_shift_u64(src_p[2], 40u, 0xffu);
    dst_p->l |= unpack_left_shift_u64(src_p[3], 32u, 0xffu);
    dst_p->l |= unpack_left_shift_u64(src_p[4], 24u, 0xffu);
    dst_p->l |= unpack_left_shift_u64(src_p[5], 16u, 0xffu);
    dst_p->l |= unpack_left_shift_u64(src_p[6], 8u, 0xffu);
    dst_p->l |= unpack_right_shift_u64(src_p[7], 0u, 0xffu);

    return (0);
}

static int padding_bit_order_msg3_check_ranges(struct padding_bit_order_msg3_t *msg)
{
    int idx = 1;

    if (!padding_bit_order_msg3_l_is_in_range(msg->l))
        return idx;

    idx++;

    return 0;
}

int padding_bit_order_msg3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double l)
{
    struct padding_bit_order_msg3_t msg;

    msg.l = padding_bit_order_msg3_l_encode(l);

    int ret = padding_bit_order_msg3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = padding_bit_order_msg3_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int padding_bit_order_msg3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *l)
{
    struct padding_bit_order_msg3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (padding_bit_order_msg3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = padding_bit_order_msg3_check_ranges(&msg);

    if (l)
        *l = padding_bit_order_msg3_l_decode(msg.l);

    return ret;
}

uint64_t padding_bit_order_msg3_l_encode(double value)
{
    return (uint64_t)(value);
}

double padding_bit_order_msg3_l_decode(uint64_t value)
{
    return ((double)value);
}

double padding_bit_order_msg3_l_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 18446744073709551615.0);
    return ret;
}

bool padding_bit_order_msg3_l_is_in_range(uint64_t value)
{
    (void)value;

    return (true);
}

int padding_bit_order_msg4_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg4_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u64(src_p->m, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u64(src_p->m, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u64(src_p->m, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u64(src_p->m, 24u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(src_p->m, 32u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(src_p->m, 40u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(src_p->m, 48u, 0xffu);
    dst_p[7] |= pack_right_shift_u64(src_p->m, 56u, 0xffu);

    return (8);
}

int padding_bit_order_msg4_unpack(
    struct padding_bit_order_msg4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->m = unpack_right_shift_u64(src_p[0], 0u, 0xffu);
    dst_p->m |= unpack_left_shift_u64(src_p[1], 8u, 0xffu);
    dst_p->m |= unpack_left_shift_u64(src_p[2], 16u, 0xffu);
    dst_p->m |= unpack_left_shift_u64(src_p[3], 24u, 0xffu);
    dst_p->m |= unpack_left_shift_u64(src_p[4], 32u, 0xffu);
    dst_p->m |= unpack_left_shift_u64(src_p[5], 40u, 0xffu);
    dst_p->m |= unpack_left_shift_u64(src_p[6], 48u, 0xffu);
    dst_p->m |= unpack_left_shift_u64(src_p[7], 56u, 0xffu);

    return (0);
}

static int padding_bit_order_msg4_check_ranges(struct padding_bit_order_msg4_t *msg)
{
    int idx = 1;

    if (!padding_bit_order_msg4_m_is_in_range(msg->m))
        return idx;

    idx++;

    return 0;
}

int padding_bit_order_msg4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double m)
{
    struct padding_bit_order_msg4_t msg;

    msg.m = padding_bit_order_msg4_m_encode(m);

    int ret = padding_bit_order_msg4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = padding_bit_order_msg4_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int padding_bit_order_msg4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *m)
{
    struct padding_bit_order_msg4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (padding_bit_order_msg4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = padding_bit_order_msg4_check_ranges(&msg);

    if (m)
        *m = padding_bit_order_msg4_m_decode(msg.m);

    return ret;
}

uint64_t padding_bit_order_msg4_m_encode(double value)
{
    return (uint64_t)(value);
}

double padding_bit_order_msg4_m_decode(uint64_t value)
{
    return ((double)value);
}

double padding_bit_order_msg4_m_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 18446744073709551615.0);
    return ret;
}

bool padding_bit_order_msg4_m_is_in_range(uint64_t value)
{
    (void)value;

    return (true);
}

bool is_extended_frame(uint32_t frame_id)
{
    return false;
}

#undef CTOOLS_MAX
#undef CTOOLS_MIN
