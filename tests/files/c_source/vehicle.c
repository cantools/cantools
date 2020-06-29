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

#include "vehicle.h"

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

static inline uint8_t pack_left_shift_u64(
    uint64_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value << shift) & mask);
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

static inline uint8_t pack_right_shift_u64(
    uint64_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint8_t)((uint8_t)(value >> shift) & mask);
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

static inline uint64_t unpack_left_shift_u64(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint64_t)((uint64_t)(value & mask) << shift);
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

static inline uint64_t unpack_right_shift_u64(
    uint8_t value,
    uint8_t shift,
    uint8_t mask)
{
    return (uint64_t)((uint64_t)(value & mask) >> shift);
}

int vehicle_rt_sb_ins_vel_body_axes_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vel_body_axes_t *src_p,
    size_t size)
{
    uint32_t ins_vel_forwards_2_d;
    uint32_t ins_vel_sideways_2_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_forwards, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_sideways, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_vel_body, 0u, 0xffu);
    ins_vel_forwards_2_d = (uint32_t)src_p->ins_vel_forwards_2_d;
    dst_p[2] |= pack_left_shift_u32(ins_vel_forwards_2_d, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(ins_vel_forwards_2_d, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(ins_vel_forwards_2_d, 16u, 0xffu);
    ins_vel_sideways_2_d = (uint32_t)src_p->ins_vel_sideways_2_d;
    dst_p[5] |= pack_left_shift_u32(ins_vel_sideways_2_d, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_vel_sideways_2_d, 8u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(ins_vel_sideways_2_d, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vel_body_axes_unpack(
    struct vehicle_rt_sb_ins_vel_body_axes_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_vel_forwards_2_d;
    uint32_t ins_vel_sideways_2_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_vel_forwards = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_vel_sideways = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_ins_vel_body = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    ins_vel_forwards_2_d = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    ins_vel_forwards_2_d |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    ins_vel_forwards_2_d |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);

    if ((ins_vel_forwards_2_d & (1u << 23)) != 0u) {
        ins_vel_forwards_2_d |= 0xff000000u;
    }

    dst_p->ins_vel_forwards_2_d = (int32_t)ins_vel_forwards_2_d;
    ins_vel_sideways_2_d = unpack_right_shift_u32(src_p[5], 0u, 0xffu);
    ins_vel_sideways_2_d |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    ins_vel_sideways_2_d |= unpack_left_shift_u32(src_p[7], 16u, 0xffu);

    if ((ins_vel_sideways_2_d & (1u << 23)) != 0u) {
        ins_vel_sideways_2_d |= 0xff000000u;
    }

    dst_p->ins_vel_sideways_2_d = (int32_t)ins_vel_sideways_2_d;

    return (0);
}

uint8_t vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_sideways_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_sideways_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_sideways_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_vel_body_axes_accuracy_ins_vel_body_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_body_axes_accuracy_ins_vel_body_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_body_axes_accuracy_ins_vel_body_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_ins_vel_body_axes_ins_vel_forwards_2_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_body_axes_ins_vel_forwards_2_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_body_axes_ins_vel_forwards_2_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_ins_vel_body_axes_ins_vel_sideways_2_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_body_axes_ins_vel_sideways_2_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_body_axes_ins_vel_sideways_2_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_dl1_mk3_speed_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_speed_t *src_p,
    size_t size)
{
    uint32_t speed;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_speed, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_speed, 0u, 0xffu);
    speed = (uint32_t)src_p->speed;
    dst_p[2] |= pack_left_shift_u32(speed, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(speed, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(speed, 16u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(speed, 24u, 0xffu);

    return (8);
}

int vehicle_rt_dl1_mk3_speed_unpack(
    struct vehicle_rt_dl1_mk3_speed_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t speed;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_speed = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->accuracy_speed = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    speed = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    speed |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    speed |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);
    speed |= unpack_left_shift_u32(src_p[5], 24u, 0xffu);
    dst_p->speed = (int32_t)speed;

    return (0);
}

uint8_t vehicle_rt_dl1_mk3_speed_validity_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_speed_validity_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_speed_validity_speed_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_speed_accuracy_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_speed_accuracy_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_speed_accuracy_speed_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_dl1_mk3_speed_speed_encode(double value)
{
    return (int32_t)(value / 0.00001);
}

double vehicle_rt_dl1_mk3_speed_speed_decode(int32_t value)
{
    return ((double)value * 0.00001);
}

bool vehicle_rt_dl1_mk3_speed_speed_is_in_range(int32_t value)
{
    return ((value >= -2000000000) && (value <= 2000000000));
}

int vehicle_rt_dl1_mk3_gps_time_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_gps_time_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_time, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_week, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_time, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u32(src_p->gps_time, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->gps_time, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(src_p->gps_time, 16u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(src_p->gps_time, 24u, 0xffu);
    dst_p[6] |= pack_left_shift_u16(src_p->gps_week, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(src_p->gps_week, 8u, 0xffu);

    return (8);
}

int vehicle_rt_dl1_mk3_gps_time_unpack(
    struct vehicle_rt_dl1_mk3_gps_time_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_time = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_week = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_time = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->gps_time = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[5], 24u, 0xffu);
    dst_p->gps_week = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    dst_p->gps_week |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_dl1_mk3_gps_time_validity_gps_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_time_validity_gps_time_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_time_validity_gps_time_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_gps_time_validity_gps_week_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_time_validity_gps_week_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_time_validity_gps_week_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_gps_time_accuracy_gps_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_time_accuracy_gps_time_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_time_accuracy_gps_time_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint32_t vehicle_rt_dl1_mk3_gps_time_gps_time_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_gps_time_gps_time_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_gps_time_gps_time_is_in_range(uint32_t value)
{
    return (value <= 604800000u);
}

uint16_t vehicle_rt_dl1_mk3_gps_time_gps_week_encode(double value)
{
    return (uint16_t)(value);
}

double vehicle_rt_dl1_mk3_gps_time_gps_week_decode(uint16_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_time_gps_week_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_gps_pos_llh_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_gps_pos_llh_2_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_altitude;
    uint32_t gps_pos_llh_longitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    gps_pos_llh_longitude = (uint32_t)src_p->gps_pos_llh_longitude;
    dst_p[0] |= pack_left_shift_u32(gps_pos_llh_longitude, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(gps_pos_llh_longitude, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(gps_pos_llh_longitude, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(gps_pos_llh_longitude, 24u, 0xffu);
    gps_pos_llh_altitude = (uint32_t)src_p->gps_pos_llh_altitude;
    dst_p[4] |= pack_left_shift_u32(gps_pos_llh_altitude, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_pos_llh_altitude, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_pos_llh_altitude, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(gps_pos_llh_altitude, 24u, 0xffu);

    return (8);
}

int vehicle_rt_dl1_mk3_gps_pos_llh_2_unpack(
    struct vehicle_rt_dl1_mk3_gps_pos_llh_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_altitude;
    uint32_t gps_pos_llh_longitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    gps_pos_llh_longitude = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    gps_pos_llh_longitude |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    gps_pos_llh_longitude |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    gps_pos_llh_longitude |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->gps_pos_llh_longitude = (int32_t)gps_pos_llh_longitude;
    gps_pos_llh_altitude = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_pos_llh_altitude |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_pos_llh_altitude |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    gps_pos_llh_altitude |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->gps_pos_llh_altitude = (int32_t)gps_pos_llh_altitude;

    return (0);
}

int32_t vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_is_in_range(int32_t value)
{
    return ((value >= -1800000000) && (value <= 1800000000));
}

int32_t vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_altitude_encode(double value)
{
    return (int32_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_altitude_decode(int32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_altitude_is_in_range(int32_t value)
{
    return ((value >= -1000000) && (value <= 100000000));
}

int vehicle_rt_dl1_mk3_gps_pos_llh_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_gps_pos_llh_1_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_latitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_llh_latitude, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_llh_longitude, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_llh_altitude, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_pos_llh_latitude, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->accuracy_gps_pos_llh_longitude, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->accuracy_gps_pos_llh_altitude, 0u, 0xffu);
    gps_pos_llh_latitude = (uint32_t)src_p->gps_pos_llh_latitude;
    dst_p[4] |= pack_left_shift_u32(gps_pos_llh_latitude, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_pos_llh_latitude, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_pos_llh_latitude, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(gps_pos_llh_latitude, 24u, 0xffu);

    return (8);
}

int vehicle_rt_dl1_mk3_gps_pos_llh_1_unpack(
    struct vehicle_rt_dl1_mk3_gps_pos_llh_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_latitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_pos_llh_latitude = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_pos_llh_longitude = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_gps_pos_llh_altitude = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_gps_pos_llh_latitude = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->accuracy_gps_pos_llh_longitude = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->accuracy_gps_pos_llh_altitude = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    gps_pos_llh_latitude = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_pos_llh_latitude |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_pos_llh_latitude |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    gps_pos_llh_latitude |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->gps_pos_llh_latitude = (int32_t)gps_pos_llh_latitude;

    return (0);
}

uint8_t vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_longitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_longitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_longitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_altitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_altitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_altitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_dl1_mk3_gps_pos_llh_1_gps_pos_llh_latitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_gps_pos_llh_latitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

bool vehicle_rt_dl1_mk3_gps_pos_llh_1_gps_pos_llh_latitude_is_in_range(int32_t value)
{
    return ((value >= -900000000) && (value <= 900000000));
}

int vehicle_rt_dl1_mk3_gps_speed_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_gps_speed_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_speed_2_d, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_speed_3_d, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_speed, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u32(src_p->gps_speed_2_d, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->gps_speed_2_d, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(src_p->gps_speed_2_d, 16u, 0xffu);
    dst_p[5] |= pack_left_shift_u32(src_p->gps_speed_3_d, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(src_p->gps_speed_3_d, 8u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(src_p->gps_speed_3_d, 16u, 0xffu);

    return (8);
}

int vehicle_rt_dl1_mk3_gps_speed_unpack(
    struct vehicle_rt_dl1_mk3_gps_speed_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_speed_2_d = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_speed_3_d = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_speed = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->gps_speed_2_d = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    dst_p->gps_speed_2_d |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    dst_p->gps_speed_2_d |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);
    dst_p->gps_speed_3_d = unpack_right_shift_u32(src_p[5], 0u, 0xffu);
    dst_p->gps_speed_3_d |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    dst_p->gps_speed_3_d |= unpack_left_shift_u32(src_p[7], 16u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_3_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_3_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_3_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_gps_speed_accuracy_gps_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_speed_accuracy_gps_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_gps_speed_accuracy_gps_speed_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint32_t vehicle_rt_dl1_mk3_gps_speed_gps_speed_2_d_encode(double value)
{
    return (uint32_t)(value / 0.0001);
}

double vehicle_rt_dl1_mk3_gps_speed_gps_speed_2_d_decode(uint32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_dl1_mk3_gps_speed_gps_speed_2_d_is_in_range(uint32_t value)
{
    return (value <= 16750000u);
}

uint32_t vehicle_rt_dl1_mk3_gps_speed_gps_speed_3_d_encode(double value)
{
    return (uint32_t)(value / 0.0001);
}

double vehicle_rt_dl1_mk3_gps_speed_gps_speed_3_d_decode(uint32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_dl1_mk3_gps_speed_gps_speed_3_d_is_in_range(uint32_t value)
{
    return (value <= 16750000u);
}

int vehicle_rt_ir_temp_temp_7_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_7_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_7;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_7 = (uint16_t)src_p->ir_temperature_7;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_7, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_7, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_7_unpack(
    struct vehicle_rt_ir_temp_temp_7_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_7;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_7 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_7 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_7 = (int16_t)ir_temperature_7;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_7_ir_temperature_7_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_7_ir_temperature_7_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_7_ir_temperature_7_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_rr_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_rr_2_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_29;
    uint16_t ir_temperature_30;
    uint16_t ir_temperature_31;
    uint16_t ir_temperature_32;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_29 = (uint16_t)src_p->ir_temperature_29;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_29, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_29, 8u, 0xffu);
    ir_temperature_30 = (uint16_t)src_p->ir_temperature_30;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_30, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_30, 8u, 0xffu);
    ir_temperature_31 = (uint16_t)src_p->ir_temperature_31;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_31, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_31, 8u, 0xffu);
    ir_temperature_32 = (uint16_t)src_p->ir_temperature_32;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_32, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_32, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_rr_2_unpack(
    struct vehicle_rt_ir_temp_temp_rr_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_29;
    uint16_t ir_temperature_30;
    uint16_t ir_temperature_31;
    uint16_t ir_temperature_32;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_29 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_29 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_29 = (int16_t)ir_temperature_29;
    ir_temperature_30 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_30 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_30 = (int16_t)ir_temperature_30;
    ir_temperature_31 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_31 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_31 = (int16_t)ir_temperature_31;
    ir_temperature_32 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_32 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_32 = (int16_t)ir_temperature_32;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rr_2_ir_temperature_30_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_30_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_2_ir_temperature_30_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rr_2_ir_temperature_31_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_31_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_2_ir_temperature_31_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rr_2_ir_temperature_32_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_32_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_2_ir_temperature_32_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_rl_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_rl_2_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_21;
    uint16_t ir_temperature_22;
    uint16_t ir_temperature_23;
    uint16_t ir_temperature_24;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_21 = (uint16_t)src_p->ir_temperature_21;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_21, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_21, 8u, 0xffu);
    ir_temperature_22 = (uint16_t)src_p->ir_temperature_22;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_22, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_22, 8u, 0xffu);
    ir_temperature_23 = (uint16_t)src_p->ir_temperature_23;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_23, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_23, 8u, 0xffu);
    ir_temperature_24 = (uint16_t)src_p->ir_temperature_24;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_24, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_24, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_rl_2_unpack(
    struct vehicle_rt_ir_temp_temp_rl_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_21;
    uint16_t ir_temperature_22;
    uint16_t ir_temperature_23;
    uint16_t ir_temperature_24;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_21 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_21 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_21 = (int16_t)ir_temperature_21;
    ir_temperature_22 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_22 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_22 = (int16_t)ir_temperature_22;
    ir_temperature_23 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_23 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_23 = (int16_t)ir_temperature_23;
    ir_temperature_24 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_24 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_24 = (int16_t)ir_temperature_24;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rl_2_ir_temperature_22_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_22_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_2_ir_temperature_22_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rl_2_ir_temperature_23_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_23_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_2_ir_temperature_23_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rl_2_ir_temperature_24_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_24_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_2_ir_temperature_24_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_fr_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_fr_2_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_13;
    uint16_t ir_temperature_14;
    uint16_t ir_temperature_15;
    uint16_t ir_temperature_16;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_13 = (uint16_t)src_p->ir_temperature_13;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_13, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_13, 8u, 0xffu);
    ir_temperature_14 = (uint16_t)src_p->ir_temperature_14;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_14, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_14, 8u, 0xffu);
    ir_temperature_15 = (uint16_t)src_p->ir_temperature_15;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_15, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_15, 8u, 0xffu);
    ir_temperature_16 = (uint16_t)src_p->ir_temperature_16;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_16, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_16, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_fr_2_unpack(
    struct vehicle_rt_ir_temp_temp_fr_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_13;
    uint16_t ir_temperature_14;
    uint16_t ir_temperature_15;
    uint16_t ir_temperature_16;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_13 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_13 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_13 = (int16_t)ir_temperature_13;
    ir_temperature_14 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_14 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_14 = (int16_t)ir_temperature_14;
    ir_temperature_15 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_15 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_15 = (int16_t)ir_temperature_15;
    ir_temperature_16 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_16 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_16 = (int16_t)ir_temperature_16;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fr_2_ir_temperature_14_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_14_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_2_ir_temperature_14_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fr_2_ir_temperature_15_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_15_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_2_ir_temperature_15_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fr_2_ir_temperature_16_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_16_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_2_ir_temperature_16_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_fl_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_fl_2_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_5;
    uint16_t ir_temperature_6;
    uint16_t ir_temperature_7;
    uint16_t ir_temperature_8;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_5 = (uint16_t)src_p->ir_temperature_5;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_5, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_5, 8u, 0xffu);
    ir_temperature_6 = (uint16_t)src_p->ir_temperature_6;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_6, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_6, 8u, 0xffu);
    ir_temperature_7 = (uint16_t)src_p->ir_temperature_7;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_7, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_7, 8u, 0xffu);
    ir_temperature_8 = (uint16_t)src_p->ir_temperature_8;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_8, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_8, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_fl_2_unpack(
    struct vehicle_rt_ir_temp_temp_fl_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_5;
    uint16_t ir_temperature_6;
    uint16_t ir_temperature_7;
    uint16_t ir_temperature_8;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_5 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_5 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_5 = (int16_t)ir_temperature_5;
    ir_temperature_6 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_6 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_6 = (int16_t)ir_temperature_6;
    ir_temperature_7 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_7 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_7 = (int16_t)ir_temperature_7;
    ir_temperature_8 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_8 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_8 = (int16_t)ir_temperature_8;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fl_2_ir_temperature_6_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_6_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_2_ir_temperature_6_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fl_2_ir_temperature_7_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_7_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_2_ir_temperature_7_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fl_2_ir_temperature_8_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_8_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_2_ir_temperature_8_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_rr_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_rr_1_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_25;
    uint16_t ir_temperature_26;
    uint16_t ir_temperature_27;
    uint16_t ir_temperature_28;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_25 = (uint16_t)src_p->ir_temperature_25;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_25, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_25, 8u, 0xffu);
    ir_temperature_26 = (uint16_t)src_p->ir_temperature_26;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_26, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_26, 8u, 0xffu);
    ir_temperature_27 = (uint16_t)src_p->ir_temperature_27;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_27, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_27, 8u, 0xffu);
    ir_temperature_28 = (uint16_t)src_p->ir_temperature_28;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_28, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_28, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_rr_1_unpack(
    struct vehicle_rt_ir_temp_temp_rr_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_25;
    uint16_t ir_temperature_26;
    uint16_t ir_temperature_27;
    uint16_t ir_temperature_28;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_25 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_25 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_25 = (int16_t)ir_temperature_25;
    ir_temperature_26 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_26 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_26 = (int16_t)ir_temperature_26;
    ir_temperature_27 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_27 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_27 = (int16_t)ir_temperature_27;
    ir_temperature_28 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_28 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_28 = (int16_t)ir_temperature_28;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rr_1_ir_temperature_26_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_26_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_1_ir_temperature_26_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rr_1_ir_temperature_27_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_27_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_1_ir_temperature_27_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rr_1_ir_temperature_28_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_28_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rr_1_ir_temperature_28_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_rl_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_rl_1_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_17;
    uint16_t ir_temperature_18;
    uint16_t ir_temperature_19;
    uint16_t ir_temperature_20;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_17 = (uint16_t)src_p->ir_temperature_17;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_17, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_17, 8u, 0xffu);
    ir_temperature_18 = (uint16_t)src_p->ir_temperature_18;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_18, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_18, 8u, 0xffu);
    ir_temperature_19 = (uint16_t)src_p->ir_temperature_19;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_19, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_19, 8u, 0xffu);
    ir_temperature_20 = (uint16_t)src_p->ir_temperature_20;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_20, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_20, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_rl_1_unpack(
    struct vehicle_rt_ir_temp_temp_rl_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_17;
    uint16_t ir_temperature_18;
    uint16_t ir_temperature_19;
    uint16_t ir_temperature_20;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_17 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_17 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_17 = (int16_t)ir_temperature_17;
    ir_temperature_18 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_18 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_18 = (int16_t)ir_temperature_18;
    ir_temperature_19 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_19 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_19 = (int16_t)ir_temperature_19;
    ir_temperature_20 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_20 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_20 = (int16_t)ir_temperature_20;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rl_1_ir_temperature_18_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_18_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_1_ir_temperature_18_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rl_1_ir_temperature_19_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_19_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_1_ir_temperature_19_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_rl_1_ir_temperature_20_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_20_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_rl_1_ir_temperature_20_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_fr_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_fr_1_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_10;
    uint16_t ir_temperature_11;
    uint16_t ir_temperature_12;
    uint16_t ir_temperature_9;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_9 = (uint16_t)src_p->ir_temperature_9;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_9, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_9, 8u, 0xffu);
    ir_temperature_10 = (uint16_t)src_p->ir_temperature_10;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_10, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_10, 8u, 0xffu);
    ir_temperature_11 = (uint16_t)src_p->ir_temperature_11;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_11, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_11, 8u, 0xffu);
    ir_temperature_12 = (uint16_t)src_p->ir_temperature_12;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_12, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_12, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_fr_1_unpack(
    struct vehicle_rt_ir_temp_temp_fr_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_10;
    uint16_t ir_temperature_11;
    uint16_t ir_temperature_12;
    uint16_t ir_temperature_9;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_9 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_9 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_9 = (int16_t)ir_temperature_9;
    ir_temperature_10 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_10 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_10 = (int16_t)ir_temperature_10;
    ir_temperature_11 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_11 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_11 = (int16_t)ir_temperature_11;
    ir_temperature_12 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_12 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_12 = (int16_t)ir_temperature_12;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fr_1_ir_temperature_10_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_10_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_1_ir_temperature_10_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fr_1_ir_temperature_11_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_11_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_1_ir_temperature_11_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fr_1_ir_temperature_12_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_12_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fr_1_ir_temperature_12_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_fl_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_fl_1_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_1;
    uint16_t ir_temperature_2;
    uint16_t ir_temperature_3;
    uint16_t ir_temperature_4;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ir_temperature_1 = (uint16_t)src_p->ir_temperature_1;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_1, 8u, 0xffu);
    ir_temperature_2 = (uint16_t)src_p->ir_temperature_2;
    dst_p[2] |= pack_left_shift_u16(ir_temperature_2, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ir_temperature_2, 8u, 0xffu);
    ir_temperature_3 = (uint16_t)src_p->ir_temperature_3;
    dst_p[4] |= pack_left_shift_u16(ir_temperature_3, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(ir_temperature_3, 8u, 0xffu);
    ir_temperature_4 = (uint16_t)src_p->ir_temperature_4;
    dst_p[6] |= pack_left_shift_u16(ir_temperature_4, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(ir_temperature_4, 8u, 0xffu);

    return (8);
}

int vehicle_rt_ir_temp_temp_fl_1_unpack(
    struct vehicle_rt_ir_temp_temp_fl_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_1;
    uint16_t ir_temperature_2;
    uint16_t ir_temperature_3;
    uint16_t ir_temperature_4;

    if (size < 8u) {
        return (-EINVAL);
    }

    ir_temperature_1 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_1 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_1 = (int16_t)ir_temperature_1;
    ir_temperature_2 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ir_temperature_2 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ir_temperature_2 = (int16_t)ir_temperature_2;
    ir_temperature_3 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    ir_temperature_3 |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->ir_temperature_3 = (int16_t)ir_temperature_3;
    ir_temperature_4 = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    ir_temperature_4 |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->ir_temperature_4 = (int16_t)ir_temperature_4;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fl_1_ir_temperature_2_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_2_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_1_ir_temperature_2_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fl_1_ir_temperature_3_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_3_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_1_ir_temperature_3_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_ir_temp_temp_fl_1_ir_temperature_4_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_4_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_fl_1_ir_temperature_4_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_32_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_32_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_32;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_32 = (uint16_t)src_p->ir_temperature_32;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_32, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_32, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_32_unpack(
    struct vehicle_rt_ir_temp_temp_32_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_32;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_32 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_32 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_32 = (int16_t)ir_temperature_32;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_32_ir_temperature_32_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_32_ir_temperature_32_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_32_ir_temperature_32_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_31_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_31_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_31;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_31 = (uint16_t)src_p->ir_temperature_31;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_31, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_31, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_31_unpack(
    struct vehicle_rt_ir_temp_temp_31_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_31;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_31 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_31 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_31 = (int16_t)ir_temperature_31;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_31_ir_temperature_31_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_31_ir_temperature_31_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_31_ir_temperature_31_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_30_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_30_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_30;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_30 = (uint16_t)src_p->ir_temperature_30;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_30, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_30, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_30_unpack(
    struct vehicle_rt_ir_temp_temp_30_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_30;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_30 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_30 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_30 = (int16_t)ir_temperature_30;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_30_ir_temperature_30_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_30_ir_temperature_30_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_30_ir_temperature_30_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_29_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_29_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_29;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_29 = (uint16_t)src_p->ir_temperature_29;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_29, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_29, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_29_unpack(
    struct vehicle_rt_ir_temp_temp_29_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_29;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_29 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_29 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_29 = (int16_t)ir_temperature_29;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_29_ir_temperature_29_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_29_ir_temperature_29_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_29_ir_temperature_29_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_28_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_28_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_28;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_28 = (uint16_t)src_p->ir_temperature_28;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_28, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_28, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_28_unpack(
    struct vehicle_rt_ir_temp_temp_28_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_28;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_28 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_28 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_28 = (int16_t)ir_temperature_28;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_28_ir_temperature_28_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_28_ir_temperature_28_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_28_ir_temperature_28_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_27_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_27_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_27;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_27 = (uint16_t)src_p->ir_temperature_27;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_27, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_27, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_27_unpack(
    struct vehicle_rt_ir_temp_temp_27_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_27;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_27 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_27 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_27 = (int16_t)ir_temperature_27;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_27_ir_temperature_27_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_27_ir_temperature_27_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_27_ir_temperature_27_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_26_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_26_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_26;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_26 = (uint16_t)src_p->ir_temperature_26;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_26, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_26, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_26_unpack(
    struct vehicle_rt_ir_temp_temp_26_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_26;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_26 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_26 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_26 = (int16_t)ir_temperature_26;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_26_ir_temperature_26_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_26_ir_temperature_26_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_26_ir_temperature_26_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_25_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_25_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_25;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_25 = (uint16_t)src_p->ir_temperature_25;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_25, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_25, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_25_unpack(
    struct vehicle_rt_ir_temp_temp_25_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_25;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_25 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_25 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_25 = (int16_t)ir_temperature_25;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_25_ir_temperature_25_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_25_ir_temperature_25_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_25_ir_temperature_25_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_24_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_24_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_24;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_24 = (uint16_t)src_p->ir_temperature_24;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_24, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_24, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_24_unpack(
    struct vehicle_rt_ir_temp_temp_24_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_24;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_24 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_24 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_24 = (int16_t)ir_temperature_24;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_24_ir_temperature_24_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_24_ir_temperature_24_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_24_ir_temperature_24_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_22_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_22_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_22;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_22 = (uint16_t)src_p->ir_temperature_22;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_22, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_22, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_22_unpack(
    struct vehicle_rt_ir_temp_temp_22_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_22;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_22 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_22 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_22 = (int16_t)ir_temperature_22;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_22_ir_temperature_22_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_22_ir_temperature_22_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_22_ir_temperature_22_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_23_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_23_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_23;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_23 = (uint16_t)src_p->ir_temperature_23;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_23, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_23, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_23_unpack(
    struct vehicle_rt_ir_temp_temp_23_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_23;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_23 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_23 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_23 = (int16_t)ir_temperature_23;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_23_ir_temperature_23_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_23_ir_temperature_23_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_23_ir_temperature_23_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_21_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_21_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_21;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_21 = (uint16_t)src_p->ir_temperature_21;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_21, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_21, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_21_unpack(
    struct vehicle_rt_ir_temp_temp_21_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_21;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_21 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_21 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_21 = (int16_t)ir_temperature_21;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_21_ir_temperature_21_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_21_ir_temperature_21_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_21_ir_temperature_21_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_20_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_20_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_20;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_20 = (uint16_t)src_p->ir_temperature_20;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_20, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_20, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_20_unpack(
    struct vehicle_rt_ir_temp_temp_20_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_20;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_20 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_20 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_20 = (int16_t)ir_temperature_20;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_20_ir_temperature_20_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_20_ir_temperature_20_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_20_ir_temperature_20_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_19_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_19_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_19;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_19 = (uint16_t)src_p->ir_temperature_19;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_19, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_19, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_19_unpack(
    struct vehicle_rt_ir_temp_temp_19_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_19;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_19 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_19 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_19 = (int16_t)ir_temperature_19;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_19_ir_temperature_19_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_19_ir_temperature_19_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_19_ir_temperature_19_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_18_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_18_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_18;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_18 = (uint16_t)src_p->ir_temperature_18;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_18, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_18, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_18_unpack(
    struct vehicle_rt_ir_temp_temp_18_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_18;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_18 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_18 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_18 = (int16_t)ir_temperature_18;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_18_ir_temperature_18_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_18_ir_temperature_18_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_18_ir_temperature_18_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_16_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_16_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_16;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_16 = (uint16_t)src_p->ir_temperature_16;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_16, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_16, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_16_unpack(
    struct vehicle_rt_ir_temp_temp_16_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_16;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_16 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_16 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_16 = (int16_t)ir_temperature_16;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_16_ir_temperature_16_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_16_ir_temperature_16_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_16_ir_temperature_16_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_15_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_15_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_15;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_15 = (uint16_t)src_p->ir_temperature_15;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_15, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_15, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_15_unpack(
    struct vehicle_rt_ir_temp_temp_15_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_15;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_15 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_15 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_15 = (int16_t)ir_temperature_15;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_15_ir_temperature_15_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_15_ir_temperature_15_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_15_ir_temperature_15_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_14_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_14_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_14;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_14 = (uint16_t)src_p->ir_temperature_14;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_14, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_14, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_14_unpack(
    struct vehicle_rt_ir_temp_temp_14_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_14;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_14 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_14 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_14 = (int16_t)ir_temperature_14;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_14_ir_temperature_14_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_14_ir_temperature_14_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_14_ir_temperature_14_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_13_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_13_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_13;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_13 = (uint16_t)src_p->ir_temperature_13;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_13, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_13, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_13_unpack(
    struct vehicle_rt_ir_temp_temp_13_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_13;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_13 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_13 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_13 = (int16_t)ir_temperature_13;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_13_ir_temperature_13_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_13_ir_temperature_13_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_13_ir_temperature_13_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_12_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_12_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_12;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_12 = (uint16_t)src_p->ir_temperature_12;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_12, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_12, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_12_unpack(
    struct vehicle_rt_ir_temp_temp_12_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_12;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_12 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_12 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_12 = (int16_t)ir_temperature_12;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_12_ir_temperature_12_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_12_ir_temperature_12_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_12_ir_temperature_12_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_11_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_11_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_11;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_11 = (uint16_t)src_p->ir_temperature_11;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_11, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_11, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_11_unpack(
    struct vehicle_rt_ir_temp_temp_11_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_11;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_11 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_11 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_11 = (int16_t)ir_temperature_11;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_11_ir_temperature_11_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_11_ir_temperature_11_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_11_ir_temperature_11_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_10_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_10_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_10;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_10 = (uint16_t)src_p->ir_temperature_10;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_10, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_10, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_10_unpack(
    struct vehicle_rt_ir_temp_temp_10_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_10;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_10 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_10 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_10 = (int16_t)ir_temperature_10;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_10_ir_temperature_10_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_10_ir_temperature_10_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_10_ir_temperature_10_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_8_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_8;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_8 = (uint16_t)src_p->ir_temperature_8;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_8, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_8, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_8_unpack(
    struct vehicle_rt_ir_temp_temp_8_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_8;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_8 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_8 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_8 = (int16_t)ir_temperature_8;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_8_ir_temperature_8_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_8_ir_temperature_8_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_8_ir_temperature_8_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_9_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_9_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_9;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_9 = (uint16_t)src_p->ir_temperature_9;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_9, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_9, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_9_unpack(
    struct vehicle_rt_ir_temp_temp_9_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_9;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_9 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_9 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_9 = (int16_t)ir_temperature_9;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_9_ir_temperature_9_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_9_ir_temperature_9_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_9_ir_temperature_9_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_17_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_17_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_17;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_17 = (uint16_t)src_p->ir_temperature_17;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_17, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_17, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_17_unpack(
    struct vehicle_rt_ir_temp_temp_17_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_17;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_17 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_17 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_17 = (int16_t)ir_temperature_17;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_17_ir_temperature_17_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_17_ir_temperature_17_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_17_ir_temperature_17_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_6_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_6_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_6;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_6 = (uint16_t)src_p->ir_temperature_6;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_6, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_6, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_6_unpack(
    struct vehicle_rt_ir_temp_temp_6_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_6;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_6 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_6 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_6 = (int16_t)ir_temperature_6;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_6_ir_temperature_6_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_6_ir_temperature_6_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_6_ir_temperature_6_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_5_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_5_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_5;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_5 = (uint16_t)src_p->ir_temperature_5;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_5, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_5, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_5_unpack(
    struct vehicle_rt_ir_temp_temp_5_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_5;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_5 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_5 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_5 = (int16_t)ir_temperature_5;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_5_ir_temperature_5_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_5_ir_temperature_5_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_5_ir_temperature_5_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_4_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_4_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_4;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_4 = (uint16_t)src_p->ir_temperature_4;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_4, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_4, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_4_unpack(
    struct vehicle_rt_ir_temp_temp_4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_4;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_4 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_4 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_4 = (int16_t)ir_temperature_4;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_4_ir_temperature_4_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_4_ir_temperature_4_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_4_ir_temperature_4_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_3_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_3;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_3 = (uint16_t)src_p->ir_temperature_3;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_3, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_3_unpack(
    struct vehicle_rt_ir_temp_temp_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_3;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_3 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_3 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_3 = (int16_t)ir_temperature_3;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_3_ir_temperature_3_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_3_ir_temperature_3_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_3_ir_temperature_3_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_2_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_2;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_2 = (uint16_t)src_p->ir_temperature_2;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_2, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_2_unpack(
    struct vehicle_rt_ir_temp_temp_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_2;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_2 = (int16_t)ir_temperature_2;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_2_ir_temperature_2_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_2_ir_temperature_2_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_2_ir_temperature_2_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_ir_temp_temp_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_ir_temp_temp_1_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_1;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    ir_temperature_1 = (uint16_t)src_p->ir_temperature_1;
    dst_p[0] |= pack_left_shift_u16(ir_temperature_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(ir_temperature_1, 8u, 0xffu);

    return (2);
}

int vehicle_rt_ir_temp_temp_1_unpack(
    struct vehicle_rt_ir_temp_temp_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ir_temperature_1;

    if (size < 2u) {
        return (-EINVAL);
    }

    ir_temperature_1 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    ir_temperature_1 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->ir_temperature_1 = (int16_t)ir_temperature_1;

    return (0);
}

int16_t vehicle_rt_ir_temp_temp_1_ir_temperature_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_1_ir_temperature_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_ir_temp_temp_1_ir_temperature_1_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_trig_final_condition_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_trig_final_condition_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_final_speed, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u32(src_p->final_speed, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->final_speed, 8u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->final_speed, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_trig_final_condition_unpack(
    struct vehicle_rt_sb_trig_final_condition_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_final_speed = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->final_speed = unpack_right_shift_u32(src_p[1], 0u, 0xffu);
    dst_p->final_speed |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    dst_p->final_speed |= unpack_left_shift_u32(src_p[3], 16u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_trig_final_condition_validity_final_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_final_condition_validity_final_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_final_condition_validity_final_speed_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint32_t vehicle_rt_sb_trig_final_condition_final_speed_encode(double value)
{
    return (uint32_t)(value / 0.0001);
}

double vehicle_rt_sb_trig_final_condition_final_speed_decode(uint32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_trig_final_condition_final_speed_is_in_range(uint32_t value)
{
    return (value <= 16750000u);
}

int vehicle_rt_sb_trig_initial_condition_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_trig_initial_condition_t *src_p,
    size_t size)
{
    uint16_t initial_heading;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_initial_speed, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_initial_heading, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u32(src_p->initial_speed, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->initial_speed, 8u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->initial_speed, 16u, 0xffu);
    initial_heading = (uint16_t)src_p->initial_heading;
    dst_p[4] |= pack_left_shift_u16(initial_heading, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(initial_heading, 8u, 0xffu);
    dst_p[6] |= pack_left_shift_u8(src_p->mfdd_start_threshold, 0u, 0xffu);
    dst_p[7] |= pack_left_shift_u8(src_p->mfdd_end_threshold, 0u, 0xffu);

    return (8);
}

int vehicle_rt_sb_trig_initial_condition_unpack(
    struct vehicle_rt_sb_trig_initial_condition_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t initial_heading;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_initial_speed = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_initial_heading = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->initial_speed = unpack_right_shift_u32(src_p[1], 0u, 0xffu);
    dst_p->initial_speed |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    dst_p->initial_speed |= unpack_left_shift_u32(src_p[3], 16u, 0xffu);
    initial_heading = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    initial_heading |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->initial_heading = (int16_t)initial_heading;
    dst_p->mfdd_start_threshold = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
    dst_p->mfdd_end_threshold = unpack_right_shift_u8(src_p[7], 0u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_trig_initial_condition_validity_initial_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_initial_condition_validity_initial_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_initial_condition_validity_initial_speed_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_trig_initial_condition_validity_initial_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_initial_condition_validity_initial_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_initial_condition_validity_initial_heading_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint32_t vehicle_rt_sb_trig_initial_condition_initial_speed_encode(double value)
{
    return (uint32_t)(value / 0.0001);
}

double vehicle_rt_sb_trig_initial_condition_initial_speed_decode(uint32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_trig_initial_condition_initial_speed_is_in_range(uint32_t value)
{
    return (value <= 16750000u);
}

int16_t vehicle_rt_sb_trig_initial_condition_initial_heading_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_trig_initial_condition_initial_heading_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_trig_initial_condition_initial_heading_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

uint8_t vehicle_rt_sb_trig_initial_condition_mfdd_start_threshold_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_initial_condition_mfdd_start_threshold_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_initial_condition_mfdd_start_threshold_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

uint8_t vehicle_rt_sb_trig_initial_condition_mfdd_end_threshold_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_initial_condition_mfdd_end_threshold_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_initial_condition_mfdd_end_threshold_is_in_range(uint8_t value)
{
    return (value <= 100u);
}

int vehicle_rt_sb_trig_direct_dist_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_trig_direct_dist_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u32(src_p->direct_distance, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->direct_distance, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->direct_distance, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->direct_distance, 24u, 0xffu);
    dst_p[4] |= pack_left_shift_u32(src_p->path_distance_2_d, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(src_p->path_distance_2_d, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(src_p->path_distance_2_d, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(src_p->path_distance_2_d, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_trig_direct_dist_unpack(
    struct vehicle_rt_sb_trig_direct_dist_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->direct_distance = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->direct_distance |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->direct_distance |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    dst_p->direct_distance |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->path_distance_2_d = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    dst_p->path_distance_2_d |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    dst_p->path_distance_2_d |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    dst_p->path_distance_2_d |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_sb_trig_direct_dist_direct_distance_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_direct_dist_direct_distance_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_trig_direct_dist_direct_distance_is_in_range(uint32_t value)
{
    return (value <= 4294967000u);
}

uint32_t vehicle_rt_sb_trig_direct_dist_path_distance_2_d_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_direct_dist_path_distance_2_d_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_trig_direct_dist_path_distance_2_d_is_in_range(uint32_t value)
{
    return (value <= 4294967000u);
}

int vehicle_rt_sb_trig_forward_dist_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_trig_forward_dist_t *src_p,
    size_t size)
{
    uint32_t deviation_distance;
    uint32_t forward_distance;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    forward_distance = (uint32_t)src_p->forward_distance;
    dst_p[0] |= pack_left_shift_u32(forward_distance, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(forward_distance, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(forward_distance, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(forward_distance, 24u, 0xffu);
    deviation_distance = (uint32_t)src_p->deviation_distance;
    dst_p[4] |= pack_left_shift_u32(deviation_distance, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(deviation_distance, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(deviation_distance, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(deviation_distance, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_trig_forward_dist_unpack(
    struct vehicle_rt_sb_trig_forward_dist_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t deviation_distance;
    uint32_t forward_distance;

    if (size < 8u) {
        return (-EINVAL);
    }

    forward_distance = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    forward_distance |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    forward_distance |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    forward_distance |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->forward_distance = (int32_t)forward_distance;
    deviation_distance = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    deviation_distance |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    deviation_distance |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    deviation_distance |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->deviation_distance = (int32_t)deviation_distance;

    return (0);
}

int32_t vehicle_rt_sb_trig_forward_dist_forward_distance_encode(double value)
{
    return (int32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_forward_dist_forward_distance_decode(int32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_trig_forward_dist_forward_distance_is_in_range(int32_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_trig_forward_dist_deviation_distance_encode(double value)
{
    return (int32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_forward_dist_deviation_distance_decode(int32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_trig_forward_dist_deviation_distance_is_in_range(int32_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_trig_path_dist_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_trig_path_dist_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u32(src_p->path_distance_3_d, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->path_distance_3_d, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->path_distance_3_d, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->path_distance_3_d, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_trig_path_dist_unpack(
    struct vehicle_rt_sb_trig_path_dist_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->path_distance_3_d = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->path_distance_3_d |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->path_distance_3_d |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    dst_p->path_distance_3_d |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_sb_trig_path_dist_path_distance_3_d_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_path_dist_path_distance_3_d_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_trig_path_dist_path_distance_3_d_is_in_range(uint32_t value)
{
    return (value <= 4294967000u);
}

int vehicle_rt_sb_trig_accel_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_trig_accel_t *src_p,
    size_t size)
{
    uint16_t average_accel;
    uint16_t mfdd;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_mfdd, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_average_accel, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_triggered_time, 2u, 0x04u);
    mfdd = (uint16_t)src_p->mfdd;
    dst_p[1] |= pack_left_shift_u16(mfdd, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u16(mfdd, 8u, 0xffu);
    average_accel = (uint16_t)src_p->average_accel;
    dst_p[3] |= pack_left_shift_u16(average_accel, 0u, 0xffu);
    dst_p[4] |= pack_right_shift_u16(average_accel, 8u, 0xffu);
    dst_p[5] |= pack_left_shift_u32(src_p->triggered_time, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(src_p->triggered_time, 8u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(src_p->triggered_time, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_trig_accel_unpack(
    struct vehicle_rt_sb_trig_accel_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t average_accel;
    uint16_t mfdd;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_mfdd = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_average_accel = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_triggered_time = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    mfdd = unpack_right_shift_u16(src_p[1], 0u, 0xffu);
    mfdd |= unpack_left_shift_u16(src_p[2], 8u, 0xffu);
    dst_p->mfdd = (int16_t)mfdd;
    average_accel = unpack_right_shift_u16(src_p[3], 0u, 0xffu);
    average_accel |= unpack_left_shift_u16(src_p[4], 8u, 0xffu);
    dst_p->average_accel = (int16_t)average_accel;
    dst_p->triggered_time = unpack_right_shift_u32(src_p[5], 0u, 0xffu);
    dst_p->triggered_time |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    dst_p->triggered_time |= unpack_left_shift_u32(src_p[7], 16u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_trig_accel_validity_mfdd_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_accel_validity_mfdd_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_accel_validity_mfdd_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_trig_accel_validity_average_accel_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_accel_validity_average_accel_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_accel_validity_average_accel_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_trig_accel_validity_triggered_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_accel_validity_triggered_time_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trig_accel_validity_triggered_time_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

int16_t vehicle_rt_sb_trig_accel_mfdd_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_trig_accel_mfdd_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_trig_accel_mfdd_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_trig_accel_average_accel_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_trig_accel_average_accel_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_trig_accel_average_accel_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

uint32_t vehicle_rt_sb_trig_accel_triggered_time_encode(double value)
{
    return (uint32_t)(value / 0.01);
}

double vehicle_rt_sb_trig_accel_triggered_time_decode(uint32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_trig_accel_triggered_time_is_in_range(uint32_t value)
{
    return (value <= 16777200u);
}

int vehicle_rt_dl1_mk3_measure_time_12_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_12_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_12, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_12, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_12, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_12_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_12_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_12 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_12 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_12 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_11_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_11_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_11, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_11, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_11, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_11_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_11_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_11 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_11 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_11 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_10_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_10_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_10, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_10, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_10, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_10_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_10_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_10 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_10 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_10 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_9_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_9_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_9, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_9, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_9, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_9_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_9_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_9 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_9 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_9 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_8_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_8, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_8, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_8, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_8_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_8_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_8 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_8 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_8 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_7_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_7_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_7, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_7, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_7, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_7_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_7_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_7 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_7 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_7 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_6_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_6_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_6, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_6, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_6, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_6_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_6_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_6 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_6 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_6 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_5_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_5_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_5, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_5, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_5, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_5_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_5_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_5 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_5 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_5 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_4_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_4_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_4, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_4, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_4, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_4_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_4 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_4 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_4 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_3_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_3, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_3, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_3_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_3 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_3 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_3 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_2_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_2, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_2, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_2_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_2 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_2 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_2 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_measure_time_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_measure_time_1_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->measured_time_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->measured_time_1, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->measured_time_1, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_measure_time_1_unpack(
    struct vehicle_rt_dl1_mk3_measure_time_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->measured_time_1 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->measured_time_1 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->measured_time_1 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_decode(uint32_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_rpm_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_rpm_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->rpm, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->rpm, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_rpm_unpack(
    struct vehicle_rt_dl1_mk3_rpm_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->rpm = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->rpm |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_rpm_rpm_encode(double value)
{
    return (uint16_t)(value);
}

double vehicle_rt_dl1_mk3_rpm_rpm_decode(uint16_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_rpm_rpm_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_freq_4_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_freq_4_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->frequency_4, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->frequency_4, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_freq_4_unpack(
    struct vehicle_rt_dl1_mk3_freq_4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->frequency_4 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->frequency_4 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_freq_4_frequency_4_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_freq_4_frequency_4_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_freq_4_frequency_4_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_freq_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_freq_3_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->frequency_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->frequency_3, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_freq_3_unpack(
    struct vehicle_rt_dl1_mk3_freq_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->frequency_3 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->frequency_3 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_freq_3_frequency_3_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_freq_3_frequency_3_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_freq_3_frequency_3_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_freq_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_freq_2_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->frequency_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->frequency_2, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_freq_2_unpack(
    struct vehicle_rt_dl1_mk3_freq_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->frequency_2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->frequency_2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_freq_2_frequency_2_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_freq_2_frequency_2_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_freq_2_frequency_2_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_misc_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_misc_3_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->misc_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->misc_3, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_misc_3_unpack(
    struct vehicle_rt_dl1_mk3_misc_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->misc_3 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->misc_3 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_misc_3_misc_3_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_misc_3_misc_3_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_misc_3_misc_3_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_misc_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_misc_2_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->misc_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->misc_2, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_misc_2_unpack(
    struct vehicle_rt_dl1_mk3_misc_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->misc_2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->misc_2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_misc_2_misc_2_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_misc_2_misc_2_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_misc_2_misc_2_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_misc_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_misc_1_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->misc_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->misc_1, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_misc_1_unpack(
    struct vehicle_rt_dl1_mk3_misc_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->misc_1 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->misc_1 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_misc_1_misc_1_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_misc_1_misc_1_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_misc_1_misc_1_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_31_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_31_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_31, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_31, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_31_unpack(
    struct vehicle_rt_dl1_mk3_aux_31_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_31 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_31 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_31_aux_31_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_31_aux_31_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_31_aux_31_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_30_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_30_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_30, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_30, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_30_unpack(
    struct vehicle_rt_dl1_mk3_aux_30_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_30 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_30 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_30_aux_30_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_30_aux_30_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_30_aux_30_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_29_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_29_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_29, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_29, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_29_unpack(
    struct vehicle_rt_dl1_mk3_aux_29_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_29 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_29 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_29_aux_29_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_29_aux_29_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_29_aux_29_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_28_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_28_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_28, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_28, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_28_unpack(
    struct vehicle_rt_dl1_mk3_aux_28_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_28 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_28 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_28_aux_28_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_28_aux_28_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_28_aux_28_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_27_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_27_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_27, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_27, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_27_unpack(
    struct vehicle_rt_dl1_mk3_aux_27_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_27 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_27 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_27_aux_27_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_27_aux_27_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_27_aux_27_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_26_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_26_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_26, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_26, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_26_unpack(
    struct vehicle_rt_dl1_mk3_aux_26_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_26 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_26 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_26_aux_26_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_26_aux_26_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_26_aux_26_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_25_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_25_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_25, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_25, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_25_unpack(
    struct vehicle_rt_dl1_mk3_aux_25_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_25 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_25 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_25_aux_25_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_25_aux_25_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_25_aux_25_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_24_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_24_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_24, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_24, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_24_unpack(
    struct vehicle_rt_dl1_mk3_aux_24_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_24 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_24 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_24_aux_24_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_24_aux_24_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_24_aux_24_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_23_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_23_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_23, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_23, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_23_unpack(
    struct vehicle_rt_dl1_mk3_aux_23_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_23 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_23 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_23_aux_23_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_23_aux_23_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_23_aux_23_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_22_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_22_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_22, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_22, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_22_unpack(
    struct vehicle_rt_dl1_mk3_aux_22_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_22 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_22 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_22_aux_22_encode(double value)
{
    return (uint16_t)(value / 10.0);
}

double vehicle_rt_dl1_mk3_aux_22_aux_22_decode(uint16_t value)
{
    return ((double)value * 10.0);
}

bool vehicle_rt_dl1_mk3_aux_22_aux_22_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_21_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_21_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_21, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_21, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_21_unpack(
    struct vehicle_rt_dl1_mk3_aux_21_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_21 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_21 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_21_aux_21_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_21_aux_21_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_21_aux_21_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_20_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_20_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_20, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_20, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_20_unpack(
    struct vehicle_rt_dl1_mk3_aux_20_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_20 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_20 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_20_aux_20_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_20_aux_20_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_20_aux_20_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_19_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_19_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_19, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_19, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_19_unpack(
    struct vehicle_rt_dl1_mk3_aux_19_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_19 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_19 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_19_aux_19_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_19_aux_19_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_19_aux_19_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_18_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_18_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_18, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_18, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_18_unpack(
    struct vehicle_rt_dl1_mk3_aux_18_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_18 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_18 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_18_aux_18_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_18_aux_18_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_18_aux_18_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_17_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_17_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_17, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_17, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_17_unpack(
    struct vehicle_rt_dl1_mk3_aux_17_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_17 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_17 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_17_aux_17_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_17_aux_17_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_17_aux_17_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_16_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_16_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_16, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_16, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_16_unpack(
    struct vehicle_rt_dl1_mk3_aux_16_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_16 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_16 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_16_aux_16_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_16_aux_16_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_16_aux_16_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_15_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_15_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_15, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_15, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_15_unpack(
    struct vehicle_rt_dl1_mk3_aux_15_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_15 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_15 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_15_aux_15_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_15_aux_15_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_15_aux_15_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_14_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_14_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_14, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_14, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_14_unpack(
    struct vehicle_rt_dl1_mk3_aux_14_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_14 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_14 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_14_aux_14_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_14_aux_14_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_14_aux_14_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_13_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_13_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_13, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_13, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_13_unpack(
    struct vehicle_rt_dl1_mk3_aux_13_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_13 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_13 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_13_aux_13_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_13_aux_13_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_13_aux_13_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_12_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_12_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_12, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_12, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_12_unpack(
    struct vehicle_rt_dl1_mk3_aux_12_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_12 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_12 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_12_aux_12_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_12_aux_12_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_12_aux_12_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_11_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_11_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_11, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_11, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_11_unpack(
    struct vehicle_rt_dl1_mk3_aux_11_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_11 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_11 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_11_aux_11_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_11_aux_11_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_11_aux_11_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_9_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_9_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_9, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_9, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_9_unpack(
    struct vehicle_rt_dl1_mk3_aux_9_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_9 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_9 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_9_aux_9_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_9_aux_9_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_9_aux_9_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_10_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_10_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_10, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_10, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_10_unpack(
    struct vehicle_rt_dl1_mk3_aux_10_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_10 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_10 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_10_aux_10_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_10_aux_10_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_10_aux_10_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_8_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_8, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_8, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_8_unpack(
    struct vehicle_rt_dl1_mk3_aux_8_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_8 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_8 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_8_aux_8_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_8_aux_8_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_8_aux_8_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_7_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_7_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_7, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_7, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_7_unpack(
    struct vehicle_rt_dl1_mk3_aux_7_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_7 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_7 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_7_aux_7_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_7_aux_7_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_7_aux_7_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_6_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_6_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_6, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_6, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_6_unpack(
    struct vehicle_rt_dl1_mk3_aux_6_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_6 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_6 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_6_aux_6_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_6_aux_6_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_6_aux_6_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_5_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_5_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_5, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_5, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_5_unpack(
    struct vehicle_rt_dl1_mk3_aux_5_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_5 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_5 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_5_aux_5_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_5_aux_5_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_5_aux_5_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_4_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_4_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_4, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_4, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_4_unpack(
    struct vehicle_rt_dl1_mk3_aux_4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_4 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_4 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_4_aux_4_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_4_aux_4_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_4_aux_4_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_3_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_3, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_3_unpack(
    struct vehicle_rt_dl1_mk3_aux_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_3 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_3 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_3_aux_3_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_3_aux_3_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_3_aux_3_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_2_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_2, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_2_unpack(
    struct vehicle_rt_dl1_mk3_aux_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_2_aux_2_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_2_aux_2_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_2_aux_2_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_aux_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_aux_1_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_left_shift_u16(src_p->aux_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(src_p->aux_1, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_aux_1_unpack(
    struct vehicle_rt_dl1_mk3_aux_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->aux_1 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    dst_p->aux_1 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_aux_1_aux_1_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_1_aux_1_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_aux_1_aux_1_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_pressure_5_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_pressure_5_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->pressure_5, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->pressure_5, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->pressure_5, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_pressure_5_unpack(
    struct vehicle_rt_dl1_mk3_pressure_5_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->pressure_5 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->pressure_5 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->pressure_5 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_pressure_5_pressure_5_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_5_pressure_5_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_pressure_5_pressure_5_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_pressure_4_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_pressure_4_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->pressure_4, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->pressure_4, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->pressure_4, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_pressure_4_unpack(
    struct vehicle_rt_dl1_mk3_pressure_4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->pressure_4 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->pressure_4 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->pressure_4 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_pressure_4_pressure_4_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_4_pressure_4_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_pressure_4_pressure_4_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_pressure_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_pressure_3_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->pressure_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->pressure_3, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->pressure_3, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_pressure_3_unpack(
    struct vehicle_rt_dl1_mk3_pressure_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->pressure_3 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->pressure_3 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->pressure_3 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_pressure_3_pressure_3_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_3_pressure_3_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_pressure_3_pressure_3_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_pressure_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_pressure_2_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->pressure_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->pressure_2, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->pressure_2, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_pressure_2_unpack(
    struct vehicle_rt_dl1_mk3_pressure_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->pressure_2 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->pressure_2 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->pressure_2 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_pressure_2_pressure_2_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_2_pressure_2_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_pressure_2_pressure_2_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_pressure_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_pressure_1_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u32(src_p->pressure_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(src_p->pressure_1, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->pressure_1, 16u, 0xffu);

    return (3);
}

int vehicle_rt_dl1_mk3_pressure_1_unpack(
    struct vehicle_rt_dl1_mk3_pressure_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->pressure_1 = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    dst_p->pressure_1 |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    dst_p->pressure_1 |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    return (0);
}

uint32_t vehicle_rt_dl1_mk3_pressure_1_pressure_1_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_1_pressure_1_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_pressure_1_pressure_1_is_in_range(uint32_t value)
{
    return (value <= 16777215u);
}

int vehicle_rt_dl1_mk3_angle_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_angle_3_t *src_p,
    size_t size)
{
    uint16_t angle_3;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    angle_3 = (uint16_t)src_p->angle_3;
    dst_p[0] |= pack_left_shift_u16(angle_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(angle_3, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_angle_3_unpack(
    struct vehicle_rt_dl1_mk3_angle_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t angle_3;

    if (size < 2u) {
        return (-EINVAL);
    }

    angle_3 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    angle_3 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->angle_3 = (int16_t)angle_3;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_angle_3_angle_3_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_angle_3_angle_3_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_angle_3_angle_3_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_angle_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_angle_2_t *src_p,
    size_t size)
{
    uint16_t angle_2;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    angle_2 = (uint16_t)src_p->angle_2;
    dst_p[0] |= pack_left_shift_u16(angle_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(angle_2, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_angle_2_unpack(
    struct vehicle_rt_dl1_mk3_angle_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t angle_2;

    if (size < 2u) {
        return (-EINVAL);
    }

    angle_2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    angle_2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->angle_2 = (int16_t)angle_2;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_angle_2_angle_2_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_angle_2_angle_2_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_angle_2_angle_2_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_angle_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_angle_1_t *src_p,
    size_t size)
{
    uint16_t angle_1;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    angle_1 = (uint16_t)src_p->angle_1;
    dst_p[0] |= pack_left_shift_u16(angle_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(angle_1, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_angle_1_unpack(
    struct vehicle_rt_dl1_mk3_angle_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t angle_1;

    if (size < 2u) {
        return (-EINVAL);
    }

    angle_1 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    angle_1 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->angle_1 = (int16_t)angle_1;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_angle_1_angle_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_angle_1_angle_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_angle_1_angle_1_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_25_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_25_t *src_p,
    size_t size)
{
    uint16_t temperature_25;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_25 = (uint16_t)src_p->temperature_25;
    dst_p[0] |= pack_left_shift_u16(temperature_25, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_25, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_25_unpack(
    struct vehicle_rt_dl1_mk3_temp_25_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_25;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_25 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_25 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_25 = (int16_t)temperature_25;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_25_temperature_25_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_25_temperature_25_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_25_temperature_25_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_24_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_24_t *src_p,
    size_t size)
{
    uint16_t temperature_24;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_24 = (uint16_t)src_p->temperature_24;
    dst_p[0] |= pack_left_shift_u16(temperature_24, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_24, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_24_unpack(
    struct vehicle_rt_dl1_mk3_temp_24_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_24;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_24 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_24 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_24 = (int16_t)temperature_24;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_24_temperature_24_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_24_temperature_24_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_24_temperature_24_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_23_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_23_t *src_p,
    size_t size)
{
    uint16_t temperature_23;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_23 = (uint16_t)src_p->temperature_23;
    dst_p[0] |= pack_left_shift_u16(temperature_23, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_23, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_23_unpack(
    struct vehicle_rt_dl1_mk3_temp_23_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_23;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_23 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_23 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_23 = (int16_t)temperature_23;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_23_temperature_23_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_23_temperature_23_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_23_temperature_23_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_22_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_22_t *src_p,
    size_t size)
{
    uint16_t temperature_22;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_22 = (uint16_t)src_p->temperature_22;
    dst_p[0] |= pack_left_shift_u16(temperature_22, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_22, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_22_unpack(
    struct vehicle_rt_dl1_mk3_temp_22_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_22;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_22 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_22 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_22 = (int16_t)temperature_22;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_22_temperature_22_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_22_temperature_22_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_22_temperature_22_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_21_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_21_t *src_p,
    size_t size)
{
    uint16_t temperature_21;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_21 = (uint16_t)src_p->temperature_21;
    dst_p[0] |= pack_left_shift_u16(temperature_21, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_21, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_21_unpack(
    struct vehicle_rt_dl1_mk3_temp_21_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_21;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_21 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_21 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_21 = (int16_t)temperature_21;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_21_temperature_21_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_21_temperature_21_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_21_temperature_21_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_20_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_20_t *src_p,
    size_t size)
{
    uint16_t temperature_20;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_20 = (uint16_t)src_p->temperature_20;
    dst_p[0] |= pack_left_shift_u16(temperature_20, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_20, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_20_unpack(
    struct vehicle_rt_dl1_mk3_temp_20_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_20;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_20 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_20 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_20 = (int16_t)temperature_20;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_20_temperature_20_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_20_temperature_20_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_20_temperature_20_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_19_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_19_t *src_p,
    size_t size)
{
    uint16_t temperature_19;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_19 = (uint16_t)src_p->temperature_19;
    dst_p[0] |= pack_left_shift_u16(temperature_19, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_19, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_19_unpack(
    struct vehicle_rt_dl1_mk3_temp_19_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_19;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_19 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_19 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_19 = (int16_t)temperature_19;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_19_temperature_19_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_19_temperature_19_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_19_temperature_19_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_18_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_18_t *src_p,
    size_t size)
{
    uint16_t temperature_18;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_18 = (uint16_t)src_p->temperature_18;
    dst_p[0] |= pack_left_shift_u16(temperature_18, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_18, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_18_unpack(
    struct vehicle_rt_dl1_mk3_temp_18_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_18;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_18 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_18 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_18 = (int16_t)temperature_18;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_18_temperature_18_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_18_temperature_18_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_18_temperature_18_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_17_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_17_t *src_p,
    size_t size)
{
    uint16_t temperature_17;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_17 = (uint16_t)src_p->temperature_17;
    dst_p[0] |= pack_left_shift_u16(temperature_17, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_17, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_17_unpack(
    struct vehicle_rt_dl1_mk3_temp_17_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_17;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_17 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_17 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_17 = (int16_t)temperature_17;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_17_temperature_17_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_17_temperature_17_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_17_temperature_17_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_16_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_16_t *src_p,
    size_t size)
{
    uint16_t temperature_16;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_16 = (uint16_t)src_p->temperature_16;
    dst_p[0] |= pack_left_shift_u16(temperature_16, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_16, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_16_unpack(
    struct vehicle_rt_dl1_mk3_temp_16_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_16;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_16 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_16 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_16 = (int16_t)temperature_16;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_16_temperature_16_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_16_temperature_16_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_16_temperature_16_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_15_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_15_t *src_p,
    size_t size)
{
    uint16_t temperature_15;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_15 = (uint16_t)src_p->temperature_15;
    dst_p[0] |= pack_left_shift_u16(temperature_15, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_15, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_15_unpack(
    struct vehicle_rt_dl1_mk3_temp_15_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_15;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_15 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_15 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_15 = (int16_t)temperature_15;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_15_temperature_15_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_15_temperature_15_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_15_temperature_15_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_14_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_14_t *src_p,
    size_t size)
{
    uint16_t temperature_14;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_14 = (uint16_t)src_p->temperature_14;
    dst_p[0] |= pack_left_shift_u16(temperature_14, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_14, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_14_unpack(
    struct vehicle_rt_dl1_mk3_temp_14_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_14;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_14 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_14 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_14 = (int16_t)temperature_14;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_14_temperature_14_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_14_temperature_14_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_14_temperature_14_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_13_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_13_t *src_p,
    size_t size)
{
    uint16_t temperature_13;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_13 = (uint16_t)src_p->temperature_13;
    dst_p[0] |= pack_left_shift_u16(temperature_13, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_13, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_13_unpack(
    struct vehicle_rt_dl1_mk3_temp_13_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_13;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_13 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_13 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_13 = (int16_t)temperature_13;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_13_temperature_13_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_13_temperature_13_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_13_temperature_13_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_12_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_12_t *src_p,
    size_t size)
{
    uint16_t temperature_12;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_12 = (uint16_t)src_p->temperature_12;
    dst_p[0] |= pack_left_shift_u16(temperature_12, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_12, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_12_unpack(
    struct vehicle_rt_dl1_mk3_temp_12_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_12;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_12 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_12 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_12 = (int16_t)temperature_12;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_12_temperature_12_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_12_temperature_12_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_12_temperature_12_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_11_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_11_t *src_p,
    size_t size)
{
    uint16_t temperature_11;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_11 = (uint16_t)src_p->temperature_11;
    dst_p[0] |= pack_left_shift_u16(temperature_11, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_11, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_11_unpack(
    struct vehicle_rt_dl1_mk3_temp_11_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_11;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_11 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_11 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_11 = (int16_t)temperature_11;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_11_temperature_11_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_11_temperature_11_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_11_temperature_11_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_10_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_10_t *src_p,
    size_t size)
{
    uint16_t temperature_10;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_10 = (uint16_t)src_p->temperature_10;
    dst_p[0] |= pack_left_shift_u16(temperature_10, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_10, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_10_unpack(
    struct vehicle_rt_dl1_mk3_temp_10_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_10;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_10 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_10 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_10 = (int16_t)temperature_10;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_10_temperature_10_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_10_temperature_10_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_10_temperature_10_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_9_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_9_t *src_p,
    size_t size)
{
    uint16_t temperature_9;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_9 = (uint16_t)src_p->temperature_9;
    dst_p[0] |= pack_left_shift_u16(temperature_9, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_9, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_9_unpack(
    struct vehicle_rt_dl1_mk3_temp_9_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_9;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_9 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_9 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_9 = (int16_t)temperature_9;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_9_temperature_9_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_9_temperature_9_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_9_temperature_9_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_8_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_8_t *src_p,
    size_t size)
{
    uint16_t temperature_8;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_8 = (uint16_t)src_p->temperature_8;
    dst_p[0] |= pack_left_shift_u16(temperature_8, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_8, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_8_unpack(
    struct vehicle_rt_dl1_mk3_temp_8_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_8;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_8 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_8 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_8 = (int16_t)temperature_8;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_8_temperature_8_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_8_temperature_8_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_8_temperature_8_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_7_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_7_t *src_p,
    size_t size)
{
    uint16_t temperature_7;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_7 = (uint16_t)src_p->temperature_7;
    dst_p[0] |= pack_left_shift_u16(temperature_7, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_7, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_7_unpack(
    struct vehicle_rt_dl1_mk3_temp_7_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_7;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_7 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_7 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_7 = (int16_t)temperature_7;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_7_temperature_7_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_7_temperature_7_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_7_temperature_7_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_6_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_6_t *src_p,
    size_t size)
{
    uint16_t temperature_6;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_6 = (uint16_t)src_p->temperature_6;
    dst_p[0] |= pack_left_shift_u16(temperature_6, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_6, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_6_unpack(
    struct vehicle_rt_dl1_mk3_temp_6_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_6;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_6 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_6 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_6 = (int16_t)temperature_6;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_6_temperature_6_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_6_temperature_6_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_6_temperature_6_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_5_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_5_t *src_p,
    size_t size)
{
    uint16_t temperature_5;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_5 = (uint16_t)src_p->temperature_5;
    dst_p[0] |= pack_left_shift_u16(temperature_5, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_5, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_5_unpack(
    struct vehicle_rt_dl1_mk3_temp_5_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_5;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_5 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_5 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_5 = (int16_t)temperature_5;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_5_temperature_5_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_5_temperature_5_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_5_temperature_5_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_4_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_4_t *src_p,
    size_t size)
{
    uint16_t temperature_4;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_4 = (uint16_t)src_p->temperature_4;
    dst_p[0] |= pack_left_shift_u16(temperature_4, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_4, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_4_unpack(
    struct vehicle_rt_dl1_mk3_temp_4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_4;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_4 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_4 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_4 = (int16_t)temperature_4;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_4_temperature_4_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_4_temperature_4_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_4_temperature_4_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_3_t *src_p,
    size_t size)
{
    uint16_t temperature_3;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_3 = (uint16_t)src_p->temperature_3;
    dst_p[0] |= pack_left_shift_u16(temperature_3, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_3, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_3_unpack(
    struct vehicle_rt_dl1_mk3_temp_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_3;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_3 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_3 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_3 = (int16_t)temperature_3;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_3_temperature_3_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_3_temperature_3_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_3_temperature_3_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_2_t *src_p,
    size_t size)
{
    uint16_t temperature_2;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_2 = (uint16_t)src_p->temperature_2;
    dst_p[0] |= pack_left_shift_u16(temperature_2, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_2, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_2_unpack(
    struct vehicle_rt_dl1_mk3_temp_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_2;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_2 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_2 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_2 = (int16_t)temperature_2;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_2_temperature_2_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_2_temperature_2_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_2_temperature_2_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_temp_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_temp_1_t *src_p,
    size_t size)
{
    uint16_t temperature_1;

    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    temperature_1 = (uint16_t)src_p->temperature_1;
    dst_p[0] |= pack_left_shift_u16(temperature_1, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(temperature_1, 8u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_temp_1_unpack(
    struct vehicle_rt_dl1_mk3_temp_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t temperature_1;

    if (size < 2u) {
        return (-EINVAL);
    }

    temperature_1 = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    temperature_1 |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->temperature_1 = (int16_t)temperature_1;

    return (0);
}

int16_t vehicle_rt_dl1_mk3_temp_1_temperature_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_1_temperature_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

bool vehicle_rt_dl1_mk3_temp_1_temperature_1_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_32_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_32_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_32, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_32, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_32_unpack(
    struct vehicle_rt_dl1_mk3_analog_32_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_32 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_32 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_32_analog_32_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_32_analog_32_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_32_analog_32_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_31_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_31_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_31, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_31, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_31_unpack(
    struct vehicle_rt_dl1_mk3_analog_31_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_31 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_31 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_31_analog_31_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_31_analog_31_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_31_analog_31_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_30_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_30_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_30, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_30, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_30_unpack(
    struct vehicle_rt_dl1_mk3_analog_30_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_30 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_30 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_30_analog_30_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_30_analog_30_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_30_analog_30_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_29_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_29_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_29, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_29, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_29_unpack(
    struct vehicle_rt_dl1_mk3_analog_29_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_29 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_29 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_29_analog_29_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_29_analog_29_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_29_analog_29_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_28_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_28_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_28, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_28, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_28_unpack(
    struct vehicle_rt_dl1_mk3_analog_28_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_28 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_28 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_28_analog_28_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_28_analog_28_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_28_analog_28_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_27_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_27_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_27, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_27, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_27_unpack(
    struct vehicle_rt_dl1_mk3_analog_27_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_27 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_27 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_27_analog_27_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_27_analog_27_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_27_analog_27_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_26_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_26_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_26, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_26, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_26_unpack(
    struct vehicle_rt_dl1_mk3_analog_26_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_26 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_26 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_26_analog_26_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_26_analog_26_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_26_analog_26_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_25_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_25_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_25, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_25, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_25_unpack(
    struct vehicle_rt_dl1_mk3_analog_25_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_25 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_25 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_25_analog_25_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_25_analog_25_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_25_analog_25_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_15_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_15_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_15, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_15, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_15_unpack(
    struct vehicle_rt_dl1_mk3_analog_15_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_15 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_15 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_15_analog_15_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_15_analog_15_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_15_analog_15_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_14_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_14_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_14, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_14, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_14_unpack(
    struct vehicle_rt_dl1_mk3_analog_14_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_14 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_14 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_14_analog_14_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_14_analog_14_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_14_analog_14_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_17_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_17_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_17, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_17, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_17_unpack(
    struct vehicle_rt_dl1_mk3_analog_17_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_17 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_17 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_17_analog_17_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_17_analog_17_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_17_analog_17_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_24_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_24_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_24, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_24, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_24_unpack(
    struct vehicle_rt_dl1_mk3_analog_24_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_24 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_24 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_24_analog_24_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_24_analog_24_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_24_analog_24_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_23_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_23_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_23, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_23, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_23_unpack(
    struct vehicle_rt_dl1_mk3_analog_23_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_23 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_23 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_23_analog_23_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_23_analog_23_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_23_analog_23_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_22_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_22_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_22, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_22, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_22_unpack(
    struct vehicle_rt_dl1_mk3_analog_22_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_22 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_22 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_22_analog_22_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_22_analog_22_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_22_analog_22_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_21_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_21_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_21, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_21, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_21_unpack(
    struct vehicle_rt_dl1_mk3_analog_21_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_21 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_21 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_21_analog_21_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_21_analog_21_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_21_analog_21_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_20_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_20_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_20, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_20, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_20_unpack(
    struct vehicle_rt_dl1_mk3_analog_20_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_20 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_20 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_20_analog_20_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_20_analog_20_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_20_analog_20_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_19_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_19_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_19, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_19, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_19_unpack(
    struct vehicle_rt_dl1_mk3_analog_19_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_19 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_19 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_19_analog_19_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_19_analog_19_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_19_analog_19_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_16_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_16_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_16, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_16, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_16_unpack(
    struct vehicle_rt_dl1_mk3_analog_16_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_16 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_16 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_16_analog_16_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_16_analog_16_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_16_analog_16_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_18_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_18_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_18, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_18, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_18_unpack(
    struct vehicle_rt_dl1_mk3_analog_18_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_18 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_18 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_18_analog_18_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_18_analog_18_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_18_analog_18_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_12_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_12_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_12, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_12, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_12_unpack(
    struct vehicle_rt_dl1_mk3_analog_12_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_12 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_12 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_12_analog_12_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_12_analog_12_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_12_analog_12_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_11_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_11_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_11, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_11, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_11_unpack(
    struct vehicle_rt_dl1_mk3_analog_11_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_11 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_11 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_11_analog_11_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_11_analog_11_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_11_analog_11_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_10_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_10_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_10, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_10, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_10_unpack(
    struct vehicle_rt_dl1_mk3_analog_10_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_10 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_10 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_10_analog_10_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_10_analog_10_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_10_analog_10_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_9_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_9_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_9, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_9, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_9_unpack(
    struct vehicle_rt_dl1_mk3_analog_9_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_9 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_9 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_9_analog_9_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_9_analog_9_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_9_analog_9_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_8_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_8, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_8, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_8_unpack(
    struct vehicle_rt_dl1_mk3_analog_8_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_8 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_8 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_8_analog_8_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_8_analog_8_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_8_analog_8_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_7_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_7_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_7, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_7, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_7_unpack(
    struct vehicle_rt_dl1_mk3_analog_7_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_7 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_7 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_7_analog_7_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_7_analog_7_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_7_analog_7_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_6_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_6_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_6, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_6, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_6_unpack(
    struct vehicle_rt_dl1_mk3_analog_6_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_6 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_6 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_6_analog_6_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_6_analog_6_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_6_analog_6_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_5_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_5_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_5, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_5, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_5_unpack(
    struct vehicle_rt_dl1_mk3_analog_5_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_5 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_5 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_5_analog_5_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_5_analog_5_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_5_analog_5_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_4_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_4_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_4, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_4, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_4_unpack(
    struct vehicle_rt_dl1_mk3_analog_4_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_4 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_4 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_4_analog_4_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_4_analog_4_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_4_analog_4_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_3_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_3_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_3, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_3, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_3_unpack(
    struct vehicle_rt_dl1_mk3_analog_3_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_3 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_3 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_3_analog_3_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_3_analog_3_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_3_analog_3_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_2_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_2, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_2, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_2_unpack(
    struct vehicle_rt_dl1_mk3_analog_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_2 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_2 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_2_analog_2_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_2_analog_2_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_2_analog_2_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_analog_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_analog_1_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 2);

    dst_p[0] |= pack_right_shift_u16(src_p->analog_1, 8u, 0xffu);
    dst_p[1] |= pack_left_shift_u16(src_p->analog_1, 0u, 0xffu);

    return (2);
}

int vehicle_rt_dl1_mk3_analog_1_unpack(
    struct vehicle_rt_dl1_mk3_analog_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 2u) {
        return (-EINVAL);
    }

    dst_p->analog_1 = unpack_left_shift_u16(src_p[0], 8u, 0xffu);
    dst_p->analog_1 |= unpack_right_shift_u16(src_p[1], 0u, 0xffu);

    return (0);
}

uint16_t vehicle_rt_dl1_mk3_analog_1_analog_1_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_1_analog_1_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_analog_1_analog_1_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_dl1_mk3_accel_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_dl1_mk3_accel_t *src_p,
    size_t size)
{
    uint16_t accel_lateral;
    uint16_t accel_longitudinal;
    uint16_t accel_vertical;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_longitudinal, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_lateral, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_vertical, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_accel, 0u, 0xffu);
    accel_longitudinal = (uint16_t)src_p->accel_longitudinal;
    dst_p[2] |= pack_left_shift_u16(accel_longitudinal, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(accel_longitudinal, 8u, 0xffu);
    accel_lateral = (uint16_t)src_p->accel_lateral;
    dst_p[4] |= pack_left_shift_u16(accel_lateral, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(accel_lateral, 8u, 0xffu);
    accel_vertical = (uint16_t)src_p->accel_vertical;
    dst_p[6] |= pack_left_shift_u16(accel_vertical, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(accel_vertical, 8u, 0xffu);

    return (8);
}

int vehicle_rt_dl1_mk3_accel_unpack(
    struct vehicle_rt_dl1_mk3_accel_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t accel_lateral;
    uint16_t accel_longitudinal;
    uint16_t accel_vertical;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_accel_longitudinal = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_accel_lateral = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_accel_vertical = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_accel = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    accel_longitudinal = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    accel_longitudinal |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->accel_longitudinal = (int16_t)accel_longitudinal;
    accel_lateral = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    accel_lateral |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->accel_lateral = (int16_t)accel_lateral;
    accel_vertical = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    accel_vertical |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->accel_vertical = (int16_t)accel_vertical;

    return (0);
}

uint8_t vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_accel_validity_accel_lateral_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_accel_validity_accel_lateral_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_accel_validity_accel_lateral_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_accel_validity_accel_vertical_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_accel_validity_accel_vertical_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_accel_validity_accel_vertical_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_dl1_mk3_accel_accuracy_accel_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_accel_accuracy_accel_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_dl1_mk3_accel_accuracy_accel_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_dl1_mk3_accel_accel_longitudinal_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_accel_accel_longitudinal_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_accel_accel_longitudinal_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_dl1_mk3_accel_accel_lateral_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_accel_accel_lateral_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_accel_accel_lateral_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_dl1_mk3_accel_accel_vertical_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_accel_accel_vertical_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_dl1_mk3_accel_accel_vertical_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_4_vel_ned_2_t *src_p,
    size_t size)
{
    uint16_t virtual_4_heading;
    uint16_t virtual_4_slip;
    uint32_t virtual_4_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_4_vel_ned_d = (uint32_t)src_p->virtual_4_vel_ned_d;
    dst_p[0] |= pack_left_shift_u32(virtual_4_vel_ned_d, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_4_vel_ned_d, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_4_vel_ned_d, 16u, 0xffu);
    virtual_4_heading = (uint16_t)src_p->virtual_4_heading;
    dst_p[4] |= pack_left_shift_u16(virtual_4_heading, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_4_heading, 8u, 0xffu);
    virtual_4_slip = (uint16_t)src_p->virtual_4_slip;
    dst_p[6] |= pack_left_shift_u16(virtual_4_slip, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(virtual_4_slip, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_2_unpack(
    struct vehicle_rt_sb_ins_vpt_4_vel_ned_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_4_heading;
    uint16_t virtual_4_slip;
    uint32_t virtual_4_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_4_vel_ned_d = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_4_vel_ned_d |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_4_vel_ned_d |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_4_vel_ned_d & (1u << 23)) != 0u) {
        virtual_4_vel_ned_d |= 0xff000000u;
    }

    dst_p->virtual_4_vel_ned_d = (int32_t)virtual_4_vel_ned_d;
    virtual_4_heading = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_4_heading |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_4_heading = (int16_t)virtual_4_heading;
    virtual_4_slip = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    virtual_4_slip |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->virtual_4_slip = (int16_t)virtual_4_slip;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int16_t vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_heading_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_heading_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_heading_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int16_t vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_slip_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_slip_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_slip_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_4_vel_ned_1_t *src_p,
    size_t size)
{
    uint32_t virtual_4_vel_ned_e;
    uint32_t virtual_4_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_4_vel_ned_n = (uint32_t)src_p->virtual_4_vel_ned_n;
    dst_p[0] |= pack_left_shift_u32(virtual_4_vel_ned_n, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_4_vel_ned_n, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_4_vel_ned_n, 16u, 0xffu);
    virtual_4_vel_ned_e = (uint32_t)src_p->virtual_4_vel_ned_e;
    dst_p[4] |= pack_left_shift_u32(virtual_4_vel_ned_e, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(virtual_4_vel_ned_e, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(virtual_4_vel_ned_e, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_1_unpack(
    struct vehicle_rt_sb_ins_vpt_4_vel_ned_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t virtual_4_vel_ned_e;
    uint32_t virtual_4_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_4_vel_ned_n = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_4_vel_ned_n |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_4_vel_ned_n |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_4_vel_ned_n & (1u << 23)) != 0u) {
        virtual_4_vel_ned_n |= 0xff000000u;
    }

    dst_p->virtual_4_vel_ned_n = (int32_t)virtual_4_vel_ned_n;
    virtual_4_vel_ned_e = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    virtual_4_vel_ned_e |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    virtual_4_vel_ned_e |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((virtual_4_vel_ned_e & (1u << 23)) != 0u) {
        virtual_4_vel_ned_e |= 0xff000000u;
    }

    dst_p->virtual_4_vel_ned_e = (int32_t)virtual_4_vel_ned_e;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_e_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_e_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_e_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_vpt_4_offset_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_4_offset_t *src_p,
    size_t size)
{
    uint16_t virtual_4_offset_x;
    uint16_t virtual_4_offset_y;
    uint16_t virtual_4_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_4_offset_x = (uint16_t)src_p->virtual_4_offset_x;
    dst_p[0] |= pack_left_shift_u16(virtual_4_offset_x, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(virtual_4_offset_x, 8u, 0xffu);
    virtual_4_offset_y = (uint16_t)src_p->virtual_4_offset_y;
    dst_p[2] |= pack_left_shift_u16(virtual_4_offset_y, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(virtual_4_offset_y, 8u, 0xffu);
    virtual_4_offset_z = (uint16_t)src_p->virtual_4_offset_z;
    dst_p[4] |= pack_left_shift_u16(virtual_4_offset_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_4_offset_z, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_4_offset_unpack(
    struct vehicle_rt_sb_ins_vpt_4_offset_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_4_offset_x;
    uint16_t virtual_4_offset_y;
    uint16_t virtual_4_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_4_offset_x = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    virtual_4_offset_x |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->virtual_4_offset_x = (int16_t)virtual_4_offset_x;
    virtual_4_offset_y = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    virtual_4_offset_y |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->virtual_4_offset_y = (int16_t)virtual_4_offset_y;
    virtual_4_offset_z = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_4_offset_z |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_4_offset_z = (int16_t)virtual_4_offset_z;

    return (0);
}

int16_t vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_y_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_y_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_y_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_z_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_z_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_z_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_3_vel_ned_2_t *src_p,
    size_t size)
{
    uint16_t virtual_3_heading;
    uint16_t virtual_3_slip;
    uint32_t virtual_3_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_3_vel_ned_d = (uint32_t)src_p->virtual_3_vel_ned_d;
    dst_p[0] |= pack_left_shift_u32(virtual_3_vel_ned_d, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_3_vel_ned_d, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_3_vel_ned_d, 16u, 0xffu);
    virtual_3_heading = (uint16_t)src_p->virtual_3_heading;
    dst_p[4] |= pack_left_shift_u16(virtual_3_heading, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_3_heading, 8u, 0xffu);
    virtual_3_slip = (uint16_t)src_p->virtual_3_slip;
    dst_p[6] |= pack_left_shift_u16(virtual_3_slip, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(virtual_3_slip, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_2_unpack(
    struct vehicle_rt_sb_ins_vpt_3_vel_ned_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_3_heading;
    uint16_t virtual_3_slip;
    uint32_t virtual_3_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_3_vel_ned_d = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_3_vel_ned_d |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_3_vel_ned_d |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_3_vel_ned_d & (1u << 23)) != 0u) {
        virtual_3_vel_ned_d |= 0xff000000u;
    }

    dst_p->virtual_3_vel_ned_d = (int32_t)virtual_3_vel_ned_d;
    virtual_3_heading = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_3_heading |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_3_heading = (int16_t)virtual_3_heading;
    virtual_3_slip = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    virtual_3_slip |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->virtual_3_slip = (int16_t)virtual_3_slip;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int16_t vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_heading_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_heading_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_heading_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int16_t vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_slip_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_slip_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_slip_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_3_vel_ned_1_t *src_p,
    size_t size)
{
    uint32_t virtual_3_vel_ned_e;
    uint32_t virtual_3_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_3_vel_ned_n = (uint32_t)src_p->virtual_3_vel_ned_n;
    dst_p[0] |= pack_left_shift_u32(virtual_3_vel_ned_n, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_3_vel_ned_n, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_3_vel_ned_n, 16u, 0xffu);
    virtual_3_vel_ned_e = (uint32_t)src_p->virtual_3_vel_ned_e;
    dst_p[4] |= pack_left_shift_u32(virtual_3_vel_ned_e, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(virtual_3_vel_ned_e, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(virtual_3_vel_ned_e, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_1_unpack(
    struct vehicle_rt_sb_ins_vpt_3_vel_ned_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t virtual_3_vel_ned_e;
    uint32_t virtual_3_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_3_vel_ned_n = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_3_vel_ned_n |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_3_vel_ned_n |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_3_vel_ned_n & (1u << 23)) != 0u) {
        virtual_3_vel_ned_n |= 0xff000000u;
    }

    dst_p->virtual_3_vel_ned_n = (int32_t)virtual_3_vel_ned_n;
    virtual_3_vel_ned_e = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    virtual_3_vel_ned_e |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    virtual_3_vel_ned_e |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((virtual_3_vel_ned_e & (1u << 23)) != 0u) {
        virtual_3_vel_ned_e |= 0xff000000u;
    }

    dst_p->virtual_3_vel_ned_e = (int32_t)virtual_3_vel_ned_e;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_e_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_e_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_e_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_vpt_3_offset_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_3_offset_t *src_p,
    size_t size)
{
    uint16_t virtual_3_offset_x;
    uint16_t virtual_3_offset_y;
    uint16_t virtual_3_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_3_offset_x = (uint16_t)src_p->virtual_3_offset_x;
    dst_p[0] |= pack_left_shift_u16(virtual_3_offset_x, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(virtual_3_offset_x, 8u, 0xffu);
    virtual_3_offset_y = (uint16_t)src_p->virtual_3_offset_y;
    dst_p[2] |= pack_left_shift_u16(virtual_3_offset_y, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(virtual_3_offset_y, 8u, 0xffu);
    virtual_3_offset_z = (uint16_t)src_p->virtual_3_offset_z;
    dst_p[4] |= pack_left_shift_u16(virtual_3_offset_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_3_offset_z, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_3_offset_unpack(
    struct vehicle_rt_sb_ins_vpt_3_offset_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_3_offset_x;
    uint16_t virtual_3_offset_y;
    uint16_t virtual_3_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_3_offset_x = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    virtual_3_offset_x |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->virtual_3_offset_x = (int16_t)virtual_3_offset_x;
    virtual_3_offset_y = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    virtual_3_offset_y |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->virtual_3_offset_y = (int16_t)virtual_3_offset_y;
    virtual_3_offset_z = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_3_offset_z |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_3_offset_z = (int16_t)virtual_3_offset_z;

    return (0);
}

int16_t vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_y_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_y_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_y_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_z_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_z_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_z_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_2_vel_ned_2_t *src_p,
    size_t size)
{
    uint16_t virtual_2_heading;
    uint16_t virtual_2_slip;
    uint32_t virtual_2_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_2_vel_ned_d = (uint32_t)src_p->virtual_2_vel_ned_d;
    dst_p[0] |= pack_left_shift_u32(virtual_2_vel_ned_d, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_2_vel_ned_d, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_2_vel_ned_d, 16u, 0xffu);
    virtual_2_heading = (uint16_t)src_p->virtual_2_heading;
    dst_p[4] |= pack_left_shift_u16(virtual_2_heading, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_2_heading, 8u, 0xffu);
    virtual_2_slip = (uint16_t)src_p->virtual_2_slip;
    dst_p[6] |= pack_left_shift_u16(virtual_2_slip, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(virtual_2_slip, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_2_unpack(
    struct vehicle_rt_sb_ins_vpt_2_vel_ned_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_2_heading;
    uint16_t virtual_2_slip;
    uint32_t virtual_2_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_2_vel_ned_d = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_2_vel_ned_d |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_2_vel_ned_d |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_2_vel_ned_d & (1u << 23)) != 0u) {
        virtual_2_vel_ned_d |= 0xff000000u;
    }

    dst_p->virtual_2_vel_ned_d = (int32_t)virtual_2_vel_ned_d;
    virtual_2_heading = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_2_heading |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_2_heading = (int16_t)virtual_2_heading;
    virtual_2_slip = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    virtual_2_slip |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->virtual_2_slip = (int16_t)virtual_2_slip;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int16_t vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_heading_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_heading_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_heading_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int16_t vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_slip_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_slip_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_slip_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_2_vel_ned_1_t *src_p,
    size_t size)
{
    uint32_t virtual_2_vel_ned_e;
    uint32_t virtual_2_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_2_vel_ned_n = (uint32_t)src_p->virtual_2_vel_ned_n;
    dst_p[0] |= pack_left_shift_u32(virtual_2_vel_ned_n, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_2_vel_ned_n, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_2_vel_ned_n, 16u, 0xffu);
    virtual_2_vel_ned_e = (uint32_t)src_p->virtual_2_vel_ned_e;
    dst_p[4] |= pack_left_shift_u32(virtual_2_vel_ned_e, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(virtual_2_vel_ned_e, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(virtual_2_vel_ned_e, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_1_unpack(
    struct vehicle_rt_sb_ins_vpt_2_vel_ned_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t virtual_2_vel_ned_e;
    uint32_t virtual_2_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_2_vel_ned_n = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_2_vel_ned_n |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_2_vel_ned_n |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_2_vel_ned_n & (1u << 23)) != 0u) {
        virtual_2_vel_ned_n |= 0xff000000u;
    }

    dst_p->virtual_2_vel_ned_n = (int32_t)virtual_2_vel_ned_n;
    virtual_2_vel_ned_e = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    virtual_2_vel_ned_e |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    virtual_2_vel_ned_e |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((virtual_2_vel_ned_e & (1u << 23)) != 0u) {
        virtual_2_vel_ned_e |= 0xff000000u;
    }

    dst_p->virtual_2_vel_ned_e = (int32_t)virtual_2_vel_ned_e;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_e_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_e_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_e_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_vpt_2_offset_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_2_offset_t *src_p,
    size_t size)
{
    uint16_t virtual_2_offset_x;
    uint16_t virtual_2_offset_y;
    uint16_t virtual_2_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_2_offset_x = (uint16_t)src_p->virtual_2_offset_x;
    dst_p[0] |= pack_left_shift_u16(virtual_2_offset_x, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(virtual_2_offset_x, 8u, 0xffu);
    virtual_2_offset_y = (uint16_t)src_p->virtual_2_offset_y;
    dst_p[2] |= pack_left_shift_u16(virtual_2_offset_y, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(virtual_2_offset_y, 8u, 0xffu);
    virtual_2_offset_z = (uint16_t)src_p->virtual_2_offset_z;
    dst_p[4] |= pack_left_shift_u16(virtual_2_offset_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_2_offset_z, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_2_offset_unpack(
    struct vehicle_rt_sb_ins_vpt_2_offset_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_2_offset_x;
    uint16_t virtual_2_offset_y;
    uint16_t virtual_2_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_2_offset_x = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    virtual_2_offset_x |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->virtual_2_offset_x = (int16_t)virtual_2_offset_x;
    virtual_2_offset_y = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    virtual_2_offset_y |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->virtual_2_offset_y = (int16_t)virtual_2_offset_y;
    virtual_2_offset_z = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_2_offset_z |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_2_offset_z = (int16_t)virtual_2_offset_z;

    return (0);
}

int16_t vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_y_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_y_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_y_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_z_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_z_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_z_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_1_vel_ned_2_t *src_p,
    size_t size)
{
    uint16_t virtual_1_heading;
    uint16_t virtual_1_slip;
    uint32_t virtual_1_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_1_vel_ned_d = (uint32_t)src_p->virtual_1_vel_ned_d;
    dst_p[0] |= pack_left_shift_u32(virtual_1_vel_ned_d, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_1_vel_ned_d, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_1_vel_ned_d, 16u, 0xffu);
    virtual_1_heading = (uint16_t)src_p->virtual_1_heading;
    dst_p[4] |= pack_left_shift_u16(virtual_1_heading, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_1_heading, 8u, 0xffu);
    virtual_1_slip = (uint16_t)src_p->virtual_1_slip;
    dst_p[6] |= pack_left_shift_u16(virtual_1_slip, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(virtual_1_slip, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_2_unpack(
    struct vehicle_rt_sb_ins_vpt_1_vel_ned_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_1_heading;
    uint16_t virtual_1_slip;
    uint32_t virtual_1_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_1_vel_ned_d = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_1_vel_ned_d |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_1_vel_ned_d |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_1_vel_ned_d & (1u << 23)) != 0u) {
        virtual_1_vel_ned_d |= 0xff000000u;
    }

    dst_p->virtual_1_vel_ned_d = (int32_t)virtual_1_vel_ned_d;
    virtual_1_heading = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_1_heading |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_1_heading = (int16_t)virtual_1_heading;
    virtual_1_slip = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    virtual_1_slip |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->virtual_1_slip = (int16_t)virtual_1_slip;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int16_t vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_heading_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_heading_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_heading_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int16_t vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_slip_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_slip_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_slip_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_1_vel_ned_1_t *src_p,
    size_t size)
{
    uint32_t virtual_1_vel_ned_e;
    uint32_t virtual_1_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_1_vel_ned_n = (uint32_t)src_p->virtual_1_vel_ned_n;
    dst_p[0] |= pack_left_shift_u32(virtual_1_vel_ned_n, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(virtual_1_vel_ned_n, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(virtual_1_vel_ned_n, 16u, 0xffu);
    virtual_1_vel_ned_e = (uint32_t)src_p->virtual_1_vel_ned_e;
    dst_p[4] |= pack_left_shift_u32(virtual_1_vel_ned_e, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(virtual_1_vel_ned_e, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(virtual_1_vel_ned_e, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_1_unpack(
    struct vehicle_rt_sb_ins_vpt_1_vel_ned_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t virtual_1_vel_ned_e;
    uint32_t virtual_1_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_1_vel_ned_n = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    virtual_1_vel_ned_n |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    virtual_1_vel_ned_n |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);

    if ((virtual_1_vel_ned_n & (1u << 23)) != 0u) {
        virtual_1_vel_ned_n |= 0xff000000u;
    }

    dst_p->virtual_1_vel_ned_n = (int32_t)virtual_1_vel_ned_n;
    virtual_1_vel_ned_e = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    virtual_1_vel_ned_e |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    virtual_1_vel_ned_e |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((virtual_1_vel_ned_e & (1u << 23)) != 0u) {
        virtual_1_vel_ned_e |= 0xff000000u;
    }

    dst_p->virtual_1_vel_ned_e = (int32_t)virtual_1_vel_ned_e;

    return (0);
}

int32_t vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_e_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_e_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_e_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_vpt_1_offset_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vpt_1_offset_t *src_p,
    size_t size)
{
    uint16_t virtual_1_offset_x;
    uint16_t virtual_1_offset_y;
    uint16_t virtual_1_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    virtual_1_offset_x = (uint16_t)src_p->virtual_1_offset_x;
    dst_p[0] |= pack_left_shift_u16(virtual_1_offset_x, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u16(virtual_1_offset_x, 8u, 0xffu);
    virtual_1_offset_y = (uint16_t)src_p->virtual_1_offset_y;
    dst_p[2] |= pack_left_shift_u16(virtual_1_offset_y, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(virtual_1_offset_y, 8u, 0xffu);
    virtual_1_offset_z = (uint16_t)src_p->virtual_1_offset_z;
    dst_p[4] |= pack_left_shift_u16(virtual_1_offset_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(virtual_1_offset_z, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vpt_1_offset_unpack(
    struct vehicle_rt_sb_ins_vpt_1_offset_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t virtual_1_offset_x;
    uint16_t virtual_1_offset_y;
    uint16_t virtual_1_offset_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    virtual_1_offset_x = unpack_right_shift_u16(src_p[0], 0u, 0xffu);
    virtual_1_offset_x |= unpack_left_shift_u16(src_p[1], 8u, 0xffu);
    dst_p->virtual_1_offset_x = (int16_t)virtual_1_offset_x;
    virtual_1_offset_y = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    virtual_1_offset_y |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->virtual_1_offset_y = (int16_t)virtual_1_offset_y;
    virtual_1_offset_z = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    virtual_1_offset_z |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->virtual_1_offset_z = (int16_t)virtual_1_offset_z;

    return (0);
}

int16_t vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_y_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_y_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_y_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_z_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_z_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_z_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_ins_slip_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_slip_t *src_p,
    size_t size)
{
    uint16_t ins_slip;
    uint16_t ins_squat;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_slip, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_squat, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_slip, 0u, 0xffu);
    ins_slip = (uint16_t)src_p->ins_slip;
    dst_p[2] |= pack_left_shift_u16(ins_slip, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ins_slip, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->accuracy_ins_squat, 0u, 0xffu);
    ins_squat = (uint16_t)src_p->ins_squat;
    dst_p[5] |= pack_left_shift_u16(ins_squat, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(ins_squat, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_slip_unpack(
    struct vehicle_rt_sb_ins_slip_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ins_slip;
    uint16_t ins_squat;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_slip = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_squat = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_ins_slip = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    ins_slip = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ins_slip |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ins_slip = (int16_t)ins_slip;
    dst_p->accuracy_ins_squat = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    ins_squat = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    ins_squat |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);
    dst_p->ins_squat = (int16_t)ins_squat;

    return (0);
}

uint8_t vehicle_rt_sb_ins_slip_validity_ins_slip_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_slip_validity_ins_slip_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_slip_validity_ins_slip_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_slip_validity_ins_squat_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_slip_validity_ins_squat_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_slip_validity_ins_squat_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_slip_accuracy_ins_slip_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_slip_accuracy_ins_slip_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_slip_accuracy_ins_slip_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_slip_ins_slip_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_slip_ins_slip_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_slip_ins_slip_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_ins_slip_accuracy_ins_squat_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_slip_accuracy_ins_squat_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_slip_accuracy_ins_squat_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_slip_ins_squat_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_slip_ins_squat_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_slip_ins_squat_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_ins_vel_ecef_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vel_ecef_2_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ecef_y;
    uint32_t ins_vel_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_ecef_y, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_ecef_z, 1u, 0x02u);
    ins_vel_ecef_y = (uint32_t)src_p->ins_vel_ecef_y;
    dst_p[1] |= pack_left_shift_u32(ins_vel_ecef_y, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(ins_vel_ecef_y, 8u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(ins_vel_ecef_y, 16u, 0xffu);
    ins_vel_ecef_z = (uint32_t)src_p->ins_vel_ecef_z;
    dst_p[4] |= pack_left_shift_u32(ins_vel_ecef_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(ins_vel_ecef_z, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_vel_ecef_z, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vel_ecef_2_unpack(
    struct vehicle_rt_sb_ins_vel_ecef_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ecef_y;
    uint32_t ins_vel_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_vel_ecef_y = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_vel_ecef_z = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    ins_vel_ecef_y = unpack_right_shift_u32(src_p[1], 0u, 0xffu);
    ins_vel_ecef_y |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    ins_vel_ecef_y |= unpack_left_shift_u32(src_p[3], 16u, 0xffu);

    if ((ins_vel_ecef_y & (1u << 23)) != 0u) {
        ins_vel_ecef_y |= 0xff000000u;
    }

    dst_p->ins_vel_ecef_y = (int32_t)ins_vel_ecef_y;
    ins_vel_ecef_z = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    ins_vel_ecef_z |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    ins_vel_ecef_z |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((ins_vel_ecef_z & (1u << 23)) != 0u) {
        ins_vel_ecef_z |= 0xff000000u;
    }

    dst_p->ins_vel_ecef_z = (int32_t)ins_vel_ecef_z;

    return (0);
}

uint8_t vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_z_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

int32_t vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_y_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_y_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_y_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_z_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_z_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_z_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_vel_ecef_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vel_ecef_1_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_ecef_x, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_vel_ecef_x, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->accuracy_ins_vel_ecef_y, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->accuracy_ins_vel_ecef_z, 0u, 0xffu);
    ins_vel_ecef_x = (uint32_t)src_p->ins_vel_ecef_x;
    dst_p[4] |= pack_left_shift_u32(ins_vel_ecef_x, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(ins_vel_ecef_x, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_vel_ecef_x, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vel_ecef_1_unpack(
    struct vehicle_rt_sb_ins_vel_ecef_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_vel_ecef_x = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->accuracy_ins_vel_ecef_x = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->accuracy_ins_vel_ecef_y = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->accuracy_ins_vel_ecef_z = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    ins_vel_ecef_x = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    ins_vel_ecef_x |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    ins_vel_ecef_x |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((ins_vel_ecef_x & (1u << 23)) != 0u) {
        ins_vel_ecef_x |= 0xff000000u;
    }

    dst_p->ins_vel_ecef_x = (int32_t)ins_vel_ecef_x;

    return (0);
}

uint8_t vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_x_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_y_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_z_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_ins_vel_ecef_1_ins_vel_ecef_x_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_ecef_1_ins_vel_ecef_x_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_ecef_1_ins_vel_ecef_x_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_vel_ned_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vel_ned_2_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_ned_d, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_vel_d, 0u, 0xffu);
    ins_vel_ned_d = (uint32_t)src_p->ins_vel_ned_d;
    dst_p[2] |= pack_left_shift_u32(ins_vel_ned_d, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(ins_vel_ned_d, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(ins_vel_ned_d, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vel_ned_2_unpack(
    struct vehicle_rt_sb_ins_vel_ned_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_vel_ned_d = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->accuracy_ins_vel_d = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    ins_vel_ned_d = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    ins_vel_ned_d |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    ins_vel_ned_d |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);

    if ((ins_vel_ned_d & (1u << 23)) != 0u) {
        ins_vel_ned_d |= 0xff000000u;
    }

    dst_p->ins_vel_ned_d = (int32_t)ins_vel_ned_d;

    return (0);
}

uint8_t vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_vel_ned_2_accuracy_ins_vel_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ned_2_accuracy_ins_vel_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ned_2_accuracy_ins_vel_d_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_ins_vel_ned_2_ins_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_ned_2_ins_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_ned_2_ins_vel_ned_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_vel_ned_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_vel_ned_1_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ned_e;
    uint32_t ins_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_ned_n, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_vel_ned_e, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_vel_ne, 0u, 0xffu);
    ins_vel_ned_n = (uint32_t)src_p->ins_vel_ned_n;
    dst_p[2] |= pack_left_shift_u32(ins_vel_ned_n, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(ins_vel_ned_n, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(ins_vel_ned_n, 16u, 0xffu);
    ins_vel_ned_e = (uint32_t)src_p->ins_vel_ned_e;
    dst_p[5] |= pack_left_shift_u32(ins_vel_ned_e, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_vel_ned_e, 8u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(ins_vel_ned_e, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_vel_ned_1_unpack(
    struct vehicle_rt_sb_ins_vel_ned_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_vel_ned_e;
    uint32_t ins_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_vel_ned_n = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_vel_ned_e = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_ins_vel_ne = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    ins_vel_ned_n = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    ins_vel_ned_n |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    ins_vel_ned_n |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);

    if ((ins_vel_ned_n & (1u << 23)) != 0u) {
        ins_vel_ned_n |= 0xff000000u;
    }

    dst_p->ins_vel_ned_n = (int32_t)ins_vel_ned_n;
    ins_vel_ned_e = unpack_right_shift_u32(src_p[5], 0u, 0xffu);
    ins_vel_ned_e |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    ins_vel_ned_e |= unpack_left_shift_u32(src_p[7], 16u, 0xffu);

    if ((ins_vel_ned_e & (1u << 23)) != 0u) {
        ins_vel_ned_e |= 0xff000000u;
    }

    dst_p->ins_vel_ned_e = (int32_t)ins_vel_ned_e;

    return (0);
}

uint8_t vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_e_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_e_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_e_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_vel_ned_1_accuracy_ins_vel_ne_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ned_1_accuracy_ins_vel_ne_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_vel_ned_1_accuracy_ins_vel_ne_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_n_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_e_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_e_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_e_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_ins_pos_ecef_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_pos_ecef_2_t *src_p,
    size_t size)
{
    uint32_t ins_pos_ecef_y;
    uint32_t ins_pos_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ins_pos_ecef_y = (uint32_t)src_p->ins_pos_ecef_y;
    dst_p[0] |= pack_left_shift_u32(ins_pos_ecef_y, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(ins_pos_ecef_y, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(ins_pos_ecef_y, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(ins_pos_ecef_y, 24u, 0xffu);
    ins_pos_ecef_z = (uint32_t)src_p->ins_pos_ecef_z;
    dst_p[4] |= pack_left_shift_u32(ins_pos_ecef_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(ins_pos_ecef_z, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_pos_ecef_z, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(ins_pos_ecef_z, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_pos_ecef_2_unpack(
    struct vehicle_rt_sb_ins_pos_ecef_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_pos_ecef_y;
    uint32_t ins_pos_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    ins_pos_ecef_y = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    ins_pos_ecef_y |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    ins_pos_ecef_y |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    ins_pos_ecef_y |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->ins_pos_ecef_y = (int32_t)ins_pos_ecef_y;
    ins_pos_ecef_z = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    ins_pos_ecef_z |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    ins_pos_ecef_z |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    ins_pos_ecef_z |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->ins_pos_ecef_z = (int32_t)ins_pos_ecef_z;

    return (0);
}

int32_t vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_is_in_range(int32_t value)
{
    return ((value >= -1000000000) && (value <= 1000000000));
}

int32_t vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_z_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_z_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_z_is_in_range(int32_t value)
{
    return ((value >= -1000000000) && (value <= 1000000000));
}

int vehicle_rt_sb_ins_pos_ecef_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_pos_ecef_1_t *src_p,
    size_t size)
{
    uint32_t ins_pos_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_pos_ecef_x, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_pos_ecef_y, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_pos_ecef_z, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_pos_ecef_x, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->accuracy_ins_pos_ecef_y, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->accuracy_ins_pos_ecef_z, 0u, 0xffu);
    ins_pos_ecef_x = (uint32_t)src_p->ins_pos_ecef_x;
    dst_p[4] |= pack_left_shift_u32(ins_pos_ecef_x, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(ins_pos_ecef_x, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_pos_ecef_x, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(ins_pos_ecef_x, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_pos_ecef_1_unpack(
    struct vehicle_rt_sb_ins_pos_ecef_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_pos_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_pos_ecef_x = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_pos_ecef_y = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_ins_pos_ecef_z = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_ins_pos_ecef_x = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->accuracy_ins_pos_ecef_y = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->accuracy_ins_pos_ecef_z = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    ins_pos_ecef_x = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    ins_pos_ecef_x |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    ins_pos_ecef_x |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    ins_pos_ecef_x |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->ins_pos_ecef_x = (int32_t)ins_pos_ecef_x;

    return (0);
}

uint8_t vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_y_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_z_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_x_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_y_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_z_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_ins_pos_ecef_1_ins_pos_ecef_x_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_ins_pos_ecef_1_ins_pos_ecef_x_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_pos_ecef_1_ins_pos_ecef_x_is_in_range(int32_t value)
{
    return ((value >= -1000000000) && (value <= 1000000000));
}

int vehicle_rt_sb_ins_pos_llh_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_pos_llh_2_t *src_p,
    size_t size)
{
    uint32_t ins_pos_llh_altitude;
    uint32_t ins_pos_llh_longitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    ins_pos_llh_longitude = (uint32_t)src_p->ins_pos_llh_longitude;
    dst_p[0] |= pack_left_shift_u32(ins_pos_llh_longitude, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(ins_pos_llh_longitude, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(ins_pos_llh_longitude, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(ins_pos_llh_longitude, 24u, 0xffu);
    ins_pos_llh_altitude = (uint32_t)src_p->ins_pos_llh_altitude;
    dst_p[4] |= pack_left_shift_u32(ins_pos_llh_altitude, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(ins_pos_llh_altitude, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_pos_llh_altitude, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(ins_pos_llh_altitude, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_pos_llh_2_unpack(
    struct vehicle_rt_sb_ins_pos_llh_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_pos_llh_altitude;
    uint32_t ins_pos_llh_longitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    ins_pos_llh_longitude = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    ins_pos_llh_longitude |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    ins_pos_llh_longitude |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    ins_pos_llh_longitude |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->ins_pos_llh_longitude = (int32_t)ins_pos_llh_longitude;
    ins_pos_llh_altitude = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    ins_pos_llh_altitude |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    ins_pos_llh_altitude |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    ins_pos_llh_altitude |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->ins_pos_llh_altitude = (int32_t)ins_pos_llh_altitude;

    return (0);
}

int32_t vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

bool vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_is_in_range(int32_t value)
{
    return ((value >= -1800000000) && (value <= 1800000000));
}

int32_t vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_altitude_encode(double value)
{
    return (int32_t)(value / 0.001);
}

double vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_altitude_decode(int32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_altitude_is_in_range(int32_t value)
{
    return ((value >= -1000000) && (value <= 100000000));
}

int vehicle_rt_sb_ins_pos_llh_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_pos_llh_1_t *src_p,
    size_t size)
{
    uint32_t ins_pos_llh_latitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_pos_llh_latitude, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_pos_llh_longitude, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_pos_llh_altitude, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_pos_llh_latitude, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->accuracy_ins_pos_llh_longitude, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->accuracy_ins_pos_llh_altitude, 0u, 0xffu);
    ins_pos_llh_latitude = (uint32_t)src_p->ins_pos_llh_latitude;
    dst_p[4] |= pack_left_shift_u32(ins_pos_llh_latitude, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(ins_pos_llh_latitude, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(ins_pos_llh_latitude, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(ins_pos_llh_latitude, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_pos_llh_1_unpack(
    struct vehicle_rt_sb_ins_pos_llh_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t ins_pos_llh_latitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_pos_llh_latitude = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_pos_llh_longitude = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_ins_pos_llh_altitude = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_ins_pos_llh_latitude = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->accuracy_ins_pos_llh_longitude = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->accuracy_ins_pos_llh_altitude = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    ins_pos_llh_latitude = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    ins_pos_llh_latitude |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    ins_pos_llh_latitude |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    ins_pos_llh_latitude |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->ins_pos_llh_latitude = (int32_t)ins_pos_llh_latitude;

    return (0);
}

uint8_t vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_longitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_longitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_longitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_altitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_altitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_altitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_latitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_longitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_longitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_longitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_altitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_altitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_altitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_ins_pos_llh_1_ins_pos_llh_latitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_sb_ins_pos_llh_1_ins_pos_llh_latitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

bool vehicle_rt_sb_ins_pos_llh_1_ins_pos_llh_latitude_is_in_range(int32_t value)
{
    return ((value >= -900000000) && (value <= 900000000));
}

int vehicle_rt_sb_ins_heading_gradient_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_heading_gradient_2_t *src_p,
    size_t size)
{
    uint16_t ins_gradient;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_heading, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_gradient, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_heading, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u16(src_p->ins_heading_2, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->ins_heading_2, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->accuracy_ins_gradient, 0u, 0xffu);
    ins_gradient = (uint16_t)src_p->ins_gradient;
    dst_p[5] |= pack_left_shift_u16(ins_gradient, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(ins_gradient, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_heading_gradient_2_unpack(
    struct vehicle_rt_sb_ins_heading_gradient_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ins_gradient;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_heading = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_gradient = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_ins_heading = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->ins_heading_2 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->ins_heading_2 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->accuracy_ins_gradient = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    ins_gradient = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    ins_gradient |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);
    dst_p->ins_gradient = (int16_t)ins_gradient;

    return (0);
}

uint8_t vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_heading_gradient_2_validity_ins_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_2_validity_ins_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_2_validity_ins_gradient_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_heading_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint16_t vehicle_rt_sb_ins_heading_gradient_2_ins_heading_2_encode(double value)
{
    return (uint16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_heading_gradient_2_ins_heading_2_decode(uint16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_heading_gradient_2_ins_heading_2_is_in_range(uint16_t value)
{
    return (value <= 36000u);
}

uint8_t vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_gradient_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_heading_gradient_2_ins_gradient_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_heading_gradient_2_ins_gradient_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_heading_gradient_2_ins_gradient_is_in_range(int16_t value)
{
    return ((value >= -9000) && (value <= 9000));
}

int vehicle_rt_sb_ins_heading_gradient_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_heading_gradient_t *src_p,
    size_t size)
{
    uint16_t ins_gradient;
    uint16_t ins_heading;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_heading, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_ins_gradient, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_ins_heading, 0u, 0xffu);
    ins_heading = (uint16_t)src_p->ins_heading;
    dst_p[2] |= pack_left_shift_u16(ins_heading, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(ins_heading, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->accuracy_ins_gradient, 0u, 0xffu);
    ins_gradient = (uint16_t)src_p->ins_gradient;
    dst_p[5] |= pack_left_shift_u16(ins_gradient, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(ins_gradient, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_heading_gradient_unpack(
    struct vehicle_rt_sb_ins_heading_gradient_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t ins_gradient;
    uint16_t ins_heading;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_ins_heading = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_ins_gradient = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_ins_heading = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    ins_heading = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    ins_heading |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->ins_heading = (int16_t)ins_heading;
    dst_p->accuracy_ins_gradient = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    ins_gradient = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    ins_gradient |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);
    dst_p->ins_gradient = (int16_t)ins_gradient;

    return (0);
}

uint8_t vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_heading_gradient_validity_ins_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_validity_ins_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_validity_ins_gradient_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_heading_gradient_accuracy_ins_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_accuracy_ins_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_accuracy_ins_heading_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_heading_gradient_ins_heading_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_heading_gradient_ins_heading_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_heading_gradient_ins_heading_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

uint8_t vehicle_rt_sb_ins_heading_gradient_accuracy_ins_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_accuracy_ins_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_heading_gradient_accuracy_ins_gradient_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_heading_gradient_ins_gradient_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_heading_gradient_ins_gradient_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_heading_gradient_ins_gradient_is_in_range(int16_t value)
{
    return ((value >= -9000) && (value <= 9000));
}

int vehicle_rt_sb_ins_status_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_status_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->ins_status, 0u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_status_unpack(
    struct vehicle_rt_sb_ins_status_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->ins_status = unpack_right_shift_u8(src_p[0], 0u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_ins_status_ins_status_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_status_ins_status_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_status_ins_status_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_ins_attitude_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_ins_attitude_t *src_p,
    size_t size)
{
    uint16_t attitude_pitch;
    uint16_t attitude_roll;
    uint16_t attitude_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_yaw, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_pitch, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_roll, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_attitude, 0u, 0xffu);
    attitude_yaw = (uint16_t)src_p->attitude_yaw;
    dst_p[2] |= pack_left_shift_u16(attitude_yaw, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(attitude_yaw, 8u, 0xffu);
    attitude_pitch = (uint16_t)src_p->attitude_pitch;
    dst_p[4] |= pack_left_shift_u16(attitude_pitch, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(attitude_pitch, 8u, 0xffu);
    attitude_roll = (uint16_t)src_p->attitude_roll;
    dst_p[6] |= pack_left_shift_u16(attitude_roll, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(attitude_roll, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_ins_attitude_unpack(
    struct vehicle_rt_sb_ins_attitude_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t attitude_pitch;
    uint16_t attitude_roll;
    uint16_t attitude_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_yaw = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_pitch = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_roll = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_attitude = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    attitude_yaw = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    attitude_yaw |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->attitude_yaw = (int16_t)attitude_yaw;
    attitude_pitch = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    attitude_pitch |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->attitude_pitch = (int16_t)attitude_pitch;
    attitude_roll = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    attitude_roll |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->attitude_roll = (int16_t)attitude_roll;

    return (0);
}

uint8_t vehicle_rt_sb_ins_attitude_validity_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_attitude_validity_yaw_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_attitude_validity_yaw_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_attitude_validity_pitch_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_attitude_validity_pitch_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_attitude_validity_pitch_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_attitude_validity_roll_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_attitude_validity_roll_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_attitude_validity_roll_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_ins_attitude_accuracy_attitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_attitude_accuracy_attitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_ins_attitude_accuracy_attitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_attitude_attitude_yaw_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_attitude_attitude_yaw_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_attitude_attitude_yaw_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_attitude_attitude_pitch_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_attitude_attitude_pitch_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_attitude_attitude_pitch_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_ins_attitude_attitude_roll_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_ins_attitude_attitude_roll_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_ins_attitude_attitude_roll_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_output_status_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_output_status_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_status_timestamp, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_analogue_1, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_analogue_2, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_analogue_3, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_analogue_4, 3u, 0x08u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_pulse_output, 4u, 0x10u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_serial_output_1, 5u, 0x20u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_serial_output_2, 6u, 0x40u);
    dst_p[1] |= pack_left_shift_u8(src_p->status_trigger, 7u, 0x80u);
    dst_p[4] |= pack_left_shift_u32(src_p->gps_time, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(src_p->gps_time, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(src_p->gps_time, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(src_p->gps_time, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_output_status_unpack(
    struct vehicle_rt_sb_output_status_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_status_timestamp = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->status_analogue_1 = unpack_right_shift_u8(src_p[1], 0u, 0x01u);
    dst_p->status_analogue_2 = unpack_right_shift_u8(src_p[1], 1u, 0x02u);
    dst_p->status_analogue_3 = unpack_right_shift_u8(src_p[1], 2u, 0x04u);
    dst_p->status_analogue_4 = unpack_right_shift_u8(src_p[1], 3u, 0x08u);
    dst_p->status_pulse_output = unpack_right_shift_u8(src_p[1], 4u, 0x10u);
    dst_p->status_serial_output_1 = unpack_right_shift_u8(src_p[1], 5u, 0x20u);
    dst_p->status_serial_output_2 = unpack_right_shift_u8(src_p[1], 6u, 0x40u);
    dst_p->status_trigger = unpack_right_shift_u8(src_p[1], 7u, 0x80u);
    dst_p->gps_time = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_output_status_validity_status_timestamp_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_validity_status_timestamp_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_validity_status_timestamp_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_analogue_1_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_analogue_1_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_analogue_1_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_analogue_2_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_analogue_2_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_analogue_2_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_analogue_3_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_analogue_3_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_analogue_3_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_analogue_4_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_analogue_4_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_analogue_4_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_pulse_output_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_pulse_output_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_pulse_output_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_serial_output_1_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_serial_output_1_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_serial_output_1_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_serial_output_2_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_serial_output_2_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_serial_output_2_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_output_status_status_trigger_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_status_trigger_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_output_status_status_trigger_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint32_t vehicle_rt_sb_output_status_gps_time_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_output_status_gps_time_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_output_status_gps_time_is_in_range(uint32_t value)
{
    return (value <= 604800000u);
}

int vehicle_rt_sb_gps_heading_gradient_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_heading_gradient_2_t *src_p,
    size_t size)
{
    uint16_t gps_gradient;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_heading, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_gradient, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_heading, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u16(src_p->gps_heading_2, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->gps_heading_2, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->accuracy_gps_gradient, 0u, 0xffu);
    gps_gradient = (uint16_t)src_p->gps_gradient;
    dst_p[5] |= pack_left_shift_u16(gps_gradient, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(gps_gradient, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_heading_gradient_2_unpack(
    struct vehicle_rt_sb_gps_heading_gradient_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t gps_gradient;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_heading = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_gradient = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_heading = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->gps_heading_2 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->gps_heading_2 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->accuracy_gps_gradient = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    gps_gradient = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    gps_gradient |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);
    dst_p->gps_gradient = (int16_t)gps_gradient;

    return (0);
}

uint8_t vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_heading_gradient_2_validity_gps_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_2_validity_gps_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_2_validity_gps_gradient_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_heading_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint16_t vehicle_rt_sb_gps_heading_gradient_2_gps_heading_2_encode(double value)
{
    return (uint16_t)(value / 0.01);
}

double vehicle_rt_sb_gps_heading_gradient_2_gps_heading_2_decode(uint16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_heading_gradient_2_gps_heading_2_is_in_range(uint16_t value)
{
    return (value <= 36000u);
}

uint8_t vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_gradient_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_gps_heading_gradient_2_gps_gradient_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_gps_heading_gradient_2_gps_gradient_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_heading_gradient_2_gps_gradient_is_in_range(int16_t value)
{
    return ((value >= -9000) && (value <= 9000));
}

int vehicle_rt_sb_cumulative_distance_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_cumulative_distance_2_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_cumulative_time, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_cumulative_distance, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u32(src_p->cumulative_time, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->cumulative_time, 8u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->cumulative_time, 16u, 0xffu);
    dst_p[4] |= pack_left_shift_u32(src_p->cumulative_distance, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(src_p->cumulative_distance, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(src_p->cumulative_distance, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(src_p->cumulative_distance, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_cumulative_distance_2_unpack(
    struct vehicle_rt_sb_cumulative_distance_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_cumulative_time = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_cumulative_distance = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->cumulative_time = unpack_right_shift_u32(src_p[1], 0u, 0xffu);
    dst_p->cumulative_time |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    dst_p->cumulative_time |= unpack_left_shift_u32(src_p[3], 16u, 0xffu);
    dst_p->cumulative_distance = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    dst_p->cumulative_distance |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    dst_p->cumulative_distance |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    dst_p->cumulative_distance |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_cumulative_distance_2_validity_cumulative_distance_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_cumulative_distance_2_validity_cumulative_distance_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_cumulative_distance_2_validity_cumulative_distance_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint32_t vehicle_rt_sb_cumulative_distance_2_cumulative_time_encode(double value)
{
    return (uint32_t)(value / 0.01);
}

double vehicle_rt_sb_cumulative_distance_2_cumulative_time_decode(uint32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_cumulative_distance_2_cumulative_time_is_in_range(uint32_t value)
{
    return (value <= 16777200u);
}

uint32_t vehicle_rt_sb_cumulative_distance_2_cumulative_distance_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_cumulative_distance_2_cumulative_distance_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_cumulative_distance_2_cumulative_distance_is_in_range(uint32_t value)
{
    return (value <= 4294967000u);
}

int vehicle_rt_sb_cumulative_distance_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_cumulative_distance_1_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_cumulative_time, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_cumulative_distance, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u32(src_p->cumulative_time, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(src_p->cumulative_time, 8u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->cumulative_time, 16u, 0xffu);
    dst_p[4] |= pack_left_shift_u32(src_p->cumulative_distance, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(src_p->cumulative_distance, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(src_p->cumulative_distance, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(src_p->cumulative_distance, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_cumulative_distance_1_unpack(
    struct vehicle_rt_sb_cumulative_distance_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_cumulative_time = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_cumulative_distance = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->cumulative_time = unpack_right_shift_u32(src_p[1], 0u, 0xffu);
    dst_p->cumulative_time |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    dst_p->cumulative_time |= unpack_left_shift_u32(src_p[3], 16u, 0xffu);
    dst_p->cumulative_distance = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    dst_p->cumulative_distance |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    dst_p->cumulative_distance |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    dst_p->cumulative_distance |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_cumulative_distance_1_validity_cumulative_distance_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_cumulative_distance_1_validity_cumulative_distance_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_cumulative_distance_1_validity_cumulative_distance_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint32_t vehicle_rt_sb_cumulative_distance_1_cumulative_time_encode(double value)
{
    return (uint32_t)(value / 0.01);
}

double vehicle_rt_sb_cumulative_distance_1_cumulative_time_decode(uint32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_cumulative_distance_1_cumulative_time_is_in_range(uint32_t value)
{
    return (value <= 16777200u);
}

uint32_t vehicle_rt_sb_cumulative_distance_1_cumulative_distance_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_cumulative_distance_1_cumulative_distance_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_cumulative_distance_1_cumulative_distance_is_in_range(uint32_t value)
{
    return (value <= 4294967000u);
}

int vehicle_rt_sb_trigger_timestamp_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_trigger_timestamp_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_trigger_timestamp, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_trigger_timestamp, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->trigger_number, 0u, 0x7fu);
    dst_p[2] |= pack_left_shift_u8(src_p->trigger_timestamp_type, 7u, 0x80u);
    dst_p[3] |= pack_left_shift_u64(src_p->gps_high_resolution_time, 0u, 0xffu);
    dst_p[4] |= pack_right_shift_u64(src_p->gps_high_resolution_time, 8u, 0xffu);
    dst_p[5] |= pack_right_shift_u64(src_p->gps_high_resolution_time, 16u, 0xffu);
    dst_p[6] |= pack_right_shift_u64(src_p->gps_high_resolution_time, 24u, 0xffu);
    dst_p[7] |= pack_right_shift_u64(src_p->gps_high_resolution_time, 32u, 0xffu);

    return (8);
}

int vehicle_rt_sb_trigger_timestamp_unpack(
    struct vehicle_rt_sb_trigger_timestamp_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_trigger_timestamp = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->accuracy_trigger_timestamp = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->trigger_number = unpack_right_shift_u8(src_p[2], 0u, 0x7fu);
    dst_p->trigger_timestamp_type = unpack_right_shift_u8(src_p[2], 7u, 0x80u);
    dst_p->gps_high_resolution_time = unpack_right_shift_u64(src_p[3], 0u, 0xffu);
    dst_p->gps_high_resolution_time |= unpack_left_shift_u64(src_p[4], 8u, 0xffu);
    dst_p->gps_high_resolution_time |= unpack_left_shift_u64(src_p[5], 16u, 0xffu);
    dst_p->gps_high_resolution_time |= unpack_left_shift_u64(src_p[6], 24u, 0xffu);
    dst_p->gps_high_resolution_time |= unpack_left_shift_u64(src_p[7], 32u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_trigger_timestamp_accuracy_trigger_timestamp_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trigger_timestamp_accuracy_trigger_timestamp_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trigger_timestamp_accuracy_trigger_timestamp_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_trigger_timestamp_trigger_number_encode(double value)
{
    return (uint8_t)(value - 1.0);
}

double vehicle_rt_sb_trigger_timestamp_trigger_number_decode(uint8_t value)
{
    return ((double)value + 1.0);
}

bool vehicle_rt_sb_trigger_timestamp_trigger_number_is_in_range(uint8_t value)
{
    return (value <= 127u);
}

uint8_t vehicle_rt_sb_trigger_timestamp_trigger_timestamp_type_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trigger_timestamp_trigger_timestamp_type_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_trigger_timestamp_trigger_timestamp_type_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint64_t vehicle_rt_sb_trigger_timestamp_gps_high_resolution_time_encode(double value)
{
    return (uint64_t)(value / 0.000001);
}

double vehicle_rt_sb_trigger_timestamp_gps_high_resolution_time_decode(uint64_t value)
{
    return ((double)value * 0.000001);
}

bool vehicle_rt_sb_trigger_timestamp_gps_high_resolution_time_is_in_range(uint64_t value)
{
    return (value <= 604800000000ull);
}

int vehicle_rt_imu06_gyro_rates_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_imu06_gyro_rates_t *src_p,
    size_t size)
{
    uint16_t gyro_rate_pitch;
    uint16_t gyro_rate_roll;
    uint16_t gyro_rate_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gyro_rate_yaw, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gyro_rate_pitch, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gyro_rate_roll, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gyro_rates, 0u, 0xffu);
    gyro_rate_yaw = (uint16_t)src_p->gyro_rate_yaw;
    dst_p[2] |= pack_left_shift_u16(gyro_rate_yaw, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(gyro_rate_yaw, 8u, 0xffu);
    gyro_rate_pitch = (uint16_t)src_p->gyro_rate_pitch;
    dst_p[4] |= pack_left_shift_u16(gyro_rate_pitch, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(gyro_rate_pitch, 8u, 0xffu);
    gyro_rate_roll = (uint16_t)src_p->gyro_rate_roll;
    dst_p[6] |= pack_left_shift_u16(gyro_rate_roll, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(gyro_rate_roll, 8u, 0xffu);

    return (8);
}

int vehicle_rt_imu06_gyro_rates_unpack(
    struct vehicle_rt_imu06_gyro_rates_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t gyro_rate_pitch;
    uint16_t gyro_rate_roll;
    uint16_t gyro_rate_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gyro_rate_yaw = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gyro_rate_pitch = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_gyro_rate_roll = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_gyro_rates = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    gyro_rate_yaw = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    gyro_rate_yaw |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->gyro_rate_yaw = (int16_t)gyro_rate_yaw;
    gyro_rate_pitch = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    gyro_rate_pitch |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->gyro_rate_pitch = (int16_t)gyro_rate_pitch;
    gyro_rate_roll = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    gyro_rate_roll |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->gyro_rate_roll = (int16_t)gyro_rate_roll;

    return (0);
}

uint8_t vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_imu06_gyro_rates_validity_gyro_rate_pitch_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_gyro_rates_validity_gyro_rate_pitch_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_gyro_rates_validity_gyro_rate_pitch_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_imu06_gyro_rates_validity_gyro_rate_roll_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_gyro_rates_validity_gyro_rate_roll_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_gyro_rates_validity_gyro_rate_roll_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_imu06_gyro_rates_accuracy_gyro_rates_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_gyro_rates_accuracy_gyro_rates_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_gyro_rates_accuracy_gyro_rates_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_imu06_gyro_rates_gyro_rate_yaw_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_imu06_gyro_rates_gyro_rate_yaw_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_imu06_gyro_rates_gyro_rate_yaw_is_in_range(int16_t value)
{
    return ((value >= -32700) && (value <= 32700));
}

int16_t vehicle_rt_imu06_gyro_rates_gyro_rate_pitch_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_imu06_gyro_rates_gyro_rate_pitch_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_imu06_gyro_rates_gyro_rate_pitch_is_in_range(int16_t value)
{
    return ((value >= -32700) && (value <= 32700));
}

int16_t vehicle_rt_imu06_gyro_rates_gyro_rate_roll_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_imu06_gyro_rates_gyro_rate_roll_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_imu06_gyro_rates_gyro_rate_roll_is_in_range(int16_t value)
{
    return ((value >= -32700) && (value <= 32700));
}

int vehicle_rt_imu06_accel_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_imu06_accel_t *src_p,
    size_t size)
{
    uint16_t accel_lateral;
    uint16_t accel_longitudinal;
    uint16_t accel_vertical;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_longitudinal, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_lateral, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_vertical, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_accel, 0u, 0xffu);
    accel_longitudinal = (uint16_t)src_p->accel_longitudinal;
    dst_p[2] |= pack_left_shift_u16(accel_longitudinal, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(accel_longitudinal, 8u, 0xffu);
    accel_lateral = (uint16_t)src_p->accel_lateral;
    dst_p[4] |= pack_left_shift_u16(accel_lateral, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(accel_lateral, 8u, 0xffu);
    accel_vertical = (uint16_t)src_p->accel_vertical;
    dst_p[6] |= pack_left_shift_u16(accel_vertical, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(accel_vertical, 8u, 0xffu);

    return (8);
}

int vehicle_rt_imu06_accel_unpack(
    struct vehicle_rt_imu06_accel_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t accel_lateral;
    uint16_t accel_longitudinal;
    uint16_t accel_vertical;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_accel_longitudinal = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_accel_lateral = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_accel_vertical = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_accel = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    accel_longitudinal = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    accel_longitudinal |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->accel_longitudinal = (int16_t)accel_longitudinal;
    accel_lateral = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    accel_lateral |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->accel_lateral = (int16_t)accel_lateral;
    accel_vertical = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    accel_vertical |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->accel_vertical = (int16_t)accel_vertical;

    return (0);
}

uint8_t vehicle_rt_imu06_accel_validity_accel_longitudinal_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_accel_validity_accel_longitudinal_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_accel_validity_accel_longitudinal_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_imu06_accel_validity_accel_lateral_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_accel_validity_accel_lateral_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_accel_validity_accel_lateral_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_imu06_accel_validity_accel_vertical_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_accel_validity_accel_vertical_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_accel_validity_accel_vertical_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_imu06_accel_accuracy_accel_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_accel_accuracy_accel_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_imu06_accel_accuracy_accel_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_imu06_accel_accel_longitudinal_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_imu06_accel_accel_longitudinal_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_imu06_accel_accel_longitudinal_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_imu06_accel_accel_lateral_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_imu06_accel_accel_lateral_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_imu06_accel_accel_lateral_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_imu06_accel_accel_vertical_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_imu06_accel_accel_vertical_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_imu06_accel_accel_vertical_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_speed_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_speed_t *src_p,
    size_t size)
{
    uint32_t speed;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_speed, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_speed, 0u, 0xffu);
    speed = (uint32_t)src_p->speed;
    dst_p[2] |= pack_left_shift_u32(speed, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(speed, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(speed, 16u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(speed, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_speed_unpack(
    struct vehicle_rt_sb_speed_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t speed;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_speed = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->accuracy_speed = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    speed = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    speed |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    speed |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);
    speed |= unpack_left_shift_u32(src_p[5], 24u, 0xffu);
    dst_p->speed = (int32_t)speed;

    return (0);
}

uint8_t vehicle_rt_sb_speed_validity_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_speed_validity_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_speed_validity_speed_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_speed_accuracy_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_speed_accuracy_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_speed_accuracy_speed_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_speed_speed_encode(double value)
{
    return (int32_t)(value / 0.00001);
}

double vehicle_rt_sb_speed_speed_decode(int32_t value)
{
    return ((double)value * 0.00001);
}

bool vehicle_rt_sb_speed_speed_is_in_range(int32_t value)
{
    return ((value >= -2000000000) && (value <= 2000000000));
}

int vehicle_rt_sb_rtk_slip_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_rtk_slip_t *src_p,
    size_t size)
{
    uint16_t rtk_slip;
    uint16_t rtk_squat;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_rtk_slip, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_rtk_squat, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_rtk_baseline, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_rtk_baseline, 0u, 0xffu);
    rtk_slip = (uint16_t)src_p->rtk_slip;
    dst_p[2] |= pack_left_shift_u16(rtk_slip, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(rtk_slip, 8u, 0xffu);
    rtk_squat = (uint16_t)src_p->rtk_squat;
    dst_p[4] |= pack_left_shift_u16(rtk_squat, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(rtk_squat, 8u, 0xffu);
    dst_p[6] |= pack_left_shift_u16(src_p->rtk_baseline, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(src_p->rtk_baseline, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_rtk_slip_unpack(
    struct vehicle_rt_sb_rtk_slip_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t rtk_slip;
    uint16_t rtk_squat;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_rtk_slip = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_rtk_squat = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_rtk_baseline = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_rtk_baseline = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    rtk_slip = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    rtk_slip |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->rtk_slip = (int16_t)rtk_slip;
    rtk_squat = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    rtk_squat |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->rtk_squat = (int16_t)rtk_squat;
    dst_p->rtk_baseline = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    dst_p->rtk_baseline |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_rtk_slip_validity_rtk_slip_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_slip_validity_rtk_slip_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_slip_validity_rtk_slip_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_rtk_slip_validity_rtk_squat_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_slip_validity_rtk_squat_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_slip_validity_rtk_squat_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_rtk_slip_validity_rtk_baseline_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_slip_validity_rtk_baseline_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_slip_validity_rtk_baseline_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_rtk_slip_accuracy_rtk_baseline_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_slip_accuracy_rtk_baseline_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_slip_accuracy_rtk_baseline_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_rtk_slip_rtk_slip_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_rtk_slip_rtk_slip_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_rtk_slip_rtk_slip_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_rtk_slip_rtk_squat_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_rtk_slip_rtk_squat_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_rtk_slip_rtk_squat_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

uint16_t vehicle_rt_sb_rtk_slip_rtk_baseline_encode(double value)
{
    return (uint16_t)(value);
}

double vehicle_rt_sb_rtk_slip_rtk_baseline_decode(uint16_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_slip_rtk_baseline_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_rtk_attitude_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_rtk_attitude_t *src_p,
    size_t size)
{
    uint16_t rtk_attitude_pitch;
    uint16_t rtk_attitude_roll;
    uint16_t rtk_attitude_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_rtk_yaw, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_rtk_pitch, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_rtk_roll, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_rtk_attitude, 0u, 0xffu);
    rtk_attitude_yaw = (uint16_t)src_p->rtk_attitude_yaw;
    dst_p[2] |= pack_left_shift_u16(rtk_attitude_yaw, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(rtk_attitude_yaw, 8u, 0xffu);
    rtk_attitude_pitch = (uint16_t)src_p->rtk_attitude_pitch;
    dst_p[4] |= pack_left_shift_u16(rtk_attitude_pitch, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(rtk_attitude_pitch, 8u, 0xffu);
    rtk_attitude_roll = (uint16_t)src_p->rtk_attitude_roll;
    dst_p[6] |= pack_left_shift_u16(rtk_attitude_roll, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(rtk_attitude_roll, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_rtk_attitude_unpack(
    struct vehicle_rt_sb_rtk_attitude_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t rtk_attitude_pitch;
    uint16_t rtk_attitude_roll;
    uint16_t rtk_attitude_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_rtk_yaw = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_rtk_pitch = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_rtk_roll = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_rtk_attitude = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    rtk_attitude_yaw = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    rtk_attitude_yaw |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->rtk_attitude_yaw = (int16_t)rtk_attitude_yaw;
    rtk_attitude_pitch = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    rtk_attitude_pitch |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->rtk_attitude_pitch = (int16_t)rtk_attitude_pitch;
    rtk_attitude_roll = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    rtk_attitude_roll |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->rtk_attitude_roll = (int16_t)rtk_attitude_roll;

    return (0);
}

uint8_t vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_rtk_attitude_validity_rtk_pitch_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_attitude_validity_rtk_pitch_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_attitude_validity_rtk_pitch_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_rtk_attitude_validity_rtk_roll_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_attitude_validity_rtk_roll_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_attitude_validity_rtk_roll_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_rtk_attitude_accuracy_rtk_attitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_attitude_accuracy_rtk_attitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_rtk_attitude_accuracy_rtk_attitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_rtk_attitude_rtk_attitude_yaw_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_rtk_attitude_rtk_attitude_yaw_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_rtk_attitude_rtk_attitude_yaw_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_rtk_attitude_rtk_attitude_pitch_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_rtk_attitude_rtk_attitude_pitch_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_rtk_attitude_rtk_attitude_pitch_is_in_range(int16_t value)
{
    return ((value >= -9000) && (value <= 9000));
}

int16_t vehicle_rt_sb_rtk_attitude_rtk_attitude_roll_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_rtk_attitude_rtk_attitude_roll_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_rtk_attitude_rtk_attitude_roll_is_in_range(int16_t value)
{
    return ((value >= -9000) && (value <= 9000));
}

int vehicle_rt_sb_gps_mcycle_lean_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_mcycle_lean_t *src_p,
    size_t size)
{
    uint16_t gps_lateral_accel;
    uint16_t gps_mcycle_lean_angle;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_lateral_accel, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_mcycle_lean, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_lateral_accel, 0u, 0xffu);
    gps_lateral_accel = (uint16_t)src_p->gps_lateral_accel;
    dst_p[2] |= pack_left_shift_u16(gps_lateral_accel, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(gps_lateral_accel, 8u, 0xffu);
    gps_mcycle_lean_angle = (uint16_t)src_p->gps_mcycle_lean_angle;
    dst_p[4] |= pack_left_shift_u16(gps_mcycle_lean_angle, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(gps_mcycle_lean_angle, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_mcycle_lean_unpack(
    struct vehicle_rt_sb_gps_mcycle_lean_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t gps_lateral_accel;
    uint16_t gps_mcycle_lean_angle;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_lateral_accel = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_mcycle_lean = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_lateral_accel = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    gps_lateral_accel = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    gps_lateral_accel |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->gps_lateral_accel = (int16_t)gps_lateral_accel;
    gps_mcycle_lean_angle = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    gps_mcycle_lean_angle |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->gps_mcycle_lean_angle = (int16_t)gps_mcycle_lean_angle;

    return (0);
}

uint8_t vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_mcycle_lean_validity_gps_mcycle_lean_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_mcycle_lean_validity_gps_mcycle_lean_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_mcycle_lean_validity_gps_mcycle_lean_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_mcycle_lean_accuracy_gps_lateral_accel_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_mcycle_lean_accuracy_gps_lateral_accel_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_mcycle_lean_accuracy_gps_lateral_accel_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_gps_mcycle_lean_gps_lateral_accel_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_gps_mcycle_lean_gps_lateral_accel_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_gps_mcycle_lean_gps_lateral_accel_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_gps_mcycle_lean_gps_mcycle_lean_angle_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_gps_mcycle_lean_gps_mcycle_lean_angle_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_mcycle_lean_gps_mcycle_lean_angle_is_in_range(int16_t value)
{
    return ((value >= -9000) && (value <= 9000));
}

int vehicle_rt_sb_gps_status_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_status_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->gps_status, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->firmware_version_major, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->firmware_version_intermediate, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->firmware_version_minor, 0u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->gps_n_sv, 0u, 0xffu);
    dst_p[5] |= pack_left_shift_u8(src_p->gps_n_sv_2, 0u, 0xffu);
    dst_p[6] |= pack_left_shift_u8(src_p->gps_n_sv_rtk, 0u, 0xffu);
    dst_p[7] |= pack_left_shift_u8(src_p->rtk_status, 0u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_status_unpack(
    struct vehicle_rt_sb_gps_status_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->gps_status = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->firmware_version_major = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->firmware_version_intermediate = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->firmware_version_minor = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    dst_p->gps_n_sv = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    dst_p->gps_n_sv_2 = unpack_right_shift_u8(src_p[5], 0u, 0xffu);
    dst_p->gps_n_sv_rtk = unpack_right_shift_u8(src_p[6], 0u, 0xffu);
    dst_p->rtk_status = unpack_right_shift_u8(src_p[7], 0u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_gps_status_gps_status_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_gps_status_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_gps_status_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_status_firmware_version_major_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_firmware_version_major_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_firmware_version_major_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_status_firmware_version_intermediate_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_firmware_version_intermediate_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_firmware_version_intermediate_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_status_firmware_version_minor_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_firmware_version_minor_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_firmware_version_minor_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_status_gps_n_sv_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_gps_n_sv_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_gps_n_sv_is_in_range(uint8_t value)
{
    return (value <= 16u);
}

uint8_t vehicle_rt_sb_gps_status_gps_n_sv_2_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_gps_n_sv_2_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_gps_n_sv_2_is_in_range(uint8_t value)
{
    return (value <= 16u);
}

uint8_t vehicle_rt_sb_gps_status_gps_n_sv_rtk_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_gps_n_sv_rtk_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_gps_n_sv_rtk_is_in_range(uint8_t value)
{
    return (value <= 16u);
}

uint8_t vehicle_rt_sb_gps_status_rtk_status_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_rtk_status_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_status_rtk_status_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_gps_pos_ecef_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_pos_ecef_2_t *src_p,
    size_t size)
{
    uint32_t gps_pos_ecef_y;
    uint32_t gps_pos_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    gps_pos_ecef_y = (uint32_t)src_p->gps_pos_ecef_y;
    dst_p[0] |= pack_left_shift_u32(gps_pos_ecef_y, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(gps_pos_ecef_y, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(gps_pos_ecef_y, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(gps_pos_ecef_y, 24u, 0xffu);
    gps_pos_ecef_z = (uint32_t)src_p->gps_pos_ecef_z;
    dst_p[4] |= pack_left_shift_u32(gps_pos_ecef_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_pos_ecef_z, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_pos_ecef_z, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(gps_pos_ecef_z, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_pos_ecef_2_unpack(
    struct vehicle_rt_sb_gps_pos_ecef_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_pos_ecef_y;
    uint32_t gps_pos_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    gps_pos_ecef_y = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    gps_pos_ecef_y |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    gps_pos_ecef_y |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    gps_pos_ecef_y |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->gps_pos_ecef_y = (int32_t)gps_pos_ecef_y;
    gps_pos_ecef_z = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_pos_ecef_z |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_pos_ecef_z |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    gps_pos_ecef_z |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->gps_pos_ecef_z = (int32_t)gps_pos_ecef_z;

    return (0);
}

int32_t vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_is_in_range(int32_t value)
{
    return ((value >= -1000000000) && (value <= 1000000000));
}

int32_t vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_z_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_z_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_z_is_in_range(int32_t value)
{
    return ((value >= -1000000000) && (value <= 1000000000));
}

int vehicle_rt_sb_gps_pos_ecef_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_pos_ecef_1_t *src_p,
    size_t size)
{
    uint32_t gps_pos_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_ecef_x, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_ecef_y, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_ecef_z, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_pos_ecef_x, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->accuracy_gps_pos_ecef_y, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->accuracy_gps_pos_ecef_z, 0u, 0xffu);
    gps_pos_ecef_x = (uint32_t)src_p->gps_pos_ecef_x;
    dst_p[4] |= pack_left_shift_u32(gps_pos_ecef_x, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_pos_ecef_x, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_pos_ecef_x, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(gps_pos_ecef_x, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_pos_ecef_1_unpack(
    struct vehicle_rt_sb_gps_pos_ecef_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_pos_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_pos_ecef_x = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_pos_ecef_y = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_gps_pos_ecef_z = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_gps_pos_ecef_x = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->accuracy_gps_pos_ecef_y = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->accuracy_gps_pos_ecef_z = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    gps_pos_ecef_x = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_pos_ecef_x |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_pos_ecef_x |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    gps_pos_ecef_x |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->gps_pos_ecef_x = (int32_t)gps_pos_ecef_x;

    return (0);
}

uint8_t vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_y_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_z_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_x_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_y_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_z_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_gps_pos_ecef_1_gps_pos_ecef_x_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_gps_pos_ecef_1_gps_pos_ecef_x_decode(int32_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_pos_ecef_1_gps_pos_ecef_x_is_in_range(int32_t value)
{
    return ((value >= -1000000000) && (value <= 1000000000));
}

int vehicle_rt_sb_gps_pos_llh_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_pos_llh_2_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_altitude;
    uint32_t gps_pos_llh_longitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    gps_pos_llh_longitude = (uint32_t)src_p->gps_pos_llh_longitude;
    dst_p[0] |= pack_left_shift_u32(gps_pos_llh_longitude, 0u, 0xffu);
    dst_p[1] |= pack_right_shift_u32(gps_pos_llh_longitude, 8u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(gps_pos_llh_longitude, 16u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(gps_pos_llh_longitude, 24u, 0xffu);
    gps_pos_llh_altitude = (uint32_t)src_p->gps_pos_llh_altitude;
    dst_p[4] |= pack_left_shift_u32(gps_pos_llh_altitude, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_pos_llh_altitude, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_pos_llh_altitude, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(gps_pos_llh_altitude, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_pos_llh_2_unpack(
    struct vehicle_rt_sb_gps_pos_llh_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_altitude;
    uint32_t gps_pos_llh_longitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    gps_pos_llh_longitude = unpack_right_shift_u32(src_p[0], 0u, 0xffu);
    gps_pos_llh_longitude |= unpack_left_shift_u32(src_p[1], 8u, 0xffu);
    gps_pos_llh_longitude |= unpack_left_shift_u32(src_p[2], 16u, 0xffu);
    gps_pos_llh_longitude |= unpack_left_shift_u32(src_p[3], 24u, 0xffu);
    dst_p->gps_pos_llh_longitude = (int32_t)gps_pos_llh_longitude;
    gps_pos_llh_altitude = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_pos_llh_altitude |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_pos_llh_altitude |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    gps_pos_llh_altitude |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->gps_pos_llh_altitude = (int32_t)gps_pos_llh_altitude;

    return (0);
}

int32_t vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

bool vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_is_in_range(int32_t value)
{
    return ((value >= -1800000000) && (value <= 1800000000));
}

int32_t vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_altitude_encode(double value)
{
    return (int32_t)(value / 0.001);
}

double vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_altitude_decode(int32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_altitude_is_in_range(int32_t value)
{
    return ((value >= -1000000) && (value <= 100000000));
}

int vehicle_rt_sb_gps_pos_llh_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_pos_llh_1_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_latitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_llh_latitude, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_llh_longitude, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_pos_llh_altitude, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_pos_llh_latitude, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->accuracy_gps_pos_llh_longitude, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->accuracy_gps_pos_llh_altitude, 0u, 0xffu);
    gps_pos_llh_latitude = (uint32_t)src_p->gps_pos_llh_latitude;
    dst_p[4] |= pack_left_shift_u32(gps_pos_llh_latitude, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_pos_llh_latitude, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_pos_llh_latitude, 16u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(gps_pos_llh_latitude, 24u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_pos_llh_1_unpack(
    struct vehicle_rt_sb_gps_pos_llh_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_pos_llh_latitude;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_pos_llh_latitude = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_pos_llh_longitude = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_gps_pos_llh_altitude = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_gps_pos_llh_latitude = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->accuracy_gps_pos_llh_longitude = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->accuracy_gps_pos_llh_altitude = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    gps_pos_llh_latitude = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_pos_llh_latitude |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_pos_llh_latitude |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);
    gps_pos_llh_latitude |= unpack_left_shift_u32(src_p[7], 24u, 0xffu);
    dst_p->gps_pos_llh_latitude = (int32_t)gps_pos_llh_latitude;

    return (0);
}

uint8_t vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_longitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_longitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_longitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_altitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_altitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_altitude_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_gps_pos_llh_1_gps_pos_llh_latitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_sb_gps_pos_llh_1_gps_pos_llh_latitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

bool vehicle_rt_sb_gps_pos_llh_1_gps_pos_llh_latitude_is_in_range(int32_t value)
{
    return ((value >= -900000000) && (value <= 900000000));
}

int vehicle_rt_sb_gps_heading_gradient_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_heading_gradient_t *src_p,
    size_t size)
{
    uint16_t gps_gradient;
    uint16_t gps_heading;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_heading, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_gradient, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_heading, 0u, 0xffu);
    gps_heading = (uint16_t)src_p->gps_heading;
    dst_p[2] |= pack_left_shift_u16(gps_heading, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(gps_heading, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u8(src_p->accuracy_gps_gradient, 0u, 0xffu);
    gps_gradient = (uint16_t)src_p->gps_gradient;
    dst_p[5] |= pack_left_shift_u16(gps_gradient, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u16(gps_gradient, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_heading_gradient_unpack(
    struct vehicle_rt_sb_gps_heading_gradient_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t gps_gradient;
    uint16_t gps_heading;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_heading = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_gradient = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_heading = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    gps_heading = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    gps_heading |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->gps_heading = (int16_t)gps_heading;
    dst_p->accuracy_gps_gradient = unpack_right_shift_u8(src_p[4], 0u, 0xffu);
    gps_gradient = unpack_right_shift_u16(src_p[5], 0u, 0xffu);
    gps_gradient |= unpack_left_shift_u16(src_p[6], 8u, 0xffu);
    dst_p->gps_gradient = (int16_t)gps_gradient;

    return (0);
}

uint8_t vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_heading_gradient_validity_gps_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_validity_gps_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_validity_gps_gradient_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_heading_gradient_accuracy_gps_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_accuracy_gps_heading_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_accuracy_gps_heading_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_gps_heading_gradient_gps_heading_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_gps_heading_gradient_gps_heading_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_heading_gradient_gps_heading_is_in_range(int16_t value)
{
    return ((value >= -18000) && (value <= 18000));
}

uint8_t vehicle_rt_sb_gps_heading_gradient_accuracy_gps_gradient_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_accuracy_gps_gradient_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_heading_gradient_accuracy_gps_gradient_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_gps_heading_gradient_gps_gradient_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_gps_heading_gradient_gps_gradient_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gps_heading_gradient_gps_gradient_is_in_range(int16_t value)
{
    return ((value >= -9000) && (value <= 9000));
}

int vehicle_rt_sb_gps_vel_ecef_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_vel_ecef_2_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ecef_y;
    uint32_t gps_vel_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_vel_ecef_y, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_vel_ecef_z, 1u, 0x02u);
    gps_vel_ecef_y = (uint32_t)src_p->gps_vel_ecef_y;
    dst_p[1] |= pack_left_shift_u32(gps_vel_ecef_y, 0u, 0xffu);
    dst_p[2] |= pack_right_shift_u32(gps_vel_ecef_y, 8u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(gps_vel_ecef_y, 16u, 0xffu);
    gps_vel_ecef_z = (uint32_t)src_p->gps_vel_ecef_z;
    dst_p[4] |= pack_left_shift_u32(gps_vel_ecef_z, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_vel_ecef_z, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_vel_ecef_z, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_vel_ecef_2_unpack(
    struct vehicle_rt_sb_gps_vel_ecef_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ecef_y;
    uint32_t gps_vel_ecef_z;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_vel_ecef_y = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_vel_ecef_z = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    gps_vel_ecef_y = unpack_right_shift_u32(src_p[1], 0u, 0xffu);
    gps_vel_ecef_y |= unpack_left_shift_u32(src_p[2], 8u, 0xffu);
    gps_vel_ecef_y |= unpack_left_shift_u32(src_p[3], 16u, 0xffu);

    if ((gps_vel_ecef_y & (1u << 23)) != 0u) {
        gps_vel_ecef_y |= 0xff000000u;
    }

    dst_p->gps_vel_ecef_y = (int32_t)gps_vel_ecef_y;
    gps_vel_ecef_z = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_vel_ecef_z |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_vel_ecef_z |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((gps_vel_ecef_z & (1u << 23)) != 0u) {
        gps_vel_ecef_z |= 0xff000000u;
    }

    dst_p->gps_vel_ecef_z = (int32_t)gps_vel_ecef_z;

    return (0);
}

uint8_t vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_z_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

int32_t vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_y_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_y_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_y_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_z_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_z_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_z_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_gps_vel_ecef_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_vel_ecef_1_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_vel_ecef_x, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_vel_ecef_x, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u8(src_p->accuracy_gps_vel_ecef_y, 0u, 0xffu);
    dst_p[3] |= pack_left_shift_u8(src_p->accuracy_gps_vel_ecef_z, 0u, 0xffu);
    gps_vel_ecef_x = (uint32_t)src_p->gps_vel_ecef_x;
    dst_p[4] |= pack_left_shift_u32(gps_vel_ecef_x, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(gps_vel_ecef_x, 8u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_vel_ecef_x, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_vel_ecef_1_unpack(
    struct vehicle_rt_sb_gps_vel_ecef_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ecef_x;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_vel_ecef_x = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->accuracy_gps_vel_ecef_x = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->accuracy_gps_vel_ecef_y = unpack_right_shift_u8(src_p[2], 0u, 0xffu);
    dst_p->accuracy_gps_vel_ecef_z = unpack_right_shift_u8(src_p[3], 0u, 0xffu);
    gps_vel_ecef_x = unpack_right_shift_u32(src_p[4], 0u, 0xffu);
    gps_vel_ecef_x |= unpack_left_shift_u32(src_p[5], 8u, 0xffu);
    gps_vel_ecef_x |= unpack_left_shift_u32(src_p[6], 16u, 0xffu);

    if ((gps_vel_ecef_x & (1u << 23)) != 0u) {
        gps_vel_ecef_x |= 0xff000000u;
    }

    dst_p->gps_vel_ecef_x = (int32_t)gps_vel_ecef_x;

    return (0);
}

uint8_t vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_x_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_y_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_z_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_z_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_z_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_gps_vel_ecef_1_gps_vel_ecef_x_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_vel_ecef_1_gps_vel_ecef_x_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_vel_ecef_1_gps_vel_ecef_x_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_gps_vel_ned_2_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_vel_ned_2_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_vel_ned_d, 0u, 0x01u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_vel_d, 0u, 0xffu);
    gps_vel_ned_d = (uint32_t)src_p->gps_vel_ned_d;
    dst_p[2] |= pack_left_shift_u32(gps_vel_ned_d, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(gps_vel_ned_d, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(gps_vel_ned_d, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_vel_ned_2_unpack(
    struct vehicle_rt_sb_gps_vel_ned_2_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ned_d;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_vel_ned_d = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->accuracy_gps_vel_d = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    gps_vel_ned_d = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    gps_vel_ned_d |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    gps_vel_ned_d |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);

    if ((gps_vel_ned_d & (1u << 23)) != 0u) {
        gps_vel_ned_d |= 0xff000000u;
    }

    dst_p->gps_vel_ned_d = (int32_t)gps_vel_ned_d;

    return (0);
}

uint8_t vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_vel_ned_2_accuracy_gps_vel_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ned_2_accuracy_gps_vel_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ned_2_accuracy_gps_vel_d_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_gps_vel_ned_2_gps_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_vel_ned_2_gps_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_vel_ned_2_gps_vel_ned_d_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_gps_vel_ned_1_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_vel_ned_1_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ned_e;
    uint32_t gps_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_vel_ned_n, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_vel_ned_e, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_vel_ne, 0u, 0xffu);
    gps_vel_ned_n = (uint32_t)src_p->gps_vel_ned_n;
    dst_p[2] |= pack_left_shift_u32(gps_vel_ned_n, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(gps_vel_ned_n, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(gps_vel_ned_n, 16u, 0xffu);
    gps_vel_ned_e = (uint32_t)src_p->gps_vel_ned_e;
    dst_p[5] |= pack_left_shift_u32(gps_vel_ned_e, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(gps_vel_ned_e, 8u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(gps_vel_ned_e, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_vel_ned_1_unpack(
    struct vehicle_rt_sb_gps_vel_ned_1_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint32_t gps_vel_ned_e;
    uint32_t gps_vel_ned_n;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_vel_ned_n = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_vel_ned_e = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_vel_ne = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    gps_vel_ned_n = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    gps_vel_ned_n |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    gps_vel_ned_n |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);

    if ((gps_vel_ned_n & (1u << 23)) != 0u) {
        gps_vel_ned_n |= 0xff000000u;
    }

    dst_p->gps_vel_ned_n = (int32_t)gps_vel_ned_n;
    gps_vel_ned_e = unpack_right_shift_u32(src_p[5], 0u, 0xffu);
    gps_vel_ned_e |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    gps_vel_ned_e |= unpack_left_shift_u32(src_p[7], 16u, 0xffu);

    if ((gps_vel_ned_e & (1u << 23)) != 0u) {
        gps_vel_ned_e |= 0xff000000u;
    }

    dst_p->gps_vel_ned_e = (int32_t)gps_vel_ned_e;

    return (0);
}

uint8_t vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_e_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_e_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_e_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_vel_ned_1_accuracy_gps_vel_ne_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ned_1_accuracy_gps_vel_ne_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_vel_ned_1_accuracy_gps_vel_ne_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int32_t vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_n_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int32_t vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_e_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_e_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_e_is_in_range(int32_t value)
{
    return ((value >= -8380000) && (value <= 8380000));
}

int vehicle_rt_sb_gps_speed_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_speed_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_speed_2_d, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_speed_3_d, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_speed, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u32(src_p->gps_speed_2_d, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->gps_speed_2_d, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(src_p->gps_speed_2_d, 16u, 0xffu);
    dst_p[5] |= pack_left_shift_u32(src_p->gps_speed_3_d, 0u, 0xffu);
    dst_p[6] |= pack_right_shift_u32(src_p->gps_speed_3_d, 8u, 0xffu);
    dst_p[7] |= pack_right_shift_u32(src_p->gps_speed_3_d, 16u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_speed_unpack(
    struct vehicle_rt_sb_gps_speed_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_speed_2_d = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_speed_3_d = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_speed = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->gps_speed_2_d = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    dst_p->gps_speed_2_d |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    dst_p->gps_speed_2_d |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);
    dst_p->gps_speed_3_d = unpack_right_shift_u32(src_p[5], 0u, 0xffu);
    dst_p->gps_speed_3_d |= unpack_left_shift_u32(src_p[6], 8u, 0xffu);
    dst_p->gps_speed_3_d |= unpack_left_shift_u32(src_p[7], 16u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_speed_validity_gps_speed_3_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_speed_validity_gps_speed_3_d_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_speed_validity_gps_speed_3_d_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_speed_accuracy_gps_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_speed_accuracy_gps_speed_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_speed_accuracy_gps_speed_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint32_t vehicle_rt_sb_gps_speed_gps_speed_2_d_encode(double value)
{
    return (uint32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_speed_gps_speed_2_d_decode(uint32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_speed_gps_speed_2_d_is_in_range(uint32_t value)
{
    return (value <= 16750000u);
}

uint32_t vehicle_rt_sb_gps_speed_gps_speed_3_d_encode(double value)
{
    return (uint32_t)(value / 0.0001);
}

double vehicle_rt_sb_gps_speed_gps_speed_3_d_decode(uint32_t value)
{
    return ((double)value * 0.0001);
}

bool vehicle_rt_sb_gps_speed_gps_speed_3_d_is_in_range(uint32_t value)
{
    return (value <= 16750000u);
}

int vehicle_rt_sb_gps_time_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gps_time_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_time, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gps_week, 1u, 0x02u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gps_time, 0u, 0xffu);
    dst_p[2] |= pack_left_shift_u32(src_p->gps_time, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u32(src_p->gps_time, 8u, 0xffu);
    dst_p[4] |= pack_right_shift_u32(src_p->gps_time, 16u, 0xffu);
    dst_p[5] |= pack_right_shift_u32(src_p->gps_time, 24u, 0xffu);
    dst_p[6] |= pack_left_shift_u16(src_p->gps_week, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(src_p->gps_week, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gps_time_unpack(
    struct vehicle_rt_sb_gps_time_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gps_time = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gps_week = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->accuracy_gps_time = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    dst_p->gps_time = unpack_right_shift_u32(src_p[2], 0u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[3], 8u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[4], 16u, 0xffu);
    dst_p->gps_time |= unpack_left_shift_u32(src_p[5], 24u, 0xffu);
    dst_p->gps_week = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    dst_p->gps_week |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);

    return (0);
}

uint8_t vehicle_rt_sb_gps_time_validity_gps_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_time_validity_gps_time_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_time_validity_gps_time_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_time_validity_gps_week_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_time_validity_gps_week_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_time_validity_gps_week_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gps_time_accuracy_gps_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_time_accuracy_gps_time_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_time_accuracy_gps_time_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint32_t vehicle_rt_sb_gps_time_gps_time_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_gps_time_gps_time_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_gps_time_gps_time_is_in_range(uint32_t value)
{
    return (value <= 604800000u);
}

uint16_t vehicle_rt_sb_gps_time_gps_week_encode(double value)
{
    return (uint16_t)(value);
}

double vehicle_rt_sb_gps_time_gps_week_decode(uint16_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gps_time_gps_week_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_accel_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_accel_t *src_p,
    size_t size)
{
    uint16_t accel_lateral;
    uint16_t accel_longitudinal;
    uint16_t accel_vertical;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_longitudinal, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_lateral, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_accel_vertical, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_accel, 0u, 0xffu);
    accel_longitudinal = (uint16_t)src_p->accel_longitudinal;
    dst_p[2] |= pack_left_shift_u16(accel_longitudinal, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(accel_longitudinal, 8u, 0xffu);
    accel_lateral = (uint16_t)src_p->accel_lateral;
    dst_p[4] |= pack_left_shift_u16(accel_lateral, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(accel_lateral, 8u, 0xffu);
    accel_vertical = (uint16_t)src_p->accel_vertical;
    dst_p[6] |= pack_left_shift_u16(accel_vertical, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(accel_vertical, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_accel_unpack(
    struct vehicle_rt_sb_accel_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t accel_lateral;
    uint16_t accel_longitudinal;
    uint16_t accel_vertical;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_accel_longitudinal = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_accel_lateral = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_accel_vertical = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_accel = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    accel_longitudinal = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    accel_longitudinal |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->accel_longitudinal = (int16_t)accel_longitudinal;
    accel_lateral = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    accel_lateral |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->accel_lateral = (int16_t)accel_lateral;
    accel_vertical = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    accel_vertical |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->accel_vertical = (int16_t)accel_vertical;

    return (0);
}

uint8_t vehicle_rt_sb_accel_validity_accel_longitudinal_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_accel_validity_accel_longitudinal_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_accel_validity_accel_longitudinal_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_accel_validity_accel_lateral_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_accel_validity_accel_lateral_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_accel_validity_accel_lateral_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_accel_validity_accel_vertical_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_accel_validity_accel_vertical_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_accel_validity_accel_vertical_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_accel_accuracy_accel_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_accel_accuracy_accel_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_accel_accuracy_accel_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_accel_accel_longitudinal_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_accel_accel_longitudinal_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_accel_accel_longitudinal_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_accel_accel_lateral_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_accel_accel_lateral_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_accel_accel_lateral_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_accel_accel_vertical_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_accel_accel_vertical_decode(int16_t value)
{
    return ((double)value * 0.001);
}

bool vehicle_rt_sb_accel_accel_vertical_is_in_range(int16_t value)
{
    (void)value;

    return (true);
}

int vehicle_rt_sb_gyro_rates_pack(
    uint8_t *dst_p,
    const struct vehicle_rt_sb_gyro_rates_t *src_p,
    size_t size)
{
    uint16_t gyro_rate_pitch;
    uint16_t gyro_rate_roll;
    uint16_t gyro_rate_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 8);

    dst_p[0] |= pack_left_shift_u8(src_p->validity_gyro_rate_yaw, 0u, 0x01u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gyro_rate_pitch, 1u, 0x02u);
    dst_p[0] |= pack_left_shift_u8(src_p->validity_gyro_rate_roll, 2u, 0x04u);
    dst_p[1] |= pack_left_shift_u8(src_p->accuracy_gyro_rates, 0u, 0xffu);
    gyro_rate_yaw = (uint16_t)src_p->gyro_rate_yaw;
    dst_p[2] |= pack_left_shift_u16(gyro_rate_yaw, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(gyro_rate_yaw, 8u, 0xffu);
    gyro_rate_pitch = (uint16_t)src_p->gyro_rate_pitch;
    dst_p[4] |= pack_left_shift_u16(gyro_rate_pitch, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(gyro_rate_pitch, 8u, 0xffu);
    gyro_rate_roll = (uint16_t)src_p->gyro_rate_roll;
    dst_p[6] |= pack_left_shift_u16(gyro_rate_roll, 0u, 0xffu);
    dst_p[7] |= pack_right_shift_u16(gyro_rate_roll, 8u, 0xffu);

    return (8);
}

int vehicle_rt_sb_gyro_rates_unpack(
    struct vehicle_rt_sb_gyro_rates_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t gyro_rate_pitch;
    uint16_t gyro_rate_roll;
    uint16_t gyro_rate_yaw;

    if (size < 8u) {
        return (-EINVAL);
    }

    dst_p->validity_gyro_rate_yaw = unpack_right_shift_u8(src_p[0], 0u, 0x01u);
    dst_p->validity_gyro_rate_pitch = unpack_right_shift_u8(src_p[0], 1u, 0x02u);
    dst_p->validity_gyro_rate_roll = unpack_right_shift_u8(src_p[0], 2u, 0x04u);
    dst_p->accuracy_gyro_rates = unpack_right_shift_u8(src_p[1], 0u, 0xffu);
    gyro_rate_yaw = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    gyro_rate_yaw |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->gyro_rate_yaw = (int16_t)gyro_rate_yaw;
    gyro_rate_pitch = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    gyro_rate_pitch |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);
    dst_p->gyro_rate_pitch = (int16_t)gyro_rate_pitch;
    gyro_rate_roll = unpack_right_shift_u16(src_p[6], 0u, 0xffu);
    gyro_rate_roll |= unpack_left_shift_u16(src_p[7], 8u, 0xffu);
    dst_p->gyro_rate_roll = (int16_t)gyro_rate_roll;

    return (0);
}

uint8_t vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gyro_rates_validity_gyro_rate_pitch_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gyro_rates_validity_gyro_rate_pitch_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gyro_rates_validity_gyro_rate_pitch_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gyro_rates_validity_gyro_rate_roll_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gyro_rates_validity_gyro_rate_roll_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gyro_rates_validity_gyro_rate_roll_is_in_range(uint8_t value)
{
    return (value <= 1u);
}

uint8_t vehicle_rt_sb_gyro_rates_accuracy_gyro_rates_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gyro_rates_accuracy_gyro_rates_decode(uint8_t value)
{
    return ((double)value);
}

bool vehicle_rt_sb_gyro_rates_accuracy_gyro_rates_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int16_t vehicle_rt_sb_gyro_rates_gyro_rate_yaw_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_gyro_rates_gyro_rate_yaw_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gyro_rates_gyro_rate_yaw_is_in_range(int16_t value)
{
    return ((value >= -32700) && (value <= 32700));
}

int16_t vehicle_rt_sb_gyro_rates_gyro_rate_pitch_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_gyro_rates_gyro_rate_pitch_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gyro_rates_gyro_rate_pitch_is_in_range(int16_t value)
{
    return ((value >= -32700) && (value <= 32700));
}

int16_t vehicle_rt_sb_gyro_rates_gyro_rate_roll_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double vehicle_rt_sb_gyro_rates_gyro_rate_roll_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool vehicle_rt_sb_gyro_rates_gyro_rate_roll_is_in_range(int16_t value)
{
    return ((value >= -32700) && (value <= 32700));
}
