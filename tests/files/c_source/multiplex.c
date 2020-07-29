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
