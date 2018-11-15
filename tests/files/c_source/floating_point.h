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

#ifndef __FLOATING_POINT_H__
#define __FLOATING_POINT_H__

#include <stdint.h>
#include <unistd.h>

#ifndef EINVAL
#    define EINVAL -22
#endif

#define FLOATING_POINT_FRAME_ID_MESSAGE1 (0x400U)
#define FLOATING_POINT_FRAME_ID_MESSAGE2 (0x401U)

/**
 * Signals in message Message1.
 *
 * @param signal1
 *            Scale: 1
 *            Offset: 0
 */
struct floating_point_message1_t {
    double signal1;
};

/**
 * Signals in message Message2.
 *
 * @param signal1
 *            Scale: 1
 *            Offset: 0
 * @param signal2
 *            Scale: 1
 *            Offset: 0
 */
struct floating_point_message2_t {
    float signal1;
    float signal2;
};

/**
 * Encode message Message1.
 *
 * @param[out] dst_p Buffer to encode the message into.
 * @param[in] src_p Data to encode.
 * @param[in] size Size of dst_p.
 *
 * @return Size of encoded data, or negative error code.
 */
ssize_t floating_point_message1_encode(
    uint8_t *dst_p,
    struct floating_point_message1_t *src_p,
    size_t size);

/**
 * Decode message Message1.
 *
 * @param[out] dst_p Object to decode the message into.
 * @param[in] src_p Message to decode.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int floating_point_message1_decode(
    struct floating_point_message1_t *dst_p,
    uint8_t *src_p,
    size_t size);

/**
 * Encode message Message2.
 *
 * @param[out] dst_p Buffer to encode the message into.
 * @param[in] src_p Data to encode.
 * @param[in] size Size of dst_p.
 *
 * @return Size of encoded data, or negative error code.
 */
ssize_t floating_point_message2_encode(
    uint8_t *dst_p,
    struct floating_point_message2_t *src_p,
    size_t size);

/**
 * Decode message Message2.
 *
 * @param[out] dst_p Object to decode the message into.
 * @param[in] src_p Message to decode.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int floating_point_message2_decode(
    struct floating_point_message2_t *dst_p,
    uint8_t *src_p,
    size_t size);

#endif
