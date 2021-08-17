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

#ifndef MULTIPLEX_H
#define MULTIPLEX_H

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
#define MULTIPLEX_MESSAGE1_FRAME_ID (0x123456u)

/* Frame lengths in bytes. */
#define MULTIPLEX_MESSAGE1_LENGTH (8u)

/* Extended or standard frame types. */
#define MULTIPLEX_MESSAGE1_IS_EXTENDED (1)

/* Frame cycle times in milliseconds. */
#define MULTIPLEX_MESSAGE1_CYCLE_TIME_MS (0u)

/* Signal choices. */


/**
 * Signals in message Message1.
 *
 * All signal values are as on the CAN bus.
 */
struct multiplex_message1_t {
    /**
     * Defines data content for response messages.
     *
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t multiplexor;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_j;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_c;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_g;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_l;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_a;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_k;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_e;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_d;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_b;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_h;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint8_t bit_f;
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
int multiplex_message1_pack(
    uint8_t *dst_p,
    const struct multiplex_message1_t *src_p,
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
int multiplex_message1_unpack(
    struct multiplex_message1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_multiplexor_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_multiplexor_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_multiplexor_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_multiplexor_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_j_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_j_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_j_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_j_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_c_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_c_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_c_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_c_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_g_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_g_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_g_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_g_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_l_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_l_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_l_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_l_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_a_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_a_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_a_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_a_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_k_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_k_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_k_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_k_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_e_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_e_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_e_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_e_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_d_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_d_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_d_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_d_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_b_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_b_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_b_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_b_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_h_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_h_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_h_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_h_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t multiplex_message1_bit_f_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double multiplex_message1_bit_f_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double multiplex_message1_bit_f_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool multiplex_message1_bit_f_is_in_range(uint8_t value);

/**
 * Create message Message1 if range check ok.
 * @param[out] outbuf:    buffer to write message into
 * @param[in]  outbuf_sz: size of outbuf
 *
 * @returns zero (success),
 *          -1   (problem packing, likely buffer too small)
 *          n>0  (nth value out of range)
 */
int multiplex_message1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double multiplexor,
    double bit_j,
    double bit_c,
    double bit_g,
    double bit_l,
    double bit_a,
    double bit_k,
    double bit_e,
    double bit_d,
    double bit_b,
    double bit_h,
    double bit_f);

/**
 * unpack message Message1 and check for allowable ranges
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
int multiplex_message1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *multiplexor,
    double *bit_j,
    double *bit_c,
    double *bit_g,
    double *bit_l,
    double *bit_a,
    double *bit_k,
    double *bit_e,
    double *bit_d,
    double *bit_b,
    double *bit_h,
    double *bit_f);


#ifdef __cplusplus
}
#endif

#endif
