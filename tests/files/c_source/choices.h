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

#ifndef CHOICES_H
#define CHOICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef EINVAL
#    define EINVAL 22
#endif

/* Frame ids. */
#define CHOICES_FOO_FRAME_ID (0x00u)

/* Frame lengths in bytes. */
#define CHOICES_FOO_LENGTH (8u)

/* Extended or standard frame types. */
#define CHOICES_FOO_IS_EXTENDED (0)
/* return whether a certain frame uses an extended id */
bool is_extended_frame(uint32_t frame_id);

/* Frame cycle times in milliseconds. */


/* Signal choices. */
#define CHOICES_FOO_FOO_A_NEGATIVE_VALUE_CHOICE (-5)
#define CHOICES_FOO_FOO_WITH_SPACE_CHOICE (0)
#define CHOICES_FOO_FOO________CHOICE (1)
#define CHOICES_FOO_FOO_UNUSED_2__CHOICE (2)
#define CHOICES_FOO_FOO_UNUSED_3_CHOICE (3)
#define CHOICES_FOO_FOO_UNUSED_2_CHOICE (4)
#define CHOICES_FOO_FOO_RESERVED_5_CHOICE (5)
#define CHOICES_FOO_FOO_RESERVED_6_CHOICE (6)

/**
 * Signals in message Foo.
 *
 * All signal values are as on the CAN bus.
 */
struct choices_foo_t {
    /**
     * Range: -128..127 (-128..127 -)
     * Scale: 1
     * Offset: 0
     */
    int8_t foo;
};

/**
 * Pack message Foo.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int choices_foo_pack(
    uint8_t *dst_p,
    const struct choices_foo_t *src_p,
    size_t size);

/**
 * Unpack message Foo.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int choices_foo_unpack(
    struct choices_foo_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t choices_foo_foo_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double choices_foo_foo_decode(int8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double choices_foo_foo_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool choices_foo_foo_is_in_range(int8_t value);

/**
 * Create message Foo if range check ok.
 * @param[out] outbuf:    buffer to write message into
 * @param[in]  outbuf_sz: size of outbuf
 *
 * @returns zero (success),
 *          -1   (problem packing, likely buffer too small)
 *          n>0  (nth value out of range)
 */
int choices_foo_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double foo);

/**
 * unpack message Foo and check for allowable ranges
 * @param[in]  inbuf:    buffer to read from
 * @param[in]  inbuf_sz: length in bytes
 * @param[out] rest:     pointers to data to fill
 *
 * @returns: zero: on success
 *           -1:   error during unpacking
 *           n>0:  nth parameter out of range
 *
 * even if parameters are out of range, the output values will be set.
 */
int choices_foo_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *foo);


#ifdef __cplusplus
}
#endif

#endif
