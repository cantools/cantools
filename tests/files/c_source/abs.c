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

#include <string.h>

#include "abs.h"

#define CTOOLS_MAX(x,y) (((x) < (y)) ? (y) : (x))
#define CTOOLS_MIN(x,y) (((x) < (y)) ? (x) : (y))

static inline uint8_t pack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_left_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
}

static inline uint8_t pack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t pack_right_shift_u32(
    uint32_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
}

static inline uint8_t unpack_left_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) << shift);
}

static inline uint16_t unpack_left_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) << shift);
}

static inline uint32_t unpack_left_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) << shift);
}

static inline uint8_t unpack_right_shift_u8(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value & mask) >> shift);
}

static inline uint16_t unpack_right_shift_u16(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint16_t)((uint16_t)(value & mask) >> shift);
}

static inline uint32_t unpack_right_shift_u32(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint32_t)((uint32_t)(value & mask) >> shift);
}

int abs_bremse_33_pack(
    uint8_t *dst_p,
    const struct abs_bremse_33_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->whlspeed_fl, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->whlspeed_fl, 8u, 0xffu);
    dst_p[2] |= pack_left_shift_u16(src_p->whlspeed_fr, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->whlspeed_fr, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(src_p->whlspeed_rl, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->whlspeed_rl, 8u, 0xffu);
    dst_p[6] |= pack_left_shift_u16(src_p->whlspeed_rr, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(src_p->whlspeed_rr, 8u, 0xffu);

    return (8);
}

int abs_bremse_33_unpack(
    struct abs_bremse_33_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->whlspeed_fl = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->whlspeed_fl |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->whlspeed_fr = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->whlspeed_fr |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->whlspeed_rl = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->whlspeed_rl |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->whlspeed_rr = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    dst_p->whlspeed_rr |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);

    return (0);
}

static int abs_bremse_33_check_ranges(struct abs_bremse_33_t *msg)
{
    int idx = 1;

    if (!abs_bremse_33_whlspeed_fl_is_in_range(msg->whlspeed_fl))
        return idx;

    idx++;

    if (!abs_bremse_33_whlspeed_fr_is_in_range(msg->whlspeed_fr))
        return idx;

    idx++;

    if (!abs_bremse_33_whlspeed_rl_is_in_range(msg->whlspeed_rl))
        return idx;

    idx++;

    if (!abs_bremse_33_whlspeed_rr_is_in_range(msg->whlspeed_rr))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_33_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double whlspeed_fl,
    double whlspeed_fr,
    double whlspeed_rl,
    double whlspeed_rr)
{
    struct abs_bremse_33_t msg;

    msg.whlspeed_fl = abs_bremse_33_whlspeed_fl_encode(whlspeed_fl);
    msg.whlspeed_fr = abs_bremse_33_whlspeed_fr_encode(whlspeed_fr);
    msg.whlspeed_rl = abs_bremse_33_whlspeed_rl_encode(whlspeed_rl);
    msg.whlspeed_rr = abs_bremse_33_whlspeed_rr_encode(whlspeed_rr);

    int ret = abs_bremse_33_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_33_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_33_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *whlspeed_fl,
    double *whlspeed_fr,
    double *whlspeed_rl,
    double *whlspeed_rr)
{
    struct abs_bremse_33_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_33_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_33_check_ranges(&msg);

    if (whlspeed_fl)
        *whlspeed_fl = abs_bremse_33_whlspeed_fl_decode(msg.whlspeed_fl);

    if (whlspeed_fr)
        *whlspeed_fr = abs_bremse_33_whlspeed_fr_decode(msg.whlspeed_fr);

    if (whlspeed_rl)
        *whlspeed_rl = abs_bremse_33_whlspeed_rl_decode(msg.whlspeed_rl);

    if (whlspeed_rr)
        *whlspeed_rr = abs_bremse_33_whlspeed_rr_decode(msg.whlspeed_rr);

    return ret;
}

uint16_t abs_bremse_33_whlspeed_fl_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_33_whlspeed_fl_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_33_whlspeed_fl_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_33_whlspeed_fl_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

uint16_t abs_bremse_33_whlspeed_fr_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_33_whlspeed_fr_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_33_whlspeed_fr_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_33_whlspeed_fr_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

uint16_t abs_bremse_33_whlspeed_rl_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_33_whlspeed_rl_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_33_whlspeed_rl_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_33_whlspeed_rl_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

uint16_t abs_bremse_33_whlspeed_rr_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_33_whlspeed_rr_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_33_whlspeed_rr_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_33_whlspeed_rr_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

int abs_bremse_10_pack(
    uint8_t *dst_p,
    const struct abs_bremse_10_t *src_p,
    size_t size)
{
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    return (8);
}

int abs_bremse_10_unpack(
    struct abs_bremse_10_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    return (0);
}

static int abs_bremse_10_check_ranges(struct abs_bremse_10_t *msg)
{
    int idx = 1;
    (void)msg;
    (void)idx;

    return 0;
}

int abs_bremse_10_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz)
{
    struct abs_bremse_10_t msg;


    int ret = abs_bremse_10_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_10_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_10_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz)
{
    struct abs_bremse_10_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_10_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_10_check_ranges(&msg);

    return ret;
}

int abs_bremse_11_pack(
    uint8_t *dst_p,
    const struct abs_bremse_11_t *src_p,
    size_t size)
{
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    return (8);
}

int abs_bremse_11_unpack(
    struct abs_bremse_11_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    return (0);
}

static int abs_bremse_11_check_ranges(struct abs_bremse_11_t *msg)
{
    int idx = 1;
    (void)msg;
    (void)idx;

    return 0;
}

int abs_bremse_11_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz)
{
    struct abs_bremse_11_t msg;


    int ret = abs_bremse_11_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_11_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_11_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz)
{
    struct abs_bremse_11_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_11_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_11_check_ranges(&msg);

    return ret;
}

int abs_bremse_12_pack(
    uint8_t *dst_p,
    const struct abs_bremse_12_t *src_p,
    size_t size)
{
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    return (8);
}

int abs_bremse_12_unpack(
    struct abs_bremse_12_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    return (0);
}

static int abs_bremse_12_check_ranges(struct abs_bremse_12_t *msg)
{
    int idx = 1;
    (void)msg;
    (void)idx;

    return 0;
}

int abs_bremse_12_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz)
{
    struct abs_bremse_12_t msg;


    int ret = abs_bremse_12_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_12_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_12_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz)
{
    struct abs_bremse_12_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_12_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_12_check_ranges(&msg);

    return ret;
}

int abs_bremse_13_pack(
    uint8_t *dst_p,
    const struct abs_bremse_13_t *src_p,
    size_t size)
{
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    return (8);
}

int abs_bremse_13_unpack(
    struct abs_bremse_13_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    return (0);
}

static int abs_bremse_13_check_ranges(struct abs_bremse_13_t *msg)
{
    int idx = 1;
    (void)msg;
    (void)idx;

    return 0;
}

int abs_bremse_13_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz)
{
    struct abs_bremse_13_t msg;


    int ret = abs_bremse_13_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_13_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_13_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz)
{
    struct abs_bremse_13_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_13_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_13_check_ranges(&msg);

    return ret;
}

int abs_drs_rx_id0_pack(
    uint8_t *dst_p,
    const struct abs_drs_rx_id0_t *src_p,
    size_t size)
{
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    return (8);
}

int abs_drs_rx_id0_unpack(
    struct abs_drs_rx_id0_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    return (0);
}

static int abs_drs_rx_id0_check_ranges(struct abs_drs_rx_id0_t *msg)
{
    int idx = 1;
    (void)msg;
    (void)idx;

    return 0;
}

int abs_drs_rx_id0_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz)
{
    struct abs_drs_rx_id0_t msg;


    int ret = abs_drs_rx_id0_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_drs_rx_id0_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_drs_rx_id0_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz)
{
    struct abs_drs_rx_id0_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_drs_rx_id0_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_drs_rx_id0_check_ranges(&msg);

    return ret;
}

int abs_mm5_10_tx1_pack(
    uint8_t *dst_p,
    const struct abs_mm5_10_tx1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->yaw_rate, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->yaw_rate, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(src_p->ay1, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->ay1, 8u, 0xffu);

    return (8);
}

int abs_mm5_10_tx1_unpack(
    struct abs_mm5_10_tx1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->yaw_rate = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->yaw_rate |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ay1 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->ay1 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);

    return (0);
}

static int abs_mm5_10_tx1_check_ranges(struct abs_mm5_10_tx1_t *msg)
{
    int idx = 1;

    if (!abs_mm5_10_tx1_yaw_rate_is_in_range(msg->yaw_rate))
        return idx;

    idx++;

    if (!abs_mm5_10_tx1_ay1_is_in_range(msg->ay1))
        return idx;

    idx++;

    return 0;
}

int abs_mm5_10_tx1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double yaw_rate,
    double ay1)
{
    struct abs_mm5_10_tx1_t msg;

    msg.yaw_rate = abs_mm5_10_tx1_yaw_rate_encode(yaw_rate);
    msg.ay1 = abs_mm5_10_tx1_ay1_encode(ay1);

    int ret = abs_mm5_10_tx1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_mm5_10_tx1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_mm5_10_tx1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *yaw_rate,
    double *ay1)
{
    struct abs_mm5_10_tx1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_mm5_10_tx1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_mm5_10_tx1_check_ranges(&msg);

    if (yaw_rate)
        *yaw_rate = abs_mm5_10_tx1_yaw_rate_decode(msg.yaw_rate);

    if (ay1)
        *ay1 = abs_mm5_10_tx1_ay1_decode(msg.ay1);

    return ret;
}

uint16_t abs_mm5_10_tx1_yaw_rate_encode(double value)
{
    return (uint16_t)((value - -163.84) / 0.005);
}

double abs_mm5_10_tx1_yaw_rate_decode(uint16_t value)
{
    return (((double)value * 0.005) + -163.84);
}

double abs_mm5_10_tx1_yaw_rate_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -163.84);
    ret = CTOOLS_MIN(ret, 163.83);
    return ret;
}

bool abs_mm5_10_tx1_yaw_rate_is_in_range(uint16_t value)
{
    return (value <= 65534u);
}

