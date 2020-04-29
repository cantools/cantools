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

#include "no_signals.h"

int no_signals_message1_pack(
    uint8_t *dst_p,
    const struct no_signals_message1_t *src_p,
    size_t size)
{
    (void)src_p;

    if (size < 5u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 5);

    return (5);
}

int no_signals_message1_unpack(
    struct no_signals_message1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;

    if (size < 5u) {
        return (-EINVAL);
    }

    return (0);
}

int no_signals_message2_pack(
    uint8_t *dst_p,
    const struct no_signals_message2_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;
    (void)size;

    return (0);
}

int no_signals_message2_unpack(
    struct no_signals_message2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;
    (void)size;

    return (0);
}
