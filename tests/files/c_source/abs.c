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

uint16_t abs_bremse_33_whlspeed_fl_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_33_whlspeed_fl_decode(uint16_t value)
{
    return ((double)value * 0.015625);
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

uint16_t abs_mm5_10_tx1_yaw_rate_encode(double value)
{
    return (uint16_t)((value - -163.84) / 0.005);
}

double abs_mm5_10_tx1_yaw_rate_decode(uint16_t value)
{
    return (((double)value * 0.005) + -163.84);
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

uint16_t abs_mm5_10_tx2_roll_rate_encode(double value)
{
    return (uint16_t)((value - -163.84) / 0.005);
}

double abs_mm5_10_tx2_roll_rate_decode(uint16_t value)
{
    return (((double)value * 0.005) + -163.84);
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

uint16_t abs_mm5_10_tx3_az_encode(double value)
{
    return (uint16_t)((value - -4.1768) / 0.000127465);
}

double abs_mm5_10_tx3_az_decode(uint16_t value)
{
    return (((double)value * 0.000127465) + -4.1768);
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

uint16_t abs_bremse_2_whlspeed_fl_bremse2_encode(double value)
{
    return (uint16_t)(value / 0.015625);
}

double abs_bremse_2_whlspeed_fl_bremse2_decode(uint16_t value)
{
    return ((double)value * 0.015625);
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

uint8_t abs_abs_switch_abs_switchposition_encode(double value)
{
    return (uint8_t)(value);
}

double abs_abs_switch_abs_switchposition_decode(uint8_t value)
{
    return ((double)value);
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

uint16_t abs_bremse_31_idle_time_encode(double value)
{
    return (uint16_t)(value);
}

double abs_bremse_31_idle_time_decode(uint16_t value)
{
    return ((double)value);
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

uint8_t abs_bremse_32_acc_fa_encode(double value)
{
    return (uint8_t)(value / 0.05);
}

double abs_bremse_32_acc_fa_decode(uint8_t value)
{
    return ((double)value * 0.05);
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

uint16_t abs_bremse_51_ax1_abs_int_encode(double value)
{
    return (uint16_t)((value - -4.1768) / 0.00012742);
}

double abs_bremse_51_ax1_abs_int_decode(uint16_t value)
{
    return (((double)value * 0.00012742) + -4.1768);
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

uint8_t abs_bremse_52_mplx_sw_info_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_52_mplx_sw_info_decode(uint8_t value)
{
    return ((double)value);
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

uint16_t abs_bremse_50_brake_bal_at50_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double abs_bremse_50_brake_bal_at50_decode(uint16_t value)
{
    return ((double)value * 0.1);
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

uint8_t abs_bremse_53_switch_position_encode(double value)
{
    return (uint8_t)(value);
}

double abs_bremse_53_switch_position_decode(uint8_t value)
{
    return ((double)value);
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

bool abs_bremse_53_p_ra_is_in_range(int16_t value)
{
    return ((value >= -2785) && (value <= 27850));
}