uint16_t abs_mm5_10_tx1_ay1_encode(double value)
{
    return (uint16_t)((value - -4.1768) / 0.000127465);
}

double abs_mm5_10_tx1_ay1_decode(uint16_t value)
{
    return (((double)value * 0.000127465) + -4.1768);
}

double abs_mm5_10_tx1_ay1_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -4.1768);
    ret = CTOOLS_MIN(ret, 4.1765);
    return ret;
}

bool abs_mm5_10_tx1_ay1_is_in_range(uint16_t value)
{
    return (value <= 65534u);
}

int abs_mm5_10_tx2_pack(
    uint8_t *dst_p,
    const struct abs_mm5_10_tx2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->roll_rate, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->roll_rate, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(src_p->ax1, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->ax1, 8u, 0xffu);

    return (8);
}

int abs_mm5_10_tx2_unpack(
    struct abs_mm5_10_tx2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->roll_rate = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->roll_rate |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ax1 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->ax1 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);

    return (0);
}

static int abs_mm5_10_tx2_check_ranges(struct abs_mm5_10_tx2_t *msg)
{
    int idx = 1;

    if (!abs_mm5_10_tx2_roll_rate_is_in_range(msg->roll_rate))
        return idx;

    idx++;

    if (!abs_mm5_10_tx2_ax1_is_in_range(msg->ax1))
        return idx;

    idx++;

    return 0;
}

int abs_mm5_10_tx2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double roll_rate,
    double ax1)
{
    struct abs_mm5_10_tx2_t msg;

    msg.roll_rate = abs_mm5_10_tx2_roll_rate_encode(roll_rate);
    msg.ax1 = abs_mm5_10_tx2_ax1_encode(ax1);

    int ret = abs_mm5_10_tx2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_mm5_10_tx2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_mm5_10_tx2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *roll_rate,
    double *ax1)
{
    struct abs_mm5_10_tx2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_mm5_10_tx2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_mm5_10_tx2_check_ranges(&msg);

    if (roll_rate)
        *roll_rate = abs_mm5_10_tx2_roll_rate_decode(msg.roll_rate);

    if (ax1)
        *ax1 = abs_mm5_10_tx2_ax1_decode(msg.ax1);

    return ret;
}

uint16_t abs_mm5_10_tx2_roll_rate_encode(double value)
{
    return (uint16_t)((value - -163.84) / 0.005);
}

double abs_mm5_10_tx2_roll_rate_decode(uint16_t value)
{
    return (((double)value * 0.005) + -163.84);
}

double abs_mm5_10_tx2_roll_rate_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -163.84);
    ret = CTOOLS_MIN(ret, 163.835);
    return ret;
}

bool abs_mm5_10_tx2_roll_rate_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

uint16_t abs_mm5_10_tx2_ax1_encode(double value)
{
    return (uint16_t)((value - -4.1768) / 0.000127465);
}

double abs_mm5_10_tx2_ax1_decode(uint16_t value)
{
    return (((double)value * 0.000127465) + -4.1768);
}

double abs_mm5_10_tx2_ax1_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -4.1768);
    ret = CTOOLS_MIN(ret, 4.1765);
    return ret;
}

bool abs_mm5_10_tx2_ax1_is_in_range(uint16_t value)
{
    return (value <= 65534u);
}

int abs_mm5_10_tx3_pack(
    uint8_t *dst_p,
    const struct abs_mm5_10_tx3_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[4] |= pack_left_shift_u16(src_p->az, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->az, 8u, 0xffu);

    return (8);
}

int abs_mm5_10_tx3_unpack(
    struct abs_mm5_10_tx3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->az = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->az |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);

    return (0);
}

static int abs_mm5_10_tx3_check_ranges(struct abs_mm5_10_tx3_t *msg)
{
    int idx = 1;

    if (!abs_mm5_10_tx3_az_is_in_range(msg->az))
        return idx;

    idx++;

    return 0;
}

int abs_mm5_10_tx3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double az)
{
    struct abs_mm5_10_tx3_t msg;

    msg.az = abs_mm5_10_tx3_az_encode(az);

    int ret = abs_mm5_10_tx3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_mm5_10_tx3_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_mm5_10_tx3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *az)
{
    struct abs_mm5_10_tx3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_mm5_10_tx3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_mm5_10_tx3_check_ranges(&msg);

    if (az)
        *az = abs_mm5_10_tx3_az_decode(msg.az);

    return ret;
}

uint16_t abs_mm5_10_tx3_az_encode(double value)
{
    return (uint16_t)((value - -4.1768) / 0.000127465);
}

double abs_mm5_10_tx3_az_decode(uint16_t value)
{
    return (((double)value * 0.000127465) + -4.1768);
}

double abs_mm5_10_tx3_az_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -4.1768);
    ret = CTOOLS_MIN(ret, 4.1765);
    return ret;
}

bool abs_mm5_10_tx3_az_is_in_range(uint16_t value)
{
    return (value <= 65534u);
}

int abs_bremse_2_pack(
    uint8_t *dst_p,
    const struct abs_bremse_2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->whlspeed_fl_bremse2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->whlspeed_fl_bremse2, 8u, 0xffu);
    dst_p[2] |= pack_left_shift_u16(src_p->whlspeed_fr_bremse2, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->whlspeed_fr_bremse2, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(src_p->whlspeed_rl_bremse2, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->whlspeed_rl_bremse2, 8u, 0xffu);
    dst_p[6] |= pack_left_shift_u16(src_p->whlspeed_rr_bremse2, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(src_p->whlspeed_rr_bremse2, 8u, 0xffu);

    return (8);
}

int abs_bremse_2_unpack(
    struct abs_bremse_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->whlspeed_fl_bremse2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->whlspeed_fl_bremse2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->whlspeed_fr_bremse2 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->whlspeed_fr_bremse2 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->whlspeed_rl_bremse2 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->whlspeed_rl_bremse2 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->whlspeed_rr_bremse2 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    dst_p->whlspeed_rr_bremse2 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);

    return (0);
}

static int abs_bremse_2_check_ranges(struct abs_bremse_2_t *msg)
{
    int idx = 1;

    if (!abs_bremse_2_whlspeed_fl_bremse2_is_in_range(msg->whlspeed_fl_bremse2))
        return idx;

    idx++;

    if (!abs_bremse_2_whlspeed_fr_bremse2_is_in_range(msg->whlspeed_fr_bremse2))
        return idx;

    idx++;

    if (!abs_bremse_2_whlspeed_rl_bremse2_is_in_range(msg->whlspeed_rl_bremse2))
        return idx;

    idx++;

    if (!abs_bremse_2_whlspeed_rr_bremse2_is_in_range(msg->whlspeed_rr_bremse2))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double whlspeed_fl_bremse2,
    double whlspeed_fr_bremse2,
    double whlspeed_rl_bremse2,
    double whlspeed_rr_bremse2)
{
    struct abs_bremse_2_t msg;

    msg.whlspeed_fl_bremse2 = abs_bremse_2_whlspeed_fl_bremse2_encode(whlspeed_fl_bremse2);
    msg.whlspeed_fr_bremse2 = abs_bremse_2_whlspeed_fr_bremse2_encode(whlspeed_fr_bremse2);
    msg.whlspeed_rl_bremse2 = abs_bremse_2_whlspeed_rl_bremse2_encode(whlspeed_rl_bremse2);
    msg.whlspeed_rr_bremse2 = abs_bremse_2_whlspeed_rr_bremse2_encode(whlspeed_rr_bremse2);

    int ret = abs_bremse_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *whlspeed_fl_bremse2,
    double *whlspeed_fr_bremse2,
    double *whlspeed_rl_bremse2,
    double *whlspeed_rr_bremse2)
{
    struct abs_bremse_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_2_check_ranges(&msg);

    if (whlspeed_fl_bremse2)
        *whlspeed_fl_bremse2 = abs_bremse_2_whlspeed_fl_bremse2_decode(msg.whlspeed_fl_bremse2);

    if (whlspeed_fr_bremse2)
        *whlspeed_fr_bremse2 = abs_bremse_2_whlspeed_fr_bremse2_decode(msg.whlspeed_fr_bremse2);

    if (whlspeed_rl_bremse2)
        *whlspeed_rl_bremse2 = abs_bremse_2_whlspeed_rl_bremse2_decode(msg.whlspeed_rl_bremse2);

    if (whlspeed_rr_bremse2)
        *whlspeed_rr_bremse2 = abs_bremse_2_whlspeed_rr_bremse2_decode(msg.whlspeed_rr_bremse2);

    return ret;
}

uint16_t abs_bremse_2_whlspeed_fl_bremse2_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_2_whlspeed_fl_bremse2_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_2_whlspeed_fl_bremse2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_2_whlspeed_fl_bremse2_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

uint16_t abs_bremse_2_whlspeed_fr_bremse2_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_2_whlspeed_fr_bremse2_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_2_whlspeed_fr_bremse2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_2_whlspeed_fr_bremse2_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

uint16_t abs_bremse_2_whlspeed_rl_bremse2_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_2_whlspeed_rl_bremse2_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_2_whlspeed_rl_bremse2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_2_whlspeed_rl_bremse2_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

uint16_t abs_bremse_2_whlspeed_rr_bremse2_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_2_whlspeed_rr_bremse2_decode(uint16_t value)
{
    return ((double)value * 0.015625);
}

double abs_bremse_2_whlspeed_rr_bremse2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_2_whlspeed_rr_bremse2_is_in_range(uint16_t value)
{
    return (value <= 6400u);
}

int abs_abs_switch_pack(
    uint8_t *dst_p,
    const struct abs_abs_switch_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->abs_switchposition, 0u, 0xffu);

    return (8);
}

int abs_abs_switch_unpack(
    struct abs_abs_switch_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->abs_switchposition = unpack_right_shift_u8(src_p[0], 0u, 0xffu);

    return (0);
}

static int abs_abs_switch_check_ranges(struct abs_abs_switch_t *msg)
{
    int idx = 1;

    if (!abs_abs_switch_abs_switchposition_is_in_range(msg->abs_switchposition))
        return idx;

    idx++;

    return 0;
}

