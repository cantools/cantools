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

#ifndef MULTIPLEX_2_H
#define MULTIPLEX_2_H

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
#define MULTIPLEX_2_SHARED_FRAME_ID (0xc02fefeu)
#define MULTIPLEX_2_NORMAL_FRAME_ID (0xc01fefeu)
#define MULTIPLEX_2_EXTENDED_FRAME_ID (0xc00fefeu)
#define MULTIPLEX_2_EXTENDED_TYPES_FRAME_ID (0xc03fefeu)

/* Frame lengths in bytes. */
#define MULTIPLEX_2_SHARED_LENGTH (8u)
#define MULTIPLEX_2_NORMAL_LENGTH (8u)
#define MULTIPLEX_2_EXTENDED_LENGTH (8u)
#define MULTIPLEX_2_EXTENDED_TYPES_LENGTH (8u)

/* Extended or standard frame types. */
#define MULTIPLEX_2_SHARED_IS_EXTENDED (1)
#define MULTIPLEX_2_NORMAL_IS_EXTENDED (1)
#define MULTIPLEX_2_EXTENDED_IS_EXTENDED (1)
#define MULTIPLEX_2_EXTENDED_TYPES_IS_EXTENDED (1)

/* Frame cycle times in milliseconds. */
#define MULTIPLEX_2_SHARED_CYCLE_TIME_MS (0u)
#define MULTIPLEX_2_NORMAL_CYCLE_TIME_MS (0u)
#define MULTIPLEX_2_EXTENDED_CYCLE_TIME_MS (0u)
#define MULTIPLEX_2_EXTENDED_TYPES_CYCLE_TIME_MS (0u)

/* Signal choices. */


/**
 * Signals in message Shared.
 *
 * All signal values are as on the CAN bus.
 */
struct multiplex_2_shared_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s0;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s1;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s2;
};

/**
 * Signals in message Normal.
 *
 * All signal values are as on the CAN bus.
 */
struct multiplex_2_normal_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s0;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s1;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s2;
};

/**
 * Signals in message Extended.
 *
 * All signal values are as on the CAN bus.
 */
struct multiplex_2_extended_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s0;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int32_t s5;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s1;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int32_t s4;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s2;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int16_t s3;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s6;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s8;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int32_t s7;
};

/**
 * Signals in message ExtendedTypes.
 *
 * All signal values are as on the CAN bus.
 */
struct multiplex_2_extended_types_t {
    /**
     * Range: 2..6 (2..6 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t s11;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s0;

    /**
     * Range: -340000000000000000000000000000000000000..340000000000000000000000000000000000000 (-3.4E+38..3.4E+38 -)
     * Scale: 1
     * Offset: 0
     */
    float s10;

    /**
     * Range: -1.34..1235 (-1.34..1235 -)
     * Scale: 1
     * Offset: 0
     */
    float s9;
};

/**
 * Pack message Shared.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int multiplex_2_shared_pack(
    uint8_t *dst_p,
    const struct multiplex_2_shared_t *src_p,
    size_t size);

/**
 * Unpack message Shared.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int multiplex_2_shared_unpack(
    struct multiplex_2_shared_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_shared_s0_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_shared_s0_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_shared_s0_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_shared_s1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_shared_s1_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_shared_s1_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_shared_s2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_shared_s2_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_shared_s2_is_in_range(int8_t value);

/**
 * Pack message Normal.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int multiplex_2_normal_pack(
    uint8_t *dst_p,
    const struct multiplex_2_normal_t *src_p,
    size_t size);

/**
 * Unpack message Normal.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int multiplex_2_normal_unpack(
    struct multiplex_2_normal_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_normal_s0_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_normal_s0_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_normal_s0_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_normal_s1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_normal_s1_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_normal_s1_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_normal_s2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_normal_s2_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_normal_s2_is_in_range(int8_t value);

/**
 * Pack message Extended.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int multiplex_2_extended_pack(
    uint8_t *dst_p,
    const struct multiplex_2_extended_t *src_p,
    size_t size);

/**
 * Unpack message Extended.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int multiplex_2_extended_unpack(
    struct multiplex_2_extended_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_extended_s0_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s0_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s0_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int32_t multiplex_2_extended_s5_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s5_decode(int32_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s5_is_in_range(int32_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_extended_s1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s1_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s1_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int32_t multiplex_2_extended_s4_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s4_decode(int32_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s4_is_in_range(int32_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_extended_s2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s2_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s2_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t multiplex_2_extended_s3_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s3_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s3_is_in_range(int16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_extended_s6_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s6_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s6_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_extended_s8_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s8_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s8_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int32_t multiplex_2_extended_s7_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_s7_decode(int32_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_s7_is_in_range(int32_t value);

/**
 * Pack message ExtendedTypes.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int multiplex_2_extended_types_pack(
    uint8_t *dst_p,
    const struct multiplex_2_extended_types_t *src_p,
    size_t size);

/**
 * Unpack message ExtendedTypes.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int multiplex_2_extended_types_unpack(
    struct multiplex_2_extended_types_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_2_extended_types_s11_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_types_s11_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_types_s11_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t multiplex_2_extended_types_s0_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_types_s0_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_types_s0_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
float multiplex_2_extended_types_s10_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_types_s10_decode(float value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_types_s10_is_in_range(float value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
float multiplex_2_extended_types_s9_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_2_extended_types_s9_decode(float value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_2_extended_types_s9_is_in_range(float value);


#ifdef __cplusplus
}
#endif

#endif
