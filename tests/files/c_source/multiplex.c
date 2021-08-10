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

#include "multiplex.h"

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

int multiplex_message1_pack(
    uint8_t *dst_p,
    const struct multiplex_message1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->multiplexor, 2u, 0xfcu);

    switch (src_p->multiplexor) {

    case 8:
        dst_p[2] |= pack_left_shift_u8(src_p->bit_j, 2u, 0x04u);
        dst_p[2] |= pack_left_shift_u8(src_p->bit_c, 3u, 0x08u);
        dst_p[2] |= pack_left_shift_u8(src_p->bit_g, 7u, 0x80u);
        dst_p[3] |= pack_left_shift_u8(src_p->bit_l, 0u, 0x01u);
        break;

    case 16:
        dst_p[2] |= pack_left_shift_u8(src_p->bit_j, 2u, 0x04u);
        dst_p[2] |= pack_left_shift_u8(src_p->bit_c, 3u, 0x08u);
        dst_p[2] |= pack_left_shift_u8(src_p->bit_g, 7u, 0x80u);
        dst_p[3] |= pack_left_shift_u8(src_p->bit_l, 0u, 0x01u);
        break;

    case 24:
        dst_p[2] |= pack_left_shift_u8(src_p->bit_j, 2u, 0x04u);
        dst_p[2] |= pack_left_shift_u8(src_p->bit_c, 3u, 0x08u);
        dst_p[2] |= pack_left_shift_u8(src_p->bit_g, 7u, 0x80u);
        dst_p[3] |= pack_left_shift_u8(src_p->bit_l, 0u, 0x01u);
        dst_p[3] |= pack_left_shift_u8(src_p->bit_a, 2u, 0x04u);
        dst_p[3] |= pack_left_shift_u8(src_p->bit_k, 4u, 0x10u);
        dst_p[3] |= pack_left_shift_u8(src_p->bit_e, 5u, 0x20u);
        dst_p[4] |= pack_left_shift_u8(src_p->bit_d, 0u, 0x01u);
        dst_p[4] |= pack_left_shift_u8(src_p->bit_b, 1u, 0x02u);
        dst_p[4] |= pack_left_shift_u8(src_p->bit_h, 6u, 0x40u);
        dst_p[4] |= pack_left_shift_u8(src_p->bit_f, 7u, 0x80u);
        break;

    default:
        break;
    }

    return (8);
}

int multiplex_message1_unpack(
    struct multiplex_message1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->multiplexor = unpack_right_shift_u8(src_p[0], 2u, 0xfcu);

    switch (dst_p->multiplexor) {

    case 8:
        dst_p->bit_j = unpack_right_shift_u8(src_p[2], 2u, 0x04u);
        dst_p->bit_c = unpack_right_shift_u8(src_p[2], 3u, 0x08u);
        dst_p->bit_g = unpack_right_shift_u8(src_p[2], 7u, 0x80u);
        dst_p->bit_l = unpack_right_shift_u8(src_p[3], 0u, 0x01u);
        break;

    case 16:
        dst_p->bit_j = unpack_right_shift_u8(src_p[2], 2u, 0x04u);
        dst_p->bit_c = unpack_right_shift_u8(src_p[2], 3u, 0x08u);
        dst_p->bit_g = unpack_right_shift_u8(src_p[2], 7u, 0x80u);
        dst_p->bit_l = unpack_right_shift_u8(src_p[3], 0u, 0x01u);
        break;

    case 24:
        dst_p->bit_j = unpack_right_shift_u8(src_p[2], 2u, 0x04u);
        dst_p->bit_c = unpack_right_shift_u8(src_p[2], 3u, 0x08u);
        dst_p->bit_g = unpack_right_shift_u8(src_p[2], 7u, 0x80u);
        dst_p->bit_l = unpack_right_shift_u8(src_p[3], 0u, 0x01u);
        dst_p->bit_a = unpack_right_shift_u8(src_p[3], 2u, 0x04u);
        dst_p->bit_k = unpack_right_shift_u8(src_p[3], 4u, 0x10u);
        dst_p->bit_e = unpack_right_shift_u8(src_p[3], 5u, 0x20u);
        dst_p->bit_d = unpack_right_shift_u8(src_p[4], 0u, 0x01u);
        dst_p->bit_b = unpack_right_shift_u8(src_p[4], 1u, 0x02u);
        dst_p->bit_h = unpack_right_shift_u8(src_p[4], 6u, 0x40u);
        dst_p->bit_f = unpack_right_shift_u8(src_p[4], 7u, 0x80u);
        break;

    default:
        break;
    }

    return (0);
}