int abs_abs_switch_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double abs_switchposition)
{
    struct abs_abs_switch_t msg;

    msg.abs_switchposition = abs_abs_switch_abs_switchposition_encode(abs_switchposition);

    int ret = abs_abs_switch_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_abs_switch_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_abs_switch_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *abs_switchposition)
{
    struct abs_abs_switch_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_abs_switch_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_abs_switch_check_ranges(&msg);

    if (abs_switchposition)
        *abs_switchposition = abs_abs_switch_abs_switchposition_decode(msg.abs_switchposition);

    return ret;
}

uint8_t abs_abs_switch_abs_switchposition_encode(double value)
{
    return (uint8_t)(value);
}

double abs_abs_switch_abs_switchposition_decode(uint8_t value)
{
    return ((double)value);
}

double abs_abs_switch_abs_switchposition_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 11.0);
    return ret;
}

bool abs_abs_switch_abs_switchposition_is_in_range(uint8_t value)
{
    return (value <= 11u);
}

int abs_bremse_30_pack(
    uint8_t *dst_p,
    const struct abs_bremse_30_t *src_p,
    size_t size)
{
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    return (8);
}

int abs_bremse_30_unpack(
    struct abs_bremse_30_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    (void)dst_p;
    (void)src_p;

    if (size < 8u) {
        return (-EINVAL);
    }

    return (0);
}

static int abs_bremse_30_check_ranges(struct abs_bremse_30_t *msg)
{
    int idx = 1;
    (void)msg;
    (void)idx;

    return 0;
}

int abs_bremse_30_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz)
{
    struct abs_bremse_30_t msg;


    int ret = abs_bremse_30_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_30_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_30_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz)
{
    struct abs_bremse_30_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_30_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_30_check_ranges(&msg);

    return ret;
}

int abs_bremse_31_pack(
    uint8_t *dst_p,
    const struct abs_bremse_31_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[2] |= pack_left_shift_u16(src_p->idle_time, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->idle_time, 8u, 0xffu);

    return (8);
}

int abs_bremse_31_unpack(
    struct abs_bremse_31_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->idle_time = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->idle_time |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);

    return (0);
}

static int abs_bremse_31_check_ranges(struct abs_bremse_31_t *msg)
{
    int idx = 1;

    if (!abs_bremse_31_idle_time_is_in_range(msg->idle_time))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_31_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double idle_time)
{
    struct abs_bremse_31_t msg;

    msg.idle_time = abs_bremse_31_idle_time_encode(idle_time);

    int ret = abs_bremse_31_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_31_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_31_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *idle_time)
{
    struct abs_bremse_31_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_31_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_31_check_ranges(&msg);

    if (idle_time)
        *idle_time = abs_bremse_31_idle_time_decode(msg.idle_time);

    return ret;
}

uint16_t abs_bremse_31_idle_time_encode(double value)
{
    return (uint16_t)(value);
}

double abs_bremse_31_idle_time_decode(uint16_t value)
{
    return ((double)value);
}

double abs_bremse_31_idle_time_clamp(double val)
{
    double ret = val;


    return ret;
}

bool abs_bremse_31_idle_time_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int abs_bremse_32_pack(
    uint8_t *dst_p,
    const struct abs_bremse_32_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->acc_fa, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->acc_ra, 0u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->wheel_quality_fl, 0u, 0xffu);
    dst_p[5] |= pack_left_shift_u8(src_p->wheel_quality_fr, 0u, 0xffu);
    dst_p[6] |= pack_left_shift_u8(src_p->wheel_quality_rl, 0u, 0xffu);
    dst_p[7] |= pack_left_shift_u8(src_p->wheel_quality_rr, 0u, 0xffu);

    return (8);
}

int abs_bremse_32_unpack(
    struct abs_bremse_32_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->acc_fa = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->acc_ra = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->wheel_quality_fl = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    dst_p->wheel_quality_fr = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
    dst_p->wheel_quality_rl = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
    dst_p->wheel_quality_rr = unpack_right_shift_u8(src_p[7], 0u, 0xffu);

    return (0);
}

static int abs_bremse_32_check_ranges(struct abs_bremse_32_t *msg)
{
    int idx = 1;

    if (!abs_bremse_32_acc_fa_is_in_range(msg->acc_fa))
        return idx;

    idx++;

    if (!abs_bremse_32_acc_ra_is_in_range(msg->acc_ra))
        return idx;

    idx++;

    if (!abs_bremse_32_wheel_quality_fl_is_in_range(msg->wheel_quality_fl))
        return idx;

    idx++;

    if (!abs_bremse_32_wheel_quality_fr_is_in_range(msg->wheel_quality_fr))
        return idx;

    idx++;

    if (!abs_bremse_32_wheel_quality_rl_is_in_range(msg->wheel_quality_rl))
        return idx;

    idx++;

    if (!abs_bremse_32_wheel_quality_rr_is_in_range(msg->wheel_quality_rr))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_32_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double acc_fa,
    double acc_ra,
    double wheel_quality_fl,
    double wheel_quality_fr,
    double wheel_quality_rl,
    double wheel_quality_rr)
{
    struct abs_bremse_32_t msg;

    msg.acc_fa = abs_bremse_32_acc_fa_encode(acc_fa);
    msg.acc_ra = abs_bremse_32_acc_ra_encode(acc_ra);
    msg.wheel_quality_fl = abs_bremse_32_wheel_quality_fl_encode(wheel_quality_fl);
    msg.wheel_quality_fr = abs_bremse_32_wheel_quality_fr_encode(wheel_quality_fr);
    msg.wheel_quality_rl = abs_bremse_32_wheel_quality_rl_encode(wheel_quality_rl);
    msg.wheel_quality_rr = abs_bremse_32_wheel_quality_rr_encode(wheel_quality_rr);

    int ret = abs_bremse_32_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_32_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_32_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *acc_fa,
    double *acc_ra,
    double *wheel_quality_fl,
    double *wheel_quality_fr,
    double *wheel_quality_rl,
    double *wheel_quality_rr)
{
    struct abs_bremse_32_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_32_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_32_check_ranges(&msg);

    if (acc_fa)
        *acc_fa = abs_bremse_32_acc_fa_decode(msg.acc_fa);

    if (acc_ra)
        *acc_ra = abs_bremse_32_acc_ra_decode(msg.acc_ra);

    if (wheel_quality_fl)
        *wheel_quality_fl = abs_bremse_32_wheel_quality_fl_decode(msg.wheel_quality_fl);

    if (wheel_quality_fr)
        *wheel_quality_fr = abs_bremse_32_wheel_quality_fr_decode(msg.wheel_quality_fr);

    if (wheel_quality_rl)
        *wheel_quality_rl = abs_bremse_32_wheel_quality_rl_decode(msg.wheel_quality_rl);

    if (wheel_quality_rr)
        *wheel_quality_rr = abs_bremse_32_wheel_quality_rr_decode(msg.wheel_quality_rr);

    return ret;
}

uint8_t abs_bremse_32_acc_fa_encode(double value)
{
    return (uint8_t)(value / 0.05);
}

double abs_bremse_32_acc_fa_decode(uint8_t value)
{
    return ((double)value * 0.05);
}

double abs_bremse_32_acc_fa_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 10.0);
    return ret;
}

bool abs_bremse_32_acc_fa_is_in_range(uint8_t value)
{
    return (value <= 200u);
}

uint8_t abs_bremse_32_acc_ra_encode(double value)
{
    return (uint8_t)(value / 0.05);
}

double abs_bremse_32_acc_ra_decode(uint8_t value)
{
    return ((double)value * 0.05);
}

double abs_bremse_32_acc_ra_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 10.0);
    return ret;
}

bool abs_bremse_32_acc_ra_is_in_range(uint8_t value)
{
    return (value <= 200u);
}

uint8_t abs_bremse_32_wheel_quality_fl_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_32_wheel_quality_fl_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_32_wheel_quality_fl_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32.0);
    return ret;
}

bool abs_bremse_32_wheel_quality_fl_is_in_range(uint8_t value)
{
    return (value <= 32u);
}

uint8_t abs_bremse_32_wheel_quality_fr_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_32_wheel_quality_fr_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_32_wheel_quality_fr_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32.0);
    return ret;
}

bool abs_bremse_32_wheel_quality_fr_is_in_range(uint8_t value)
{
    return (value <= 32u);
}

uint8_t abs_bremse_32_wheel_quality_rl_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_32_wheel_quality_rl_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_32_wheel_quality_rl_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32.0);
    return ret;
}

bool abs_bremse_32_wheel_quality_rl_is_in_range(uint8_t value)
{
    return (value <= 32u);
}

uint8_t abs_bremse_32_wheel_quality_rr_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_32_wheel_quality_rr_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_32_wheel_quality_rr_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 32.0);
    return ret;
}

bool abs_bremse_32_wheel_quality_rr_is_in_range(uint8_t value)
{
    return (value <= 32u);
}

int abs_bremse_51_pack(
    uint8_t *dst_p,
    const struct abs_bremse_51_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u16(src_p->ax1_abs_int, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->ax1_abs_int, 8u, 0xffu);
    dst_p[2] |= pack_left_shift_u16(src_p->ay1_abs_int, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->ay1_abs_int, 8u, 0xffu);
    dst_p[6] |= pack_left_shift_u8(src_p->if_variant, 0u, 0x3fu);
    dst_p[6] |= pack_left_shift_u8(src_p->if_revision, 6u, 0xc0u);
    dst_p[7] |= pack_right_shift_u8(src_p->if_revision, 2u, 0x0fu);
    dst_p[7] |= pack_left_shift_u8(src_p->if_chksum, 4u, 0xf0u);

    return (8);
}

int abs_bremse_51_unpack(
    struct abs_bremse_51_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->ax1_abs_int = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->ax1_abs_int |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ay1_abs_int = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->ay1_abs_int |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->if_variant = unpack_right_shift_u8(src_p[6], 0u, 0x3fu);
    dst_p->if_revision = unpack_right_shift_u8(src_p[6], 6u, 0xc0u);
    dst_p->if_revision |= unpack_left_shift_u8(src_p[7], 2u, 0x0fu);
    dst_p->if_chksum = unpack_right_shift_u8(src_p[7], 4u, 0xf0u);

    return (0);
}

