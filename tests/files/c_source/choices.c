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

#include "choices.h"

#define CTOOLS_MAX(x,y) (((x) < (y)) ? (y) : (x))
#define CTOOLS_MIN(x,y) (((x) < (y)) ? (x) : (y))

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

int choices_foo_pack(
    uint8_t *dst_p,
    const struct choices_foo_t *src_p,
    size_t size)
{
    uint8_t foo;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    foo = (uint8_t)src_p->foo;
    dst_p[0] |= pack_left_shift_u8(foo, 0u, 0xffu);

    return (8);
}

int choices_foo_unpack(
    struct choices_foo_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint8_t foo;

    if (size < 8u) {
        return (-EINVAL);
    }

    foo = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->foo = (int8_t)foo;

    return (0);
}

static int choices_foo_check_ranges(struct choices_foo_t *msg)
{
    int idx = 1;

    if (!choices_foo_foo_is_in_range(msg->foo))
        return idx;

    idx++;

    return 0;
}

int choices_foo_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double foo)
{
    struct choices_foo_t msg;

    msg.foo = choices_foo_foo_encode(foo);

    int ret = choices_foo_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = choices_foo_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int choices_foo_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *foo)
{
    struct choices_foo_t msg;
    memset(&msg, 0, sizeof(msg));

    if (choices_foo_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = choices_foo_check_ranges(&msg);

    if (foo)
        *foo = choices_foo_foo_decode(msg.foo);

    return ret;
}

int8_t choices_foo_foo_encode(double value)
{
    return (int8_t)(value);
}

double choices_foo_foo_decode(int8_t value)
{
    return ((double)value);
}

double choices_foo_foo_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -128.0);
    ret = CTOOLS_MIN(ret, 127.0);
    return ret;
}

bool choices_foo_foo_is_in_range(int8_t value)
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
