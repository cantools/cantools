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

#ifndef FLOATING_POINT_USE_FLOAT_H
#define FLOATING_POINT_USE_FLOAT_H

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
#define FLOATING_POINT_USE_FLOAT_MESSAGE1_FRAME_ID (0x400u)
#define FLOATING_POINT_USE_FLOAT_MESSAGE2_FRAME_ID (0x401u)
#define FLOATING_POINT_USE_FLOAT_MESSAGE3_FRAME_ID (0x402u)

/* Frame lengths in bytes. */
#define FLOATING_POINT_USE_FLOAT_MESSAGE1_LENGTH (8u)
#define FLOATING_POINT_USE_FLOAT_MESSAGE2_LENGTH (8u)
#define FLOATING_POINT_USE_FLOAT_MESSAGE3_LENGTH (8u)

/* Extended or standard frame types. */
#define FLOATING_POINT_USE_FLOAT_MESSAGE1_IS_EXTENDED (0)
#define FLOATING_POINT_USE_FLOAT_MESSAGE2_IS_EXTENDED (0)
#define FLOATING_POINT_USE_FLOAT_MESSAGE3_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */


/* Signal choices. */


/* Frame Names. */
#define FLOATING_POINT_USE_FLOAT_MESSAGE1_NAME "Message1"
#define FLOATING_POINT_USE_FLOAT_MESSAGE2_NAME "Message2"
#define FLOATING_POINT_USE_FLOAT_MESSAGE3_NAME "Message3"

/* Signal Names. */
#define FLOATING_POINT_USE_FLOAT_MESSAGE1_SIGNAL1_NAME "Signal1"
#define FLOATING_POINT_USE_FLOAT_MESSAGE2_SIGNAL1_NAME "Signal1"
#define FLOATING_POINT_USE_FLOAT_MESSAGE2_SIGNAL2_NAME "Signal2"
#define FLOATING_POINT_USE_FLOAT_MESSAGE3_SIGNAL3_NAME "Signal3"

/**
 * Signals in message Message1.
 *
 * All signal values are as on the CAN bus.
 */
struct floating_point_use_float_message1_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    double signal1;
};

/**
 * Signals in message Message2.
 *
 * All signal values are as on the CAN bus.
 */
struct floating_point_use_float_message2_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    float signal1;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    float signal2;
};

/**
 * Signals in message Message3.
 *
 * All signal values are as on the CAN bus.
 */
struct floating_point_use_float_message3_t {
    /**
     * Range: -
     * Scale: 0.001
     * Offset: -0.125
     */
    uint8_t signal3;
};

/**
 * Pack message Message1.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int floating_point_use_float_message1_pack(
    uint8_t *dst_p,
    const struct floating_point_use_float_message1_t *src_p,
    size_t size);

/**
 * Unpack message Message1.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int floating_point_use_float_message1_unpack(
    struct floating_point_use_float_message1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from Message1.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int floating_point_use_float_message1_init(struct floating_point_use_float_message1_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
double floating_point_use_float_message1_signal1_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float floating_point_use_float_message1_signal1_decode(double value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool floating_point_use_float_message1_signal1_is_in_range(double value);

/**
 * Pack message Message2.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int floating_point_use_float_message2_pack(
    uint8_t *dst_p,
    const struct floating_point_use_float_message2_t *src_p,
    size_t size);

/**
 * Unpack message Message2.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int floating_point_use_float_message2_unpack(
    struct floating_point_use_float_message2_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from Message2.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int floating_point_use_float_message2_init(struct floating_point_use_float_message2_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
float floating_point_use_float_message2_signal1_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float floating_point_use_float_message2_signal1_decode(float value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool floating_point_use_float_message2_signal1_is_in_range(float value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
float floating_point_use_float_message2_signal2_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float floating_point_use_float_message2_signal2_decode(float value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool floating_point_use_float_message2_signal2_is_in_range(float value);

/**
 * Pack message Message3.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int floating_point_use_float_message3_pack(
    uint8_t *dst_p,
    const struct floating_point_use_float_message3_t *src_p,
    size_t size);

/**
 * Unpack message Message3.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int floating_point_use_float_message3_unpack(
    struct floating_point_use_float_message3_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from Message3.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int floating_point_use_float_message3_init(struct floating_point_use_float_message3_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t floating_point_use_float_message3_signal3_encode(float value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
float floating_point_use_float_message3_signal3_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool floating_point_use_float_message3_signal3_is_in_range(uint8_t value);


#ifdef __cplusplus
}
#endif

#endif