static int abs_bremse_51_check_ranges(struct abs_bremse_51_t *msg)
{
    int idx = 1;

    if (!abs_bremse_51_ax1_abs_int_is_in_range(msg->ax1_abs_int))
        return idx;

    idx++;

    if (!abs_bremse_51_ay1_abs_int_is_in_range(msg->ay1_abs_int))
        return idx;

    idx++;

    if (!abs_bremse_51_if_variant_is_in_range(msg->if_variant))
        return idx;

    idx++;

    if (!abs_bremse_51_if_revision_is_in_range(msg->if_revision))
        return idx;

    idx++;

    if (!abs_bremse_51_if_chksum_is_in_range(msg->if_chksum))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_51_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ax1_abs_int,
    double ay1_abs_int,
    double if_variant,
    double if_revision,
    double if_chksum)
{
    struct abs_bremse_51_t msg;

    msg.ax1_abs_int = abs_bremse_51_ax1_abs_int_encode(ax1_abs_int);
    msg.ay1_abs_int = abs_bremse_51_ay1_abs_int_encode(ay1_abs_int);
    msg.if_variant = abs_bremse_51_if_variant_encode(if_variant);
    msg.if_revision = abs_bremse_51_if_revision_encode(if_revision);
    msg.if_chksum = abs_bremse_51_if_chksum_encode(if_chksum);

    int ret = abs_bremse_51_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_51_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_51_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ax1_abs_int,
    double *ay1_abs_int,
    double *if_variant,
    double *if_revision,
    double *if_chksum)
{
    struct abs_bremse_51_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_51_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_51_check_ranges(&msg);

    if (ax1_abs_int)
        *ax1_abs_int = abs_bremse_51_ax1_abs_int_decode(msg.ax1_abs_int);

    if (ay1_abs_int)
        *ay1_abs_int = abs_bremse_51_ay1_abs_int_decode(msg.ay1_abs_int);

    if (if_variant)
        *if_variant = abs_bremse_51_if_variant_decode(msg.if_variant);

    if (if_revision)
        *if_revision = abs_bremse_51_if_revision_decode(msg.if_revision);

    if (if_chksum)
        *if_chksum = abs_bremse_51_if_chksum_decode(msg.if_chksum);

    return ret;
}

uint16_t abs_bremse_51_ax1_abs_int_encode(double value)
{
    return (uint16_t)((value - -4.1768) / 0.00012742);
}

double abs_bremse_51_ax1_abs_int_decode(uint16_t value)
{
    return (((double)value * 0.00012742) + -4.1768);
}

double abs_bremse_51_ax1_abs_int_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -4.1768);
    ret = CTOOLS_MIN(ret, 4.1736697);
    return ret;
}

bool abs_bremse_51_ax1_abs_int_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

uint16_t abs_bremse_51_ay1_abs_int_encode(double value)
{
    return (uint16_t)((value - -4.1768) / 0.00012742);
}

double abs_bremse_51_ay1_abs_int_decode(uint16_t value)
{
    return (((double)value * 0.00012742) + -4.1768);
}

double abs_bremse_51_ay1_abs_int_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -4.1768);
    ret = CTOOLS_MIN(ret, 4.1765);
    return ret;
}

bool abs_bremse_51_ay1_abs_int_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_51_if_variant_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_51_if_variant_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_51_if_variant_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 63.0);
    return ret;
}

bool abs_bremse_51_if_variant_is_in_range(uint8_t value)
{
    return (value <= 63u);
}

uint8_t abs_bremse_51_if_revision_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_51_if_revision_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_51_if_revision_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 63.0);
    return ret;
}

bool abs_bremse_51_if_revision_is_in_range(uint8_t value)
{
    return (value <= 63u);
}

uint8_t abs_bremse_51_if_chksum_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_51_if_chksum_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_51_if_chksum_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 15.0);
    return ret;
}

bool abs_bremse_51_if_chksum_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

int abs_bremse_52_pack(
    uint8_t *dst_p,
    const struct abs_bremse_52_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->mplx_sw_info, 0u, 0xffu);

    switch (src_p->mplx_sw_info) {

    case 1:
        dst_p[1] |= pack_left_shift_u8(src_p->sw_version_high_upper, 0u, 0xffu);
        dst_p[2] |= pack_left_shift_u8(src_p->sw_version_high_lower, 0u, 0xffu);
        dst_p[3] |= pack_left_shift_u8(src_p->sw_version_mid_upper, 0u, 0xffu);
        dst_p[4] |= pack_left_shift_u8(src_p->sw_version_mid_lower, 0u, 0xffu);
        dst_p[5] |= pack_left_shift_u8(src_p->sw_version_low_upper, 0u, 0xffu);
        dst_p[6] |= pack_left_shift_u8(src_p->sw_version_low_lower, 0u, 0xffu);
        break;

    case 2:
        dst_p[1] |= pack_left_shift_u8(src_p->bb_dig1, 0u, 0xffu);
        dst_p[2] |= pack_left_shift_u8(src_p->bb_dig2, 0u, 0xffu);
        dst_p[3] |= pack_left_shift_u8(src_p->bb_dig3, 0u, 0xffu);
        dst_p[4] |= pack_left_shift_u8(src_p->bb_dig4, 0u, 0xffu);
        dst_p[5] |= pack_left_shift_u8(src_p->bb_dig5, 0u, 0xffu);
        dst_p[6] |= pack_left_shift_u8(src_p->bb_dig6, 0u, 0xffu);
        dst_p[7] |= pack_left_shift_u8(src_p->bb_dig7, 0u, 0xffu);
        break;

    case 3:
        dst_p[1] |= pack_left_shift_u8(src_p->appl_id_01, 0u, 0xffu);
        dst_p[2] |= pack_left_shift_u8(src_p->appl_id_02, 0u, 0xffu);
        dst_p[3] |= pack_left_shift_u8(src_p->appl_id_03, 0u, 0xffu);
        dst_p[4] |= pack_left_shift_u8(src_p->appl_id_04, 0u, 0xffu);
        dst_p[5] |= pack_left_shift_u8(src_p->appl_id_05, 0u, 0xffu);
        dst_p[6] |= pack_left_shift_u8(src_p->appl_id_06, 0u, 0xffu);
        dst_p[7] |= pack_left_shift_u8(src_p->appl_id_07, 0u, 0xffu);
        break;

    case 4:
        dst_p[1] |= pack_left_shift_u8(src_p->appl_id_08, 0u, 0xffu);
        dst_p[2] |= pack_left_shift_u8(src_p->appl_id_09, 0u, 0xffu);
        dst_p[3] |= pack_left_shift_u8(src_p->appl_id_10, 0u, 0xffu);
        dst_p[4] |= pack_left_shift_u8(src_p->appl_id_11, 0u, 0xffu);
        dst_p[5] |= pack_left_shift_u8(src_p->appl_id_12, 0u, 0xffu);
        dst_p[6] |= pack_left_shift_u8(src_p->appl_id_13, 0u, 0xffu);
        dst_p[7] |= pack_left_shift_u8(src_p->appl_id_14, 0u, 0xffu);
        break;

    case 5:
        dst_p[1] |= pack_left_shift_u8(src_p->appl_date_01, 0u, 0xffu);
        dst_p[2] |= pack_left_shift_u8(src_p->appl_date_02, 0u, 0xffu);
        dst_p[3] |= pack_left_shift_u8(src_p->appl_date_03, 0u, 0xffu);
        dst_p[4] |= pack_left_shift_u8(src_p->appl_date_04, 0u, 0xffu);
        dst_p[5] |= pack_left_shift_u8(src_p->appl_date_05, 0u, 0xffu);
        dst_p[6] |= pack_left_shift_u8(src_p->appl_date_06, 0u, 0xffu);
        break;

    case 6:
        dst_p[1] |= pack_left_shift_u8(src_p->sw_can_ident, 0u, 0xffu);
        break;

    case 7:
        dst_p[1] |= pack_left_shift_u8(src_p->hu_date_year, 0u, 0xffu);
        dst_p[2] |= pack_left_shift_u8(src_p->hu_date_month, 0u, 0xffu);
        dst_p[3] |= pack_left_shift_u8(src_p->hu_date_day, 0u, 0xffu);
        dst_p[4] |= pack_left_shift_u32(src_p->ecu_serial, 0u, 0xffu);
        dst_p[5] |= pack_right_shift_u32(src_p->ecu_serial, 8u, 0xffu);
        dst_p[6] |= pack_right_shift_u32(src_p->ecu_serial, 16u, 0xffu);
        dst_p[7] |= pack_right_shift_u32(src_p->ecu_serial, 24u, 0xffu);
        break;

    default:
        break;
    }

    return (8);
}

int abs_bremse_52_unpack(
    struct abs_bremse_52_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->mplx_sw_info = unpack_right_shift_u8(src_p[0], 0u, 0xffu);

    switch (dst_p->mplx_sw_info) {

    case 1:
        dst_p->sw_version_high_upper = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->sw_version_high_lower = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
        dst_p->sw_version_mid_upper = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
        dst_p->sw_version_mid_lower = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
        dst_p->sw_version_low_upper = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
        dst_p->sw_version_low_lower = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
        break;

    case 2:
        dst_p->bb_dig1 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->bb_dig2 = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
        dst_p->bb_dig3 = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
        dst_p->bb_dig4 = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
        dst_p->bb_dig5 = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
        dst_p->bb_dig6 = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
        dst_p->bb_dig7 = unpack_right_shift_u8(src_p[7], 0u, 0xffu);
        break;

    case 3:
        dst_p->appl_id_01 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->appl_id_02 = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
        dst_p->appl_id_03 = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
        dst_p->appl_id_04 = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
        dst_p->appl_id_05 = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
        dst_p->appl_id_06 = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
        dst_p->appl_id_07 = unpack_right_shift_u8(src_p[7], 0u, 0xffu);
        break;

    case 4:
        dst_p->appl_id_08 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->appl_id_09 = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
        dst_p->appl_id_10 = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
        dst_p->appl_id_11 = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
        dst_p->appl_id_12 = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
        dst_p->appl_id_13 = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
        dst_p->appl_id_14 = unpack_right_shift_u8(src_p[7], 0u, 0xffu);
        break;

    case 5:
        dst_p->appl_date_01 = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->appl_date_02 = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
        dst_p->appl_date_03 = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
        dst_p->appl_date_04 = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
        dst_p->appl_date_05 = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
        dst_p->appl_date_06 = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
        break;

    case 6:
        dst_p->sw_can_ident = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        break;

    case 7:
        dst_p->hu_date_year = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
        dst_p->hu_date_month = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
        dst_p->hu_date_day = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
        dst_p->ecu_serial = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
        dst_p->ecu_serial |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
        dst_p->ecu_serial |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
        dst_p->ecu_serial |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
        break;

    default:
        break;
    }

    return (0);
}

