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

#ifndef SIGNED_H
#define SIGNED_H

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
#define SIGNED_MESSAGE378910_FRAME_ID (0x0au)
#define SIGNED_MESSAGE63BIG_1_FRAME_ID (0x09u)
#define SIGNED_MESSAGE63_1_FRAME_ID (0x08u)
#define SIGNED_MESSAGE63BIG_FRAME_ID (0x07u)
#define SIGNED_MESSAGE63_FRAME_ID (0x06u)
#define SIGNED_MESSAGE32BIG_FRAME_ID (0x05u)
#define SIGNED_MESSAGE33BIG_FRAME_ID (0x04u)
#define SIGNED_MESSAGE64BIG_FRAME_ID (0x03u)
#define SIGNED_MESSAGE64_FRAME_ID (0x02u)
#define SIGNED_MESSAGE33_FRAME_ID (0x01u)
#define SIGNED_MESSAGE32_FRAME_ID (0x00u)

/* Frame lengths in bytes. */
#define SIGNED_MESSAGE378910_LENGTH (8u)
#define SIGNED_MESSAGE63BIG_1_LENGTH (8u)
#define SIGNED_MESSAGE63_1_LENGTH (8u)
#define SIGNED_MESSAGE63BIG_LENGTH (8u)
#define SIGNED_MESSAGE63_LENGTH (8u)
#define SIGNED_MESSAGE32BIG_LENGTH (8u)
#define SIGNED_MESSAGE33BIG_LENGTH (8u)
#define SIGNED_MESSAGE64BIG_LENGTH (8u)
#define SIGNED_MESSAGE64_LENGTH (8u)
#define SIGNED_MESSAGE33_LENGTH (8u)
#define SIGNED_MESSAGE32_LENGTH (8u)

/* Extended or standard frame types. */
#define SIGNED_MESSAGE378910_IS_EXTENDED (0)
#define SIGNED_MESSAGE63BIG_1_IS_EXTENDED (0)
#define SIGNED_MESSAGE63_1_IS_EXTENDED (0)
#define SIGNED_MESSAGE63BIG_IS_EXTENDED (0)
#define SIGNED_MESSAGE63_IS_EXTENDED (0)
#define SIGNED_MESSAGE32BIG_IS_EXTENDED (0)
#define SIGNED_MESSAGE33BIG_IS_EXTENDED (0)
#define SIGNED_MESSAGE64BIG_IS_EXTENDED (0)
#define SIGNED_MESSAGE64_IS_EXTENDED (0)
#define SIGNED_MESSAGE33_IS_EXTENDED (0)
#define SIGNED_MESSAGE32_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */


/* Signal choices. */


/**
 * Signals in message Message378910.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message378910_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s7;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s8big;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int16_t s9;

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
    int8_t s3big;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s3;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int16_t s10big;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int8_t s7big;
};

/**
 * Signals in message Message63big_1.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message63big_1_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int64_t s63big;
};

/**
 * Signals in message Message63_1.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message63_1_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int64_t s63;
};

/**
 * Signals in message Message63big.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message63big_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int64_t s63big;
};

/**
 * Signals in message Message63.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message63_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int64_t s63;
};

/**
 * Signals in message Message32big.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message32big_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int32_t s32big;
};

/**
 * Signals in message Message33big.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message33big_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int64_t s33big;
};

/**
 * Signals in message Message64big.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message64big_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int64_t s64big;
};

/**
 * Signals in message Message64.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message64_t {
    /**
     * Range: -9223372036854780000..9223372036854780000 (-9.22337203685478E+18..9.22337203685478E+18 -)
     * Scale: 1
     * Offset: 0
     */
    int64_t s64;
};

/**
 * Signals in message Message33.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message33_t {
    /**
     * Range: -4294967296..4294967295 (-4294967296..4294967295 -)
     * Scale: 1
     * Offset: 0
     */
    int64_t s33;
};

/**
 * Signals in message Message32.
 *
 * All signal values are as on the CAN bus.
 */
struct signed_message32_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    int32_t s32;
};

