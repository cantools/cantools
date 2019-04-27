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

#ifndef LETTER_TERMINATED_CAN_ID_6_0_H
#define LETTER_TERMINATED_CAN_ID_6_0_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef EINVAL
#    define EINVAL 22
#endif

/* Frame ids. */
#define LETTER_TERMINATED_CAN_ID_6_0_SYMBOL1_FRAME_ID (0x0au)

/* Frame lengths in bytes. */
#define LETTER_TERMINATED_CAN_ID_6_0_SYMBOL1_LENGTH (8u)

/* Extended or standard frame types. */
#define LETTER_TERMINATED_CAN_ID_6_0_SYMBOL1_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */


/* Signal choices. */


/**
 * Signals in message Symbol1.
 *
 * All signal values are as on the CAN bus.
 */
struct letter_terminated_can_id_6_0_symbol1_t {
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
};

/**
 * Pack message Symbol1.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int letter_terminated_can_id_6_0_symbol1_pack(
    uint8_t *dst_p,
    const struct letter_terminated_can_id_6_0_symbol1_t *src_p,
    size_t size);

/**
 * Unpack message Symbol1.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int letter_terminated_can_id_6_0_symbol1_unpack(
    struct letter_terminated_can_id_6_0_symbol1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

#endif