static int abs_bremse_52_check_ranges(struct abs_bremse_52_t *msg)
{
    int idx = 1;

    if (!abs_bremse_52_mplx_sw_info_is_in_range(msg->mplx_sw_info))
        return idx;

    idx++;

    if (!abs_bremse_52_sw_version_high_upper_is_in_range(msg->sw_version_high_upper))
        return idx;

    idx++;

    if (!abs_bremse_52_bb_dig1_is_in_range(msg->bb_dig1))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_01_is_in_range(msg->appl_id_01))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_08_is_in_range(msg->appl_id_08))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_date_01_is_in_range(msg->appl_date_01))
        return idx;

    idx++;

    if (!abs_bremse_52_sw_can_ident_is_in_range(msg->sw_can_ident))
        return idx;

    idx++;

    if (!abs_bremse_52_hu_date_year_is_in_range(msg->hu_date_year))
        return idx;

    idx++;

    if (!abs_bremse_52_sw_version_high_lower_is_in_range(msg->sw_version_high_lower))
        return idx;

    idx++;

    if (!abs_bremse_52_bb_dig2_is_in_range(msg->bb_dig2))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_02_is_in_range(msg->appl_id_02))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_09_is_in_range(msg->appl_id_09))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_date_02_is_in_range(msg->appl_date_02))
        return idx;

    idx++;

    if (!abs_bremse_52_hu_date_month_is_in_range(msg->hu_date_month))
        return idx;

    idx++;

    if (!abs_bremse_52_sw_version_mid_upper_is_in_range(msg->sw_version_mid_upper))
        return idx;

    idx++;

    if (!abs_bremse_52_bb_dig3_is_in_range(msg->bb_dig3))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_03_is_in_range(msg->appl_id_03))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_10_is_in_range(msg->appl_id_10))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_date_03_is_in_range(msg->appl_date_03))
        return idx;

    idx++;

    if (!abs_bremse_52_hu_date_day_is_in_range(msg->hu_date_day))
        return idx;

    idx++;

    if (!abs_bremse_52_sw_version_mid_lower_is_in_range(msg->sw_version_mid_lower))
        return idx;

    idx++;

    if (!abs_bremse_52_bb_dig4_is_in_range(msg->bb_dig4))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_04_is_in_range(msg->appl_id_04))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_11_is_in_range(msg->appl_id_11))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_date_04_is_in_range(msg->appl_date_04))
        return idx;

    idx++;

    if (!abs_bremse_52_ecu_serial_is_in_range(msg->ecu_serial))
        return idx;

    idx++;

    if (!abs_bremse_52_sw_version_low_upper_is_in_range(msg->sw_version_low_upper))
        return idx;

    idx++;

    if (!abs_bremse_52_bb_dig5_is_in_range(msg->bb_dig5))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_05_is_in_range(msg->appl_id_05))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_12_is_in_range(msg->appl_id_12))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_date_05_is_in_range(msg->appl_date_05))
        return idx;

    idx++;

    if (!abs_bremse_52_sw_version_low_lower_is_in_range(msg->sw_version_low_lower))
        return idx;

    idx++;

    if (!abs_bremse_52_bb_dig6_is_in_range(msg->bb_dig6))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_06_is_in_range(msg->appl_id_06))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_13_is_in_range(msg->appl_id_13))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_date_06_is_in_range(msg->appl_date_06))
        return idx;

    idx++;

    if (!abs_bremse_52_bb_dig7_is_in_range(msg->bb_dig7))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_07_is_in_range(msg->appl_id_07))
        return idx;

    idx++;

    if (!abs_bremse_52_appl_id_14_is_in_range(msg->appl_id_14))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_52_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double mplx_sw_info,
    double sw_version_high_upper,
    double bb_dig1,
    double appl_id_01,
    double appl_id_08,
    double appl_date_01,
    double sw_can_ident,
    double hu_date_year,
    double sw_version_high_lower,
    double bb_dig2,
    double appl_id_02,
    double appl_id_09,
    double appl_date_02,
    double hu_date_month,
    double sw_version_mid_upper,
    double bb_dig3,
    double appl_id_03,
    double appl_id_10,
    double appl_date_03,
    double hu_date_day,
    double sw_version_mid_lower,
    double bb_dig4,
    double appl_id_04,
    double appl_id_11,
    double appl_date_04,
    double ecu_serial,
    double sw_version_low_upper,
    double bb_dig5,
    double appl_id_05,
    double appl_id_12,
    double appl_date_05,
    double sw_version_low_lower,
    double bb_dig6,
    double appl_id_06,
    double appl_id_13,
    double appl_date_06,
    double bb_dig7,
    double appl_id_07,
    double appl_id_14)
{
    struct abs_bremse_52_t msg;

    msg.mplx_sw_info = abs_bremse_52_mplx_sw_info_encode(mplx_sw_info);
    msg.sw_version_high_upper = abs_bremse_52_sw_version_high_upper_encode(sw_version_high_upper);
    msg.bb_dig1 = abs_bremse_52_bb_dig1_encode(bb_dig1);
    msg.appl_id_01 = abs_bremse_52_appl_id_01_encode(appl_id_01);
    msg.appl_id_08 = abs_bremse_52_appl_id_08_encode(appl_id_08);
    msg.appl_date_01 = abs_bremse_52_appl_date_01_encode(appl_date_01);
    msg.sw_can_ident = abs_bremse_52_sw_can_ident_encode(sw_can_ident);
    msg.hu_date_year = abs_bremse_52_hu_date_year_encode(hu_date_year);
    msg.sw_version_high_lower = abs_bremse_52_sw_version_high_lower_encode(sw_version_high_lower);
    msg.bb_dig2 = abs_bremse_52_bb_dig2_encode(bb_dig2);
    msg.appl_id_02 = abs_bremse_52_appl_id_02_encode(appl_id_02);
    msg.appl_id_09 = abs_bremse_52_appl_id_09_encode(appl_id_09);
    msg.appl_date_02 = abs_bremse_52_appl_date_02_encode(appl_date_02);
    msg.hu_date_month = abs_bremse_52_hu_date_month_encode(hu_date_month);
    msg.sw_version_mid_upper = abs_bremse_52_sw_version_mid_upper_encode(sw_version_mid_upper);
    msg.bb_dig3 = abs_bremse_52_bb_dig3_encode(bb_dig3);
    msg.appl_id_03 = abs_bremse_52_appl_id_03_encode(appl_id_03);
    msg.appl_id_10 = abs_bremse_52_appl_id_10_encode(appl_id_10);
    msg.appl_date_03 = abs_bremse_52_appl_date_03_encode(appl_date_03);
    msg.hu_date_day = abs_bremse_52_hu_date_day_encode(hu_date_day);
    msg.sw_version_mid_lower = abs_bremse_52_sw_version_mid_lower_encode(sw_version_mid_lower);
    msg.bb_dig4 = abs_bremse_52_bb_dig4_encode(bb_dig4);
    msg.appl_id_04 = abs_bremse_52_appl_id_04_encode(appl_id_04);
    msg.appl_id_11 = abs_bremse_52_appl_id_11_encode(appl_id_11);
    msg.appl_date_04 = abs_bremse_52_appl_date_04_encode(appl_date_04);
    msg.ecu_serial = abs_bremse_52_ecu_serial_encode(ecu_serial);
    msg.sw_version_low_upper = abs_bremse_52_sw_version_low_upper_encode(sw_version_low_upper);
    msg.bb_dig5 = abs_bremse_52_bb_dig5_encode(bb_dig5);
    msg.appl_id_05 = abs_bremse_52_appl_id_05_encode(appl_id_05);
    msg.appl_id_12 = abs_bremse_52_appl_id_12_encode(appl_id_12);
    msg.appl_date_05 = abs_bremse_52_appl_date_05_encode(appl_date_05);
    msg.sw_version_low_lower = abs_bremse_52_sw_version_low_lower_encode(sw_version_low_lower);
    msg.bb_dig6 = abs_bremse_52_bb_dig6_encode(bb_dig6);
    msg.appl_id_06 = abs_bremse_52_appl_id_06_encode(appl_id_06);
    msg.appl_id_13 = abs_bremse_52_appl_id_13_encode(appl_id_13);
    msg.appl_date_06 = abs_bremse_52_appl_date_06_encode(appl_date_06);
    msg.bb_dig7 = abs_bremse_52_bb_dig7_encode(bb_dig7);
    msg.appl_id_07 = abs_bremse_52_appl_id_07_encode(appl_id_07);
    msg.appl_id_14 = abs_bremse_52_appl_id_14_encode(appl_id_14);

    int ret = abs_bremse_52_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_52_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_52_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *mplx_sw_info,
    double *sw_version_high_upper,
    double *bb_dig1,
    double *appl_id_01,
    double *appl_id_08,
    double *appl_date_01,
    double *sw_can_ident,
    double *hu_date_year,
    double *sw_version_high_lower,
    double *bb_dig2,
    double *appl_id_02,
    double *appl_id_09,
    double *appl_date_02,
    double *hu_date_month,
    double *sw_version_mid_upper,
    double *bb_dig3,
    double *appl_id_03,
    double *appl_id_10,
    double *appl_date_03,
    double *hu_date_day,
    double *sw_version_mid_lower,
    double *bb_dig4,
    double *appl_id_04,
    double *appl_id_11,
    double *appl_date_04,
    double *ecu_serial,
    double *sw_version_low_upper,
    double *bb_dig5,
    double *appl_id_05,
    double *appl_id_12,
    double *appl_date_05,
    double *sw_version_low_lower,
    double *bb_dig6,
    double *appl_id_06,
    double *appl_id_13,
    double *appl_date_06,
    double *bb_dig7,
    double *appl_id_07,
    double *appl_id_14)
{
    struct abs_bremse_52_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_52_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_52_check_ranges(&msg);

    if (mplx_sw_info)
        *mplx_sw_info = abs_bremse_52_mplx_sw_info_decode(msg.mplx_sw_info);

    if (sw_version_high_upper)
        *sw_version_high_upper = abs_bremse_52_sw_version_high_upper_decode(msg.sw_version_high_upper);

    if (bb_dig1)
        *bb_dig1 = abs_bremse_52_bb_dig1_decode(msg.bb_dig1);

    if (appl_id_01)
        *appl_id_01 = abs_bremse_52_appl_id_01_decode(msg.appl_id_01);

    if (appl_id_08)
        *appl_id_08 = abs_bremse_52_appl_id_08_decode(msg.appl_id_08);

    if (appl_date_01)
        *appl_date_01 = abs_bremse_52_appl_date_01_decode(msg.appl_date_01);

    if (sw_can_ident)
        *sw_can_ident = abs_bremse_52_sw_can_ident_decode(msg.sw_can_ident);

    if (hu_date_year)
        *hu_date_year = abs_bremse_52_hu_date_year_decode(msg.hu_date_year);

    if (sw_version_high_lower)
        *sw_version_high_lower = abs_bremse_52_sw_version_high_lower_decode(msg.sw_version_high_lower);

    if (bb_dig2)
        *bb_dig2 = abs_bremse_52_bb_dig2_decode(msg.bb_dig2);

    if (appl_id_02)
        *appl_id_02 = abs_bremse_52_appl_id_02_decode(msg.appl_id_02);

    if (appl_id_09)
        *appl_id_09 = abs_bremse_52_appl_id_09_decode(msg.appl_id_09);

    if (appl_date_02)
        *appl_date_02 = abs_bremse_52_appl_date_02_decode(msg.appl_date_02);

    if (hu_date_month)
        *hu_date_month = abs_bremse_52_hu_date_month_decode(msg.hu_date_month);

    if (sw_version_mid_upper)
        *sw_version_mid_upper = abs_bremse_52_sw_version_mid_upper_decode(msg.sw_version_mid_upper);

    if (bb_dig3)
        *bb_dig3 = abs_bremse_52_bb_dig3_decode(msg.bb_dig3);

    if (appl_id_03)
        *appl_id_03 = abs_bremse_52_appl_id_03_decode(msg.appl_id_03);

    if (appl_id_10)
        *appl_id_10 = abs_bremse_52_appl_id_10_decode(msg.appl_id_10);

    if (appl_date_03)
        *appl_date_03 = abs_bremse_52_appl_date_03_decode(msg.appl_date_03);

    if (hu_date_day)
        *hu_date_day = abs_bremse_52_hu_date_day_decode(msg.hu_date_day);

    if (sw_version_mid_lower)
        *sw_version_mid_lower = abs_bremse_52_sw_version_mid_lower_decode(msg.sw_version_mid_lower);

    if (bb_dig4)
        *bb_dig4 = abs_bremse_52_bb_dig4_decode(msg.bb_dig4);

    if (appl_id_04)
        *appl_id_04 = abs_bremse_52_appl_id_04_decode(msg.appl_id_04);

    if (appl_id_11)
        *appl_id_11 = abs_bremse_52_appl_id_11_decode(msg.appl_id_11);

    if (appl_date_04)
        *appl_date_04 = abs_bremse_52_appl_date_04_decode(msg.appl_date_04);

    if (ecu_serial)
        *ecu_serial = abs_bremse_52_ecu_serial_decode(msg.ecu_serial);

    if (sw_version_low_upper)
        *sw_version_low_upper = abs_bremse_52_sw_version_low_upper_decode(msg.sw_version_low_upper);

    if (bb_dig5)
        *bb_dig5 = abs_bremse_52_bb_dig5_decode(msg.bb_dig5);

    if (appl_id_05)
        *appl_id_05 = abs_bremse_52_appl_id_05_decode(msg.appl_id_05);

    if (appl_id_12)
        *appl_id_12 = abs_bremse_52_appl_id_12_decode(msg.appl_id_12);

    if (appl_date_05)
        *appl_date_05 = abs_bremse_52_appl_date_05_decode(msg.appl_date_05);

    if (sw_version_low_lower)
        *sw_version_low_lower = abs_bremse_52_sw_version_low_lower_decode(msg.sw_version_low_lower);

    if (bb_dig6)
        *bb_dig6 = abs_bremse_52_bb_dig6_decode(msg.bb_dig6);

    if (appl_id_06)
        *appl_id_06 = abs_bremse_52_appl_id_06_decode(msg.appl_id_06);

    if (appl_id_13)
        *appl_id_13 = abs_bremse_52_appl_id_13_decode(msg.appl_id_13);

    if (appl_date_06)
        *appl_date_06 = abs_bremse_52_appl_date_06_decode(msg.appl_date_06);

    if (bb_dig7)
        *bb_dig7 = abs_bremse_52_bb_dig7_decode(msg.bb_dig7);

    if (appl_id_07)
        *appl_id_07 = abs_bremse_52_appl_id_07_decode(msg.appl_id_07);

    if (appl_id_14)
        *appl_id_14 = abs_bremse_52_appl_id_14_decode(msg.appl_id_14);

    return ret;
}