static int multiplex_message1_check_ranges(struct multiplex_message1_t *msg)
{
    int idx = 1;

    if (!multiplex_message1_multiplexor_is_in_range(msg->multiplexor))
        return idx;

    idx++;

    if (!multiplex_message1_bit_j_is_in_range(msg->bit_j))
        return idx;

    idx++;

    if (!multiplex_message1_bit_c_is_in_range(msg->bit_c))
        return idx;

    idx++;

    if (!multiplex_message1_bit_g_is_in_range(msg->bit_g))
        return idx;

    idx++;

    if (!multiplex_message1_bit_l_is_in_range(msg->bit_l))
        return idx;

    idx++;

    if (!multiplex_message1_bit_a_is_in_range(msg->bit_a))
        return idx;

    idx++;

    if (!multiplex_message1_bit_k_is_in_range(msg->bit_k))
        return idx;

    idx++;

    if (!multiplex_message1_bit_e_is_in_range(msg->bit_e))
        return idx;

    idx++;

    if (!multiplex_message1_bit_d_is_in_range(msg->bit_d))
        return idx;

    idx++;

    if (!multiplex_message1_bit_b_is_in_range(msg->bit_b))
        return idx;

    idx++;

    if (!multiplex_message1_bit_h_is_in_range(msg->bit_h))
        return idx;

    idx++;

    if (!multiplex_message1_bit_f_is_in_range(msg->bit_f))
        return idx;

    idx++;

    return 0;
}

int multiplex_message1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double multiplexor,
    double bit_j,
    double bit_c,
    double bit_g,
    double bit_l,
    double bit_a,
    double bit_k,
    double bit_e,
    double bit_d,
    double bit_b,
    double bit_h,
    double bit_f)
{
    struct multiplex_message1_t msg;

    msg.multiplexor = multiplex_message1_multiplexor_encode(multiplexor);
    msg.bit_j = multiplex_message1_bit_j_encode(bit_j);
    msg.bit_c = multiplex_message1_bit_c_encode(bit_c);
    msg.bit_g = multiplex_message1_bit_g_encode(bit_g);
    msg.bit_l = multiplex_message1_bit_l_encode(bit_l);
    msg.bit_a = multiplex_message1_bit_a_encode(bit_a);
    msg.bit_k = multiplex_message1_bit_k_encode(bit_k);
    msg.bit_e = multiplex_message1_bit_e_encode(bit_e);
    msg.bit_d = multiplex_message1_bit_d_encode(bit_d);
    msg.bit_b = multiplex_message1_bit_b_encode(bit_b);
    msg.bit_h = multiplex_message1_bit_h_encode(bit_h);
    msg.bit_f = multiplex_message1_bit_f_encode(bit_f);

    int ret = multiplex_message1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = multiplex_message1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int multiplex_message1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *multiplexor,
    double *bit_j,
    double *bit_c,
    double *bit_g,
    double *bit_l,
    double *bit_a,
    double *bit_k,
    double *bit_e,
    double *bit_d,
    double *bit_b,
    double *bit_h,
    double *bit_f)
{
    struct multiplex_message1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (multiplex_message1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = multiplex_message1_check_ranges(&msg);

    if (multiplexor)
        *multiplexor = multiplex_message1_multiplexor_decode(msg.multiplexor);

    if (bit_j)
        *bit_j = multiplex_message1_bit_j_decode(msg.bit_j);

    if (bit_c)
        *bit_c = multiplex_message1_bit_c_decode(msg.bit_c);

    if (bit_g)
        *bit_g = multiplex_message1_bit_g_decode(msg.bit_g);

    if (bit_l)
        *bit_l = multiplex_message1_bit_l_decode(msg.bit_l);

    if (bit_a)
        *bit_a = multiplex_message1_bit_a_decode(msg.bit_a);

    if (bit_k)
        *bit_k = multiplex_message1_bit_k_decode(msg.bit_k);

    if (bit_e)
        *bit_e = multiplex_message1_bit_e_decode(msg.bit_e);

    if (bit_d)
        *bit_d = multiplex_message1_bit_d_decode(msg.bit_d);

    if (bit_b)
        *bit_b = multiplex_message1_bit_b_decode(msg.bit_b);

    if (bit_h)
        *bit_h = multiplex_message1_bit_h_decode(msg.bit_h);

    if (bit_f)
        *bit_f = multiplex_message1_bit_f_decode(msg.bit_f);

    return ret;
}

uint8_t multiplex_message1_multiplexor_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_multiplexor_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_multiplexor_is_in_range(uint8_t value)
{
    return (value <= 63u);
}

uint8_t multiplex_message1_bit_j_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_j_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_j_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_c_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_c_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_c_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_g_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_g_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_g_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_l_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_l_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_l_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_a_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_a_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_a_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_k_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_k_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_k_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_e_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_e_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_e_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_d_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_d_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_b_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_b_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_b_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_h_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_h_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_h_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t multiplex_message1_bit_f_encode(double value)
{
    return (uint8_t)(value);
}

double multiplex_message1_bit_f_decode(uint8_t value)
{
    return ((double)value);
}

bool multiplex_message1_bit_f_is_in_range(uint8_t value)
{
    return (value <= 1u);
}
