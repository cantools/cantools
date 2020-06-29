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

#ifndef MOTOHAWK_H
#define MOTOHAWK_H

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
#define MOTOHAWK_EXAMPLE_MESSAGE_FRAME_ID (0x1f0u)

/* Frame lengths in bytes. */
#define MOTOHAWK_EXAMPLE_MESSAGE_LENGTH (8u)

/* Extended or standard frame types. */
#define MOTOHAWK_EXAMPLE_MESSAGE_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */


/* Signal choices. */
#define MOTOHAWK_EXAMPLE_MESSAGE_ENABLE_DISABLED_CHOICE (0u)
#define MOTOHAWK_EXAMPLE_MESSAGE_ENABLE_ENABLED_CHOICE (1u)

/**
 * Signals in message ExampleMessage.
 *
 * Example message used as template in MotoHawk models.
 *
 * All signal values are as on the CAN bus.
 */
struct motohawk_example_message_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t enable;

    /**
     * Range: 0..50 (0..5 m)
     * Scale: 0.1
     * Offset: 0
     */
    uint8_t average_radius;

    /**
     * Range: -2048..2047 (229.52..270.47 degK)
     * Scale: 0.01
     * Offset: 250
     */
    int16_t temperature;
};

/**
 * Pack message ExampleMessage.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int motohawk_example_message_pack(
    uint8_t *dst_p,
    const struct motohawk_example_message_t *src_p,
    size_t size);

/**
 * Unpack message ExampleMessage.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int motohawk_example_message_unpack(
    struct motohawk_example_message_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool motohawk_example_message_enable_is_in_range(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool motohawk_example_message_average_radius_is_in_range(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool motohawk_example_message_temperature_is_in_range(int16_t value);


#ifdef __cplusplus
}
#endif

#endif