uint8_t abs_bremse_52_mplx_sw_info_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_mplx_sw_info_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_mplx_sw_info_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_mplx_sw_info_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_sw_version_high_upper_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_sw_version_high_upper_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_sw_version_high_upper_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_sw_version_high_upper_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_bb_dig1_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_bb_dig1_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_bb_dig1_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_bb_dig1_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_01_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_01_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_01_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_01_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_08_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_08_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_08_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_08_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_date_01_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_date_01_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_date_01_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 99.0);
    return ret;
}

bool abs_bremse_52_appl_date_01_is_in_range(uint8_t value)
{
    return (value <= 99u);
}

uint8_t abs_bremse_52_sw_can_ident_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_sw_can_ident_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_sw_can_ident_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_sw_can_ident_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_hu_date_year_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_hu_date_year_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_hu_date_year_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 99.0);
    return ret;
}

bool abs_bremse_52_hu_date_year_is_in_range(uint8_t value)
{
    return (value <= 99u);
}

uint8_t abs_bremse_52_sw_version_high_lower_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_sw_version_high_lower_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_sw_version_high_lower_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_sw_version_high_lower_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_bb_dig2_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_bb_dig2_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_bb_dig2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_bb_dig2_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_02_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_02_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_02_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_02_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_09_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_09_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_09_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_09_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_date_02_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_date_02_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_date_02_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 1.0);
    ret = CTOOLS_MIN(ret, 12.0);
    return ret;
}

bool abs_bremse_52_appl_date_02_is_in_range(uint8_t value)
{
    return ((value >= 1u) && (value <= 12u));
}

uint8_t abs_bremse_52_hu_date_month_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_hu_date_month_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_hu_date_month_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 1.0);
    ret = CTOOLS_MIN(ret, 12.0);
    return ret;
}

bool abs_bremse_52_hu_date_month_is_in_range(uint8_t value)
{
    return ((value >= 1u) && (value <= 12u));
}

uint8_t abs_bremse_52_sw_version_mid_upper_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_sw_version_mid_upper_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_sw_version_mid_upper_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_sw_version_mid_upper_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_bb_dig3_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_bb_dig3_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_bb_dig3_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_bb_dig3_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_03_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_03_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_03_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_03_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_10_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_10_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_10_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_10_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_date_03_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_date_03_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_date_03_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 1.0);
    ret = CTOOLS_MIN(ret, 31.0);
    return ret;
}

bool abs_bremse_52_appl_date_03_is_in_range(uint8_t value)
{
    return ((value >= 1u) && (value <= 31u));
}

uint8_t abs_bremse_52_hu_date_day_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_hu_date_day_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_hu_date_day_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 1.0);
    ret = CTOOLS_MIN(ret, 31.0);
    return ret;
}

bool abs_bremse_52_hu_date_day_is_in_range(uint8_t value)
{
    return ((value >= 1u) && (value <= 31u));
}

uint8_t abs_bremse_52_sw_version_mid_lower_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_sw_version_mid_lower_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_sw_version_mid_lower_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_sw_version_mid_lower_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_bb_dig4_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_bb_dig4_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_bb_dig4_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_bb_dig4_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_04_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_04_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_04_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_04_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_11_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_11_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_11_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_11_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_date_04_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_date_04_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_date_04_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 24.0);
    return ret;
}

bool abs_bremse_52_appl_date_04_is_in_range(uint8_t value)
{
    return (value <= 24u);
}

uint32_t abs_bremse_52_ecu_serial_encode(double value)
{
    return (uint32_t)(value);
}

double abs_bremse_52_ecu_serial_decode(uint32_t value)
{
    return ((double)value);
}

double abs_bremse_52_ecu_serial_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 99999.0);
    return ret;
}

bool abs_bremse_52_ecu_serial_is_in_range(uint32_t value)
{
    return (value <= 99999u);
}

uint8_t abs_bremse_52_sw_version_low_upper_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_sw_version_low_upper_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_sw_version_low_upper_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_sw_version_low_upper_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_bb_dig5_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_bb_dig5_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_bb_dig5_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_bb_dig5_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_05_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_05_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_05_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_05_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_12_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_12_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_12_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_12_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_date_05_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_date_05_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_date_05_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 59.0);
    return ret;
}

bool abs_bremse_52_appl_date_05_is_in_range(uint8_t value)
{
    return (value <= 59u);
}

uint8_t abs_bremse_52_sw_version_low_lower_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_sw_version_low_lower_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_sw_version_low_lower_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_sw_version_low_lower_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_bb_dig6_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_bb_dig6_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_bb_dig6_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_bb_dig6_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_06_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_06_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_06_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_06_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_13_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_13_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_13_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_13_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_date_06_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_date_06_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_date_06_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 59.0);
    return ret;
}

