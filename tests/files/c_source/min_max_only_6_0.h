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

#ifndef MIN_MAX_ONLY_6_0_H
#define MIN_MAX_ONLY_6_0_H

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
#define MIN_MAX_ONLY_6_0_SYMBOL1_FRAME_ID (0x00u)

/* Frame lengths in bytes. */
#define MIN_MAX_ONLY_6_0_SYMBOL1_LENGTH (8u)

/* Extended or standard frame types. */
#define MIN_MAX_ONLY_6_0_SYMBOL1_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */


/* Signal choices. */


/**
 * Signals in message Symbol1.
 *
 * All signal values are as on the CAN bus.
 */
struct min_max_only_6_0_symbol1_t {
    /**
     * Range: ..254 (..254 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t signal1;

    /**
     * Range: 5.. (5.. -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t signal2;

    /**
     * Range: ..5 (..5 -)
     * Scale: 1
     * Offset: 0
     */
    int8_t signal4;

    /**
     * Range: -2.. (-2.. -)
     * Scale: 1
     * Offset: 0
     */
    int16_t signal3;
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
int min_max_only_6_0_symbol1_pack(
    uint8_t *dst_p,
    const struct min_max_only_6_0_symbol1_t *src_p,
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
int min_max_only_6_0_symbol1_unpack(
    struct min_max_only_6_0_symbol1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t min_max_only_6_0_symbol1_signal1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double min_max_only_6_0_symbol1_signal1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool min_max_only_6_0_symbol1_signal1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t min_max_only_6_0_symbol1_signal2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double min_max_only_6_0_symbol1_signal2_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool min_max_only_6_0_symbol1_signal2_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t min_max_only_6_0_symbol1_signal4_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double min_max_only_6_0_symbol1_signal4_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool min_max_only_6_0_symbol1_signal4_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t min_max_only_6_0_symbol1_signal3_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double min_max_only_6_0_symbol1_signal3_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool min_max_only_6_0_symbol1_signal3_is_in_range(int16_t value);


#ifdef __cplusplus
}
#endif

#endif
