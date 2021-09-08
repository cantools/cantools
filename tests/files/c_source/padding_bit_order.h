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

#ifndef PADDING_BIT_ORDER_H
#define PADDING_BIT_ORDER_H

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
#define PADDING_BIT_ORDER_MSG0_FRAME_ID (0x01u)
#define PADDING_BIT_ORDER_MSG1_FRAME_ID (0x02u)
#define PADDING_BIT_ORDER_MSG2_FRAME_ID (0x03u)
#define PADDING_BIT_ORDER_MSG3_FRAME_ID (0x04u)
#define PADDING_BIT_ORDER_MSG4_FRAME_ID (0x05u)

/* Frame lengths in bytes. */
#define PADDING_BIT_ORDER_MSG0_LENGTH (8u)
#define PADDING_BIT_ORDER_MSG1_LENGTH (8u)
#define PADDING_BIT_ORDER_MSG2_LENGTH (8u)
#define PADDING_BIT_ORDER_MSG3_LENGTH (8u)
#define PADDING_BIT_ORDER_MSG4_LENGTH (8u)

/* Extended or standard frame types. */
#define PADDING_BIT_ORDER_MSG0_IS_EXTENDED (0)
#define PADDING_BIT_ORDER_MSG1_IS_EXTENDED (0)
#define PADDING_BIT_ORDER_MSG2_IS_EXTENDED (0)
#define PADDING_BIT_ORDER_MSG3_IS_EXTENDED (0)
#define PADDING_BIT_ORDER_MSG4_IS_EXTENDED (0)
/* return whether a certain frame uses an extended id */
bool is_extended_frame(uint32_t frame_id);

/* Frame cycle times in milliseconds. */


/* Signal choices. */


/**
 * Signals in message MSG0.
 *
 * All signal values are as on the CAN bus.
 */
struct padding_bit_order_msg0_t {
    /**
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t b;

    /**
     * Range: 0..32767 (0..32767 -)
     * Scale: 1
     * Offset: 0
     */
    uint16_t a;

    /**
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t d;

    /**
     * Range: 0..32767 (0..32767 -)
     * Scale: 1
     * Offset: 0
     */
    uint16_t c;
};

/**
 * Signals in message MSG1.
 *
 * All signal values are as on the CAN bus.
 */
struct padding_bit_order_msg1_t {
    /**
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t e;

    /**
     * Range: 0..32767 (0..32767 -)
     * Scale: 1
     * Offset: 0
     */
    uint16_t f;

    /**
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t g;

    /**
     * Range: 0..32767 (0..32767 -)
     * Scale: 1
     * Offset: 0
     */
    uint16_t h;
};

/**
 * Signals in message MSG2.
 *
 * All signal values are as on the CAN bus.
 */
struct padding_bit_order_msg2_t {
    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t i;

    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t j;

    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t k;
};

/**
 * Signals in message MSG3.
 *
 * All signal values are as on the CAN bus.
 */
struct padding_bit_order_msg3_t {
    /**
     * Range: 0..18446744073709551615 (0..18446744073709551615 -)
     * Scale: 1
     * Offset: 0
     */
    uint64_t l;
};

/**
 * Signals in message MSG4.
 *
 * All signal values are as on the CAN bus.
 */
struct padding_bit_order_msg4_t {
    /**
     * Range: 0..18446744073709551615 (0..18446744073709551615 -)
     * Scale: 1
     * Offset: 0
     */
    uint64_t m;
};

/**
 * Pack message MSG0.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int padding_bit_order_msg0_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg0_t *src_p,
    size_t size);

/**
 * Unpack message MSG0.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int padding_bit_order_msg0_unpack(
    struct padding_bit_order_msg0_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t padding_bit_order_msg0_b_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg0_b_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg0_b_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg0_b_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t padding_bit_order_msg0_a_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg0_a_decode(uint16_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg0_a_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg0_a_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t padding_bit_order_msg0_d_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg0_d_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg0_d_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg0_d_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t padding_bit_order_msg0_c_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg0_c_decode(uint16_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg0_c_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg0_c_is_in_range(uint16_t value);

/**
 * Create message MSG0 if range check ok.
 * @param[out] outbuf:    buffer to write message into
 * @param[in]  outbuf_sz: size of outbuf
 *
 * @returns zero (success),
 *          -1   (problem packing, likely buffer too small)
 *          n>0  (nth value out of range)
 */
int padding_bit_order_msg0_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double b,
    double a,
    double d,
    double c);