/**
 * Pack message Message378910.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message378910_pack(
    uint8_t *dst_p,
    const struct signed_message378910_t *src_p,
    size_t size);

/**
 * Unpack message Message378910.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message378910_unpack(
    struct signed_message378910_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t signed_message378910_s7_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s7_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s7_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t signed_message378910_s8big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s8big_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s8big_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t signed_message378910_s9_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s9_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s9_is_in_range(int16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t signed_message378910_s8_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s8_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s8_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t signed_message378910_s3big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s3big_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s3big_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t signed_message378910_s3_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s3_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s3_is_in_range(int8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t signed_message378910_s10big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s10big_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s10big_is_in_range(int16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int8_t signed_message378910_s7big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message378910_s7big_decode(int8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message378910_s7big_is_in_range(int8_t value);

/**
 * Pack message Message63big_1.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message63big_1_pack(
    uint8_t *dst_p,
    const struct signed_message63big_1_t *src_p,
    size_t size);

/**
 * Unpack message Message63big_1.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message63big_1_unpack(
    struct signed_message63big_1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message63big_1_s63big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message63big_1_s63big_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message63big_1_s63big_is_in_range(int64_t value);

/**
 * Pack message Message63_1.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message63_1_pack(
    uint8_t *dst_p,
    const struct signed_message63_1_t *src_p,
    size_t size);

/**
 * Unpack message Message63_1.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message63_1_unpack(
    struct signed_message63_1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message63_1_s63_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message63_1_s63_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message63_1_s63_is_in_range(int64_t value);

/**
 * Pack message Message63big.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message63big_pack(
    uint8_t *dst_p,
    const struct signed_message63big_t *src_p,
    size_t size);

/**
 * Unpack message Message63big.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message63big_unpack(
    struct signed_message63big_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message63big_s63big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message63big_s63big_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message63big_s63big_is_in_range(int64_t value);

/**
 * Pack message Message63.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message63_pack(
    uint8_t *dst_p,
    const struct signed_message63_t *src_p,
    size_t size);

/**
 * Unpack message Message63.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message63_unpack(
    struct signed_message63_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message63_s63_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message63_s63_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message63_s63_is_in_range(int64_t value);

/**
 * Pack message Message32big.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message32big_pack(
    uint8_t *dst_p,
    const struct signed_message32big_t *src_p,
    size_t size);

/**
 * Unpack message Message32big.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message32big_unpack(
    struct signed_message32big_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int32_t signed_message32big_s32big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message32big_s32big_decode(int32_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message32big_s32big_is_in_range(int32_t value);

/**
 * Pack message Message33big.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message33big_pack(
    uint8_t *dst_p,
    const struct signed_message33big_t *src_p,
    size_t size);

/**
 * Unpack message Message33big.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message33big_unpack(
    struct signed_message33big_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message33big_s33big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message33big_s33big_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message33big_s33big_is_in_range(int64_t value);

/**
 * Pack message Message64big.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message64big_pack(
    uint8_t *dst_p,
    const struct signed_message64big_t *src_p,
    size_t size);

/**
 * Unpack message Message64big.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message64big_unpack(
    struct signed_message64big_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message64big_s64big_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message64big_s64big_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message64big_s64big_is_in_range(int64_t value);

/**
 * Pack message Message64.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message64_pack(
    uint8_t *dst_p,
    const struct signed_message64_t *src_p,
    size_t size);

/**
 * Unpack message Message64.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message64_unpack(
    struct signed_message64_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message64_s64_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message64_s64_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message64_s64_is_in_range(int64_t value);

/**
 * Pack message Message33.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message33_pack(
    uint8_t *dst_p,
    const struct signed_message33_t *src_p,
    size_t size);

/**
 * Unpack message Message33.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message33_unpack(
    struct signed_message33_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int64_t signed_message33_s33_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message33_s33_decode(int64_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message33_s33_is_in_range(int64_t value);

/**
 * Pack message Message32.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int signed_message32_pack(
    uint8_t *dst_p,
    const struct signed_message32_t *src_p,
    size_t size);

/**
 * Unpack message Message32.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int signed_message32_unpack(
    struct signed_message32_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int32_t signed_message32_s32_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double signed_message32_s32_decode(int32_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool signed_message32_s32_is_in_range(int32_t value);


#ifdef __cplusplus
}
#endif

#endif
