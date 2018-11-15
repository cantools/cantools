/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Erik Moqvist
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

#define ftoi(value) (*((uint32_t *)(&(value))))
#define itof(value) (*((float *)(&(value))))
#define dtoi(value) (*((uint64_t *)(&(value))))
#define itod(value) (*((double *)(&(value))))

ssize_t floating_point_message1_encode(
    uint8_t *dst_p,
    struct floating_point_message1_t *src_p,
    size_t size)
{
    uint64_t signal1;

    if (size < 8) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    signal1 = dtoi(src_p->signal1);

    dst_p[0] |= ((signal1 << 0) & 0xff);
    dst_p[1] |= ((signal1 >> 8) & 0xff);
    dst_p[2] |= ((signal1 >> 16) & 0xff);
    dst_p[3] |= ((signal1 >> 24) & 0xff);
    dst_p[4] |= ((signal1 >> 32) & 0xff);
    dst_p[5] |= ((signal1 >> 40) & 0xff);
    dst_p[6] |= ((signal1 >> 48) & 0xff);
    dst_p[7] |= ((signal1 >> 56) & 0xff);

    return (8);
}

int floating_point_message1_decode(
    struct floating_point_message1_t *dst_p,
    uint8_t *src_p,
    size_t size)
{
    uint64_t signal1 = 0;

    if (size < 8) {
        return (-EINVAL);
    }

    memset(dst_p, 0, sizeof(*dst_p));

    signal1 |= ((uint64_t)(src_p[0] & 0xff) >> 0);
    signal1 |= ((uint64_t)(src_p[1] & 0xff) << 8);
    signal1 |= ((uint64_t)(src_p[2] & 0xff) << 16);
    signal1 |= ((uint64_t)(src_p[3] & 0xff) << 24);
    signal1 |= ((uint64_t)(src_p[4] & 0xff) << 32);
    signal1 |= ((uint64_t)(src_p[5] & 0xff) << 40);
    signal1 |= ((uint64_t)(src_p[6] & 0xff) << 48);
    signal1 |= ((uint64_t)(src_p[7] & 0xff) << 56);

    dst_p->signal1 = itod(signal1);

    return (0);
}

ssize_t floating_point_message2_encode(
    uint8_t *dst_p,
    struct floating_point_message2_t *src_p,
    size_t size)
{
    uint32_t signal1;
    uint32_t signal2;

    if (size < 8) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    signal1 = ftoi(src_p->signal1);
    signal2 = ftoi(src_p->signal2);

    dst_p[0] |= ((signal1 << 0) & 0xff);
    dst_p[1] |= ((signal1 >> 8) & 0xff);
    dst_p[2] |= ((signal1 >> 16) & 0xff);
    dst_p[3] |= ((signal1 >> 24) & 0xff);
    dst_p[4] |= ((signal2 << 0) & 0xff);
    dst_p[5] |= ((signal2 >> 8) & 0xff);
    dst_p[6] |= ((signal2 >> 16) & 0xff);
    dst_p[7] |= ((signal2 >> 24) & 0xff);

    return (8);
}

int floating_point_message2_decode(
    struct floating_point_message2_t *dst_p,
    uint8_t *src_p,
    size_t size)
{
    uint32_t signal1 = 0;
    uint32_t signal2 = 0;

    if (size < 8) {
        return (-EINVAL);
    }

    memset(dst_p, 0, sizeof(*dst_p));

    signal1 |= ((uint32_t)(src_p[0] & 0xff) >> 0);
    signal1 |= ((uint32_t)(src_p[1] & 0xff) << 8);
    signal1 |= ((uint32_t)(src_p[2] & 0xff) << 16);
    signal1 |= ((uint32_t)(src_p[3] & 0xff) << 24);
    signal2 |= ((uint32_t)(src_p[4] & 0xff) >> 0);
    signal2 |= ((uint32_t)(src_p[5] & 0xff) << 8);
    signal2 |= ((uint32_t)(src_p[6] & 0xff) << 16);
    signal2 |= ((uint32_t)(src_p[7] & 0xff) << 24);

    dst_p->signal1 = itof(signal1);
    dst_p->signal2 = itof(signal2);

    return (0);
}