bool abs_bremse_52_appl_date_06_is_in_range(uint8_t value)
{
    return (value <= 59u);
}

uint8_t abs_bremse_52_bb_dig7_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_bb_dig7_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_bb_dig7_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_bb_dig7_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_07_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_07_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_07_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_07_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t abs_bremse_52_appl_id_14_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_appl_id_14_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_52_appl_id_14_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
}

bool abs_bremse_52_appl_id_14_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int abs_bremse_50_pack(
    uint8_t *dst_p,
    const struct abs_bremse_50_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[2] |= pack_left_shift_u16(src_p->brake_bal_at50, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->brake_bal_at50, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->brake_bal_at50_advice, 0u, 0xffu);
    dst_p[5] |= pack_left_shift_u16(src_p->brake_bal_pct, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(src_p->brake_bal_pct, 8u, 0xffu);
    dst_p[7] |= pack_left_shift_u8(src_p->brake_bal_pct_advice, 0u, 0xffu);

    return (8);
}

int abs_bremse_50_unpack(
    struct abs_bremse_50_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->brake_bal_at50 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->brake_bal_at50 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->brake_bal_at50_advice = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    dst_p->brake_bal_pct = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    dst_p->brake_bal_pct |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);
    dst_p->brake_bal_pct_advice = unpack_right_shift_u8(src_p[7], 0u, 0xffu);

    return (0);
}

static int abs_bremse_50_check_ranges(struct abs_bremse_50_t *msg)
{
    int idx = 1;

    if (!abs_bremse_50_brake_bal_at50_is_in_range(msg->brake_bal_at50))
        return idx;

    idx++;

    if (!abs_bremse_50_brake_bal_at50_advice_is_in_range(msg->brake_bal_at50_advice))
        return idx;

    idx++;

    if (!abs_bremse_50_brake_bal_pct_is_in_range(msg->brake_bal_pct))
        return idx;

    idx++;

    if (!abs_bremse_50_brake_bal_pct_advice_is_in_range(msg->brake_bal_pct_advice))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_50_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double brake_bal_at50,
    double brake_bal_at50_advice,
    double brake_bal_pct,
    double brake_bal_pct_advice)
{
    struct abs_bremse_50_t msg;

    msg.brake_bal_at50 = abs_bremse_50_brake_bal_at50_encode(brake_bal_at50);
    msg.brake_bal_at50_advice = abs_bremse_50_brake_bal_at50_advice_encode(brake_bal_at50_advice);
    msg.brake_bal_pct = abs_bremse_50_brake_bal_pct_encode(brake_bal_pct);
    msg.brake_bal_pct_advice = abs_bremse_50_brake_bal_pct_advice_encode(brake_bal_pct_advice);

    int ret = abs_bremse_50_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_50_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_50_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *brake_bal_at50,
    double *brake_bal_at50_advice,
    double *brake_bal_pct,
    double *brake_bal_pct_advice)
{
    struct abs_bremse_50_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_50_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_50_check_ranges(&msg);

    if (brake_bal_at50)
        *brake_bal_at50 = abs_bremse_50_brake_bal_at50_decode(msg.brake_bal_at50);

    if (brake_bal_at50_advice)
        *brake_bal_at50_advice = abs_bremse_50_brake_bal_at50_advice_decode(msg.brake_bal_at50_advice);

    if (brake_bal_pct)
        *brake_bal_pct = abs_bremse_50_brake_bal_pct_decode(msg.brake_bal_pct);

    if (brake_bal_pct_advice)
        *brake_bal_pct_advice = abs_bremse_50_brake_bal_pct_advice_decode(msg.brake_bal_pct_advice);

    return ret;
}

uint16_t abs_bremse_50_brake_bal_at50_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double abs_bremse_50_brake_bal_at50_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double abs_bremse_50_brake_bal_at50_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_50_brake_bal_at50_is_in_range(uint16_t value)
{
    return (value <= 1000u);
}

uint8_t abs_bremse_50_brake_bal_at50_advice_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_50_brake_bal_at50_advice_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_50_brake_bal_at50_advice_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_50_brake_bal_at50_advice_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint16_t abs_bremse_50_brake_bal_pct_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double abs_bremse_50_brake_bal_pct_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double abs_bremse_50_brake_bal_pct_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_50_brake_bal_pct_is_in_range(uint16_t value)
{
    return (value <= 1000u);
}

uint8_t abs_bremse_50_brake_bal_pct_advice_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_50_brake_bal_pct_advice_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_50_brake_bal_pct_advice_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
}

bool abs_bremse_50_brake_bal_pct_advice_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

int abs_bremse_53_pack(
    uint8_t *dst_p,
    const struct abs_bremse_53_t *src_p,
    size_t size)
{
    uint16_t p_fa;
    uint16_t p_ra;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->switch_position, 0u, 0xffu);
    p_fa = (uint16_t)src_p->p_fa;
    dst_p[1] |= pack_left_shift_u16(p_fa, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u16(p_fa, 8u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->bls, 0u, 0x01u);
    dst_p[3] |= pack_left_shift_u8(src_p->bremse_53_cnt, 2u, 0x0cu);
    dst_p[3] |= pack_left_shift_u8(src_p->abs_malfunction, 4u, 0x10u);
    dst_p[3] |= pack_left_shift_u8(src_p->abs_active, 5u, 0x20u);
    dst_p[3] |= pack_left_shift_u8(src_p->ebd_lamp, 6u, 0x40u);
    dst_p[3] |= pack_left_shift_u8(src_p->abs_lamp, 7u, 0x80u);
    dst_p[4] |= pack_left_shift_u8(src_p->diag_fl, 0u, 0x03u);
    dst_p[4] |= pack_left_shift_u8(src_p->diag_fr, 2u, 0x0cu);
    dst_p[4] |= pack_left_shift_u8(src_p->diag_rl, 4u, 0x30u);
    dst_p[4] |= pack_left_shift_u8(src_p->diag_rr, 6u, 0xc0u);
    dst_p[5] |= pack_left_shift_u8(src_p->diag_abs_unit, 0u, 0x01u);
    dst_p[5] |= pack_left_shift_u8(src_p->diag_fuse_valve, 1u, 0x02u);
    dst_p[5] |= pack_left_shift_u8(src_p->diag_fuse_pump, 2u, 0x04u);
    dst_p[5] |= pack_left_shift_u8(src_p->diag_p_fa, 3u, 0x08u);
    dst_p[5] |= pack_left_shift_u8(src_p->diag_p_ra, 4u, 0x10u);
    dst_p[5] |= pack_left_shift_u8(src_p->diag_yrs, 5u, 0x20u);
    dst_p[5] |= pack_left_shift_u8(src_p->abs_fault_info, 6u, 0xc0u);
    p_ra = (uint16_t)src_p->p_ra;
    dst_p[6] |= pack_left_shift_u16(p_ra, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(p_ra, 8u, 0xffu);

    return (8);
}

int abs_bremse_53_unpack(
    struct abs_bremse_53_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t p_fa;
    uint16_t p_ra;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->switch_position = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    p_fa = unpack_right_shift_u16(src_p[1], 0u, 0xffu);
    p_fa |= unpack_left_shift_u16(src_p[2], 8u, 0xffu);
    dst_p->p_fa = (int16_t)p_fa;
    dst_p->bls = unpack_right_shift_u8(src_p[3], 0u, 0x01u);
    dst_p->bremse_53_cnt = unpack_right_shift_u8(src_p[3], 2u, 0x0cu);
    dst_p->abs_malfunction = unpack_right_shift_u8(src_p[3], 4u, 0x10u);
    dst_p->abs_active = unpack_right_shift_u8(src_p[3], 5u, 0x20u);
    dst_p->ebd_lamp = unpack_right_shift_u8(src_p[3], 6u, 0x40u);
    dst_p->abs_lamp = unpack_right_shift_u8(src_p[3], 7u, 0x80u);
    dst_p->diag_fl = unpack_right_shift_u8(src_p[4], 0u, 0x03u);
    dst_p->diag_fr = unpack_right_shift_u8(src_p[4], 2u, 0x0cu);
    dst_p->diag_rl = unpack_right_shift_u8(src_p[4], 4u, 0x30u);
    dst_p->diag_rr = unpack_right_shift_u8(src_p[4], 6u, 0xc0u);
    dst_p->diag_abs_unit = unpack_right_shift_u8(src_p[5], 0u, 0x01u);
    dst_p->diag_fuse_valve = unpack_right_shift_u8(src_p[5], 1u, 0x02u);
    dst_p->diag_fuse_pump = unpack_right_shift_u8(src_p[5], 2u, 0x04u);
    dst_p->diag_p_fa = unpack_right_shift_u8(src_p[5], 3u, 0x08u);
    dst_p->diag_p_ra = unpack_right_shift_u8(src_p[5], 4u, 0x10u);
    dst_p->diag_yrs = unpack_right_shift_u8(src_p[5], 5u, 0x20u);
    dst_p->abs_fault_info = unpack_right_shift_u8(src_p[5], 6u, 0xc0u);
    p_ra = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    p_ra |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->p_ra = (int16_t)p_ra;

    return (0);
}

static int abs_bremse_53_check_ranges(struct abs_bremse_53_t *msg)
{
    int idx = 1;

    if (!abs_bremse_53_switch_position_is_in_range(msg->switch_position))
        return idx;

    idx++;

    if (!abs_bremse_53_p_fa_is_in_range(msg->p_fa))
        return idx;

    idx++;

    if (!abs_bremse_53_bls_is_in_range(msg->bls))
        return idx;

    idx++;

    if (!abs_bremse_53_bremse_53_cnt_is_in_range(msg->bremse_53_cnt))
        return idx;

    idx++;

    if (!abs_bremse_53_abs_malfunction_is_in_range(msg->abs_malfunction))
        return idx;

    idx++;

    if (!abs_bremse_53_abs_active_is_in_range(msg->abs_active))
        return idx;

    idx++;

    if (!abs_bremse_53_ebd_lamp_is_in_range(msg->ebd_lamp))
        return idx;

    idx++;

    if (!abs_bremse_53_abs_lamp_is_in_range(msg->abs_lamp))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_fl_is_in_range(msg->diag_fl))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_fr_is_in_range(msg->diag_fr))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_rl_is_in_range(msg->diag_rl))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_rr_is_in_range(msg->diag_rr))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_abs_unit_is_in_range(msg->diag_abs_unit))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_fuse_valve_is_in_range(msg->diag_fuse_valve))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_fuse_pump_is_in_range(msg->diag_fuse_pump))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_p_fa_is_in_range(msg->diag_p_fa))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_p_ra_is_in_range(msg->diag_p_ra))
        return idx;

    idx++;

    if (!abs_bremse_53_diag_yrs_is_in_range(msg->diag_yrs))
        return idx;

    idx++;

    if (!abs_bremse_53_abs_fault_info_is_in_range(msg->abs_fault_info))
        return idx;

    idx++;

    if (!abs_bremse_53_p_ra_is_in_range(msg->p_ra))
        return idx;

    idx++;

    return 0;
}