/**
 * unpack message MSG0 and check for allowable ranges
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
int padding_bit_order_msg0_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *b,
    double *a,
    double *d,
    double *c);

/**
 * Pack message MSG1.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int padding_bit_order_msg1_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg1_t *src_p,
    size_t size);

/**
 * Unpack message MSG1.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int padding_bit_order_msg1_unpack(
    struct padding_bit_order_msg1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t padding_bit_order_msg1_e_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg1_e_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg1_e_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg1_e_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t padding_bit_order_msg1_f_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg1_f_decode(uint16_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg1_f_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg1_f_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t padding_bit_order_msg1_g_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg1_g_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg1_g_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg1_g_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t padding_bit_order_msg1_h_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg1_h_decode(uint16_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg1_h_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg1_h_is_in_range(uint16_t value);

/**
 * Create message MSG1 if range check ok.
 * @param[out] outbuf:    buffer to write message into
 * @param[in]  outbuf_sz: size of outbuf
 *
 * @returns zero (success),
 *          -1   (problem packing, likely buffer too small)
 *          n>0  (nth value out of range)
 */
int padding_bit_order_msg1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double e,
    double f,
    double g,
    double h);

/**
 * unpack message MSG1 and check for allowable ranges
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
int padding_bit_order_msg1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *e,
    double *f,
    double *g,
    double *h);

/**
 * Pack message MSG2.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int padding_bit_order_msg2_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg2_t *src_p,
    size_t size);

/**
 * Unpack message MSG2.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int padding_bit_order_msg2_unpack(
    struct padding_bit_order_msg2_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t padding_bit_order_msg2_i_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg2_i_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg2_i_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg2_i_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t padding_bit_order_msg2_j_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg2_j_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg2_j_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg2_j_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t padding_bit_order_msg2_k_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg2_k_decode(uint8_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg2_k_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg2_k_is_in_range(uint8_t value);

/**
 * Create message MSG2 if range check ok.
 * @param[out] outbuf:    buffer to write message into
 * @param[in]  outbuf_sz: size of outbuf
 *
 * @returns zero (success),
 *          -1   (problem packing, likely buffer too small)
 *          n>0  (nth value out of range)
 */
int padding_bit_order_msg2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double i,
    double j,
    double k);

/**
 * unpack message MSG2 and check for allowable ranges
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
int padding_bit_order_msg2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *i,
    double *j,
    double *k);

/**
 * Pack message MSG3.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int padding_bit_order_msg3_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg3_t *src_p,
    size_t size);

/**
 * Unpack message MSG3.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int padding_bit_order_msg3_unpack(
    struct padding_bit_order_msg3_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint64_t padding_bit_order_msg3_l_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg3_l_decode(uint64_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg3_l_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg3_l_is_in_range(uint64_t value);

/**
 * Create message MSG3 if range check ok.
 * @param[out] outbuf:    buffer to write message into
 * @param[in]  outbuf_sz: size of outbuf
 *
 * @returns zero (success),
 *          -1   (problem packing, likely buffer too small)
 *          n>0  (nth value out of range)
 */
int padding_bit_order_msg3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double l);

/**
 * unpack message MSG3 and check for allowable ranges
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
int padding_bit_order_msg3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *l);

/**
 * Pack message MSG4.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int padding_bit_order_msg4_pack(
    uint8_t *dst_p,
    const struct padding_bit_order_msg4_t *src_p,
    size_t size);

/**
 * Unpack message MSG4.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int padding_bit_order_msg4_unpack(
    struct padding_bit_order_msg4_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint64_t padding_bit_order_msg4_m_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double padding_bit_order_msg4_m_decode(uint64_t value);

/**
 * clamp signal to allowed range.
 * @param[in] val: requested value
 * @returns   clamped value
 */
double padding_bit_order_msg4_m_clamp(double val);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool padding_bit_order_msg4_m_is_in_range(uint64_t value);

/**
 * Create message MSG4 if range check ok.
 * @param[out] outbuf:    buffer to write message into
 * @param[in]  outbuf_sz: size of outbuf
 *
 * @returns zero (success),
 *          -1   (problem packing, likely buffer too small)
 *          n>0  (nth value out of range)
 */
int padding_bit_order_msg4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double m);

/**
 * unpack message MSG4 and check for allowable ranges
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
int padding_bit_order_msg4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *m);


#ifdef __cplusplus
}
#endif

#endif