int abs_bremse_53_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double switch_position,
    double p_fa,
    double bls,
    double bremse_53_cnt,
    double abs_malfunction,
    double abs_active,
    double ebd_lamp,
    double abs_lamp,
    double diag_fl,
    double diag_fr,
    double diag_rl,
    double diag_rr,
    double diag_abs_unit,
    double diag_fuse_valve,
    double diag_fuse_pump,
    double diag_p_fa,
    double diag_p_ra,
    double diag_yrs,
    double abs_fault_info,
    double p_ra)
{
    struct abs_bremse_53_t msg;

    msg.switch_position = abs_bremse_53_switch_position_encode(switch_position);
    msg.p_fa = abs_bremse_53_p_fa_encode(p_fa);
    msg.bls = abs_bremse_53_bls_encode(bls);
    msg.bremse_53_cnt = abs_bremse_53_bremse_53_cnt_encode(bremse_53_cnt);
    msg.abs_malfunction = abs_bremse_53_abs_malfunction_encode(abs_malfunction);
    msg.abs_active = abs_bremse_53_abs_active_encode(abs_active);
    msg.ebd_lamp = abs_bremse_53_ebd_lamp_encode(ebd_lamp);
    msg.abs_lamp = abs_bremse_53_abs_lamp_encode(abs_lamp);
    msg.diag_fl = abs_bremse_53_diag_fl_encode(diag_fl);
    msg.diag_fr = abs_bremse_53_diag_fr_encode(diag_fr);
    msg.diag_rl = abs_bremse_53_diag_rl_encode(diag_rl);
    msg.diag_rr = abs_bremse_53_diag_rr_encode(diag_rr);
    msg.diag_abs_unit = abs_bremse_53_diag_abs_unit_encode(diag_abs_unit);
    msg.diag_fuse_valve = abs_bremse_53_diag_fuse_valve_encode(diag_fuse_valve);
    msg.diag_fuse_pump = abs_bremse_53_diag_fuse_pump_encode(diag_fuse_pump);
    msg.diag_p_fa = abs_bremse_53_diag_p_fa_encode(diag_p_fa);
    msg.diag_p_ra = abs_bremse_53_diag_p_ra_encode(diag_p_ra);
    msg.diag_yrs = abs_bremse_53_diag_yrs_encode(diag_yrs);
    msg.abs_fault_info = abs_bremse_53_abs_fault_info_encode(abs_fault_info);
    msg.p_ra = abs_bremse_53_p_ra_encode(p_ra);

    int ret = abs_bremse_53_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = abs_bremse_53_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int abs_bremse_53_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *switch_position,
    double *p_fa,
    double *bls,
    double *bremse_53_cnt,
    double *abs_malfunction,
    double *abs_active,
    double *ebd_lamp,
    double *abs_lamp,
    double *diag_fl,
    double *diag_fr,
    double *diag_rl,
    double *diag_rr,
    double *diag_abs_unit,
    double *diag_fuse_valve,
    double *diag_fuse_pump,
    double *diag_p_fa,
    double *diag_p_ra,
    double *diag_yrs,
    double *abs_fault_info,
    double *p_ra)
{
    struct abs_bremse_53_t msg;
    memset(&msg, 0, sizeof(msg));

    if (abs_bremse_53_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = abs_bremse_53_check_ranges(&msg);

    if (switch_position)
        *switch_position = abs_bremse_53_switch_position_decode(msg.switch_position);

    if (p_fa)
        *p_fa = abs_bremse_53_p_fa_decode(msg.p_fa);

    if (bls)
        *bls = abs_bremse_53_bls_decode(msg.bls);

    if (bremse_53_cnt)
        *bremse_53_cnt = abs_bremse_53_bremse_53_cnt_decode(msg.bremse_53_cnt);

    if (abs_malfunction)
        *abs_malfunction = abs_bremse_53_abs_malfunction_decode(msg.abs_malfunction);

    if (abs_active)
        *abs_active = abs_bremse_53_abs_active_decode(msg.abs_active);

    if (ebd_lamp)
        *ebd_lamp = abs_bremse_53_ebd_lamp_decode(msg.ebd_lamp);

    if (abs_lamp)
        *abs_lamp = abs_bremse_53_abs_lamp_decode(msg.abs_lamp);

    if (diag_fl)
        *diag_fl = abs_bremse_53_diag_fl_decode(msg.diag_fl);

    if (diag_fr)
        *diag_fr = abs_bremse_53_diag_fr_decode(msg.diag_fr);

    if (diag_rl)
        *diag_rl = abs_bremse_53_diag_rl_decode(msg.diag_rl);

    if (diag_rr)
        *diag_rr = abs_bremse_53_diag_rr_decode(msg.diag_rr);

    if (diag_abs_unit)
        *diag_abs_unit = abs_bremse_53_diag_abs_unit_decode(msg.diag_abs_unit);

    if (diag_fuse_valve)
        *diag_fuse_valve = abs_bremse_53_diag_fuse_valve_decode(msg.diag_fuse_valve);

    if (diag_fuse_pump)
        *diag_fuse_pump = abs_bremse_53_diag_fuse_pump_decode(msg.diag_fuse_pump);

    if (diag_p_fa)
        *diag_p_fa = abs_bremse_53_diag_p_fa_decode(msg.diag_p_fa);

    if (diag_p_ra)
        *diag_p_ra = abs_bremse_53_diag_p_ra_decode(msg.diag_p_ra);

    if (diag_yrs)
        *diag_yrs = abs_bremse_53_diag_yrs_decode(msg.diag_yrs);

    if (abs_fault_info)
        *abs_fault_info = abs_bremse_53_abs_fault_info_decode(msg.abs_fault_info);

    if (p_ra)
        *p_ra = abs_bremse_53_p_ra_decode(msg.p_ra);

    return ret;
}

uint8_t abs_bremse_53_switch_position_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_switch_position_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_switch_position_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 1.0);
    ret = CTOOLS_MIN(ret, 12.0);
    return ret;
}

bool abs_bremse_53_switch_position_is_in_range(uint8_t value)
{
    return ((value >= 1u) && (value <= 12u));
}

int16_t abs_bremse_53_p_fa_encode(double value)
{
    return (int16_t)(value / 0.01526);
}

double abs_bremse_53_p_fa_decode(int16_t value)
{
    return ((double)value * 0.01526);
}

double abs_bremse_53_p_fa_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -42.5);
    ret = CTOOLS_MIN(ret, 425.0);
    return ret;
}

bool abs_bremse_53_p_fa_is_in_range(int16_t value)
{
    return ((value >= -2785) && (value <= 27850));
}

uint8_t abs_bremse_53_bls_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_bls_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_bls_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_bls_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_bremse_53_cnt_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_bremse_53_cnt_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_bremse_53_cnt_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 3.0);
    return ret;
}

bool abs_bremse_53_bremse_53_cnt_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t abs_bremse_53_abs_malfunction_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_abs_malfunction_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_abs_malfunction_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_abs_malfunction_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_abs_active_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_abs_active_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_abs_active_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_abs_active_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_ebd_lamp_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_ebd_lamp_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_ebd_lamp_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_ebd_lamp_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_abs_lamp_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_abs_lamp_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_abs_lamp_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_abs_lamp_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_diag_fl_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_fl_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_fl_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 3.0);
    return ret;
}

bool abs_bremse_53_diag_fl_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t abs_bremse_53_diag_fr_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_fr_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_fr_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 3.0);
    return ret;
}

bool abs_bremse_53_diag_fr_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t abs_bremse_53_diag_rl_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_rl_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_rl_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 3.0);
    return ret;
}

bool abs_bremse_53_diag_rl_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t abs_bremse_53_diag_rr_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_rr_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_rr_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 3.0);
    return ret;
}

bool abs_bremse_53_diag_rr_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t abs_bremse_53_diag_abs_unit_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_abs_unit_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_abs_unit_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_diag_abs_unit_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_diag_fuse_valve_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_fuse_valve_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_fuse_valve_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_diag_fuse_valve_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_diag_fuse_pump_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_fuse_pump_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_fuse_pump_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_diag_fuse_pump_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_diag_p_fa_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_p_fa_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_p_fa_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_diag_p_fa_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_diag_p_ra_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_p_ra_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_p_ra_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_diag_p_ra_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_diag_yrs_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_diag_yrs_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_diag_yrs_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
}

bool abs_bremse_53_diag_yrs_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t abs_bremse_53_abs_fault_info_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_abs_fault_info_decode(uint8_t value)
{
    return ((double)value);
}

double abs_bremse_53_abs_fault_info_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 3.0);
    return ret;
}

bool abs_bremse_53_abs_fault_info_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

int16_t abs_bremse_53_p_ra_encode(double value)
{
    return (int16_t)(value / 0.01526);
}

double abs_bremse_53_p_ra_decode(int16_t value)
{
    return ((double)value * 0.01526);
}

double abs_bremse_53_p_ra_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -42.5);
    ret = CTOOLS_MIN(ret, 425.0);
    return ret;
}

bool abs_bremse_53_p_ra_is_in_range(int16_t value)
{
    return ((value >= -2785) && (value <= 27850));
}

bool is_extended_frame(uint32_t frame_id)
{
    return false;
}

#undef CTOOLS_MAX
#undef CTOOLS_MIN
