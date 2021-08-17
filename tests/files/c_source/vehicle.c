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

static int vehicle_rt_sb_ins_vel_body_axes_check_ranges(struct vehicle_rt_sb_ins_vel_body_axes_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_is_in_range(msg->validity_ins_vel_forwards))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_sideways_is_in_range(msg->validity_ins_vel_sideways))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_body_axes_accuracy_ins_vel_body_is_in_range(msg->accuracy_ins_vel_body))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_body_axes_ins_vel_forwards_2_d_is_in_range(msg->ins_vel_forwards_2_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_body_axes_ins_vel_sideways_2_d_is_in_range(msg->ins_vel_sideways_2_d))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vel_body_axes_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_vel_forwards,
    double validity_ins_vel_sideways,
    double accuracy_ins_vel_body,
    double ins_vel_forwards_2_d,
    double ins_vel_sideways_2_d)
{
    struct vehicle_rt_sb_ins_vel_body_axes_t msg;

    msg.validity_ins_vel_forwards = vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_encode(validity_ins_vel_forwards);
    msg.validity_ins_vel_sideways = vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_sideways_encode(validity_ins_vel_sideways);
    msg.accuracy_ins_vel_body = vehicle_rt_sb_ins_vel_body_axes_accuracy_ins_vel_body_encode(accuracy_ins_vel_body);
    msg.ins_vel_forwards_2_d = vehicle_rt_sb_ins_vel_body_axes_ins_vel_forwards_2_d_encode(ins_vel_forwards_2_d);
    msg.ins_vel_sideways_2_d = vehicle_rt_sb_ins_vel_body_axes_ins_vel_sideways_2_d_encode(ins_vel_sideways_2_d);

    int ret = vehicle_rt_sb_ins_vel_body_axes_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vel_body_axes_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vel_body_axes_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_vel_forwards,
    double *validity_ins_vel_sideways,
    double *accuracy_ins_vel_body,
    double *ins_vel_forwards_2_d,
    double *ins_vel_sideways_2_d)
{
    struct vehicle_rt_sb_ins_vel_body_axes_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vel_body_axes_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vel_body_axes_check_ranges(&msg);

    if (validity_ins_vel_forwards)
        *validity_ins_vel_forwards = vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_decode(msg.validity_ins_vel_forwards);

    if (validity_ins_vel_sideways)
        *validity_ins_vel_sideways = vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_sideways_decode(msg.validity_ins_vel_sideways);

    if (accuracy_ins_vel_body)
        *accuracy_ins_vel_body = vehicle_rt_sb_ins_vel_body_axes_accuracy_ins_vel_body_decode(msg.accuracy_ins_vel_body);

    if (ins_vel_forwards_2_d)
        *ins_vel_forwards_2_d = vehicle_rt_sb_ins_vel_body_axes_ins_vel_forwards_2_d_decode(msg.ins_vel_forwards_2_d);

    if (ins_vel_sideways_2_d)
        *ins_vel_sideways_2_d = vehicle_rt_sb_ins_vel_body_axes_ins_vel_sideways_2_d_decode(msg.ins_vel_sideways_2_d);

    return ret;
}

uint8_t vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_forwards_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_body_axes_validity_ins_vel_sideways_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_body_axes_accuracy_ins_vel_body_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_body_axes_ins_vel_forwards_2_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_body_axes_ins_vel_sideways_2_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_dl1_mk3_speed_check_ranges(struct vehicle_rt_dl1_mk3_speed_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_speed_validity_speed_is_in_range(msg->validity_speed))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_speed_accuracy_speed_is_in_range(msg->accuracy_speed))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_speed_speed_is_in_range(msg->speed))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_speed_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_speed,
    double accuracy_speed,
    double speed)
{
    struct vehicle_rt_dl1_mk3_speed_t msg;

    msg.validity_speed = vehicle_rt_dl1_mk3_speed_validity_speed_encode(validity_speed);
    msg.accuracy_speed = vehicle_rt_dl1_mk3_speed_accuracy_speed_encode(accuracy_speed);
    msg.speed = vehicle_rt_dl1_mk3_speed_speed_encode(speed);

    int ret = vehicle_rt_dl1_mk3_speed_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_speed_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_speed_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_speed,
    double *accuracy_speed,
    double *speed)
{
    struct vehicle_rt_dl1_mk3_speed_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_speed_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_speed_check_ranges(&msg);

    if (validity_speed)
        *validity_speed = vehicle_rt_dl1_mk3_speed_validity_speed_decode(msg.validity_speed);

    if (accuracy_speed)
        *accuracy_speed = vehicle_rt_dl1_mk3_speed_accuracy_speed_decode(msg.accuracy_speed);

    if (speed)
        *speed = vehicle_rt_dl1_mk3_speed_speed_decode(msg.speed);

    return ret;
}

uint8_t vehicle_rt_dl1_mk3_speed_validity_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_speed_validity_speed_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_speed_validity_speed_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_dl1_mk3_speed_accuracy_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_dl1_mk3_speed_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -20000.0);
    ret = CTOOLS_MIN(ret, 20000.0);
    return ret;
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

static int vehicle_rt_dl1_mk3_gps_time_check_ranges(struct vehicle_rt_dl1_mk3_gps_time_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_gps_time_validity_gps_time_is_in_range(msg->validity_gps_time))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_time_validity_gps_week_is_in_range(msg->validity_gps_week))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_time_accuracy_gps_time_is_in_range(msg->accuracy_gps_time))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_time_gps_time_is_in_range(msg->gps_time))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_time_gps_week_is_in_range(msg->gps_week))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_gps_time_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_time,
    double validity_gps_week,
    double accuracy_gps_time,
    double gps_time,
    double gps_week)
{
    struct vehicle_rt_dl1_mk3_gps_time_t msg;

    msg.validity_gps_time = vehicle_rt_dl1_mk3_gps_time_validity_gps_time_encode(validity_gps_time);
    msg.validity_gps_week = vehicle_rt_dl1_mk3_gps_time_validity_gps_week_encode(validity_gps_week);
    msg.accuracy_gps_time = vehicle_rt_dl1_mk3_gps_time_accuracy_gps_time_encode(accuracy_gps_time);
    msg.gps_time = vehicle_rt_dl1_mk3_gps_time_gps_time_encode(gps_time);
    msg.gps_week = vehicle_rt_dl1_mk3_gps_time_gps_week_encode(gps_week);

    int ret = vehicle_rt_dl1_mk3_gps_time_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_gps_time_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_gps_time_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_time,
    double *validity_gps_week,
    double *accuracy_gps_time,
    double *gps_time,
    double *gps_week)
{
    struct vehicle_rt_dl1_mk3_gps_time_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_gps_time_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_gps_time_check_ranges(&msg);

    if (validity_gps_time)
        *validity_gps_time = vehicle_rt_dl1_mk3_gps_time_validity_gps_time_decode(msg.validity_gps_time);

    if (validity_gps_week)
        *validity_gps_week = vehicle_rt_dl1_mk3_gps_time_validity_gps_week_decode(msg.validity_gps_week);

    if (accuracy_gps_time)
        *accuracy_gps_time = vehicle_rt_dl1_mk3_gps_time_accuracy_gps_time_decode(msg.accuracy_gps_time);

    if (gps_time)
        *gps_time = vehicle_rt_dl1_mk3_gps_time_gps_time_decode(msg.gps_time);

    if (gps_week)
        *gps_week = vehicle_rt_dl1_mk3_gps_time_gps_week_decode(msg.gps_week);

    return ret;
}

uint8_t vehicle_rt_dl1_mk3_gps_time_validity_gps_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_time_validity_gps_time_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_gps_time_validity_gps_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_time_validity_gps_week_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_time_accuracy_gps_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_time_gps_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 604800.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_time_gps_week_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 65535.0);
    return ret;
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

static int vehicle_rt_dl1_mk3_gps_pos_llh_2_check_ranges(struct vehicle_rt_dl1_mk3_gps_pos_llh_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_is_in_range(msg->gps_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_altitude_is_in_range(msg->gps_pos_llh_altitude))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_gps_pos_llh_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double gps_pos_llh_longitude,
    double gps_pos_llh_altitude)
{
    struct vehicle_rt_dl1_mk3_gps_pos_llh_2_t msg;

    msg.gps_pos_llh_longitude = vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_encode(gps_pos_llh_longitude);
    msg.gps_pos_llh_altitude = vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_altitude_encode(gps_pos_llh_altitude);

    int ret = vehicle_rt_dl1_mk3_gps_pos_llh_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_gps_pos_llh_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_gps_pos_llh_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *gps_pos_llh_longitude,
    double *gps_pos_llh_altitude)
{
    struct vehicle_rt_dl1_mk3_gps_pos_llh_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_gps_pos_llh_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_gps_pos_llh_2_check_ranges(&msg);

    if (gps_pos_llh_longitude)
        *gps_pos_llh_longitude = vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_decode(msg.gps_pos_llh_longitude);

    if (gps_pos_llh_altitude)
        *gps_pos_llh_altitude = vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_altitude_decode(msg.gps_pos_llh_altitude);

    return ret;
}

int32_t vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_pos_llh_2_gps_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -1000.0);
    ret = CTOOLS_MIN(ret, 100000.0);
    return ret;
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

static int vehicle_rt_dl1_mk3_gps_pos_llh_1_check_ranges(struct vehicle_rt_dl1_mk3_gps_pos_llh_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_is_in_range(msg->validity_gps_pos_llh_latitude))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_longitude_is_in_range(msg->validity_gps_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_altitude_is_in_range(msg->validity_gps_pos_llh_altitude))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_is_in_range(msg->accuracy_gps_pos_llh_latitude))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_is_in_range(msg->accuracy_gps_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_is_in_range(msg->accuracy_gps_pos_llh_altitude))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_pos_llh_1_gps_pos_llh_latitude_is_in_range(msg->gps_pos_llh_latitude))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_gps_pos_llh_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_pos_llh_latitude,
    double validity_gps_pos_llh_longitude,
    double validity_gps_pos_llh_altitude,
    double accuracy_gps_pos_llh_latitude,
    double accuracy_gps_pos_llh_longitude,
    double accuracy_gps_pos_llh_altitude,
    double gps_pos_llh_latitude)
{
    struct vehicle_rt_dl1_mk3_gps_pos_llh_1_t msg;

    msg.validity_gps_pos_llh_latitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_encode(validity_gps_pos_llh_latitude);
    msg.validity_gps_pos_llh_longitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_longitude_encode(validity_gps_pos_llh_longitude);
    msg.validity_gps_pos_llh_altitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_altitude_encode(validity_gps_pos_llh_altitude);
    msg.accuracy_gps_pos_llh_latitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_encode(accuracy_gps_pos_llh_latitude);
    msg.accuracy_gps_pos_llh_longitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_encode(accuracy_gps_pos_llh_longitude);
    msg.accuracy_gps_pos_llh_altitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_encode(accuracy_gps_pos_llh_altitude);
    msg.gps_pos_llh_latitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_gps_pos_llh_latitude_encode(gps_pos_llh_latitude);

    int ret = vehicle_rt_dl1_mk3_gps_pos_llh_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_gps_pos_llh_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_gps_pos_llh_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_pos_llh_latitude,
    double *validity_gps_pos_llh_longitude,
    double *validity_gps_pos_llh_altitude,
    double *accuracy_gps_pos_llh_latitude,
    double *accuracy_gps_pos_llh_longitude,
    double *accuracy_gps_pos_llh_altitude,
    double *gps_pos_llh_latitude)
{
    struct vehicle_rt_dl1_mk3_gps_pos_llh_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_gps_pos_llh_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_gps_pos_llh_1_check_ranges(&msg);

    if (validity_gps_pos_llh_latitude)
        *validity_gps_pos_llh_latitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_decode(msg.validity_gps_pos_llh_latitude);

    if (validity_gps_pos_llh_longitude)
        *validity_gps_pos_llh_longitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_longitude_decode(msg.validity_gps_pos_llh_longitude);

    if (validity_gps_pos_llh_altitude)
        *validity_gps_pos_llh_altitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_altitude_decode(msg.validity_gps_pos_llh_altitude);

    if (accuracy_gps_pos_llh_latitude)
        *accuracy_gps_pos_llh_latitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_decode(msg.accuracy_gps_pos_llh_latitude);

    if (accuracy_gps_pos_llh_longitude)
        *accuracy_gps_pos_llh_longitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_decode(msg.accuracy_gps_pos_llh_longitude);

    if (accuracy_gps_pos_llh_altitude)
        *accuracy_gps_pos_llh_altitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_decode(msg.accuracy_gps_pos_llh_altitude);

    if (gps_pos_llh_latitude)
        *gps_pos_llh_latitude = vehicle_rt_dl1_mk3_gps_pos_llh_1_gps_pos_llh_latitude_decode(msg.gps_pos_llh_latitude);

    return ret;
}

uint8_t vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_pos_llh_1_validity_gps_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_pos_llh_1_gps_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_dl1_mk3_gps_speed_check_ranges(struct vehicle_rt_dl1_mk3_gps_speed_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_is_in_range(msg->validity_gps_speed_2_d))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_3_d_is_in_range(msg->validity_gps_speed_3_d))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_speed_accuracy_gps_speed_is_in_range(msg->accuracy_gps_speed))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_speed_gps_speed_2_d_is_in_range(msg->gps_speed_2_d))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_gps_speed_gps_speed_3_d_is_in_range(msg->gps_speed_3_d))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_gps_speed_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_speed_2_d,
    double validity_gps_speed_3_d,
    double accuracy_gps_speed,
    double gps_speed_2_d,
    double gps_speed_3_d)
{
    struct vehicle_rt_dl1_mk3_gps_speed_t msg;

    msg.validity_gps_speed_2_d = vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_encode(validity_gps_speed_2_d);
    msg.validity_gps_speed_3_d = vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_3_d_encode(validity_gps_speed_3_d);
    msg.accuracy_gps_speed = vehicle_rt_dl1_mk3_gps_speed_accuracy_gps_speed_encode(accuracy_gps_speed);
    msg.gps_speed_2_d = vehicle_rt_dl1_mk3_gps_speed_gps_speed_2_d_encode(gps_speed_2_d);
    msg.gps_speed_3_d = vehicle_rt_dl1_mk3_gps_speed_gps_speed_3_d_encode(gps_speed_3_d);

    int ret = vehicle_rt_dl1_mk3_gps_speed_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_gps_speed_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_gps_speed_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_speed_2_d,
    double *validity_gps_speed_3_d,
    double *accuracy_gps_speed,
    double *gps_speed_2_d,
    double *gps_speed_3_d)
{
    struct vehicle_rt_dl1_mk3_gps_speed_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_gps_speed_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_gps_speed_check_ranges(&msg);

    if (validity_gps_speed_2_d)
        *validity_gps_speed_2_d = vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_decode(msg.validity_gps_speed_2_d);

    if (validity_gps_speed_3_d)
        *validity_gps_speed_3_d = vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_3_d_decode(msg.validity_gps_speed_3_d);

    if (accuracy_gps_speed)
        *accuracy_gps_speed = vehicle_rt_dl1_mk3_gps_speed_accuracy_gps_speed_decode(msg.accuracy_gps_speed);

    if (gps_speed_2_d)
        *gps_speed_2_d = vehicle_rt_dl1_mk3_gps_speed_gps_speed_2_d_decode(msg.gps_speed_2_d);

    if (gps_speed_3_d)
        *gps_speed_3_d = vehicle_rt_dl1_mk3_gps_speed_gps_speed_3_d_decode(msg.gps_speed_3_d);

    return ret;
}

uint8_t vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_2_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_speed_validity_gps_speed_3_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_speed_accuracy_gps_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_speed_gps_speed_2_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1675.0);
    return ret;
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

double vehicle_rt_dl1_mk3_gps_speed_gps_speed_3_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1675.0);
    return ret;
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

static int vehicle_rt_ir_temp_temp_7_check_ranges(struct vehicle_rt_ir_temp_temp_7_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_7_ir_temperature_7_is_in_range(msg->ir_temperature_7))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_7_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_7)
{
    struct vehicle_rt_ir_temp_temp_7_t msg;

    msg.ir_temperature_7 = vehicle_rt_ir_temp_temp_7_ir_temperature_7_encode(ir_temperature_7);

    int ret = vehicle_rt_ir_temp_temp_7_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_7_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_7_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_7)
{
    struct vehicle_rt_ir_temp_temp_7_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_7_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_7_check_ranges(&msg);

    if (ir_temperature_7)
        *ir_temperature_7 = vehicle_rt_ir_temp_temp_7_ir_temperature_7_decode(msg.ir_temperature_7);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_7_ir_temperature_7_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_7_ir_temperature_7_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_7_ir_temperature_7_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_rr_2_check_ranges(struct vehicle_rt_ir_temp_temp_rr_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_is_in_range(msg->ir_temperature_29))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rr_2_ir_temperature_30_is_in_range(msg->ir_temperature_30))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rr_2_ir_temperature_31_is_in_range(msg->ir_temperature_31))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rr_2_ir_temperature_32_is_in_range(msg->ir_temperature_32))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_rr_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_29,
    double ir_temperature_30,
    double ir_temperature_31,
    double ir_temperature_32)
{
    struct vehicle_rt_ir_temp_temp_rr_2_t msg;

    msg.ir_temperature_29 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_encode(ir_temperature_29);
    msg.ir_temperature_30 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_30_encode(ir_temperature_30);
    msg.ir_temperature_31 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_31_encode(ir_temperature_31);
    msg.ir_temperature_32 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_32_encode(ir_temperature_32);

    int ret = vehicle_rt_ir_temp_temp_rr_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_rr_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_rr_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_29,
    double *ir_temperature_30,
    double *ir_temperature_31,
    double *ir_temperature_32)
{
    struct vehicle_rt_ir_temp_temp_rr_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_rr_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_rr_2_check_ranges(&msg);

    if (ir_temperature_29)
        *ir_temperature_29 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_decode(msg.ir_temperature_29);

    if (ir_temperature_30)
        *ir_temperature_30 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_30_decode(msg.ir_temperature_30);

    if (ir_temperature_31)
        *ir_temperature_31 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_31_decode(msg.ir_temperature_31);

    if (ir_temperature_32)
        *ir_temperature_32 = vehicle_rt_ir_temp_temp_rr_2_ir_temperature_32_decode(msg.ir_temperature_32);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_29_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_30_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_31_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rr_2_ir_temperature_32_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_rl_2_check_ranges(struct vehicle_rt_ir_temp_temp_rl_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_is_in_range(msg->ir_temperature_21))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rl_2_ir_temperature_22_is_in_range(msg->ir_temperature_22))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rl_2_ir_temperature_23_is_in_range(msg->ir_temperature_23))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rl_2_ir_temperature_24_is_in_range(msg->ir_temperature_24))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_rl_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_21,
    double ir_temperature_22,
    double ir_temperature_23,
    double ir_temperature_24)
{
    struct vehicle_rt_ir_temp_temp_rl_2_t msg;

    msg.ir_temperature_21 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_encode(ir_temperature_21);
    msg.ir_temperature_22 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_22_encode(ir_temperature_22);
    msg.ir_temperature_23 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_23_encode(ir_temperature_23);
    msg.ir_temperature_24 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_24_encode(ir_temperature_24);

    int ret = vehicle_rt_ir_temp_temp_rl_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_rl_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_rl_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_21,
    double *ir_temperature_22,
    double *ir_temperature_23,
    double *ir_temperature_24)
{
    struct vehicle_rt_ir_temp_temp_rl_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_rl_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_rl_2_check_ranges(&msg);

    if (ir_temperature_21)
        *ir_temperature_21 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_decode(msg.ir_temperature_21);

    if (ir_temperature_22)
        *ir_temperature_22 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_22_decode(msg.ir_temperature_22);

    if (ir_temperature_23)
        *ir_temperature_23 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_23_decode(msg.ir_temperature_23);

    if (ir_temperature_24)
        *ir_temperature_24 = vehicle_rt_ir_temp_temp_rl_2_ir_temperature_24_decode(msg.ir_temperature_24);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_21_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_22_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_23_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rl_2_ir_temperature_24_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_fr_2_check_ranges(struct vehicle_rt_ir_temp_temp_fr_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_is_in_range(msg->ir_temperature_13))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fr_2_ir_temperature_14_is_in_range(msg->ir_temperature_14))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fr_2_ir_temperature_15_is_in_range(msg->ir_temperature_15))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fr_2_ir_temperature_16_is_in_range(msg->ir_temperature_16))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_fr_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_13,
    double ir_temperature_14,
    double ir_temperature_15,
    double ir_temperature_16)
{
    struct vehicle_rt_ir_temp_temp_fr_2_t msg;

    msg.ir_temperature_13 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_encode(ir_temperature_13);
    msg.ir_temperature_14 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_14_encode(ir_temperature_14);
    msg.ir_temperature_15 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_15_encode(ir_temperature_15);
    msg.ir_temperature_16 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_16_encode(ir_temperature_16);

    int ret = vehicle_rt_ir_temp_temp_fr_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_fr_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_fr_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_13,
    double *ir_temperature_14,
    double *ir_temperature_15,
    double *ir_temperature_16)
{
    struct vehicle_rt_ir_temp_temp_fr_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_fr_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_fr_2_check_ranges(&msg);

    if (ir_temperature_13)
        *ir_temperature_13 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_decode(msg.ir_temperature_13);

    if (ir_temperature_14)
        *ir_temperature_14 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_14_decode(msg.ir_temperature_14);

    if (ir_temperature_15)
        *ir_temperature_15 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_15_decode(msg.ir_temperature_15);

    if (ir_temperature_16)
        *ir_temperature_16 = vehicle_rt_ir_temp_temp_fr_2_ir_temperature_16_decode(msg.ir_temperature_16);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_13_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_14_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_15_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fr_2_ir_temperature_16_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_fl_2_check_ranges(struct vehicle_rt_ir_temp_temp_fl_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_is_in_range(msg->ir_temperature_5))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fl_2_ir_temperature_6_is_in_range(msg->ir_temperature_6))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fl_2_ir_temperature_7_is_in_range(msg->ir_temperature_7))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fl_2_ir_temperature_8_is_in_range(msg->ir_temperature_8))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_fl_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_5,
    double ir_temperature_6,
    double ir_temperature_7,
    double ir_temperature_8)
{
    struct vehicle_rt_ir_temp_temp_fl_2_t msg;

    msg.ir_temperature_5 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_encode(ir_temperature_5);
    msg.ir_temperature_6 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_6_encode(ir_temperature_6);
    msg.ir_temperature_7 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_7_encode(ir_temperature_7);
    msg.ir_temperature_8 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_8_encode(ir_temperature_8);

    int ret = vehicle_rt_ir_temp_temp_fl_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_fl_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_fl_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_5,
    double *ir_temperature_6,
    double *ir_temperature_7,
    double *ir_temperature_8)
{
    struct vehicle_rt_ir_temp_temp_fl_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_fl_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_fl_2_check_ranges(&msg);

    if (ir_temperature_5)
        *ir_temperature_5 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_decode(msg.ir_temperature_5);

    if (ir_temperature_6)
        *ir_temperature_6 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_6_decode(msg.ir_temperature_6);

    if (ir_temperature_7)
        *ir_temperature_7 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_7_decode(msg.ir_temperature_7);

    if (ir_temperature_8)
        *ir_temperature_8 = vehicle_rt_ir_temp_temp_fl_2_ir_temperature_8_decode(msg.ir_temperature_8);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_5_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_6_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_7_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fl_2_ir_temperature_8_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_rr_1_check_ranges(struct vehicle_rt_ir_temp_temp_rr_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_is_in_range(msg->ir_temperature_25))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rr_1_ir_temperature_26_is_in_range(msg->ir_temperature_26))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rr_1_ir_temperature_27_is_in_range(msg->ir_temperature_27))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rr_1_ir_temperature_28_is_in_range(msg->ir_temperature_28))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_rr_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_25,
    double ir_temperature_26,
    double ir_temperature_27,
    double ir_temperature_28)
{
    struct vehicle_rt_ir_temp_temp_rr_1_t msg;

    msg.ir_temperature_25 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_encode(ir_temperature_25);
    msg.ir_temperature_26 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_26_encode(ir_temperature_26);
    msg.ir_temperature_27 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_27_encode(ir_temperature_27);
    msg.ir_temperature_28 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_28_encode(ir_temperature_28);

    int ret = vehicle_rt_ir_temp_temp_rr_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_rr_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_rr_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_25,
    double *ir_temperature_26,
    double *ir_temperature_27,
    double *ir_temperature_28)
{
    struct vehicle_rt_ir_temp_temp_rr_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_rr_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_rr_1_check_ranges(&msg);

    if (ir_temperature_25)
        *ir_temperature_25 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_decode(msg.ir_temperature_25);

    if (ir_temperature_26)
        *ir_temperature_26 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_26_decode(msg.ir_temperature_26);

    if (ir_temperature_27)
        *ir_temperature_27 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_27_decode(msg.ir_temperature_27);

    if (ir_temperature_28)
        *ir_temperature_28 = vehicle_rt_ir_temp_temp_rr_1_ir_temperature_28_decode(msg.ir_temperature_28);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_25_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_26_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_27_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rr_1_ir_temperature_28_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_rl_1_check_ranges(struct vehicle_rt_ir_temp_temp_rl_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_is_in_range(msg->ir_temperature_17))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rl_1_ir_temperature_18_is_in_range(msg->ir_temperature_18))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rl_1_ir_temperature_19_is_in_range(msg->ir_temperature_19))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_rl_1_ir_temperature_20_is_in_range(msg->ir_temperature_20))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_rl_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_17,
    double ir_temperature_18,
    double ir_temperature_19,
    double ir_temperature_20)
{
    struct vehicle_rt_ir_temp_temp_rl_1_t msg;

    msg.ir_temperature_17 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_encode(ir_temperature_17);
    msg.ir_temperature_18 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_18_encode(ir_temperature_18);
    msg.ir_temperature_19 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_19_encode(ir_temperature_19);
    msg.ir_temperature_20 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_20_encode(ir_temperature_20);

    int ret = vehicle_rt_ir_temp_temp_rl_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_rl_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_rl_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_17,
    double *ir_temperature_18,
    double *ir_temperature_19,
    double *ir_temperature_20)
{
    struct vehicle_rt_ir_temp_temp_rl_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_rl_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_rl_1_check_ranges(&msg);

    if (ir_temperature_17)
        *ir_temperature_17 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_decode(msg.ir_temperature_17);

    if (ir_temperature_18)
        *ir_temperature_18 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_18_decode(msg.ir_temperature_18);

    if (ir_temperature_19)
        *ir_temperature_19 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_19_decode(msg.ir_temperature_19);

    if (ir_temperature_20)
        *ir_temperature_20 = vehicle_rt_ir_temp_temp_rl_1_ir_temperature_20_decode(msg.ir_temperature_20);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_17_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_18_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_19_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_rl_1_ir_temperature_20_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_fr_1_check_ranges(struct vehicle_rt_ir_temp_temp_fr_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_is_in_range(msg->ir_temperature_9))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fr_1_ir_temperature_10_is_in_range(msg->ir_temperature_10))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fr_1_ir_temperature_11_is_in_range(msg->ir_temperature_11))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fr_1_ir_temperature_12_is_in_range(msg->ir_temperature_12))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_fr_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_9,
    double ir_temperature_10,
    double ir_temperature_11,
    double ir_temperature_12)
{
    struct vehicle_rt_ir_temp_temp_fr_1_t msg;

    msg.ir_temperature_9 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_encode(ir_temperature_9);
    msg.ir_temperature_10 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_10_encode(ir_temperature_10);
    msg.ir_temperature_11 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_11_encode(ir_temperature_11);
    msg.ir_temperature_12 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_12_encode(ir_temperature_12);

    int ret = vehicle_rt_ir_temp_temp_fr_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_fr_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_fr_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_9,
    double *ir_temperature_10,
    double *ir_temperature_11,
    double *ir_temperature_12)
{
    struct vehicle_rt_ir_temp_temp_fr_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_fr_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_fr_1_check_ranges(&msg);

    if (ir_temperature_9)
        *ir_temperature_9 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_decode(msg.ir_temperature_9);

    if (ir_temperature_10)
        *ir_temperature_10 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_10_decode(msg.ir_temperature_10);

    if (ir_temperature_11)
        *ir_temperature_11 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_11_decode(msg.ir_temperature_11);

    if (ir_temperature_12)
        *ir_temperature_12 = vehicle_rt_ir_temp_temp_fr_1_ir_temperature_12_decode(msg.ir_temperature_12);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_9_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_10_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_11_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fr_1_ir_temperature_12_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_fl_1_check_ranges(struct vehicle_rt_ir_temp_temp_fl_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_is_in_range(msg->ir_temperature_1))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fl_1_ir_temperature_2_is_in_range(msg->ir_temperature_2))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fl_1_ir_temperature_3_is_in_range(msg->ir_temperature_3))
        return idx;

    idx++;

    if (!vehicle_rt_ir_temp_temp_fl_1_ir_temperature_4_is_in_range(msg->ir_temperature_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_fl_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_1,
    double ir_temperature_2,
    double ir_temperature_3,
    double ir_temperature_4)
{
    struct vehicle_rt_ir_temp_temp_fl_1_t msg;

    msg.ir_temperature_1 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_encode(ir_temperature_1);
    msg.ir_temperature_2 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_2_encode(ir_temperature_2);
    msg.ir_temperature_3 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_3_encode(ir_temperature_3);
    msg.ir_temperature_4 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_4_encode(ir_temperature_4);

    int ret = vehicle_rt_ir_temp_temp_fl_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_fl_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_fl_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_1,
    double *ir_temperature_2,
    double *ir_temperature_3,
    double *ir_temperature_4)
{
    struct vehicle_rt_ir_temp_temp_fl_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_fl_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_fl_1_check_ranges(&msg);

    if (ir_temperature_1)
        *ir_temperature_1 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_decode(msg.ir_temperature_1);

    if (ir_temperature_2)
        *ir_temperature_2 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_2_decode(msg.ir_temperature_2);

    if (ir_temperature_3)
        *ir_temperature_3 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_3_decode(msg.ir_temperature_3);

    if (ir_temperature_4)
        *ir_temperature_4 = vehicle_rt_ir_temp_temp_fl_1_ir_temperature_4_decode(msg.ir_temperature_4);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_1_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_2_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_3_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_ir_temp_temp_fl_1_ir_temperature_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_32_check_ranges(struct vehicle_rt_ir_temp_temp_32_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_32_ir_temperature_32_is_in_range(msg->ir_temperature_32))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_32_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_32)
{
    struct vehicle_rt_ir_temp_temp_32_t msg;

    msg.ir_temperature_32 = vehicle_rt_ir_temp_temp_32_ir_temperature_32_encode(ir_temperature_32);

    int ret = vehicle_rt_ir_temp_temp_32_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_32_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_32_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_32)
{
    struct vehicle_rt_ir_temp_temp_32_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_32_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_32_check_ranges(&msg);

    if (ir_temperature_32)
        *ir_temperature_32 = vehicle_rt_ir_temp_temp_32_ir_temperature_32_decode(msg.ir_temperature_32);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_32_ir_temperature_32_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_32_ir_temperature_32_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_32_ir_temperature_32_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_31_check_ranges(struct vehicle_rt_ir_temp_temp_31_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_31_ir_temperature_31_is_in_range(msg->ir_temperature_31))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_31_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_31)
{
    struct vehicle_rt_ir_temp_temp_31_t msg;

    msg.ir_temperature_31 = vehicle_rt_ir_temp_temp_31_ir_temperature_31_encode(ir_temperature_31);

    int ret = vehicle_rt_ir_temp_temp_31_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_31_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_31_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_31)
{
    struct vehicle_rt_ir_temp_temp_31_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_31_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_31_check_ranges(&msg);

    if (ir_temperature_31)
        *ir_temperature_31 = vehicle_rt_ir_temp_temp_31_ir_temperature_31_decode(msg.ir_temperature_31);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_31_ir_temperature_31_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_31_ir_temperature_31_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_31_ir_temperature_31_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_30_check_ranges(struct vehicle_rt_ir_temp_temp_30_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_30_ir_temperature_30_is_in_range(msg->ir_temperature_30))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_30_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_30)
{
    struct vehicle_rt_ir_temp_temp_30_t msg;

    msg.ir_temperature_30 = vehicle_rt_ir_temp_temp_30_ir_temperature_30_encode(ir_temperature_30);

    int ret = vehicle_rt_ir_temp_temp_30_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_30_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_30_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_30)
{
    struct vehicle_rt_ir_temp_temp_30_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_30_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_30_check_ranges(&msg);

    if (ir_temperature_30)
        *ir_temperature_30 = vehicle_rt_ir_temp_temp_30_ir_temperature_30_decode(msg.ir_temperature_30);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_30_ir_temperature_30_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_30_ir_temperature_30_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_30_ir_temperature_30_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_29_check_ranges(struct vehicle_rt_ir_temp_temp_29_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_29_ir_temperature_29_is_in_range(msg->ir_temperature_29))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_29_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_29)
{
    struct vehicle_rt_ir_temp_temp_29_t msg;

    msg.ir_temperature_29 = vehicle_rt_ir_temp_temp_29_ir_temperature_29_encode(ir_temperature_29);

    int ret = vehicle_rt_ir_temp_temp_29_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_29_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_29_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_29)
{
    struct vehicle_rt_ir_temp_temp_29_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_29_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_29_check_ranges(&msg);

    if (ir_temperature_29)
        *ir_temperature_29 = vehicle_rt_ir_temp_temp_29_ir_temperature_29_decode(msg.ir_temperature_29);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_29_ir_temperature_29_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_29_ir_temperature_29_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_29_ir_temperature_29_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_28_check_ranges(struct vehicle_rt_ir_temp_temp_28_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_28_ir_temperature_28_is_in_range(msg->ir_temperature_28))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_28_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_28)
{
    struct vehicle_rt_ir_temp_temp_28_t msg;

    msg.ir_temperature_28 = vehicle_rt_ir_temp_temp_28_ir_temperature_28_encode(ir_temperature_28);

    int ret = vehicle_rt_ir_temp_temp_28_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_28_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_28_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_28)
{
    struct vehicle_rt_ir_temp_temp_28_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_28_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_28_check_ranges(&msg);

    if (ir_temperature_28)
        *ir_temperature_28 = vehicle_rt_ir_temp_temp_28_ir_temperature_28_decode(msg.ir_temperature_28);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_28_ir_temperature_28_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_28_ir_temperature_28_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_28_ir_temperature_28_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_27_check_ranges(struct vehicle_rt_ir_temp_temp_27_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_27_ir_temperature_27_is_in_range(msg->ir_temperature_27))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_27_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_27)
{
    struct vehicle_rt_ir_temp_temp_27_t msg;

    msg.ir_temperature_27 = vehicle_rt_ir_temp_temp_27_ir_temperature_27_encode(ir_temperature_27);

    int ret = vehicle_rt_ir_temp_temp_27_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_27_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_27_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_27)
{
    struct vehicle_rt_ir_temp_temp_27_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_27_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_27_check_ranges(&msg);

    if (ir_temperature_27)
        *ir_temperature_27 = vehicle_rt_ir_temp_temp_27_ir_temperature_27_decode(msg.ir_temperature_27);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_27_ir_temperature_27_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_27_ir_temperature_27_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_27_ir_temperature_27_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_26_check_ranges(struct vehicle_rt_ir_temp_temp_26_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_26_ir_temperature_26_is_in_range(msg->ir_temperature_26))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_26_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_26)
{
    struct vehicle_rt_ir_temp_temp_26_t msg;

    msg.ir_temperature_26 = vehicle_rt_ir_temp_temp_26_ir_temperature_26_encode(ir_temperature_26);

    int ret = vehicle_rt_ir_temp_temp_26_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_26_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_26_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_26)
{
    struct vehicle_rt_ir_temp_temp_26_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_26_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_26_check_ranges(&msg);

    if (ir_temperature_26)
        *ir_temperature_26 = vehicle_rt_ir_temp_temp_26_ir_temperature_26_decode(msg.ir_temperature_26);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_26_ir_temperature_26_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_26_ir_temperature_26_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_26_ir_temperature_26_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_25_check_ranges(struct vehicle_rt_ir_temp_temp_25_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_25_ir_temperature_25_is_in_range(msg->ir_temperature_25))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_25_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_25)
{
    struct vehicle_rt_ir_temp_temp_25_t msg;

    msg.ir_temperature_25 = vehicle_rt_ir_temp_temp_25_ir_temperature_25_encode(ir_temperature_25);

    int ret = vehicle_rt_ir_temp_temp_25_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_25_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_25_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_25)
{
    struct vehicle_rt_ir_temp_temp_25_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_25_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_25_check_ranges(&msg);

    if (ir_temperature_25)
        *ir_temperature_25 = vehicle_rt_ir_temp_temp_25_ir_temperature_25_decode(msg.ir_temperature_25);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_25_ir_temperature_25_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_25_ir_temperature_25_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_25_ir_temperature_25_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_24_check_ranges(struct vehicle_rt_ir_temp_temp_24_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_24_ir_temperature_24_is_in_range(msg->ir_temperature_24))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_24_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_24)
{
    struct vehicle_rt_ir_temp_temp_24_t msg;

    msg.ir_temperature_24 = vehicle_rt_ir_temp_temp_24_ir_temperature_24_encode(ir_temperature_24);

    int ret = vehicle_rt_ir_temp_temp_24_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_24_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_24_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_24)
{
    struct vehicle_rt_ir_temp_temp_24_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_24_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_24_check_ranges(&msg);

    if (ir_temperature_24)
        *ir_temperature_24 = vehicle_rt_ir_temp_temp_24_ir_temperature_24_decode(msg.ir_temperature_24);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_24_ir_temperature_24_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_24_ir_temperature_24_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_24_ir_temperature_24_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_22_check_ranges(struct vehicle_rt_ir_temp_temp_22_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_22_ir_temperature_22_is_in_range(msg->ir_temperature_22))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_22_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_22)
{
    struct vehicle_rt_ir_temp_temp_22_t msg;

    msg.ir_temperature_22 = vehicle_rt_ir_temp_temp_22_ir_temperature_22_encode(ir_temperature_22);

    int ret = vehicle_rt_ir_temp_temp_22_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_22_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_22_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_22)
{
    struct vehicle_rt_ir_temp_temp_22_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_22_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_22_check_ranges(&msg);

    if (ir_temperature_22)
        *ir_temperature_22 = vehicle_rt_ir_temp_temp_22_ir_temperature_22_decode(msg.ir_temperature_22);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_22_ir_temperature_22_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_22_ir_temperature_22_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_22_ir_temperature_22_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_23_check_ranges(struct vehicle_rt_ir_temp_temp_23_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_23_ir_temperature_23_is_in_range(msg->ir_temperature_23))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_23_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_23)
{
    struct vehicle_rt_ir_temp_temp_23_t msg;

    msg.ir_temperature_23 = vehicle_rt_ir_temp_temp_23_ir_temperature_23_encode(ir_temperature_23);

    int ret = vehicle_rt_ir_temp_temp_23_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_23_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_23_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_23)
{
    struct vehicle_rt_ir_temp_temp_23_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_23_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_23_check_ranges(&msg);

    if (ir_temperature_23)
        *ir_temperature_23 = vehicle_rt_ir_temp_temp_23_ir_temperature_23_decode(msg.ir_temperature_23);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_23_ir_temperature_23_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_23_ir_temperature_23_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_23_ir_temperature_23_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_21_check_ranges(struct vehicle_rt_ir_temp_temp_21_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_21_ir_temperature_21_is_in_range(msg->ir_temperature_21))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_21_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_21)
{
    struct vehicle_rt_ir_temp_temp_21_t msg;

    msg.ir_temperature_21 = vehicle_rt_ir_temp_temp_21_ir_temperature_21_encode(ir_temperature_21);

    int ret = vehicle_rt_ir_temp_temp_21_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_21_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_21_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_21)
{
    struct vehicle_rt_ir_temp_temp_21_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_21_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_21_check_ranges(&msg);

    if (ir_temperature_21)
        *ir_temperature_21 = vehicle_rt_ir_temp_temp_21_ir_temperature_21_decode(msg.ir_temperature_21);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_21_ir_temperature_21_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_21_ir_temperature_21_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_21_ir_temperature_21_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_20_check_ranges(struct vehicle_rt_ir_temp_temp_20_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_20_ir_temperature_20_is_in_range(msg->ir_temperature_20))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_20_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_20)
{
    struct vehicle_rt_ir_temp_temp_20_t msg;

    msg.ir_temperature_20 = vehicle_rt_ir_temp_temp_20_ir_temperature_20_encode(ir_temperature_20);

    int ret = vehicle_rt_ir_temp_temp_20_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_20_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_20_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_20)
{
    struct vehicle_rt_ir_temp_temp_20_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_20_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_20_check_ranges(&msg);

    if (ir_temperature_20)
        *ir_temperature_20 = vehicle_rt_ir_temp_temp_20_ir_temperature_20_decode(msg.ir_temperature_20);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_20_ir_temperature_20_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_20_ir_temperature_20_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_20_ir_temperature_20_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_19_check_ranges(struct vehicle_rt_ir_temp_temp_19_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_19_ir_temperature_19_is_in_range(msg->ir_temperature_19))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_19_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_19)
{
    struct vehicle_rt_ir_temp_temp_19_t msg;

    msg.ir_temperature_19 = vehicle_rt_ir_temp_temp_19_ir_temperature_19_encode(ir_temperature_19);

    int ret = vehicle_rt_ir_temp_temp_19_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_19_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_19_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_19)
{
    struct vehicle_rt_ir_temp_temp_19_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_19_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_19_check_ranges(&msg);

    if (ir_temperature_19)
        *ir_temperature_19 = vehicle_rt_ir_temp_temp_19_ir_temperature_19_decode(msg.ir_temperature_19);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_19_ir_temperature_19_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_19_ir_temperature_19_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_19_ir_temperature_19_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_18_check_ranges(struct vehicle_rt_ir_temp_temp_18_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_18_ir_temperature_18_is_in_range(msg->ir_temperature_18))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_18_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_18)
{
    struct vehicle_rt_ir_temp_temp_18_t msg;

    msg.ir_temperature_18 = vehicle_rt_ir_temp_temp_18_ir_temperature_18_encode(ir_temperature_18);

    int ret = vehicle_rt_ir_temp_temp_18_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_18_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_18_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_18)
{
    struct vehicle_rt_ir_temp_temp_18_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_18_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_18_check_ranges(&msg);

    if (ir_temperature_18)
        *ir_temperature_18 = vehicle_rt_ir_temp_temp_18_ir_temperature_18_decode(msg.ir_temperature_18);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_18_ir_temperature_18_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_18_ir_temperature_18_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_18_ir_temperature_18_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_16_check_ranges(struct vehicle_rt_ir_temp_temp_16_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_16_ir_temperature_16_is_in_range(msg->ir_temperature_16))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_16_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_16)
{
    struct vehicle_rt_ir_temp_temp_16_t msg;

    msg.ir_temperature_16 = vehicle_rt_ir_temp_temp_16_ir_temperature_16_encode(ir_temperature_16);

    int ret = vehicle_rt_ir_temp_temp_16_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_16_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_16_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_16)
{
    struct vehicle_rt_ir_temp_temp_16_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_16_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_16_check_ranges(&msg);

    if (ir_temperature_16)
        *ir_temperature_16 = vehicle_rt_ir_temp_temp_16_ir_temperature_16_decode(msg.ir_temperature_16);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_16_ir_temperature_16_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_16_ir_temperature_16_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_16_ir_temperature_16_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_15_check_ranges(struct vehicle_rt_ir_temp_temp_15_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_15_ir_temperature_15_is_in_range(msg->ir_temperature_15))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_15_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_15)
{
    struct vehicle_rt_ir_temp_temp_15_t msg;

    msg.ir_temperature_15 = vehicle_rt_ir_temp_temp_15_ir_temperature_15_encode(ir_temperature_15);

    int ret = vehicle_rt_ir_temp_temp_15_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_15_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_15_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_15)
{
    struct vehicle_rt_ir_temp_temp_15_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_15_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_15_check_ranges(&msg);

    if (ir_temperature_15)
        *ir_temperature_15 = vehicle_rt_ir_temp_temp_15_ir_temperature_15_decode(msg.ir_temperature_15);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_15_ir_temperature_15_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_15_ir_temperature_15_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_15_ir_temperature_15_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_14_check_ranges(struct vehicle_rt_ir_temp_temp_14_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_14_ir_temperature_14_is_in_range(msg->ir_temperature_14))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_14_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_14)
{
    struct vehicle_rt_ir_temp_temp_14_t msg;

    msg.ir_temperature_14 = vehicle_rt_ir_temp_temp_14_ir_temperature_14_encode(ir_temperature_14);

    int ret = vehicle_rt_ir_temp_temp_14_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_14_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_14_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_14)
{
    struct vehicle_rt_ir_temp_temp_14_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_14_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_14_check_ranges(&msg);

    if (ir_temperature_14)
        *ir_temperature_14 = vehicle_rt_ir_temp_temp_14_ir_temperature_14_decode(msg.ir_temperature_14);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_14_ir_temperature_14_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_14_ir_temperature_14_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_14_ir_temperature_14_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_13_check_ranges(struct vehicle_rt_ir_temp_temp_13_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_13_ir_temperature_13_is_in_range(msg->ir_temperature_13))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_13_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_13)
{
    struct vehicle_rt_ir_temp_temp_13_t msg;

    msg.ir_temperature_13 = vehicle_rt_ir_temp_temp_13_ir_temperature_13_encode(ir_temperature_13);

    int ret = vehicle_rt_ir_temp_temp_13_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_13_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_13_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_13)
{
    struct vehicle_rt_ir_temp_temp_13_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_13_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_13_check_ranges(&msg);

    if (ir_temperature_13)
        *ir_temperature_13 = vehicle_rt_ir_temp_temp_13_ir_temperature_13_decode(msg.ir_temperature_13);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_13_ir_temperature_13_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_13_ir_temperature_13_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_13_ir_temperature_13_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_12_check_ranges(struct vehicle_rt_ir_temp_temp_12_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_12_ir_temperature_12_is_in_range(msg->ir_temperature_12))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_12_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_12)
{
    struct vehicle_rt_ir_temp_temp_12_t msg;

    msg.ir_temperature_12 = vehicle_rt_ir_temp_temp_12_ir_temperature_12_encode(ir_temperature_12);

    int ret = vehicle_rt_ir_temp_temp_12_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_12_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_12_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_12)
{
    struct vehicle_rt_ir_temp_temp_12_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_12_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_12_check_ranges(&msg);

    if (ir_temperature_12)
        *ir_temperature_12 = vehicle_rt_ir_temp_temp_12_ir_temperature_12_decode(msg.ir_temperature_12);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_12_ir_temperature_12_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_12_ir_temperature_12_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_12_ir_temperature_12_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_11_check_ranges(struct vehicle_rt_ir_temp_temp_11_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_11_ir_temperature_11_is_in_range(msg->ir_temperature_11))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_11_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_11)
{
    struct vehicle_rt_ir_temp_temp_11_t msg;

    msg.ir_temperature_11 = vehicle_rt_ir_temp_temp_11_ir_temperature_11_encode(ir_temperature_11);

    int ret = vehicle_rt_ir_temp_temp_11_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_11_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_11_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_11)
{
    struct vehicle_rt_ir_temp_temp_11_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_11_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_11_check_ranges(&msg);

    if (ir_temperature_11)
        *ir_temperature_11 = vehicle_rt_ir_temp_temp_11_ir_temperature_11_decode(msg.ir_temperature_11);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_11_ir_temperature_11_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_11_ir_temperature_11_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_11_ir_temperature_11_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_10_check_ranges(struct vehicle_rt_ir_temp_temp_10_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_10_ir_temperature_10_is_in_range(msg->ir_temperature_10))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_10_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_10)
{
    struct vehicle_rt_ir_temp_temp_10_t msg;

    msg.ir_temperature_10 = vehicle_rt_ir_temp_temp_10_ir_temperature_10_encode(ir_temperature_10);

    int ret = vehicle_rt_ir_temp_temp_10_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_10_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_10_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_10)
{
    struct vehicle_rt_ir_temp_temp_10_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_10_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_10_check_ranges(&msg);

    if (ir_temperature_10)
        *ir_temperature_10 = vehicle_rt_ir_temp_temp_10_ir_temperature_10_decode(msg.ir_temperature_10);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_10_ir_temperature_10_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_10_ir_temperature_10_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_10_ir_temperature_10_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_8_check_ranges(struct vehicle_rt_ir_temp_temp_8_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_8_ir_temperature_8_is_in_range(msg->ir_temperature_8))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_8_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_8)
{
    struct vehicle_rt_ir_temp_temp_8_t msg;

    msg.ir_temperature_8 = vehicle_rt_ir_temp_temp_8_ir_temperature_8_encode(ir_temperature_8);

    int ret = vehicle_rt_ir_temp_temp_8_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_8_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_8_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_8)
{
    struct vehicle_rt_ir_temp_temp_8_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_8_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_8_check_ranges(&msg);

    if (ir_temperature_8)
        *ir_temperature_8 = vehicle_rt_ir_temp_temp_8_ir_temperature_8_decode(msg.ir_temperature_8);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_8_ir_temperature_8_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_8_ir_temperature_8_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_8_ir_temperature_8_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_9_check_ranges(struct vehicle_rt_ir_temp_temp_9_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_9_ir_temperature_9_is_in_range(msg->ir_temperature_9))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_9_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_9)
{
    struct vehicle_rt_ir_temp_temp_9_t msg;

    msg.ir_temperature_9 = vehicle_rt_ir_temp_temp_9_ir_temperature_9_encode(ir_temperature_9);

    int ret = vehicle_rt_ir_temp_temp_9_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_9_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_9_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_9)
{
    struct vehicle_rt_ir_temp_temp_9_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_9_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_9_check_ranges(&msg);

    if (ir_temperature_9)
        *ir_temperature_9 = vehicle_rt_ir_temp_temp_9_ir_temperature_9_decode(msg.ir_temperature_9);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_9_ir_temperature_9_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_9_ir_temperature_9_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_9_ir_temperature_9_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_17_check_ranges(struct vehicle_rt_ir_temp_temp_17_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_17_ir_temperature_17_is_in_range(msg->ir_temperature_17))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_17_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_17)
{
    struct vehicle_rt_ir_temp_temp_17_t msg;

    msg.ir_temperature_17 = vehicle_rt_ir_temp_temp_17_ir_temperature_17_encode(ir_temperature_17);

    int ret = vehicle_rt_ir_temp_temp_17_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_17_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_17_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_17)
{
    struct vehicle_rt_ir_temp_temp_17_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_17_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_17_check_ranges(&msg);

    if (ir_temperature_17)
        *ir_temperature_17 = vehicle_rt_ir_temp_temp_17_ir_temperature_17_decode(msg.ir_temperature_17);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_17_ir_temperature_17_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_17_ir_temperature_17_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_17_ir_temperature_17_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_6_check_ranges(struct vehicle_rt_ir_temp_temp_6_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_6_ir_temperature_6_is_in_range(msg->ir_temperature_6))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_6_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_6)
{
    struct vehicle_rt_ir_temp_temp_6_t msg;

    msg.ir_temperature_6 = vehicle_rt_ir_temp_temp_6_ir_temperature_6_encode(ir_temperature_6);

    int ret = vehicle_rt_ir_temp_temp_6_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_6_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_6_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_6)
{
    struct vehicle_rt_ir_temp_temp_6_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_6_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_6_check_ranges(&msg);

    if (ir_temperature_6)
        *ir_temperature_6 = vehicle_rt_ir_temp_temp_6_ir_temperature_6_decode(msg.ir_temperature_6);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_6_ir_temperature_6_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_6_ir_temperature_6_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_6_ir_temperature_6_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_5_check_ranges(struct vehicle_rt_ir_temp_temp_5_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_5_ir_temperature_5_is_in_range(msg->ir_temperature_5))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_5_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_5)
{
    struct vehicle_rt_ir_temp_temp_5_t msg;

    msg.ir_temperature_5 = vehicle_rt_ir_temp_temp_5_ir_temperature_5_encode(ir_temperature_5);

    int ret = vehicle_rt_ir_temp_temp_5_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_5_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_5_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_5)
{
    struct vehicle_rt_ir_temp_temp_5_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_5_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_5_check_ranges(&msg);

    if (ir_temperature_5)
        *ir_temperature_5 = vehicle_rt_ir_temp_temp_5_ir_temperature_5_decode(msg.ir_temperature_5);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_5_ir_temperature_5_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_5_ir_temperature_5_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_5_ir_temperature_5_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_4_check_ranges(struct vehicle_rt_ir_temp_temp_4_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_4_ir_temperature_4_is_in_range(msg->ir_temperature_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_4)
{
    struct vehicle_rt_ir_temp_temp_4_t msg;

    msg.ir_temperature_4 = vehicle_rt_ir_temp_temp_4_ir_temperature_4_encode(ir_temperature_4);

    int ret = vehicle_rt_ir_temp_temp_4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_4_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_4)
{
    struct vehicle_rt_ir_temp_temp_4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_4_check_ranges(&msg);

    if (ir_temperature_4)
        *ir_temperature_4 = vehicle_rt_ir_temp_temp_4_ir_temperature_4_decode(msg.ir_temperature_4);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_4_ir_temperature_4_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_4_ir_temperature_4_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_4_ir_temperature_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_3_check_ranges(struct vehicle_rt_ir_temp_temp_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_3_ir_temperature_3_is_in_range(msg->ir_temperature_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_3)
{
    struct vehicle_rt_ir_temp_temp_3_t msg;

    msg.ir_temperature_3 = vehicle_rt_ir_temp_temp_3_ir_temperature_3_encode(ir_temperature_3);

    int ret = vehicle_rt_ir_temp_temp_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_3_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_3)
{
    struct vehicle_rt_ir_temp_temp_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_3_check_ranges(&msg);

    if (ir_temperature_3)
        *ir_temperature_3 = vehicle_rt_ir_temp_temp_3_ir_temperature_3_decode(msg.ir_temperature_3);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_3_ir_temperature_3_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_3_ir_temperature_3_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_3_ir_temperature_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_2_check_ranges(struct vehicle_rt_ir_temp_temp_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_2_ir_temperature_2_is_in_range(msg->ir_temperature_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_2)
{
    struct vehicle_rt_ir_temp_temp_2_t msg;

    msg.ir_temperature_2 = vehicle_rt_ir_temp_temp_2_ir_temperature_2_encode(ir_temperature_2);

    int ret = vehicle_rt_ir_temp_temp_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_2_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_2)
{
    struct vehicle_rt_ir_temp_temp_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_2_check_ranges(&msg);

    if (ir_temperature_2)
        *ir_temperature_2 = vehicle_rt_ir_temp_temp_2_ir_temperature_2_decode(msg.ir_temperature_2);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_2_ir_temperature_2_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_2_ir_temperature_2_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_2_ir_temperature_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_ir_temp_temp_1_check_ranges(struct vehicle_rt_ir_temp_temp_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_ir_temp_temp_1_ir_temperature_1_is_in_range(msg->ir_temperature_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_ir_temp_temp_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ir_temperature_1)
{
    struct vehicle_rt_ir_temp_temp_1_t msg;

    msg.ir_temperature_1 = vehicle_rt_ir_temp_temp_1_ir_temperature_1_encode(ir_temperature_1);

    int ret = vehicle_rt_ir_temp_temp_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_ir_temp_temp_1_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_ir_temp_temp_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ir_temperature_1)
{
    struct vehicle_rt_ir_temp_temp_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_ir_temp_temp_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_ir_temp_temp_1_check_ranges(&msg);

    if (ir_temperature_1)
        *ir_temperature_1 = vehicle_rt_ir_temp_temp_1_ir_temperature_1_decode(msg.ir_temperature_1);

    return ret;
}

int16_t vehicle_rt_ir_temp_temp_1_ir_temperature_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_ir_temp_temp_1_ir_temperature_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_ir_temp_temp_1_ir_temperature_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_sb_trig_final_condition_check_ranges(struct vehicle_rt_sb_trig_final_condition_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_trig_final_condition_validity_final_speed_is_in_range(msg->validity_final_speed))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_final_condition_final_speed_is_in_range(msg->final_speed))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_trig_final_condition_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_final_speed,
    double final_speed)
{
    struct vehicle_rt_sb_trig_final_condition_t msg;

    msg.validity_final_speed = vehicle_rt_sb_trig_final_condition_validity_final_speed_encode(validity_final_speed);
    msg.final_speed = vehicle_rt_sb_trig_final_condition_final_speed_encode(final_speed);

    int ret = vehicle_rt_sb_trig_final_condition_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_trig_final_condition_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_trig_final_condition_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_final_speed,
    double *final_speed)
{
    struct vehicle_rt_sb_trig_final_condition_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_trig_final_condition_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_trig_final_condition_check_ranges(&msg);

    if (validity_final_speed)
        *validity_final_speed = vehicle_rt_sb_trig_final_condition_validity_final_speed_decode(msg.validity_final_speed);

    if (final_speed)
        *final_speed = vehicle_rt_sb_trig_final_condition_final_speed_decode(msg.final_speed);

    return ret;
}

uint8_t vehicle_rt_sb_trig_final_condition_validity_final_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_final_condition_validity_final_speed_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_trig_final_condition_validity_final_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_trig_final_condition_final_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1675.0);
    return ret;
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

static int vehicle_rt_sb_trig_initial_condition_check_ranges(struct vehicle_rt_sb_trig_initial_condition_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_trig_initial_condition_validity_initial_speed_is_in_range(msg->validity_initial_speed))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_initial_condition_validity_initial_heading_is_in_range(msg->validity_initial_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_initial_condition_initial_speed_is_in_range(msg->initial_speed))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_initial_condition_initial_heading_is_in_range(msg->initial_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_initial_condition_mfdd_start_threshold_is_in_range(msg->mfdd_start_threshold))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_initial_condition_mfdd_end_threshold_is_in_range(msg->mfdd_end_threshold))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_trig_initial_condition_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_initial_speed,
    double validity_initial_heading,
    double initial_speed,
    double initial_heading,
    double mfdd_start_threshold,
    double mfdd_end_threshold)
{
    struct vehicle_rt_sb_trig_initial_condition_t msg;

    msg.validity_initial_speed = vehicle_rt_sb_trig_initial_condition_validity_initial_speed_encode(validity_initial_speed);
    msg.validity_initial_heading = vehicle_rt_sb_trig_initial_condition_validity_initial_heading_encode(validity_initial_heading);
    msg.initial_speed = vehicle_rt_sb_trig_initial_condition_initial_speed_encode(initial_speed);
    msg.initial_heading = vehicle_rt_sb_trig_initial_condition_initial_heading_encode(initial_heading);
    msg.mfdd_start_threshold = vehicle_rt_sb_trig_initial_condition_mfdd_start_threshold_encode(mfdd_start_threshold);
    msg.mfdd_end_threshold = vehicle_rt_sb_trig_initial_condition_mfdd_end_threshold_encode(mfdd_end_threshold);

    int ret = vehicle_rt_sb_trig_initial_condition_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_trig_initial_condition_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_trig_initial_condition_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_initial_speed,
    double *validity_initial_heading,
    double *initial_speed,
    double *initial_heading,
    double *mfdd_start_threshold,
    double *mfdd_end_threshold)
{
    struct vehicle_rt_sb_trig_initial_condition_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_trig_initial_condition_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_trig_initial_condition_check_ranges(&msg);

    if (validity_initial_speed)
        *validity_initial_speed = vehicle_rt_sb_trig_initial_condition_validity_initial_speed_decode(msg.validity_initial_speed);

    if (validity_initial_heading)
        *validity_initial_heading = vehicle_rt_sb_trig_initial_condition_validity_initial_heading_decode(msg.validity_initial_heading);

    if (initial_speed)
        *initial_speed = vehicle_rt_sb_trig_initial_condition_initial_speed_decode(msg.initial_speed);

    if (initial_heading)
        *initial_heading = vehicle_rt_sb_trig_initial_condition_initial_heading_decode(msg.initial_heading);

    if (mfdd_start_threshold)
        *mfdd_start_threshold = vehicle_rt_sb_trig_initial_condition_mfdd_start_threshold_decode(msg.mfdd_start_threshold);

    if (mfdd_end_threshold)
        *mfdd_end_threshold = vehicle_rt_sb_trig_initial_condition_mfdd_end_threshold_decode(msg.mfdd_end_threshold);

    return ret;
}

uint8_t vehicle_rt_sb_trig_initial_condition_validity_initial_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_initial_condition_validity_initial_speed_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_trig_initial_condition_validity_initial_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_trig_initial_condition_validity_initial_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_trig_initial_condition_initial_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1675.0);
    return ret;
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

double vehicle_rt_sb_trig_initial_condition_initial_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_trig_initial_condition_mfdd_start_threshold_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
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

double vehicle_rt_sb_trig_initial_condition_mfdd_end_threshold_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 100.0);
    return ret;
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

static int vehicle_rt_sb_trig_direct_dist_check_ranges(struct vehicle_rt_sb_trig_direct_dist_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_trig_direct_dist_direct_distance_is_in_range(msg->direct_distance))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_direct_dist_path_distance_2_d_is_in_range(msg->path_distance_2_d))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_trig_direct_dist_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double direct_distance,
    double path_distance_2_d)
{
    struct vehicle_rt_sb_trig_direct_dist_t msg;

    msg.direct_distance = vehicle_rt_sb_trig_direct_dist_direct_distance_encode(direct_distance);
    msg.path_distance_2_d = vehicle_rt_sb_trig_direct_dist_path_distance_2_d_encode(path_distance_2_d);

    int ret = vehicle_rt_sb_trig_direct_dist_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_trig_direct_dist_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_trig_direct_dist_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *direct_distance,
    double *path_distance_2_d)
{
    struct vehicle_rt_sb_trig_direct_dist_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_trig_direct_dist_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_trig_direct_dist_check_ranges(&msg);

    if (direct_distance)
        *direct_distance = vehicle_rt_sb_trig_direct_dist_direct_distance_decode(msg.direct_distance);

    if (path_distance_2_d)
        *path_distance_2_d = vehicle_rt_sb_trig_direct_dist_path_distance_2_d_decode(msg.path_distance_2_d);

    return ret;
}

uint32_t vehicle_rt_sb_trig_direct_dist_direct_distance_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_direct_dist_direct_distance_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_sb_trig_direct_dist_direct_distance_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 4294967.0);
    return ret;
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

double vehicle_rt_sb_trig_direct_dist_path_distance_2_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 4294967.0);
    return ret;
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

static int vehicle_rt_sb_trig_forward_dist_check_ranges(struct vehicle_rt_sb_trig_forward_dist_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_trig_forward_dist_forward_distance_is_in_range(msg->forward_distance))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_forward_dist_deviation_distance_is_in_range(msg->deviation_distance))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_trig_forward_dist_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double forward_distance,
    double deviation_distance)
{
    struct vehicle_rt_sb_trig_forward_dist_t msg;

    msg.forward_distance = vehicle_rt_sb_trig_forward_dist_forward_distance_encode(forward_distance);
    msg.deviation_distance = vehicle_rt_sb_trig_forward_dist_deviation_distance_encode(deviation_distance);

    int ret = vehicle_rt_sb_trig_forward_dist_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_trig_forward_dist_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_trig_forward_dist_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *forward_distance,
    double *deviation_distance)
{
    struct vehicle_rt_sb_trig_forward_dist_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_trig_forward_dist_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_trig_forward_dist_check_ranges(&msg);

    if (forward_distance)
        *forward_distance = vehicle_rt_sb_trig_forward_dist_forward_distance_decode(msg.forward_distance);

    if (deviation_distance)
        *deviation_distance = vehicle_rt_sb_trig_forward_dist_deviation_distance_decode(msg.deviation_distance);

    return ret;
}

int32_t vehicle_rt_sb_trig_forward_dist_forward_distance_encode(double value)
{
    return (int32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_forward_dist_forward_distance_decode(int32_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_sb_trig_forward_dist_forward_distance_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -2147483.648);
    ret = CTOOLS_MIN(ret, 2147483.647);
    return ret;
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

double vehicle_rt_sb_trig_forward_dist_deviation_distance_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -2147483.648);
    ret = CTOOLS_MIN(ret, 2147483.647);
    return ret;
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

static int vehicle_rt_sb_trig_path_dist_check_ranges(struct vehicle_rt_sb_trig_path_dist_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_trig_path_dist_path_distance_3_d_is_in_range(msg->path_distance_3_d))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_trig_path_dist_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double path_distance_3_d)
{
    struct vehicle_rt_sb_trig_path_dist_t msg;

    msg.path_distance_3_d = vehicle_rt_sb_trig_path_dist_path_distance_3_d_encode(path_distance_3_d);

    int ret = vehicle_rt_sb_trig_path_dist_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_trig_path_dist_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_trig_path_dist_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *path_distance_3_d)
{
    struct vehicle_rt_sb_trig_path_dist_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_trig_path_dist_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_trig_path_dist_check_ranges(&msg);

    if (path_distance_3_d)
        *path_distance_3_d = vehicle_rt_sb_trig_path_dist_path_distance_3_d_decode(msg.path_distance_3_d);

    return ret;
}

uint32_t vehicle_rt_sb_trig_path_dist_path_distance_3_d_encode(double value)
{
    return (uint32_t)(value / 0.001);
}

double vehicle_rt_sb_trig_path_dist_path_distance_3_d_decode(uint32_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_sb_trig_path_dist_path_distance_3_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 4294967.0);
    return ret;
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

static int vehicle_rt_sb_trig_accel_check_ranges(struct vehicle_rt_sb_trig_accel_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_trig_accel_validity_mfdd_is_in_range(msg->validity_mfdd))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_accel_validity_average_accel_is_in_range(msg->validity_average_accel))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_accel_validity_triggered_time_is_in_range(msg->validity_triggered_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_accel_mfdd_is_in_range(msg->mfdd))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_accel_average_accel_is_in_range(msg->average_accel))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trig_accel_triggered_time_is_in_range(msg->triggered_time))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_trig_accel_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_mfdd,
    double validity_average_accel,
    double validity_triggered_time,
    double mfdd,
    double average_accel,
    double triggered_time)
{
    struct vehicle_rt_sb_trig_accel_t msg;

    msg.validity_mfdd = vehicle_rt_sb_trig_accel_validity_mfdd_encode(validity_mfdd);
    msg.validity_average_accel = vehicle_rt_sb_trig_accel_validity_average_accel_encode(validity_average_accel);
    msg.validity_triggered_time = vehicle_rt_sb_trig_accel_validity_triggered_time_encode(validity_triggered_time);
    msg.mfdd = vehicle_rt_sb_trig_accel_mfdd_encode(mfdd);
    msg.average_accel = vehicle_rt_sb_trig_accel_average_accel_encode(average_accel);
    msg.triggered_time = vehicle_rt_sb_trig_accel_triggered_time_encode(triggered_time);

    int ret = vehicle_rt_sb_trig_accel_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_trig_accel_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_trig_accel_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_mfdd,
    double *validity_average_accel,
    double *validity_triggered_time,
    double *mfdd,
    double *average_accel,
    double *triggered_time)
{
    struct vehicle_rt_sb_trig_accel_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_trig_accel_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_trig_accel_check_ranges(&msg);

    if (validity_mfdd)
        *validity_mfdd = vehicle_rt_sb_trig_accel_validity_mfdd_decode(msg.validity_mfdd);

    if (validity_average_accel)
        *validity_average_accel = vehicle_rt_sb_trig_accel_validity_average_accel_decode(msg.validity_average_accel);

    if (validity_triggered_time)
        *validity_triggered_time = vehicle_rt_sb_trig_accel_validity_triggered_time_decode(msg.validity_triggered_time);

    if (mfdd)
        *mfdd = vehicle_rt_sb_trig_accel_mfdd_decode(msg.mfdd);

    if (average_accel)
        *average_accel = vehicle_rt_sb_trig_accel_average_accel_decode(msg.average_accel);

    if (triggered_time)
        *triggered_time = vehicle_rt_sb_trig_accel_triggered_time_decode(msg.triggered_time);

    return ret;
}

uint8_t vehicle_rt_sb_trig_accel_validity_mfdd_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trig_accel_validity_mfdd_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_trig_accel_validity_mfdd_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_trig_accel_validity_average_accel_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_trig_accel_validity_triggered_time_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_trig_accel_mfdd_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_sb_trig_accel_average_accel_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_sb_trig_accel_triggered_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 167772.0);
    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_12_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_12_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_is_in_range(msg->measured_time_12))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_12_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_12)
{
    struct vehicle_rt_dl1_mk3_measure_time_12_t msg;

    msg.measured_time_12 = vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_encode(measured_time_12);

    int ret = vehicle_rt_dl1_mk3_measure_time_12_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_12_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_12_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_12)
{
    struct vehicle_rt_dl1_mk3_measure_time_12_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_12_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_12_check_ranges(&msg);

    if (measured_time_12)
        *measured_time_12 = vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_decode(msg.measured_time_12);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_12_measured_time_12_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_11_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_11_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_is_in_range(msg->measured_time_11))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_11_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_11)
{
    struct vehicle_rt_dl1_mk3_measure_time_11_t msg;

    msg.measured_time_11 = vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_encode(measured_time_11);

    int ret = vehicle_rt_dl1_mk3_measure_time_11_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_11_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_11_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_11)
{
    struct vehicle_rt_dl1_mk3_measure_time_11_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_11_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_11_check_ranges(&msg);

    if (measured_time_11)
        *measured_time_11 = vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_decode(msg.measured_time_11);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_11_measured_time_11_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_10_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_10_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_is_in_range(msg->measured_time_10))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_10_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_10)
{
    struct vehicle_rt_dl1_mk3_measure_time_10_t msg;

    msg.measured_time_10 = vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_encode(measured_time_10);

    int ret = vehicle_rt_dl1_mk3_measure_time_10_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_10_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_10_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_10)
{
    struct vehicle_rt_dl1_mk3_measure_time_10_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_10_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_10_check_ranges(&msg);

    if (measured_time_10)
        *measured_time_10 = vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_decode(msg.measured_time_10);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_10_measured_time_10_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_9_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_9_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_is_in_range(msg->measured_time_9))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_9_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_9)
{
    struct vehicle_rt_dl1_mk3_measure_time_9_t msg;

    msg.measured_time_9 = vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_encode(measured_time_9);

    int ret = vehicle_rt_dl1_mk3_measure_time_9_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_9_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_9_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_9)
{
    struct vehicle_rt_dl1_mk3_measure_time_9_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_9_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_9_check_ranges(&msg);

    if (measured_time_9)
        *measured_time_9 = vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_decode(msg.measured_time_9);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_9_measured_time_9_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_8_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_8_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_is_in_range(msg->measured_time_8))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_8_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_8)
{
    struct vehicle_rt_dl1_mk3_measure_time_8_t msg;

    msg.measured_time_8 = vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_encode(measured_time_8);

    int ret = vehicle_rt_dl1_mk3_measure_time_8_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_8_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_8_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_8)
{
    struct vehicle_rt_dl1_mk3_measure_time_8_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_8_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_8_check_ranges(&msg);

    if (measured_time_8)
        *measured_time_8 = vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_decode(msg.measured_time_8);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_8_measured_time_8_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_7_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_7_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_is_in_range(msg->measured_time_7))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_7_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_7)
{
    struct vehicle_rt_dl1_mk3_measure_time_7_t msg;

    msg.measured_time_7 = vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_encode(measured_time_7);

    int ret = vehicle_rt_dl1_mk3_measure_time_7_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_7_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_7_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_7)
{
    struct vehicle_rt_dl1_mk3_measure_time_7_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_7_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_7_check_ranges(&msg);

    if (measured_time_7)
        *measured_time_7 = vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_decode(msg.measured_time_7);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_7_measured_time_7_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_6_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_6_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_is_in_range(msg->measured_time_6))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_6_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_6)
{
    struct vehicle_rt_dl1_mk3_measure_time_6_t msg;

    msg.measured_time_6 = vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_encode(measured_time_6);

    int ret = vehicle_rt_dl1_mk3_measure_time_6_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_6_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_6_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_6)
{
    struct vehicle_rt_dl1_mk3_measure_time_6_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_6_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_6_check_ranges(&msg);

    if (measured_time_6)
        *measured_time_6 = vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_decode(msg.measured_time_6);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_6_measured_time_6_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_5_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_5_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_is_in_range(msg->measured_time_5))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_5_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_5)
{
    struct vehicle_rt_dl1_mk3_measure_time_5_t msg;

    msg.measured_time_5 = vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_encode(measured_time_5);

    int ret = vehicle_rt_dl1_mk3_measure_time_5_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_5_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_5_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_5)
{
    struct vehicle_rt_dl1_mk3_measure_time_5_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_5_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_5_check_ranges(&msg);

    if (measured_time_5)
        *measured_time_5 = vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_decode(msg.measured_time_5);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_5_measured_time_5_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_4_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_4_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_is_in_range(msg->measured_time_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_4)
{
    struct vehicle_rt_dl1_mk3_measure_time_4_t msg;

    msg.measured_time_4 = vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_encode(measured_time_4);

    int ret = vehicle_rt_dl1_mk3_measure_time_4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_4_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_4)
{
    struct vehicle_rt_dl1_mk3_measure_time_4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_4_check_ranges(&msg);

    if (measured_time_4)
        *measured_time_4 = vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_decode(msg.measured_time_4);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_4_measured_time_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_3_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_is_in_range(msg->measured_time_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_3)
{
    struct vehicle_rt_dl1_mk3_measure_time_3_t msg;

    msg.measured_time_3 = vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_encode(measured_time_3);

    int ret = vehicle_rt_dl1_mk3_measure_time_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_3_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_3)
{
    struct vehicle_rt_dl1_mk3_measure_time_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_3_check_ranges(&msg);

    if (measured_time_3)
        *measured_time_3 = vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_decode(msg.measured_time_3);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_3_measured_time_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_2_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_is_in_range(msg->measured_time_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_2)
{
    struct vehicle_rt_dl1_mk3_measure_time_2_t msg;

    msg.measured_time_2 = vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_encode(measured_time_2);

    int ret = vehicle_rt_dl1_mk3_measure_time_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_2_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_2)
{
    struct vehicle_rt_dl1_mk3_measure_time_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_2_check_ranges(&msg);

    if (measured_time_2)
        *measured_time_2 = vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_decode(msg.measured_time_2);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_2_measured_time_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_measure_time_1_check_ranges(struct vehicle_rt_dl1_mk3_measure_time_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_is_in_range(msg->measured_time_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double measured_time_1)
{
    struct vehicle_rt_dl1_mk3_measure_time_1_t msg;

    msg.measured_time_1 = vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_encode(measured_time_1);

    int ret = vehicle_rt_dl1_mk3_measure_time_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_measure_time_1_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_measure_time_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *measured_time_1)
{
    struct vehicle_rt_dl1_mk3_measure_time_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_measure_time_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_measure_time_1_check_ranges(&msg);

    if (measured_time_1)
        *measured_time_1 = vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_decode(msg.measured_time_1);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_encode(double value)
{
    return (uint32_t)(value);
}

double vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_decode(uint32_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_measure_time_1_measured_time_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_rpm_check_ranges(struct vehicle_rt_dl1_mk3_rpm_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_rpm_rpm_is_in_range(msg->rpm))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_rpm_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double rpm)
{
    struct vehicle_rt_dl1_mk3_rpm_t msg;

    msg.rpm = vehicle_rt_dl1_mk3_rpm_rpm_encode(rpm);

    int ret = vehicle_rt_dl1_mk3_rpm_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_rpm_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_rpm_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *rpm)
{
    struct vehicle_rt_dl1_mk3_rpm_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_rpm_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_rpm_check_ranges(&msg);

    if (rpm)
        *rpm = vehicle_rt_dl1_mk3_rpm_rpm_decode(msg.rpm);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_rpm_rpm_encode(double value)
{
    return (uint16_t)(value);
}

double vehicle_rt_dl1_mk3_rpm_rpm_decode(uint16_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_rpm_rpm_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_freq_4_check_ranges(struct vehicle_rt_dl1_mk3_freq_4_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_freq_4_frequency_4_is_in_range(msg->frequency_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_freq_4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double frequency_4)
{
    struct vehicle_rt_dl1_mk3_freq_4_t msg;

    msg.frequency_4 = vehicle_rt_dl1_mk3_freq_4_frequency_4_encode(frequency_4);

    int ret = vehicle_rt_dl1_mk3_freq_4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_freq_4_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_freq_4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *frequency_4)
{
    struct vehicle_rt_dl1_mk3_freq_4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_freq_4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_freq_4_check_ranges(&msg);

    if (frequency_4)
        *frequency_4 = vehicle_rt_dl1_mk3_freq_4_frequency_4_decode(msg.frequency_4);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_freq_4_frequency_4_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_freq_4_frequency_4_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_freq_4_frequency_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_freq_3_check_ranges(struct vehicle_rt_dl1_mk3_freq_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_freq_3_frequency_3_is_in_range(msg->frequency_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_freq_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double frequency_3)
{
    struct vehicle_rt_dl1_mk3_freq_3_t msg;

    msg.frequency_3 = vehicle_rt_dl1_mk3_freq_3_frequency_3_encode(frequency_3);

    int ret = vehicle_rt_dl1_mk3_freq_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_freq_3_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_freq_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *frequency_3)
{
    struct vehicle_rt_dl1_mk3_freq_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_freq_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_freq_3_check_ranges(&msg);

    if (frequency_3)
        *frequency_3 = vehicle_rt_dl1_mk3_freq_3_frequency_3_decode(msg.frequency_3);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_freq_3_frequency_3_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_freq_3_frequency_3_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_freq_3_frequency_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_freq_2_check_ranges(struct vehicle_rt_dl1_mk3_freq_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_freq_2_frequency_2_is_in_range(msg->frequency_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_freq_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double frequency_2)
{
    struct vehicle_rt_dl1_mk3_freq_2_t msg;

    msg.frequency_2 = vehicle_rt_dl1_mk3_freq_2_frequency_2_encode(frequency_2);

    int ret = vehicle_rt_dl1_mk3_freq_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_freq_2_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_freq_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *frequency_2)
{
    struct vehicle_rt_dl1_mk3_freq_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_freq_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_freq_2_check_ranges(&msg);

    if (frequency_2)
        *frequency_2 = vehicle_rt_dl1_mk3_freq_2_frequency_2_decode(msg.frequency_2);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_freq_2_frequency_2_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_freq_2_frequency_2_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_freq_2_frequency_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_misc_3_check_ranges(struct vehicle_rt_dl1_mk3_misc_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_misc_3_misc_3_is_in_range(msg->misc_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_misc_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double misc_3)
{
    struct vehicle_rt_dl1_mk3_misc_3_t msg;

    msg.misc_3 = vehicle_rt_dl1_mk3_misc_3_misc_3_encode(misc_3);

    int ret = vehicle_rt_dl1_mk3_misc_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_misc_3_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_misc_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *misc_3)
{
    struct vehicle_rt_dl1_mk3_misc_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_misc_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_misc_3_check_ranges(&msg);

    if (misc_3)
        *misc_3 = vehicle_rt_dl1_mk3_misc_3_misc_3_decode(msg.misc_3);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_misc_3_misc_3_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_misc_3_misc_3_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_misc_3_misc_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_misc_2_check_ranges(struct vehicle_rt_dl1_mk3_misc_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_misc_2_misc_2_is_in_range(msg->misc_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_misc_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double misc_2)
{
    struct vehicle_rt_dl1_mk3_misc_2_t msg;

    msg.misc_2 = vehicle_rt_dl1_mk3_misc_2_misc_2_encode(misc_2);

    int ret = vehicle_rt_dl1_mk3_misc_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_misc_2_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_misc_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *misc_2)
{
    struct vehicle_rt_dl1_mk3_misc_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_misc_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_misc_2_check_ranges(&msg);

    if (misc_2)
        *misc_2 = vehicle_rt_dl1_mk3_misc_2_misc_2_decode(msg.misc_2);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_misc_2_misc_2_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_misc_2_misc_2_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_misc_2_misc_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_misc_1_check_ranges(struct vehicle_rt_dl1_mk3_misc_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_misc_1_misc_1_is_in_range(msg->misc_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_misc_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double misc_1)
{
    struct vehicle_rt_dl1_mk3_misc_1_t msg;

    msg.misc_1 = vehicle_rt_dl1_mk3_misc_1_misc_1_encode(misc_1);

    int ret = vehicle_rt_dl1_mk3_misc_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_misc_1_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_misc_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *misc_1)
{
    struct vehicle_rt_dl1_mk3_misc_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_misc_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_misc_1_check_ranges(&msg);

    if (misc_1)
        *misc_1 = vehicle_rt_dl1_mk3_misc_1_misc_1_decode(msg.misc_1);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_misc_1_misc_1_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_misc_1_misc_1_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_misc_1_misc_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_31_check_ranges(struct vehicle_rt_dl1_mk3_aux_31_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_31_aux_31_is_in_range(msg->aux_31))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_31_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_31)
{
    struct vehicle_rt_dl1_mk3_aux_31_t msg;

    msg.aux_31 = vehicle_rt_dl1_mk3_aux_31_aux_31_encode(aux_31);

    int ret = vehicle_rt_dl1_mk3_aux_31_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_31_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_31_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_31)
{
    struct vehicle_rt_dl1_mk3_aux_31_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_31_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_31_check_ranges(&msg);

    if (aux_31)
        *aux_31 = vehicle_rt_dl1_mk3_aux_31_aux_31_decode(msg.aux_31);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_31_aux_31_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_31_aux_31_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_31_aux_31_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_30_check_ranges(struct vehicle_rt_dl1_mk3_aux_30_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_30_aux_30_is_in_range(msg->aux_30))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_30_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_30)
{
    struct vehicle_rt_dl1_mk3_aux_30_t msg;

    msg.aux_30 = vehicle_rt_dl1_mk3_aux_30_aux_30_encode(aux_30);

    int ret = vehicle_rt_dl1_mk3_aux_30_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_30_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_30_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_30)
{
    struct vehicle_rt_dl1_mk3_aux_30_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_30_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_30_check_ranges(&msg);

    if (aux_30)
        *aux_30 = vehicle_rt_dl1_mk3_aux_30_aux_30_decode(msg.aux_30);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_30_aux_30_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_30_aux_30_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_30_aux_30_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_29_check_ranges(struct vehicle_rt_dl1_mk3_aux_29_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_29_aux_29_is_in_range(msg->aux_29))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_29_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_29)
{
    struct vehicle_rt_dl1_mk3_aux_29_t msg;

    msg.aux_29 = vehicle_rt_dl1_mk3_aux_29_aux_29_encode(aux_29);

    int ret = vehicle_rt_dl1_mk3_aux_29_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_29_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_29_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_29)
{
    struct vehicle_rt_dl1_mk3_aux_29_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_29_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_29_check_ranges(&msg);

    if (aux_29)
        *aux_29 = vehicle_rt_dl1_mk3_aux_29_aux_29_decode(msg.aux_29);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_29_aux_29_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_29_aux_29_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_29_aux_29_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_28_check_ranges(struct vehicle_rt_dl1_mk3_aux_28_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_28_aux_28_is_in_range(msg->aux_28))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_28_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_28)
{
    struct vehicle_rt_dl1_mk3_aux_28_t msg;

    msg.aux_28 = vehicle_rt_dl1_mk3_aux_28_aux_28_encode(aux_28);

    int ret = vehicle_rt_dl1_mk3_aux_28_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_28_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_28_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_28)
{
    struct vehicle_rt_dl1_mk3_aux_28_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_28_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_28_check_ranges(&msg);

    if (aux_28)
        *aux_28 = vehicle_rt_dl1_mk3_aux_28_aux_28_decode(msg.aux_28);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_28_aux_28_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_28_aux_28_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_28_aux_28_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_27_check_ranges(struct vehicle_rt_dl1_mk3_aux_27_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_27_aux_27_is_in_range(msg->aux_27))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_27_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_27)
{
    struct vehicle_rt_dl1_mk3_aux_27_t msg;

    msg.aux_27 = vehicle_rt_dl1_mk3_aux_27_aux_27_encode(aux_27);

    int ret = vehicle_rt_dl1_mk3_aux_27_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_27_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_27_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_27)
{
    struct vehicle_rt_dl1_mk3_aux_27_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_27_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_27_check_ranges(&msg);

    if (aux_27)
        *aux_27 = vehicle_rt_dl1_mk3_aux_27_aux_27_decode(msg.aux_27);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_27_aux_27_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_27_aux_27_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_27_aux_27_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_26_check_ranges(struct vehicle_rt_dl1_mk3_aux_26_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_26_aux_26_is_in_range(msg->aux_26))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_26_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_26)
{
    struct vehicle_rt_dl1_mk3_aux_26_t msg;

    msg.aux_26 = vehicle_rt_dl1_mk3_aux_26_aux_26_encode(aux_26);

    int ret = vehicle_rt_dl1_mk3_aux_26_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_26_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_26_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_26)
{
    struct vehicle_rt_dl1_mk3_aux_26_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_26_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_26_check_ranges(&msg);

    if (aux_26)
        *aux_26 = vehicle_rt_dl1_mk3_aux_26_aux_26_decode(msg.aux_26);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_26_aux_26_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_26_aux_26_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_26_aux_26_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_25_check_ranges(struct vehicle_rt_dl1_mk3_aux_25_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_25_aux_25_is_in_range(msg->aux_25))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_25_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_25)
{
    struct vehicle_rt_dl1_mk3_aux_25_t msg;

    msg.aux_25 = vehicle_rt_dl1_mk3_aux_25_aux_25_encode(aux_25);

    int ret = vehicle_rt_dl1_mk3_aux_25_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_25_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_25_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_25)
{
    struct vehicle_rt_dl1_mk3_aux_25_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_25_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_25_check_ranges(&msg);

    if (aux_25)
        *aux_25 = vehicle_rt_dl1_mk3_aux_25_aux_25_decode(msg.aux_25);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_25_aux_25_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_25_aux_25_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_25_aux_25_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_24_check_ranges(struct vehicle_rt_dl1_mk3_aux_24_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_24_aux_24_is_in_range(msg->aux_24))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_24_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_24)
{
    struct vehicle_rt_dl1_mk3_aux_24_t msg;

    msg.aux_24 = vehicle_rt_dl1_mk3_aux_24_aux_24_encode(aux_24);

    int ret = vehicle_rt_dl1_mk3_aux_24_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_24_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_24_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_24)
{
    struct vehicle_rt_dl1_mk3_aux_24_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_24_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_24_check_ranges(&msg);

    if (aux_24)
        *aux_24 = vehicle_rt_dl1_mk3_aux_24_aux_24_decode(msg.aux_24);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_24_aux_24_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_24_aux_24_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_24_aux_24_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_23_check_ranges(struct vehicle_rt_dl1_mk3_aux_23_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_23_aux_23_is_in_range(msg->aux_23))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_23_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_23)
{
    struct vehicle_rt_dl1_mk3_aux_23_t msg;

    msg.aux_23 = vehicle_rt_dl1_mk3_aux_23_aux_23_encode(aux_23);

    int ret = vehicle_rt_dl1_mk3_aux_23_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_23_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_23_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_23)
{
    struct vehicle_rt_dl1_mk3_aux_23_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_23_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_23_check_ranges(&msg);

    if (aux_23)
        *aux_23 = vehicle_rt_dl1_mk3_aux_23_aux_23_decode(msg.aux_23);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_23_aux_23_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_23_aux_23_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_23_aux_23_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_22_check_ranges(struct vehicle_rt_dl1_mk3_aux_22_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_22_aux_22_is_in_range(msg->aux_22))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_22_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_22)
{
    struct vehicle_rt_dl1_mk3_aux_22_t msg;

    msg.aux_22 = vehicle_rt_dl1_mk3_aux_22_aux_22_encode(aux_22);

    int ret = vehicle_rt_dl1_mk3_aux_22_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_22_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_22_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_22)
{
    struct vehicle_rt_dl1_mk3_aux_22_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_22_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_22_check_ranges(&msg);

    if (aux_22)
        *aux_22 = vehicle_rt_dl1_mk3_aux_22_aux_22_decode(msg.aux_22);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_22_aux_22_encode(double value)
{
    return (uint16_t)(value / 10.0);
}

double vehicle_rt_dl1_mk3_aux_22_aux_22_decode(uint16_t value)
{
    return ((double)value * 10.0);
}

double vehicle_rt_dl1_mk3_aux_22_aux_22_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_21_check_ranges(struct vehicle_rt_dl1_mk3_aux_21_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_21_aux_21_is_in_range(msg->aux_21))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_21_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_21)
{
    struct vehicle_rt_dl1_mk3_aux_21_t msg;

    msg.aux_21 = vehicle_rt_dl1_mk3_aux_21_aux_21_encode(aux_21);

    int ret = vehicle_rt_dl1_mk3_aux_21_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_21_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_21_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_21)
{
    struct vehicle_rt_dl1_mk3_aux_21_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_21_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_21_check_ranges(&msg);

    if (aux_21)
        *aux_21 = vehicle_rt_dl1_mk3_aux_21_aux_21_decode(msg.aux_21);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_21_aux_21_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_21_aux_21_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_21_aux_21_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_20_check_ranges(struct vehicle_rt_dl1_mk3_aux_20_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_20_aux_20_is_in_range(msg->aux_20))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_20_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_20)
{
    struct vehicle_rt_dl1_mk3_aux_20_t msg;

    msg.aux_20 = vehicle_rt_dl1_mk3_aux_20_aux_20_encode(aux_20);

    int ret = vehicle_rt_dl1_mk3_aux_20_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_20_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_20_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_20)
{
    struct vehicle_rt_dl1_mk3_aux_20_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_20_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_20_check_ranges(&msg);

    if (aux_20)
        *aux_20 = vehicle_rt_dl1_mk3_aux_20_aux_20_decode(msg.aux_20);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_20_aux_20_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_20_aux_20_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_20_aux_20_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_19_check_ranges(struct vehicle_rt_dl1_mk3_aux_19_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_19_aux_19_is_in_range(msg->aux_19))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_19_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_19)
{
    struct vehicle_rt_dl1_mk3_aux_19_t msg;

    msg.aux_19 = vehicle_rt_dl1_mk3_aux_19_aux_19_encode(aux_19);

    int ret = vehicle_rt_dl1_mk3_aux_19_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_19_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_19_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_19)
{
    struct vehicle_rt_dl1_mk3_aux_19_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_19_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_19_check_ranges(&msg);

    if (aux_19)
        *aux_19 = vehicle_rt_dl1_mk3_aux_19_aux_19_decode(msg.aux_19);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_19_aux_19_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_19_aux_19_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_19_aux_19_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_18_check_ranges(struct vehicle_rt_dl1_mk3_aux_18_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_18_aux_18_is_in_range(msg->aux_18))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_18_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_18)
{
    struct vehicle_rt_dl1_mk3_aux_18_t msg;

    msg.aux_18 = vehicle_rt_dl1_mk3_aux_18_aux_18_encode(aux_18);

    int ret = vehicle_rt_dl1_mk3_aux_18_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_18_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_18_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_18)
{
    struct vehicle_rt_dl1_mk3_aux_18_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_18_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_18_check_ranges(&msg);

    if (aux_18)
        *aux_18 = vehicle_rt_dl1_mk3_aux_18_aux_18_decode(msg.aux_18);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_18_aux_18_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_18_aux_18_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_18_aux_18_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_17_check_ranges(struct vehicle_rt_dl1_mk3_aux_17_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_17_aux_17_is_in_range(msg->aux_17))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_17_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_17)
{
    struct vehicle_rt_dl1_mk3_aux_17_t msg;

    msg.aux_17 = vehicle_rt_dl1_mk3_aux_17_aux_17_encode(aux_17);

    int ret = vehicle_rt_dl1_mk3_aux_17_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_17_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_17_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_17)
{
    struct vehicle_rt_dl1_mk3_aux_17_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_17_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_17_check_ranges(&msg);

    if (aux_17)
        *aux_17 = vehicle_rt_dl1_mk3_aux_17_aux_17_decode(msg.aux_17);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_17_aux_17_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_17_aux_17_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_17_aux_17_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_16_check_ranges(struct vehicle_rt_dl1_mk3_aux_16_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_16_aux_16_is_in_range(msg->aux_16))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_16_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_16)
{
    struct vehicle_rt_dl1_mk3_aux_16_t msg;

    msg.aux_16 = vehicle_rt_dl1_mk3_aux_16_aux_16_encode(aux_16);

    int ret = vehicle_rt_dl1_mk3_aux_16_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_16_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_16_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_16)
{
    struct vehicle_rt_dl1_mk3_aux_16_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_16_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_16_check_ranges(&msg);

    if (aux_16)
        *aux_16 = vehicle_rt_dl1_mk3_aux_16_aux_16_decode(msg.aux_16);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_16_aux_16_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_16_aux_16_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_16_aux_16_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_15_check_ranges(struct vehicle_rt_dl1_mk3_aux_15_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_15_aux_15_is_in_range(msg->aux_15))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_15_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_15)
{
    struct vehicle_rt_dl1_mk3_aux_15_t msg;

    msg.aux_15 = vehicle_rt_dl1_mk3_aux_15_aux_15_encode(aux_15);

    int ret = vehicle_rt_dl1_mk3_aux_15_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_15_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_15_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_15)
{
    struct vehicle_rt_dl1_mk3_aux_15_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_15_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_15_check_ranges(&msg);

    if (aux_15)
        *aux_15 = vehicle_rt_dl1_mk3_aux_15_aux_15_decode(msg.aux_15);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_15_aux_15_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_15_aux_15_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_15_aux_15_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_14_check_ranges(struct vehicle_rt_dl1_mk3_aux_14_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_14_aux_14_is_in_range(msg->aux_14))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_14_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_14)
{
    struct vehicle_rt_dl1_mk3_aux_14_t msg;

    msg.aux_14 = vehicle_rt_dl1_mk3_aux_14_aux_14_encode(aux_14);

    int ret = vehicle_rt_dl1_mk3_aux_14_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_14_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_14_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_14)
{
    struct vehicle_rt_dl1_mk3_aux_14_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_14_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_14_check_ranges(&msg);

    if (aux_14)
        *aux_14 = vehicle_rt_dl1_mk3_aux_14_aux_14_decode(msg.aux_14);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_14_aux_14_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_14_aux_14_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_14_aux_14_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_13_check_ranges(struct vehicle_rt_dl1_mk3_aux_13_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_13_aux_13_is_in_range(msg->aux_13))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_13_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_13)
{
    struct vehicle_rt_dl1_mk3_aux_13_t msg;

    msg.aux_13 = vehicle_rt_dl1_mk3_aux_13_aux_13_encode(aux_13);

    int ret = vehicle_rt_dl1_mk3_aux_13_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_13_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_13_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_13)
{
    struct vehicle_rt_dl1_mk3_aux_13_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_13_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_13_check_ranges(&msg);

    if (aux_13)
        *aux_13 = vehicle_rt_dl1_mk3_aux_13_aux_13_decode(msg.aux_13);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_13_aux_13_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_13_aux_13_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_13_aux_13_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_12_check_ranges(struct vehicle_rt_dl1_mk3_aux_12_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_12_aux_12_is_in_range(msg->aux_12))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_12_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_12)
{
    struct vehicle_rt_dl1_mk3_aux_12_t msg;

    msg.aux_12 = vehicle_rt_dl1_mk3_aux_12_aux_12_encode(aux_12);

    int ret = vehicle_rt_dl1_mk3_aux_12_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_12_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_12_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_12)
{
    struct vehicle_rt_dl1_mk3_aux_12_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_12_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_12_check_ranges(&msg);

    if (aux_12)
        *aux_12 = vehicle_rt_dl1_mk3_aux_12_aux_12_decode(msg.aux_12);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_12_aux_12_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_12_aux_12_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_12_aux_12_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_11_check_ranges(struct vehicle_rt_dl1_mk3_aux_11_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_11_aux_11_is_in_range(msg->aux_11))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_11_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_11)
{
    struct vehicle_rt_dl1_mk3_aux_11_t msg;

    msg.aux_11 = vehicle_rt_dl1_mk3_aux_11_aux_11_encode(aux_11);

    int ret = vehicle_rt_dl1_mk3_aux_11_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_11_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_11_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_11)
{
    struct vehicle_rt_dl1_mk3_aux_11_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_11_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_11_check_ranges(&msg);

    if (aux_11)
        *aux_11 = vehicle_rt_dl1_mk3_aux_11_aux_11_decode(msg.aux_11);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_11_aux_11_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_11_aux_11_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_11_aux_11_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_9_check_ranges(struct vehicle_rt_dl1_mk3_aux_9_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_9_aux_9_is_in_range(msg->aux_9))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_9_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_9)
{
    struct vehicle_rt_dl1_mk3_aux_9_t msg;

    msg.aux_9 = vehicle_rt_dl1_mk3_aux_9_aux_9_encode(aux_9);

    int ret = vehicle_rt_dl1_mk3_aux_9_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_9_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_9_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_9)
{
    struct vehicle_rt_dl1_mk3_aux_9_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_9_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_9_check_ranges(&msg);

    if (aux_9)
        *aux_9 = vehicle_rt_dl1_mk3_aux_9_aux_9_decode(msg.aux_9);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_9_aux_9_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_9_aux_9_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_9_aux_9_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_10_check_ranges(struct vehicle_rt_dl1_mk3_aux_10_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_10_aux_10_is_in_range(msg->aux_10))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_10_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_10)
{
    struct vehicle_rt_dl1_mk3_aux_10_t msg;

    msg.aux_10 = vehicle_rt_dl1_mk3_aux_10_aux_10_encode(aux_10);

    int ret = vehicle_rt_dl1_mk3_aux_10_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_10_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_10_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_10)
{
    struct vehicle_rt_dl1_mk3_aux_10_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_10_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_10_check_ranges(&msg);

    if (aux_10)
        *aux_10 = vehicle_rt_dl1_mk3_aux_10_aux_10_decode(msg.aux_10);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_10_aux_10_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_10_aux_10_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_10_aux_10_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_8_check_ranges(struct vehicle_rt_dl1_mk3_aux_8_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_8_aux_8_is_in_range(msg->aux_8))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_8_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_8)
{
    struct vehicle_rt_dl1_mk3_aux_8_t msg;

    msg.aux_8 = vehicle_rt_dl1_mk3_aux_8_aux_8_encode(aux_8);

    int ret = vehicle_rt_dl1_mk3_aux_8_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_8_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_8_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_8)
{
    struct vehicle_rt_dl1_mk3_aux_8_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_8_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_8_check_ranges(&msg);

    if (aux_8)
        *aux_8 = vehicle_rt_dl1_mk3_aux_8_aux_8_decode(msg.aux_8);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_8_aux_8_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_8_aux_8_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_8_aux_8_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_7_check_ranges(struct vehicle_rt_dl1_mk3_aux_7_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_7_aux_7_is_in_range(msg->aux_7))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_7_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_7)
{
    struct vehicle_rt_dl1_mk3_aux_7_t msg;

    msg.aux_7 = vehicle_rt_dl1_mk3_aux_7_aux_7_encode(aux_7);

    int ret = vehicle_rt_dl1_mk3_aux_7_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_7_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_7_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_7)
{
    struct vehicle_rt_dl1_mk3_aux_7_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_7_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_7_check_ranges(&msg);

    if (aux_7)
        *aux_7 = vehicle_rt_dl1_mk3_aux_7_aux_7_decode(msg.aux_7);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_7_aux_7_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_7_aux_7_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_7_aux_7_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_6_check_ranges(struct vehicle_rt_dl1_mk3_aux_6_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_6_aux_6_is_in_range(msg->aux_6))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_6_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_6)
{
    struct vehicle_rt_dl1_mk3_aux_6_t msg;

    msg.aux_6 = vehicle_rt_dl1_mk3_aux_6_aux_6_encode(aux_6);

    int ret = vehicle_rt_dl1_mk3_aux_6_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_6_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_6_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_6)
{
    struct vehicle_rt_dl1_mk3_aux_6_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_6_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_6_check_ranges(&msg);

    if (aux_6)
        *aux_6 = vehicle_rt_dl1_mk3_aux_6_aux_6_decode(msg.aux_6);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_6_aux_6_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_6_aux_6_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_6_aux_6_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_5_check_ranges(struct vehicle_rt_dl1_mk3_aux_5_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_5_aux_5_is_in_range(msg->aux_5))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_5_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_5)
{
    struct vehicle_rt_dl1_mk3_aux_5_t msg;

    msg.aux_5 = vehicle_rt_dl1_mk3_aux_5_aux_5_encode(aux_5);

    int ret = vehicle_rt_dl1_mk3_aux_5_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_5_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_5_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_5)
{
    struct vehicle_rt_dl1_mk3_aux_5_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_5_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_5_check_ranges(&msg);

    if (aux_5)
        *aux_5 = vehicle_rt_dl1_mk3_aux_5_aux_5_decode(msg.aux_5);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_5_aux_5_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_5_aux_5_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_5_aux_5_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_4_check_ranges(struct vehicle_rt_dl1_mk3_aux_4_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_4_aux_4_is_in_range(msg->aux_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_4)
{
    struct vehicle_rt_dl1_mk3_aux_4_t msg;

    msg.aux_4 = vehicle_rt_dl1_mk3_aux_4_aux_4_encode(aux_4);

    int ret = vehicle_rt_dl1_mk3_aux_4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_4_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_4)
{
    struct vehicle_rt_dl1_mk3_aux_4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_4_check_ranges(&msg);

    if (aux_4)
        *aux_4 = vehicle_rt_dl1_mk3_aux_4_aux_4_decode(msg.aux_4);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_4_aux_4_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_4_aux_4_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_4_aux_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_3_check_ranges(struct vehicle_rt_dl1_mk3_aux_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_3_aux_3_is_in_range(msg->aux_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_3)
{
    struct vehicle_rt_dl1_mk3_aux_3_t msg;

    msg.aux_3 = vehicle_rt_dl1_mk3_aux_3_aux_3_encode(aux_3);

    int ret = vehicle_rt_dl1_mk3_aux_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_3_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_3)
{
    struct vehicle_rt_dl1_mk3_aux_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_3_check_ranges(&msg);

    if (aux_3)
        *aux_3 = vehicle_rt_dl1_mk3_aux_3_aux_3_decode(msg.aux_3);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_3_aux_3_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_3_aux_3_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_3_aux_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_2_check_ranges(struct vehicle_rt_dl1_mk3_aux_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_2_aux_2_is_in_range(msg->aux_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_2)
{
    struct vehicle_rt_dl1_mk3_aux_2_t msg;

    msg.aux_2 = vehicle_rt_dl1_mk3_aux_2_aux_2_encode(aux_2);

    int ret = vehicle_rt_dl1_mk3_aux_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_2_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_2)
{
    struct vehicle_rt_dl1_mk3_aux_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_2_check_ranges(&msg);

    if (aux_2)
        *aux_2 = vehicle_rt_dl1_mk3_aux_2_aux_2_decode(msg.aux_2);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_2_aux_2_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_2_aux_2_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_2_aux_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_aux_1_check_ranges(struct vehicle_rt_dl1_mk3_aux_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_aux_1_aux_1_is_in_range(msg->aux_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_aux_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double aux_1)
{
    struct vehicle_rt_dl1_mk3_aux_1_t msg;

    msg.aux_1 = vehicle_rt_dl1_mk3_aux_1_aux_1_encode(aux_1);

    int ret = vehicle_rt_dl1_mk3_aux_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_aux_1_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_aux_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *aux_1)
{
    struct vehicle_rt_dl1_mk3_aux_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_aux_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_aux_1_check_ranges(&msg);

    if (aux_1)
        *aux_1 = vehicle_rt_dl1_mk3_aux_1_aux_1_decode(msg.aux_1);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_aux_1_aux_1_encode(double value)
{
    return (uint16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_aux_1_aux_1_decode(uint16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_aux_1_aux_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_pressure_5_check_ranges(struct vehicle_rt_dl1_mk3_pressure_5_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_pressure_5_pressure_5_is_in_range(msg->pressure_5))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_5_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double pressure_5)
{
    struct vehicle_rt_dl1_mk3_pressure_5_t msg;

    msg.pressure_5 = vehicle_rt_dl1_mk3_pressure_5_pressure_5_encode(pressure_5);

    int ret = vehicle_rt_dl1_mk3_pressure_5_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_pressure_5_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_5_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *pressure_5)
{
    struct vehicle_rt_dl1_mk3_pressure_5_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_pressure_5_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_pressure_5_check_ranges(&msg);

    if (pressure_5)
        *pressure_5 = vehicle_rt_dl1_mk3_pressure_5_pressure_5_decode(msg.pressure_5);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_pressure_5_pressure_5_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_5_pressure_5_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_pressure_5_pressure_5_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_pressure_4_check_ranges(struct vehicle_rt_dl1_mk3_pressure_4_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_pressure_4_pressure_4_is_in_range(msg->pressure_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double pressure_4)
{
    struct vehicle_rt_dl1_mk3_pressure_4_t msg;

    msg.pressure_4 = vehicle_rt_dl1_mk3_pressure_4_pressure_4_encode(pressure_4);

    int ret = vehicle_rt_dl1_mk3_pressure_4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_pressure_4_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *pressure_4)
{
    struct vehicle_rt_dl1_mk3_pressure_4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_pressure_4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_pressure_4_check_ranges(&msg);

    if (pressure_4)
        *pressure_4 = vehicle_rt_dl1_mk3_pressure_4_pressure_4_decode(msg.pressure_4);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_pressure_4_pressure_4_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_4_pressure_4_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_pressure_4_pressure_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_pressure_3_check_ranges(struct vehicle_rt_dl1_mk3_pressure_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_pressure_3_pressure_3_is_in_range(msg->pressure_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double pressure_3)
{
    struct vehicle_rt_dl1_mk3_pressure_3_t msg;

    msg.pressure_3 = vehicle_rt_dl1_mk3_pressure_3_pressure_3_encode(pressure_3);

    int ret = vehicle_rt_dl1_mk3_pressure_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_pressure_3_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *pressure_3)
{
    struct vehicle_rt_dl1_mk3_pressure_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_pressure_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_pressure_3_check_ranges(&msg);

    if (pressure_3)
        *pressure_3 = vehicle_rt_dl1_mk3_pressure_3_pressure_3_decode(msg.pressure_3);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_pressure_3_pressure_3_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_3_pressure_3_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_pressure_3_pressure_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_pressure_2_check_ranges(struct vehicle_rt_dl1_mk3_pressure_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_pressure_2_pressure_2_is_in_range(msg->pressure_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double pressure_2)
{
    struct vehicle_rt_dl1_mk3_pressure_2_t msg;

    msg.pressure_2 = vehicle_rt_dl1_mk3_pressure_2_pressure_2_encode(pressure_2);

    int ret = vehicle_rt_dl1_mk3_pressure_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_pressure_2_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *pressure_2)
{
    struct vehicle_rt_dl1_mk3_pressure_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_pressure_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_pressure_2_check_ranges(&msg);

    if (pressure_2)
        *pressure_2 = vehicle_rt_dl1_mk3_pressure_2_pressure_2_decode(msg.pressure_2);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_pressure_2_pressure_2_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_2_pressure_2_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_pressure_2_pressure_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_pressure_1_check_ranges(struct vehicle_rt_dl1_mk3_pressure_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_pressure_1_pressure_1_is_in_range(msg->pressure_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double pressure_1)
{
    struct vehicle_rt_dl1_mk3_pressure_1_t msg;

    msg.pressure_1 = vehicle_rt_dl1_mk3_pressure_1_pressure_1_encode(pressure_1);

    int ret = vehicle_rt_dl1_mk3_pressure_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_pressure_1_pack(outbuf, &msg, outbuf_sz);
    if (3 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_pressure_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *pressure_1)
{
    struct vehicle_rt_dl1_mk3_pressure_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_pressure_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_pressure_1_check_ranges(&msg);

    if (pressure_1)
        *pressure_1 = vehicle_rt_dl1_mk3_pressure_1_pressure_1_decode(msg.pressure_1);

    return ret;
}

uint32_t vehicle_rt_dl1_mk3_pressure_1_pressure_1_encode(double value)
{
    return (uint32_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_pressure_1_pressure_1_decode(uint32_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_pressure_1_pressure_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_angle_3_check_ranges(struct vehicle_rt_dl1_mk3_angle_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_angle_3_angle_3_is_in_range(msg->angle_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_angle_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double angle_3)
{
    struct vehicle_rt_dl1_mk3_angle_3_t msg;

    msg.angle_3 = vehicle_rt_dl1_mk3_angle_3_angle_3_encode(angle_3);

    int ret = vehicle_rt_dl1_mk3_angle_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_angle_3_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_angle_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *angle_3)
{
    struct vehicle_rt_dl1_mk3_angle_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_angle_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_angle_3_check_ranges(&msg);

    if (angle_3)
        *angle_3 = vehicle_rt_dl1_mk3_angle_3_angle_3_decode(msg.angle_3);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_angle_3_angle_3_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_angle_3_angle_3_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_angle_3_angle_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_angle_2_check_ranges(struct vehicle_rt_dl1_mk3_angle_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_angle_2_angle_2_is_in_range(msg->angle_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_angle_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double angle_2)
{
    struct vehicle_rt_dl1_mk3_angle_2_t msg;

    msg.angle_2 = vehicle_rt_dl1_mk3_angle_2_angle_2_encode(angle_2);

    int ret = vehicle_rt_dl1_mk3_angle_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_angle_2_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_angle_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *angle_2)
{
    struct vehicle_rt_dl1_mk3_angle_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_angle_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_angle_2_check_ranges(&msg);

    if (angle_2)
        *angle_2 = vehicle_rt_dl1_mk3_angle_2_angle_2_decode(msg.angle_2);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_angle_2_angle_2_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_angle_2_angle_2_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_angle_2_angle_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_angle_1_check_ranges(struct vehicle_rt_dl1_mk3_angle_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_angle_1_angle_1_is_in_range(msg->angle_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_angle_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double angle_1)
{
    struct vehicle_rt_dl1_mk3_angle_1_t msg;

    msg.angle_1 = vehicle_rt_dl1_mk3_angle_1_angle_1_encode(angle_1);

    int ret = vehicle_rt_dl1_mk3_angle_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_angle_1_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_angle_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *angle_1)
{
    struct vehicle_rt_dl1_mk3_angle_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_angle_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_angle_1_check_ranges(&msg);

    if (angle_1)
        *angle_1 = vehicle_rt_dl1_mk3_angle_1_angle_1_decode(msg.angle_1);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_angle_1_angle_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_angle_1_angle_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_angle_1_angle_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_25_check_ranges(struct vehicle_rt_dl1_mk3_temp_25_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_25_temperature_25_is_in_range(msg->temperature_25))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_25_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_25)
{
    struct vehicle_rt_dl1_mk3_temp_25_t msg;

    msg.temperature_25 = vehicle_rt_dl1_mk3_temp_25_temperature_25_encode(temperature_25);

    int ret = vehicle_rt_dl1_mk3_temp_25_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_25_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_25_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_25)
{
    struct vehicle_rt_dl1_mk3_temp_25_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_25_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_25_check_ranges(&msg);

    if (temperature_25)
        *temperature_25 = vehicle_rt_dl1_mk3_temp_25_temperature_25_decode(msg.temperature_25);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_25_temperature_25_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_25_temperature_25_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_25_temperature_25_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_24_check_ranges(struct vehicle_rt_dl1_mk3_temp_24_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_24_temperature_24_is_in_range(msg->temperature_24))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_24_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_24)
{
    struct vehicle_rt_dl1_mk3_temp_24_t msg;

    msg.temperature_24 = vehicle_rt_dl1_mk3_temp_24_temperature_24_encode(temperature_24);

    int ret = vehicle_rt_dl1_mk3_temp_24_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_24_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_24_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_24)
{
    struct vehicle_rt_dl1_mk3_temp_24_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_24_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_24_check_ranges(&msg);

    if (temperature_24)
        *temperature_24 = vehicle_rt_dl1_mk3_temp_24_temperature_24_decode(msg.temperature_24);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_24_temperature_24_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_24_temperature_24_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_24_temperature_24_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_23_check_ranges(struct vehicle_rt_dl1_mk3_temp_23_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_23_temperature_23_is_in_range(msg->temperature_23))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_23_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_23)
{
    struct vehicle_rt_dl1_mk3_temp_23_t msg;

    msg.temperature_23 = vehicle_rt_dl1_mk3_temp_23_temperature_23_encode(temperature_23);

    int ret = vehicle_rt_dl1_mk3_temp_23_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_23_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_23_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_23)
{
    struct vehicle_rt_dl1_mk3_temp_23_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_23_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_23_check_ranges(&msg);

    if (temperature_23)
        *temperature_23 = vehicle_rt_dl1_mk3_temp_23_temperature_23_decode(msg.temperature_23);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_23_temperature_23_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_23_temperature_23_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_23_temperature_23_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_22_check_ranges(struct vehicle_rt_dl1_mk3_temp_22_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_22_temperature_22_is_in_range(msg->temperature_22))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_22_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_22)
{
    struct vehicle_rt_dl1_mk3_temp_22_t msg;

    msg.temperature_22 = vehicle_rt_dl1_mk3_temp_22_temperature_22_encode(temperature_22);

    int ret = vehicle_rt_dl1_mk3_temp_22_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_22_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_22_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_22)
{
    struct vehicle_rt_dl1_mk3_temp_22_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_22_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_22_check_ranges(&msg);

    if (temperature_22)
        *temperature_22 = vehicle_rt_dl1_mk3_temp_22_temperature_22_decode(msg.temperature_22);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_22_temperature_22_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_22_temperature_22_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_22_temperature_22_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_21_check_ranges(struct vehicle_rt_dl1_mk3_temp_21_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_21_temperature_21_is_in_range(msg->temperature_21))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_21_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_21)
{
    struct vehicle_rt_dl1_mk3_temp_21_t msg;

    msg.temperature_21 = vehicle_rt_dl1_mk3_temp_21_temperature_21_encode(temperature_21);

    int ret = vehicle_rt_dl1_mk3_temp_21_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_21_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_21_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_21)
{
    struct vehicle_rt_dl1_mk3_temp_21_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_21_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_21_check_ranges(&msg);

    if (temperature_21)
        *temperature_21 = vehicle_rt_dl1_mk3_temp_21_temperature_21_decode(msg.temperature_21);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_21_temperature_21_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_21_temperature_21_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_21_temperature_21_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_20_check_ranges(struct vehicle_rt_dl1_mk3_temp_20_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_20_temperature_20_is_in_range(msg->temperature_20))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_20_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_20)
{
    struct vehicle_rt_dl1_mk3_temp_20_t msg;

    msg.temperature_20 = vehicle_rt_dl1_mk3_temp_20_temperature_20_encode(temperature_20);

    int ret = vehicle_rt_dl1_mk3_temp_20_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_20_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_20_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_20)
{
    struct vehicle_rt_dl1_mk3_temp_20_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_20_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_20_check_ranges(&msg);

    if (temperature_20)
        *temperature_20 = vehicle_rt_dl1_mk3_temp_20_temperature_20_decode(msg.temperature_20);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_20_temperature_20_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_20_temperature_20_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_20_temperature_20_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_19_check_ranges(struct vehicle_rt_dl1_mk3_temp_19_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_19_temperature_19_is_in_range(msg->temperature_19))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_19_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_19)
{
    struct vehicle_rt_dl1_mk3_temp_19_t msg;

    msg.temperature_19 = vehicle_rt_dl1_mk3_temp_19_temperature_19_encode(temperature_19);

    int ret = vehicle_rt_dl1_mk3_temp_19_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_19_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_19_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_19)
{
    struct vehicle_rt_dl1_mk3_temp_19_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_19_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_19_check_ranges(&msg);

    if (temperature_19)
        *temperature_19 = vehicle_rt_dl1_mk3_temp_19_temperature_19_decode(msg.temperature_19);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_19_temperature_19_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_19_temperature_19_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_19_temperature_19_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_18_check_ranges(struct vehicle_rt_dl1_mk3_temp_18_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_18_temperature_18_is_in_range(msg->temperature_18))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_18_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_18)
{
    struct vehicle_rt_dl1_mk3_temp_18_t msg;

    msg.temperature_18 = vehicle_rt_dl1_mk3_temp_18_temperature_18_encode(temperature_18);

    int ret = vehicle_rt_dl1_mk3_temp_18_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_18_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_18_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_18)
{
    struct vehicle_rt_dl1_mk3_temp_18_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_18_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_18_check_ranges(&msg);

    if (temperature_18)
        *temperature_18 = vehicle_rt_dl1_mk3_temp_18_temperature_18_decode(msg.temperature_18);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_18_temperature_18_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_18_temperature_18_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_18_temperature_18_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_17_check_ranges(struct vehicle_rt_dl1_mk3_temp_17_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_17_temperature_17_is_in_range(msg->temperature_17))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_17_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_17)
{
    struct vehicle_rt_dl1_mk3_temp_17_t msg;

    msg.temperature_17 = vehicle_rt_dl1_mk3_temp_17_temperature_17_encode(temperature_17);

    int ret = vehicle_rt_dl1_mk3_temp_17_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_17_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_17_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_17)
{
    struct vehicle_rt_dl1_mk3_temp_17_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_17_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_17_check_ranges(&msg);

    if (temperature_17)
        *temperature_17 = vehicle_rt_dl1_mk3_temp_17_temperature_17_decode(msg.temperature_17);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_17_temperature_17_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_17_temperature_17_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_17_temperature_17_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_16_check_ranges(struct vehicle_rt_dl1_mk3_temp_16_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_16_temperature_16_is_in_range(msg->temperature_16))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_16_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_16)
{
    struct vehicle_rt_dl1_mk3_temp_16_t msg;

    msg.temperature_16 = vehicle_rt_dl1_mk3_temp_16_temperature_16_encode(temperature_16);

    int ret = vehicle_rt_dl1_mk3_temp_16_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_16_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_16_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_16)
{
    struct vehicle_rt_dl1_mk3_temp_16_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_16_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_16_check_ranges(&msg);

    if (temperature_16)
        *temperature_16 = vehicle_rt_dl1_mk3_temp_16_temperature_16_decode(msg.temperature_16);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_16_temperature_16_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_16_temperature_16_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_16_temperature_16_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_15_check_ranges(struct vehicle_rt_dl1_mk3_temp_15_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_15_temperature_15_is_in_range(msg->temperature_15))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_15_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_15)
{
    struct vehicle_rt_dl1_mk3_temp_15_t msg;

    msg.temperature_15 = vehicle_rt_dl1_mk3_temp_15_temperature_15_encode(temperature_15);

    int ret = vehicle_rt_dl1_mk3_temp_15_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_15_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_15_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_15)
{
    struct vehicle_rt_dl1_mk3_temp_15_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_15_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_15_check_ranges(&msg);

    if (temperature_15)
        *temperature_15 = vehicle_rt_dl1_mk3_temp_15_temperature_15_decode(msg.temperature_15);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_15_temperature_15_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_15_temperature_15_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_15_temperature_15_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_14_check_ranges(struct vehicle_rt_dl1_mk3_temp_14_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_14_temperature_14_is_in_range(msg->temperature_14))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_14_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_14)
{
    struct vehicle_rt_dl1_mk3_temp_14_t msg;

    msg.temperature_14 = vehicle_rt_dl1_mk3_temp_14_temperature_14_encode(temperature_14);

    int ret = vehicle_rt_dl1_mk3_temp_14_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_14_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_14_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_14)
{
    struct vehicle_rt_dl1_mk3_temp_14_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_14_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_14_check_ranges(&msg);

    if (temperature_14)
        *temperature_14 = vehicle_rt_dl1_mk3_temp_14_temperature_14_decode(msg.temperature_14);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_14_temperature_14_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_14_temperature_14_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_14_temperature_14_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_13_check_ranges(struct vehicle_rt_dl1_mk3_temp_13_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_13_temperature_13_is_in_range(msg->temperature_13))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_13_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_13)
{
    struct vehicle_rt_dl1_mk3_temp_13_t msg;

    msg.temperature_13 = vehicle_rt_dl1_mk3_temp_13_temperature_13_encode(temperature_13);

    int ret = vehicle_rt_dl1_mk3_temp_13_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_13_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_13_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_13)
{
    struct vehicle_rt_dl1_mk3_temp_13_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_13_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_13_check_ranges(&msg);

    if (temperature_13)
        *temperature_13 = vehicle_rt_dl1_mk3_temp_13_temperature_13_decode(msg.temperature_13);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_13_temperature_13_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_13_temperature_13_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_13_temperature_13_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_12_check_ranges(struct vehicle_rt_dl1_mk3_temp_12_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_12_temperature_12_is_in_range(msg->temperature_12))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_12_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_12)
{
    struct vehicle_rt_dl1_mk3_temp_12_t msg;

    msg.temperature_12 = vehicle_rt_dl1_mk3_temp_12_temperature_12_encode(temperature_12);

    int ret = vehicle_rt_dl1_mk3_temp_12_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_12_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_12_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_12)
{
    struct vehicle_rt_dl1_mk3_temp_12_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_12_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_12_check_ranges(&msg);

    if (temperature_12)
        *temperature_12 = vehicle_rt_dl1_mk3_temp_12_temperature_12_decode(msg.temperature_12);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_12_temperature_12_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_12_temperature_12_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_12_temperature_12_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_11_check_ranges(struct vehicle_rt_dl1_mk3_temp_11_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_11_temperature_11_is_in_range(msg->temperature_11))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_11_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_11)
{
    struct vehicle_rt_dl1_mk3_temp_11_t msg;

    msg.temperature_11 = vehicle_rt_dl1_mk3_temp_11_temperature_11_encode(temperature_11);

    int ret = vehicle_rt_dl1_mk3_temp_11_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_11_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_11_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_11)
{
    struct vehicle_rt_dl1_mk3_temp_11_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_11_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_11_check_ranges(&msg);

    if (temperature_11)
        *temperature_11 = vehicle_rt_dl1_mk3_temp_11_temperature_11_decode(msg.temperature_11);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_11_temperature_11_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_11_temperature_11_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_11_temperature_11_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_10_check_ranges(struct vehicle_rt_dl1_mk3_temp_10_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_10_temperature_10_is_in_range(msg->temperature_10))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_10_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_10)
{
    struct vehicle_rt_dl1_mk3_temp_10_t msg;

    msg.temperature_10 = vehicle_rt_dl1_mk3_temp_10_temperature_10_encode(temperature_10);

    int ret = vehicle_rt_dl1_mk3_temp_10_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_10_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_10_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_10)
{
    struct vehicle_rt_dl1_mk3_temp_10_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_10_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_10_check_ranges(&msg);

    if (temperature_10)
        *temperature_10 = vehicle_rt_dl1_mk3_temp_10_temperature_10_decode(msg.temperature_10);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_10_temperature_10_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_10_temperature_10_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_10_temperature_10_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_9_check_ranges(struct vehicle_rt_dl1_mk3_temp_9_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_9_temperature_9_is_in_range(msg->temperature_9))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_9_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_9)
{
    struct vehicle_rt_dl1_mk3_temp_9_t msg;

    msg.temperature_9 = vehicle_rt_dl1_mk3_temp_9_temperature_9_encode(temperature_9);

    int ret = vehicle_rt_dl1_mk3_temp_9_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_9_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_9_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_9)
{
    struct vehicle_rt_dl1_mk3_temp_9_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_9_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_9_check_ranges(&msg);

    if (temperature_9)
        *temperature_9 = vehicle_rt_dl1_mk3_temp_9_temperature_9_decode(msg.temperature_9);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_9_temperature_9_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_9_temperature_9_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_9_temperature_9_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_8_check_ranges(struct vehicle_rt_dl1_mk3_temp_8_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_8_temperature_8_is_in_range(msg->temperature_8))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_8_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_8)
{
    struct vehicle_rt_dl1_mk3_temp_8_t msg;

    msg.temperature_8 = vehicle_rt_dl1_mk3_temp_8_temperature_8_encode(temperature_8);

    int ret = vehicle_rt_dl1_mk3_temp_8_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_8_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_8_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_8)
{
    struct vehicle_rt_dl1_mk3_temp_8_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_8_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_8_check_ranges(&msg);

    if (temperature_8)
        *temperature_8 = vehicle_rt_dl1_mk3_temp_8_temperature_8_decode(msg.temperature_8);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_8_temperature_8_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_8_temperature_8_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_8_temperature_8_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_7_check_ranges(struct vehicle_rt_dl1_mk3_temp_7_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_7_temperature_7_is_in_range(msg->temperature_7))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_7_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_7)
{
    struct vehicle_rt_dl1_mk3_temp_7_t msg;

    msg.temperature_7 = vehicle_rt_dl1_mk3_temp_7_temperature_7_encode(temperature_7);

    int ret = vehicle_rt_dl1_mk3_temp_7_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_7_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_7_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_7)
{
    struct vehicle_rt_dl1_mk3_temp_7_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_7_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_7_check_ranges(&msg);

    if (temperature_7)
        *temperature_7 = vehicle_rt_dl1_mk3_temp_7_temperature_7_decode(msg.temperature_7);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_7_temperature_7_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_7_temperature_7_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_7_temperature_7_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_6_check_ranges(struct vehicle_rt_dl1_mk3_temp_6_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_6_temperature_6_is_in_range(msg->temperature_6))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_6_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_6)
{
    struct vehicle_rt_dl1_mk3_temp_6_t msg;

    msg.temperature_6 = vehicle_rt_dl1_mk3_temp_6_temperature_6_encode(temperature_6);

    int ret = vehicle_rt_dl1_mk3_temp_6_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_6_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_6_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_6)
{
    struct vehicle_rt_dl1_mk3_temp_6_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_6_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_6_check_ranges(&msg);

    if (temperature_6)
        *temperature_6 = vehicle_rt_dl1_mk3_temp_6_temperature_6_decode(msg.temperature_6);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_6_temperature_6_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_6_temperature_6_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_6_temperature_6_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_5_check_ranges(struct vehicle_rt_dl1_mk3_temp_5_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_5_temperature_5_is_in_range(msg->temperature_5))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_5_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_5)
{
    struct vehicle_rt_dl1_mk3_temp_5_t msg;

    msg.temperature_5 = vehicle_rt_dl1_mk3_temp_5_temperature_5_encode(temperature_5);

    int ret = vehicle_rt_dl1_mk3_temp_5_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_5_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_5_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_5)
{
    struct vehicle_rt_dl1_mk3_temp_5_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_5_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_5_check_ranges(&msg);

    if (temperature_5)
        *temperature_5 = vehicle_rt_dl1_mk3_temp_5_temperature_5_decode(msg.temperature_5);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_5_temperature_5_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_5_temperature_5_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_5_temperature_5_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_4_check_ranges(struct vehicle_rt_dl1_mk3_temp_4_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_4_temperature_4_is_in_range(msg->temperature_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_4)
{
    struct vehicle_rt_dl1_mk3_temp_4_t msg;

    msg.temperature_4 = vehicle_rt_dl1_mk3_temp_4_temperature_4_encode(temperature_4);

    int ret = vehicle_rt_dl1_mk3_temp_4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_4_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_4)
{
    struct vehicle_rt_dl1_mk3_temp_4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_4_check_ranges(&msg);

    if (temperature_4)
        *temperature_4 = vehicle_rt_dl1_mk3_temp_4_temperature_4_decode(msg.temperature_4);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_4_temperature_4_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_4_temperature_4_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_4_temperature_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_3_check_ranges(struct vehicle_rt_dl1_mk3_temp_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_3_temperature_3_is_in_range(msg->temperature_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_3)
{
    struct vehicle_rt_dl1_mk3_temp_3_t msg;

    msg.temperature_3 = vehicle_rt_dl1_mk3_temp_3_temperature_3_encode(temperature_3);

    int ret = vehicle_rt_dl1_mk3_temp_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_3_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_3)
{
    struct vehicle_rt_dl1_mk3_temp_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_3_check_ranges(&msg);

    if (temperature_3)
        *temperature_3 = vehicle_rt_dl1_mk3_temp_3_temperature_3_decode(msg.temperature_3);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_3_temperature_3_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_3_temperature_3_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_3_temperature_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_2_check_ranges(struct vehicle_rt_dl1_mk3_temp_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_2_temperature_2_is_in_range(msg->temperature_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_2)
{
    struct vehicle_rt_dl1_mk3_temp_2_t msg;

    msg.temperature_2 = vehicle_rt_dl1_mk3_temp_2_temperature_2_encode(temperature_2);

    int ret = vehicle_rt_dl1_mk3_temp_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_2_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_2)
{
    struct vehicle_rt_dl1_mk3_temp_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_2_check_ranges(&msg);

    if (temperature_2)
        *temperature_2 = vehicle_rt_dl1_mk3_temp_2_temperature_2_decode(msg.temperature_2);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_2_temperature_2_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_2_temperature_2_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_2_temperature_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_temp_1_check_ranges(struct vehicle_rt_dl1_mk3_temp_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_temp_1_temperature_1_is_in_range(msg->temperature_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_temp_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double temperature_1)
{
    struct vehicle_rt_dl1_mk3_temp_1_t msg;

    msg.temperature_1 = vehicle_rt_dl1_mk3_temp_1_temperature_1_encode(temperature_1);

    int ret = vehicle_rt_dl1_mk3_temp_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_temp_1_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_temp_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *temperature_1)
{
    struct vehicle_rt_dl1_mk3_temp_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_temp_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_temp_1_check_ranges(&msg);

    if (temperature_1)
        *temperature_1 = vehicle_rt_dl1_mk3_temp_1_temperature_1_decode(msg.temperature_1);

    return ret;
}

int16_t vehicle_rt_dl1_mk3_temp_1_temperature_1_encode(double value)
{
    return (int16_t)(value / 0.1);
}

double vehicle_rt_dl1_mk3_temp_1_temperature_1_decode(int16_t value)
{
    return ((double)value * 0.1);
}

double vehicle_rt_dl1_mk3_temp_1_temperature_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_32_check_ranges(struct vehicle_rt_dl1_mk3_analog_32_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_32_analog_32_is_in_range(msg->analog_32))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_32_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_32)
{
    struct vehicle_rt_dl1_mk3_analog_32_t msg;

    msg.analog_32 = vehicle_rt_dl1_mk3_analog_32_analog_32_encode(analog_32);

    int ret = vehicle_rt_dl1_mk3_analog_32_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_32_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_32_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_32)
{
    struct vehicle_rt_dl1_mk3_analog_32_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_32_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_32_check_ranges(&msg);

    if (analog_32)
        *analog_32 = vehicle_rt_dl1_mk3_analog_32_analog_32_decode(msg.analog_32);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_32_analog_32_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_32_analog_32_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_32_analog_32_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_31_check_ranges(struct vehicle_rt_dl1_mk3_analog_31_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_31_analog_31_is_in_range(msg->analog_31))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_31_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_31)
{
    struct vehicle_rt_dl1_mk3_analog_31_t msg;

    msg.analog_31 = vehicle_rt_dl1_mk3_analog_31_analog_31_encode(analog_31);

    int ret = vehicle_rt_dl1_mk3_analog_31_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_31_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_31_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_31)
{
    struct vehicle_rt_dl1_mk3_analog_31_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_31_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_31_check_ranges(&msg);

    if (analog_31)
        *analog_31 = vehicle_rt_dl1_mk3_analog_31_analog_31_decode(msg.analog_31);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_31_analog_31_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_31_analog_31_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_31_analog_31_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_30_check_ranges(struct vehicle_rt_dl1_mk3_analog_30_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_30_analog_30_is_in_range(msg->analog_30))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_30_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_30)
{
    struct vehicle_rt_dl1_mk3_analog_30_t msg;

    msg.analog_30 = vehicle_rt_dl1_mk3_analog_30_analog_30_encode(analog_30);

    int ret = vehicle_rt_dl1_mk3_analog_30_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_30_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_30_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_30)
{
    struct vehicle_rt_dl1_mk3_analog_30_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_30_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_30_check_ranges(&msg);

    if (analog_30)
        *analog_30 = vehicle_rt_dl1_mk3_analog_30_analog_30_decode(msg.analog_30);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_30_analog_30_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_30_analog_30_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_30_analog_30_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_29_check_ranges(struct vehicle_rt_dl1_mk3_analog_29_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_29_analog_29_is_in_range(msg->analog_29))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_29_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_29)
{
    struct vehicle_rt_dl1_mk3_analog_29_t msg;

    msg.analog_29 = vehicle_rt_dl1_mk3_analog_29_analog_29_encode(analog_29);

    int ret = vehicle_rt_dl1_mk3_analog_29_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_29_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_29_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_29)
{
    struct vehicle_rt_dl1_mk3_analog_29_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_29_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_29_check_ranges(&msg);

    if (analog_29)
        *analog_29 = vehicle_rt_dl1_mk3_analog_29_analog_29_decode(msg.analog_29);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_29_analog_29_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_29_analog_29_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_29_analog_29_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_28_check_ranges(struct vehicle_rt_dl1_mk3_analog_28_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_28_analog_28_is_in_range(msg->analog_28))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_28_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_28)
{
    struct vehicle_rt_dl1_mk3_analog_28_t msg;

    msg.analog_28 = vehicle_rt_dl1_mk3_analog_28_analog_28_encode(analog_28);

    int ret = vehicle_rt_dl1_mk3_analog_28_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_28_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_28_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_28)
{
    struct vehicle_rt_dl1_mk3_analog_28_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_28_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_28_check_ranges(&msg);

    if (analog_28)
        *analog_28 = vehicle_rt_dl1_mk3_analog_28_analog_28_decode(msg.analog_28);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_28_analog_28_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_28_analog_28_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_28_analog_28_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_27_check_ranges(struct vehicle_rt_dl1_mk3_analog_27_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_27_analog_27_is_in_range(msg->analog_27))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_27_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_27)
{
    struct vehicle_rt_dl1_mk3_analog_27_t msg;

    msg.analog_27 = vehicle_rt_dl1_mk3_analog_27_analog_27_encode(analog_27);

    int ret = vehicle_rt_dl1_mk3_analog_27_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_27_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_27_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_27)
{
    struct vehicle_rt_dl1_mk3_analog_27_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_27_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_27_check_ranges(&msg);

    if (analog_27)
        *analog_27 = vehicle_rt_dl1_mk3_analog_27_analog_27_decode(msg.analog_27);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_27_analog_27_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_27_analog_27_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_27_analog_27_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_26_check_ranges(struct vehicle_rt_dl1_mk3_analog_26_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_26_analog_26_is_in_range(msg->analog_26))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_26_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_26)
{
    struct vehicle_rt_dl1_mk3_analog_26_t msg;

    msg.analog_26 = vehicle_rt_dl1_mk3_analog_26_analog_26_encode(analog_26);

    int ret = vehicle_rt_dl1_mk3_analog_26_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_26_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_26_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_26)
{
    struct vehicle_rt_dl1_mk3_analog_26_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_26_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_26_check_ranges(&msg);

    if (analog_26)
        *analog_26 = vehicle_rt_dl1_mk3_analog_26_analog_26_decode(msg.analog_26);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_26_analog_26_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_26_analog_26_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_26_analog_26_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_25_check_ranges(struct vehicle_rt_dl1_mk3_analog_25_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_25_analog_25_is_in_range(msg->analog_25))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_25_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_25)
{
    struct vehicle_rt_dl1_mk3_analog_25_t msg;

    msg.analog_25 = vehicle_rt_dl1_mk3_analog_25_analog_25_encode(analog_25);

    int ret = vehicle_rt_dl1_mk3_analog_25_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_25_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_25_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_25)
{
    struct vehicle_rt_dl1_mk3_analog_25_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_25_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_25_check_ranges(&msg);

    if (analog_25)
        *analog_25 = vehicle_rt_dl1_mk3_analog_25_analog_25_decode(msg.analog_25);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_25_analog_25_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_25_analog_25_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_25_analog_25_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_15_check_ranges(struct vehicle_rt_dl1_mk3_analog_15_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_15_analog_15_is_in_range(msg->analog_15))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_15_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_15)
{
    struct vehicle_rt_dl1_mk3_analog_15_t msg;

    msg.analog_15 = vehicle_rt_dl1_mk3_analog_15_analog_15_encode(analog_15);

    int ret = vehicle_rt_dl1_mk3_analog_15_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_15_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_15_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_15)
{
    struct vehicle_rt_dl1_mk3_analog_15_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_15_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_15_check_ranges(&msg);

    if (analog_15)
        *analog_15 = vehicle_rt_dl1_mk3_analog_15_analog_15_decode(msg.analog_15);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_15_analog_15_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_15_analog_15_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_15_analog_15_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_14_check_ranges(struct vehicle_rt_dl1_mk3_analog_14_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_14_analog_14_is_in_range(msg->analog_14))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_14_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_14)
{
    struct vehicle_rt_dl1_mk3_analog_14_t msg;

    msg.analog_14 = vehicle_rt_dl1_mk3_analog_14_analog_14_encode(analog_14);

    int ret = vehicle_rt_dl1_mk3_analog_14_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_14_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_14_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_14)
{
    struct vehicle_rt_dl1_mk3_analog_14_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_14_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_14_check_ranges(&msg);

    if (analog_14)
        *analog_14 = vehicle_rt_dl1_mk3_analog_14_analog_14_decode(msg.analog_14);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_14_analog_14_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_14_analog_14_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_14_analog_14_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_17_check_ranges(struct vehicle_rt_dl1_mk3_analog_17_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_17_analog_17_is_in_range(msg->analog_17))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_17_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_17)
{
    struct vehicle_rt_dl1_mk3_analog_17_t msg;

    msg.analog_17 = vehicle_rt_dl1_mk3_analog_17_analog_17_encode(analog_17);

    int ret = vehicle_rt_dl1_mk3_analog_17_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_17_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_17_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_17)
{
    struct vehicle_rt_dl1_mk3_analog_17_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_17_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_17_check_ranges(&msg);

    if (analog_17)
        *analog_17 = vehicle_rt_dl1_mk3_analog_17_analog_17_decode(msg.analog_17);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_17_analog_17_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_17_analog_17_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_17_analog_17_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_24_check_ranges(struct vehicle_rt_dl1_mk3_analog_24_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_24_analog_24_is_in_range(msg->analog_24))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_24_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_24)
{
    struct vehicle_rt_dl1_mk3_analog_24_t msg;

    msg.analog_24 = vehicle_rt_dl1_mk3_analog_24_analog_24_encode(analog_24);

    int ret = vehicle_rt_dl1_mk3_analog_24_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_24_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_24_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_24)
{
    struct vehicle_rt_dl1_mk3_analog_24_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_24_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_24_check_ranges(&msg);

    if (analog_24)
        *analog_24 = vehicle_rt_dl1_mk3_analog_24_analog_24_decode(msg.analog_24);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_24_analog_24_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_24_analog_24_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_24_analog_24_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_23_check_ranges(struct vehicle_rt_dl1_mk3_analog_23_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_23_analog_23_is_in_range(msg->analog_23))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_23_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_23)
{
    struct vehicle_rt_dl1_mk3_analog_23_t msg;

    msg.analog_23 = vehicle_rt_dl1_mk3_analog_23_analog_23_encode(analog_23);

    int ret = vehicle_rt_dl1_mk3_analog_23_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_23_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_23_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_23)
{
    struct vehicle_rt_dl1_mk3_analog_23_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_23_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_23_check_ranges(&msg);

    if (analog_23)
        *analog_23 = vehicle_rt_dl1_mk3_analog_23_analog_23_decode(msg.analog_23);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_23_analog_23_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_23_analog_23_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_23_analog_23_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_22_check_ranges(struct vehicle_rt_dl1_mk3_analog_22_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_22_analog_22_is_in_range(msg->analog_22))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_22_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_22)
{
    struct vehicle_rt_dl1_mk3_analog_22_t msg;

    msg.analog_22 = vehicle_rt_dl1_mk3_analog_22_analog_22_encode(analog_22);

    int ret = vehicle_rt_dl1_mk3_analog_22_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_22_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_22_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_22)
{
    struct vehicle_rt_dl1_mk3_analog_22_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_22_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_22_check_ranges(&msg);

    if (analog_22)
        *analog_22 = vehicle_rt_dl1_mk3_analog_22_analog_22_decode(msg.analog_22);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_22_analog_22_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_22_analog_22_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_22_analog_22_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_21_check_ranges(struct vehicle_rt_dl1_mk3_analog_21_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_21_analog_21_is_in_range(msg->analog_21))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_21_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_21)
{
    struct vehicle_rt_dl1_mk3_analog_21_t msg;

    msg.analog_21 = vehicle_rt_dl1_mk3_analog_21_analog_21_encode(analog_21);

    int ret = vehicle_rt_dl1_mk3_analog_21_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_21_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_21_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_21)
{
    struct vehicle_rt_dl1_mk3_analog_21_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_21_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_21_check_ranges(&msg);

    if (analog_21)
        *analog_21 = vehicle_rt_dl1_mk3_analog_21_analog_21_decode(msg.analog_21);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_21_analog_21_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_21_analog_21_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_21_analog_21_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_20_check_ranges(struct vehicle_rt_dl1_mk3_analog_20_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_20_analog_20_is_in_range(msg->analog_20))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_20_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_20)
{
    struct vehicle_rt_dl1_mk3_analog_20_t msg;

    msg.analog_20 = vehicle_rt_dl1_mk3_analog_20_analog_20_encode(analog_20);

    int ret = vehicle_rt_dl1_mk3_analog_20_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_20_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_20_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_20)
{
    struct vehicle_rt_dl1_mk3_analog_20_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_20_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_20_check_ranges(&msg);

    if (analog_20)
        *analog_20 = vehicle_rt_dl1_mk3_analog_20_analog_20_decode(msg.analog_20);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_20_analog_20_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_20_analog_20_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_20_analog_20_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_19_check_ranges(struct vehicle_rt_dl1_mk3_analog_19_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_19_analog_19_is_in_range(msg->analog_19))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_19_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_19)
{
    struct vehicle_rt_dl1_mk3_analog_19_t msg;

    msg.analog_19 = vehicle_rt_dl1_mk3_analog_19_analog_19_encode(analog_19);

    int ret = vehicle_rt_dl1_mk3_analog_19_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_19_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_19_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_19)
{
    struct vehicle_rt_dl1_mk3_analog_19_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_19_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_19_check_ranges(&msg);

    if (analog_19)
        *analog_19 = vehicle_rt_dl1_mk3_analog_19_analog_19_decode(msg.analog_19);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_19_analog_19_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_19_analog_19_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_19_analog_19_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_16_check_ranges(struct vehicle_rt_dl1_mk3_analog_16_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_16_analog_16_is_in_range(msg->analog_16))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_16_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_16)
{
    struct vehicle_rt_dl1_mk3_analog_16_t msg;

    msg.analog_16 = vehicle_rt_dl1_mk3_analog_16_analog_16_encode(analog_16);

    int ret = vehicle_rt_dl1_mk3_analog_16_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_16_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_16_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_16)
{
    struct vehicle_rt_dl1_mk3_analog_16_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_16_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_16_check_ranges(&msg);

    if (analog_16)
        *analog_16 = vehicle_rt_dl1_mk3_analog_16_analog_16_decode(msg.analog_16);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_16_analog_16_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_16_analog_16_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_16_analog_16_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_18_check_ranges(struct vehicle_rt_dl1_mk3_analog_18_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_18_analog_18_is_in_range(msg->analog_18))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_18_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_18)
{
    struct vehicle_rt_dl1_mk3_analog_18_t msg;

    msg.analog_18 = vehicle_rt_dl1_mk3_analog_18_analog_18_encode(analog_18);

    int ret = vehicle_rt_dl1_mk3_analog_18_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_18_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_18_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_18)
{
    struct vehicle_rt_dl1_mk3_analog_18_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_18_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_18_check_ranges(&msg);

    if (analog_18)
        *analog_18 = vehicle_rt_dl1_mk3_analog_18_analog_18_decode(msg.analog_18);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_18_analog_18_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_18_analog_18_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_18_analog_18_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_12_check_ranges(struct vehicle_rt_dl1_mk3_analog_12_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_12_analog_12_is_in_range(msg->analog_12))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_12_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_12)
{
    struct vehicle_rt_dl1_mk3_analog_12_t msg;

    msg.analog_12 = vehicle_rt_dl1_mk3_analog_12_analog_12_encode(analog_12);

    int ret = vehicle_rt_dl1_mk3_analog_12_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_12_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_12_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_12)
{
    struct vehicle_rt_dl1_mk3_analog_12_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_12_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_12_check_ranges(&msg);

    if (analog_12)
        *analog_12 = vehicle_rt_dl1_mk3_analog_12_analog_12_decode(msg.analog_12);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_12_analog_12_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_12_analog_12_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_12_analog_12_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_11_check_ranges(struct vehicle_rt_dl1_mk3_analog_11_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_11_analog_11_is_in_range(msg->analog_11))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_11_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_11)
{
    struct vehicle_rt_dl1_mk3_analog_11_t msg;

    msg.analog_11 = vehicle_rt_dl1_mk3_analog_11_analog_11_encode(analog_11);

    int ret = vehicle_rt_dl1_mk3_analog_11_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_11_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_11_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_11)
{
    struct vehicle_rt_dl1_mk3_analog_11_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_11_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_11_check_ranges(&msg);

    if (analog_11)
        *analog_11 = vehicle_rt_dl1_mk3_analog_11_analog_11_decode(msg.analog_11);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_11_analog_11_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_11_analog_11_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_11_analog_11_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_10_check_ranges(struct vehicle_rt_dl1_mk3_analog_10_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_10_analog_10_is_in_range(msg->analog_10))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_10_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_10)
{
    struct vehicle_rt_dl1_mk3_analog_10_t msg;

    msg.analog_10 = vehicle_rt_dl1_mk3_analog_10_analog_10_encode(analog_10);

    int ret = vehicle_rt_dl1_mk3_analog_10_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_10_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_10_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_10)
{
    struct vehicle_rt_dl1_mk3_analog_10_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_10_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_10_check_ranges(&msg);

    if (analog_10)
        *analog_10 = vehicle_rt_dl1_mk3_analog_10_analog_10_decode(msg.analog_10);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_10_analog_10_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_10_analog_10_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_10_analog_10_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_9_check_ranges(struct vehicle_rt_dl1_mk3_analog_9_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_9_analog_9_is_in_range(msg->analog_9))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_9_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_9)
{
    struct vehicle_rt_dl1_mk3_analog_9_t msg;

    msg.analog_9 = vehicle_rt_dl1_mk3_analog_9_analog_9_encode(analog_9);

    int ret = vehicle_rt_dl1_mk3_analog_9_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_9_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_9_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_9)
{
    struct vehicle_rt_dl1_mk3_analog_9_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_9_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_9_check_ranges(&msg);

    if (analog_9)
        *analog_9 = vehicle_rt_dl1_mk3_analog_9_analog_9_decode(msg.analog_9);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_9_analog_9_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_9_analog_9_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_9_analog_9_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_8_check_ranges(struct vehicle_rt_dl1_mk3_analog_8_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_8_analog_8_is_in_range(msg->analog_8))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_8_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_8)
{
    struct vehicle_rt_dl1_mk3_analog_8_t msg;

    msg.analog_8 = vehicle_rt_dl1_mk3_analog_8_analog_8_encode(analog_8);

    int ret = vehicle_rt_dl1_mk3_analog_8_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_8_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_8_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_8)
{
    struct vehicle_rt_dl1_mk3_analog_8_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_8_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_8_check_ranges(&msg);

    if (analog_8)
        *analog_8 = vehicle_rt_dl1_mk3_analog_8_analog_8_decode(msg.analog_8);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_8_analog_8_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_8_analog_8_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_8_analog_8_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_7_check_ranges(struct vehicle_rt_dl1_mk3_analog_7_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_7_analog_7_is_in_range(msg->analog_7))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_7_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_7)
{
    struct vehicle_rt_dl1_mk3_analog_7_t msg;

    msg.analog_7 = vehicle_rt_dl1_mk3_analog_7_analog_7_encode(analog_7);

    int ret = vehicle_rt_dl1_mk3_analog_7_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_7_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_7_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_7)
{
    struct vehicle_rt_dl1_mk3_analog_7_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_7_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_7_check_ranges(&msg);

    if (analog_7)
        *analog_7 = vehicle_rt_dl1_mk3_analog_7_analog_7_decode(msg.analog_7);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_7_analog_7_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_7_analog_7_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_7_analog_7_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_6_check_ranges(struct vehicle_rt_dl1_mk3_analog_6_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_6_analog_6_is_in_range(msg->analog_6))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_6_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_6)
{
    struct vehicle_rt_dl1_mk3_analog_6_t msg;

    msg.analog_6 = vehicle_rt_dl1_mk3_analog_6_analog_6_encode(analog_6);

    int ret = vehicle_rt_dl1_mk3_analog_6_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_6_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_6_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_6)
{
    struct vehicle_rt_dl1_mk3_analog_6_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_6_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_6_check_ranges(&msg);

    if (analog_6)
        *analog_6 = vehicle_rt_dl1_mk3_analog_6_analog_6_decode(msg.analog_6);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_6_analog_6_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_6_analog_6_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_6_analog_6_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_5_check_ranges(struct vehicle_rt_dl1_mk3_analog_5_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_5_analog_5_is_in_range(msg->analog_5))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_5_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_5)
{
    struct vehicle_rt_dl1_mk3_analog_5_t msg;

    msg.analog_5 = vehicle_rt_dl1_mk3_analog_5_analog_5_encode(analog_5);

    int ret = vehicle_rt_dl1_mk3_analog_5_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_5_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_5_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_5)
{
    struct vehicle_rt_dl1_mk3_analog_5_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_5_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_5_check_ranges(&msg);

    if (analog_5)
        *analog_5 = vehicle_rt_dl1_mk3_analog_5_analog_5_decode(msg.analog_5);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_5_analog_5_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_5_analog_5_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_5_analog_5_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_4_check_ranges(struct vehicle_rt_dl1_mk3_analog_4_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_4_analog_4_is_in_range(msg->analog_4))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_4_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_4)
{
    struct vehicle_rt_dl1_mk3_analog_4_t msg;

    msg.analog_4 = vehicle_rt_dl1_mk3_analog_4_analog_4_encode(analog_4);

    int ret = vehicle_rt_dl1_mk3_analog_4_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_4_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_4_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_4)
{
    struct vehicle_rt_dl1_mk3_analog_4_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_4_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_4_check_ranges(&msg);

    if (analog_4)
        *analog_4 = vehicle_rt_dl1_mk3_analog_4_analog_4_decode(msg.analog_4);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_4_analog_4_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_4_analog_4_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_4_analog_4_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_3_check_ranges(struct vehicle_rt_dl1_mk3_analog_3_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_3_analog_3_is_in_range(msg->analog_3))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_3_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_3)
{
    struct vehicle_rt_dl1_mk3_analog_3_t msg;

    msg.analog_3 = vehicle_rt_dl1_mk3_analog_3_analog_3_encode(analog_3);

    int ret = vehicle_rt_dl1_mk3_analog_3_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_3_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_3_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_3)
{
    struct vehicle_rt_dl1_mk3_analog_3_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_3_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_3_check_ranges(&msg);

    if (analog_3)
        *analog_3 = vehicle_rt_dl1_mk3_analog_3_analog_3_decode(msg.analog_3);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_3_analog_3_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_3_analog_3_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_3_analog_3_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_2_check_ranges(struct vehicle_rt_dl1_mk3_analog_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_2_analog_2_is_in_range(msg->analog_2))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_2)
{
    struct vehicle_rt_dl1_mk3_analog_2_t msg;

    msg.analog_2 = vehicle_rt_dl1_mk3_analog_2_analog_2_encode(analog_2);

    int ret = vehicle_rt_dl1_mk3_analog_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_2_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_2)
{
    struct vehicle_rt_dl1_mk3_analog_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_2_check_ranges(&msg);

    if (analog_2)
        *analog_2 = vehicle_rt_dl1_mk3_analog_2_analog_2_decode(msg.analog_2);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_2_analog_2_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_2_analog_2_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_2_analog_2_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_analog_1_check_ranges(struct vehicle_rt_dl1_mk3_analog_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_analog_1_analog_1_is_in_range(msg->analog_1))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_analog_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double analog_1)
{
    struct vehicle_rt_dl1_mk3_analog_1_t msg;

    msg.analog_1 = vehicle_rt_dl1_mk3_analog_1_analog_1_encode(analog_1);

    int ret = vehicle_rt_dl1_mk3_analog_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_analog_1_pack(outbuf, &msg, outbuf_sz);
    if (2 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_analog_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *analog_1)
{
    struct vehicle_rt_dl1_mk3_analog_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_analog_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_analog_1_check_ranges(&msg);

    if (analog_1)
        *analog_1 = vehicle_rt_dl1_mk3_analog_1_analog_1_decode(msg.analog_1);

    return ret;
}

uint16_t vehicle_rt_dl1_mk3_analog_1_analog_1_encode(double value)
{
    return (uint16_t)(value / 0.001);
}

double vehicle_rt_dl1_mk3_analog_1_analog_1_decode(uint16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_dl1_mk3_analog_1_analog_1_clamp(double val)
{
    double ret = val;


    return ret;
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

static int vehicle_rt_dl1_mk3_accel_check_ranges(struct vehicle_rt_dl1_mk3_accel_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_is_in_range(msg->validity_accel_longitudinal))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_accel_validity_accel_lateral_is_in_range(msg->validity_accel_lateral))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_accel_validity_accel_vertical_is_in_range(msg->validity_accel_vertical))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_accel_accuracy_accel_is_in_range(msg->accuracy_accel))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_accel_accel_longitudinal_is_in_range(msg->accel_longitudinal))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_accel_accel_lateral_is_in_range(msg->accel_lateral))
        return idx;

    idx++;

    if (!vehicle_rt_dl1_mk3_accel_accel_vertical_is_in_range(msg->accel_vertical))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_dl1_mk3_accel_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_accel_longitudinal,
    double validity_accel_lateral,
    double validity_accel_vertical,
    double accuracy_accel,
    double accel_longitudinal,
    double accel_lateral,
    double accel_vertical)
{
    struct vehicle_rt_dl1_mk3_accel_t msg;

    msg.validity_accel_longitudinal = vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_encode(validity_accel_longitudinal);
    msg.validity_accel_lateral = vehicle_rt_dl1_mk3_accel_validity_accel_lateral_encode(validity_accel_lateral);
    msg.validity_accel_vertical = vehicle_rt_dl1_mk3_accel_validity_accel_vertical_encode(validity_accel_vertical);
    msg.accuracy_accel = vehicle_rt_dl1_mk3_accel_accuracy_accel_encode(accuracy_accel);
    msg.accel_longitudinal = vehicle_rt_dl1_mk3_accel_accel_longitudinal_encode(accel_longitudinal);
    msg.accel_lateral = vehicle_rt_dl1_mk3_accel_accel_lateral_encode(accel_lateral);
    msg.accel_vertical = vehicle_rt_dl1_mk3_accel_accel_vertical_encode(accel_vertical);

    int ret = vehicle_rt_dl1_mk3_accel_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_dl1_mk3_accel_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_dl1_mk3_accel_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_accel_longitudinal,
    double *validity_accel_lateral,
    double *validity_accel_vertical,
    double *accuracy_accel,
    double *accel_longitudinal,
    double *accel_lateral,
    double *accel_vertical)
{
    struct vehicle_rt_dl1_mk3_accel_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_dl1_mk3_accel_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_dl1_mk3_accel_check_ranges(&msg);

    if (validity_accel_longitudinal)
        *validity_accel_longitudinal = vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_decode(msg.validity_accel_longitudinal);

    if (validity_accel_lateral)
        *validity_accel_lateral = vehicle_rt_dl1_mk3_accel_validity_accel_lateral_decode(msg.validity_accel_lateral);

    if (validity_accel_vertical)
        *validity_accel_vertical = vehicle_rt_dl1_mk3_accel_validity_accel_vertical_decode(msg.validity_accel_vertical);

    if (accuracy_accel)
        *accuracy_accel = vehicle_rt_dl1_mk3_accel_accuracy_accel_decode(msg.accuracy_accel);

    if (accel_longitudinal)
        *accel_longitudinal = vehicle_rt_dl1_mk3_accel_accel_longitudinal_decode(msg.accel_longitudinal);

    if (accel_lateral)
        *accel_lateral = vehicle_rt_dl1_mk3_accel_accel_lateral_decode(msg.accel_lateral);

    if (accel_vertical)
        *accel_vertical = vehicle_rt_dl1_mk3_accel_accel_vertical_decode(msg.accel_vertical);

    return ret;
}

uint8_t vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_dl1_mk3_accel_validity_accel_longitudinal_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_dl1_mk3_accel_validity_accel_lateral_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_dl1_mk3_accel_validity_accel_vertical_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_dl1_mk3_accel_accuracy_accel_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_dl1_mk3_accel_accel_longitudinal_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_dl1_mk3_accel_accel_lateral_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_dl1_mk3_accel_accel_vertical_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_4_vel_ned_2_check_ranges(struct vehicle_rt_sb_ins_vpt_4_vel_ned_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_is_in_range(msg->virtual_4_vel_ned_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_heading_is_in_range(msg->virtual_4_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_slip_is_in_range(msg->virtual_4_slip))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_4_vel_ned_d,
    double virtual_4_heading,
    double virtual_4_slip)
{
    struct vehicle_rt_sb_ins_vpt_4_vel_ned_2_t msg;

    msg.virtual_4_vel_ned_d = vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_encode(virtual_4_vel_ned_d);
    msg.virtual_4_heading = vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_heading_encode(virtual_4_heading);
    msg.virtual_4_slip = vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_slip_encode(virtual_4_slip);

    int ret = vehicle_rt_sb_ins_vpt_4_vel_ned_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_4_vel_ned_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_4_vel_ned_d,
    double *virtual_4_heading,
    double *virtual_4_slip)
{
    struct vehicle_rt_sb_ins_vpt_4_vel_ned_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_4_vel_ned_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_4_vel_ned_2_check_ranges(&msg);

    if (virtual_4_vel_ned_d)
        *virtual_4_vel_ned_d = vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_decode(msg.virtual_4_vel_ned_d);

    if (virtual_4_heading)
        *virtual_4_heading = vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_heading_decode(msg.virtual_4_heading);

    if (virtual_4_slip)
        *virtual_4_slip = vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_slip_decode(msg.virtual_4_slip);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_4_vel_ned_2_virtual_4_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_4_vel_ned_1_check_ranges(struct vehicle_rt_sb_ins_vpt_4_vel_ned_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_is_in_range(msg->virtual_4_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_e_is_in_range(msg->virtual_4_vel_ned_e))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_4_vel_ned_n,
    double virtual_4_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_4_vel_ned_1_t msg;

    msg.virtual_4_vel_ned_n = vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_encode(virtual_4_vel_ned_n);
    msg.virtual_4_vel_ned_e = vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_e_encode(virtual_4_vel_ned_e);

    int ret = vehicle_rt_sb_ins_vpt_4_vel_ned_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_4_vel_ned_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_4_vel_ned_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_4_vel_ned_n,
    double *virtual_4_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_4_vel_ned_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_4_vel_ned_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_4_vel_ned_1_check_ranges(&msg);

    if (virtual_4_vel_ned_n)
        *virtual_4_vel_ned_n = vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_decode(msg.virtual_4_vel_ned_n);

    if (virtual_4_vel_ned_e)
        *virtual_4_vel_ned_e = vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_e_decode(msg.virtual_4_vel_ned_e);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_4_vel_ned_1_virtual_4_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_4_offset_check_ranges(struct vehicle_rt_sb_ins_vpt_4_offset_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_is_in_range(msg->virtual_4_offset_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_y_is_in_range(msg->virtual_4_offset_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_z_is_in_range(msg->virtual_4_offset_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_4_offset_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_4_offset_x,
    double virtual_4_offset_y,
    double virtual_4_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_4_offset_t msg;

    msg.virtual_4_offset_x = vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_encode(virtual_4_offset_x);
    msg.virtual_4_offset_y = vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_y_encode(virtual_4_offset_y);
    msg.virtual_4_offset_z = vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_z_encode(virtual_4_offset_z);

    int ret = vehicle_rt_sb_ins_vpt_4_offset_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_4_offset_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_4_offset_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_4_offset_x,
    double *virtual_4_offset_y,
    double *virtual_4_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_4_offset_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_4_offset_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_4_offset_check_ranges(&msg);

    if (virtual_4_offset_x)
        *virtual_4_offset_x = vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_decode(msg.virtual_4_offset_x);

    if (virtual_4_offset_y)
        *virtual_4_offset_y = vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_y_decode(msg.virtual_4_offset_y);

    if (virtual_4_offset_z)
        *virtual_4_offset_z = vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_z_decode(msg.virtual_4_offset_z);

    return ret;
}

int16_t vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_4_offset_virtual_4_offset_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_3_vel_ned_2_check_ranges(struct vehicle_rt_sb_ins_vpt_3_vel_ned_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_is_in_range(msg->virtual_3_vel_ned_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_heading_is_in_range(msg->virtual_3_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_slip_is_in_range(msg->virtual_3_slip))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_3_vel_ned_d,
    double virtual_3_heading,
    double virtual_3_slip)
{
    struct vehicle_rt_sb_ins_vpt_3_vel_ned_2_t msg;

    msg.virtual_3_vel_ned_d = vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_encode(virtual_3_vel_ned_d);
    msg.virtual_3_heading = vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_heading_encode(virtual_3_heading);
    msg.virtual_3_slip = vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_slip_encode(virtual_3_slip);

    int ret = vehicle_rt_sb_ins_vpt_3_vel_ned_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_3_vel_ned_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_3_vel_ned_d,
    double *virtual_3_heading,
    double *virtual_3_slip)
{
    struct vehicle_rt_sb_ins_vpt_3_vel_ned_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_3_vel_ned_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_3_vel_ned_2_check_ranges(&msg);

    if (virtual_3_vel_ned_d)
        *virtual_3_vel_ned_d = vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_decode(msg.virtual_3_vel_ned_d);

    if (virtual_3_heading)
        *virtual_3_heading = vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_heading_decode(msg.virtual_3_heading);

    if (virtual_3_slip)
        *virtual_3_slip = vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_slip_decode(msg.virtual_3_slip);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_3_vel_ned_2_virtual_3_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_3_vel_ned_1_check_ranges(struct vehicle_rt_sb_ins_vpt_3_vel_ned_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_is_in_range(msg->virtual_3_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_e_is_in_range(msg->virtual_3_vel_ned_e))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_3_vel_ned_n,
    double virtual_3_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_3_vel_ned_1_t msg;

    msg.virtual_3_vel_ned_n = vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_encode(virtual_3_vel_ned_n);
    msg.virtual_3_vel_ned_e = vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_e_encode(virtual_3_vel_ned_e);

    int ret = vehicle_rt_sb_ins_vpt_3_vel_ned_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_3_vel_ned_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_3_vel_ned_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_3_vel_ned_n,
    double *virtual_3_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_3_vel_ned_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_3_vel_ned_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_3_vel_ned_1_check_ranges(&msg);

    if (virtual_3_vel_ned_n)
        *virtual_3_vel_ned_n = vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_decode(msg.virtual_3_vel_ned_n);

    if (virtual_3_vel_ned_e)
        *virtual_3_vel_ned_e = vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_e_decode(msg.virtual_3_vel_ned_e);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_3_vel_ned_1_virtual_3_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_3_offset_check_ranges(struct vehicle_rt_sb_ins_vpt_3_offset_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_is_in_range(msg->virtual_3_offset_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_y_is_in_range(msg->virtual_3_offset_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_z_is_in_range(msg->virtual_3_offset_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_3_offset_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_3_offset_x,
    double virtual_3_offset_y,
    double virtual_3_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_3_offset_t msg;

    msg.virtual_3_offset_x = vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_encode(virtual_3_offset_x);
    msg.virtual_3_offset_y = vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_y_encode(virtual_3_offset_y);
    msg.virtual_3_offset_z = vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_z_encode(virtual_3_offset_z);

    int ret = vehicle_rt_sb_ins_vpt_3_offset_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_3_offset_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_3_offset_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_3_offset_x,
    double *virtual_3_offset_y,
    double *virtual_3_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_3_offset_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_3_offset_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_3_offset_check_ranges(&msg);

    if (virtual_3_offset_x)
        *virtual_3_offset_x = vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_decode(msg.virtual_3_offset_x);

    if (virtual_3_offset_y)
        *virtual_3_offset_y = vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_y_decode(msg.virtual_3_offset_y);

    if (virtual_3_offset_z)
        *virtual_3_offset_z = vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_z_decode(msg.virtual_3_offset_z);

    return ret;
}

int16_t vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_3_offset_virtual_3_offset_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_2_vel_ned_2_check_ranges(struct vehicle_rt_sb_ins_vpt_2_vel_ned_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_is_in_range(msg->virtual_2_vel_ned_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_heading_is_in_range(msg->virtual_2_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_slip_is_in_range(msg->virtual_2_slip))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_2_vel_ned_d,
    double virtual_2_heading,
    double virtual_2_slip)
{
    struct vehicle_rt_sb_ins_vpt_2_vel_ned_2_t msg;

    msg.virtual_2_vel_ned_d = vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_encode(virtual_2_vel_ned_d);
    msg.virtual_2_heading = vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_heading_encode(virtual_2_heading);
    msg.virtual_2_slip = vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_slip_encode(virtual_2_slip);

    int ret = vehicle_rt_sb_ins_vpt_2_vel_ned_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_2_vel_ned_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_2_vel_ned_d,
    double *virtual_2_heading,
    double *virtual_2_slip)
{
    struct vehicle_rt_sb_ins_vpt_2_vel_ned_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_2_vel_ned_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_2_vel_ned_2_check_ranges(&msg);

    if (virtual_2_vel_ned_d)
        *virtual_2_vel_ned_d = vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_decode(msg.virtual_2_vel_ned_d);

    if (virtual_2_heading)
        *virtual_2_heading = vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_heading_decode(msg.virtual_2_heading);

    if (virtual_2_slip)
        *virtual_2_slip = vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_slip_decode(msg.virtual_2_slip);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_2_vel_ned_2_virtual_2_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_2_vel_ned_1_check_ranges(struct vehicle_rt_sb_ins_vpt_2_vel_ned_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_is_in_range(msg->virtual_2_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_e_is_in_range(msg->virtual_2_vel_ned_e))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_2_vel_ned_n,
    double virtual_2_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_2_vel_ned_1_t msg;

    msg.virtual_2_vel_ned_n = vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_encode(virtual_2_vel_ned_n);
    msg.virtual_2_vel_ned_e = vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_e_encode(virtual_2_vel_ned_e);

    int ret = vehicle_rt_sb_ins_vpt_2_vel_ned_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_2_vel_ned_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_2_vel_ned_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_2_vel_ned_n,
    double *virtual_2_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_2_vel_ned_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_2_vel_ned_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_2_vel_ned_1_check_ranges(&msg);

    if (virtual_2_vel_ned_n)
        *virtual_2_vel_ned_n = vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_decode(msg.virtual_2_vel_ned_n);

    if (virtual_2_vel_ned_e)
        *virtual_2_vel_ned_e = vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_e_decode(msg.virtual_2_vel_ned_e);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_2_vel_ned_1_virtual_2_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_2_offset_check_ranges(struct vehicle_rt_sb_ins_vpt_2_offset_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_is_in_range(msg->virtual_2_offset_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_y_is_in_range(msg->virtual_2_offset_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_z_is_in_range(msg->virtual_2_offset_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_2_offset_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_2_offset_x,
    double virtual_2_offset_y,
    double virtual_2_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_2_offset_t msg;

    msg.virtual_2_offset_x = vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_encode(virtual_2_offset_x);
    msg.virtual_2_offset_y = vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_y_encode(virtual_2_offset_y);
    msg.virtual_2_offset_z = vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_z_encode(virtual_2_offset_z);

    int ret = vehicle_rt_sb_ins_vpt_2_offset_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_2_offset_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_2_offset_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_2_offset_x,
    double *virtual_2_offset_y,
    double *virtual_2_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_2_offset_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_2_offset_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_2_offset_check_ranges(&msg);

    if (virtual_2_offset_x)
        *virtual_2_offset_x = vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_decode(msg.virtual_2_offset_x);

    if (virtual_2_offset_y)
        *virtual_2_offset_y = vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_y_decode(msg.virtual_2_offset_y);

    if (virtual_2_offset_z)
        *virtual_2_offset_z = vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_z_decode(msg.virtual_2_offset_z);

    return ret;
}

int16_t vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_2_offset_virtual_2_offset_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_1_vel_ned_2_check_ranges(struct vehicle_rt_sb_ins_vpt_1_vel_ned_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_is_in_range(msg->virtual_1_vel_ned_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_heading_is_in_range(msg->virtual_1_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_slip_is_in_range(msg->virtual_1_slip))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_1_vel_ned_d,
    double virtual_1_heading,
    double virtual_1_slip)
{
    struct vehicle_rt_sb_ins_vpt_1_vel_ned_2_t msg;

    msg.virtual_1_vel_ned_d = vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_encode(virtual_1_vel_ned_d);
    msg.virtual_1_heading = vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_heading_encode(virtual_1_heading);
    msg.virtual_1_slip = vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_slip_encode(virtual_1_slip);

    int ret = vehicle_rt_sb_ins_vpt_1_vel_ned_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_1_vel_ned_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_1_vel_ned_d,
    double *virtual_1_heading,
    double *virtual_1_slip)
{
    struct vehicle_rt_sb_ins_vpt_1_vel_ned_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_1_vel_ned_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_1_vel_ned_2_check_ranges(&msg);

    if (virtual_1_vel_ned_d)
        *virtual_1_vel_ned_d = vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_decode(msg.virtual_1_vel_ned_d);

    if (virtual_1_heading)
        *virtual_1_heading = vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_heading_decode(msg.virtual_1_heading);

    if (virtual_1_slip)
        *virtual_1_slip = vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_slip_decode(msg.virtual_1_slip);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_1_vel_ned_2_virtual_1_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_1_vel_ned_1_check_ranges(struct vehicle_rt_sb_ins_vpt_1_vel_ned_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_is_in_range(msg->virtual_1_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_e_is_in_range(msg->virtual_1_vel_ned_e))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_1_vel_ned_n,
    double virtual_1_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_1_vel_ned_1_t msg;

    msg.virtual_1_vel_ned_n = vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_encode(virtual_1_vel_ned_n);
    msg.virtual_1_vel_ned_e = vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_e_encode(virtual_1_vel_ned_e);

    int ret = vehicle_rt_sb_ins_vpt_1_vel_ned_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_1_vel_ned_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_1_vel_ned_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_1_vel_ned_n,
    double *virtual_1_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vpt_1_vel_ned_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_1_vel_ned_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_1_vel_ned_1_check_ranges(&msg);

    if (virtual_1_vel_ned_n)
        *virtual_1_vel_ned_n = vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_decode(msg.virtual_1_vel_ned_n);

    if (virtual_1_vel_ned_e)
        *virtual_1_vel_ned_e = vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_e_decode(msg.virtual_1_vel_ned_e);

    return ret;
}

int32_t vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_encode(double value)
{
    return (int32_t)(value / 0.0001);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_decode(int32_t value)
{
    return ((double)value * 0.0001);
}

double vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vpt_1_vel_ned_1_virtual_1_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_vpt_1_offset_check_ranges(struct vehicle_rt_sb_ins_vpt_1_offset_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_is_in_range(msg->virtual_1_offset_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_y_is_in_range(msg->virtual_1_offset_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_z_is_in_range(msg->virtual_1_offset_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vpt_1_offset_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double virtual_1_offset_x,
    double virtual_1_offset_y,
    double virtual_1_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_1_offset_t msg;

    msg.virtual_1_offset_x = vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_encode(virtual_1_offset_x);
    msg.virtual_1_offset_y = vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_y_encode(virtual_1_offset_y);
    msg.virtual_1_offset_z = vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_z_encode(virtual_1_offset_z);

    int ret = vehicle_rt_sb_ins_vpt_1_offset_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vpt_1_offset_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vpt_1_offset_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *virtual_1_offset_x,
    double *virtual_1_offset_y,
    double *virtual_1_offset_z)
{
    struct vehicle_rt_sb_ins_vpt_1_offset_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vpt_1_offset_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vpt_1_offset_check_ranges(&msg);

    if (virtual_1_offset_x)
        *virtual_1_offset_x = vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_decode(msg.virtual_1_offset_x);

    if (virtual_1_offset_y)
        *virtual_1_offset_y = vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_y_decode(msg.virtual_1_offset_y);

    if (virtual_1_offset_z)
        *virtual_1_offset_z = vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_z_decode(msg.virtual_1_offset_z);

    return ret;
}

int16_t vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_encode(double value)
{
    return (int16_t)(value / 0.001);
}

double vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_decode(int16_t value)
{
    return ((double)value * 0.001);
}

double vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

double vehicle_rt_sb_ins_vpt_1_offset_virtual_1_offset_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -32.768);
    ret = CTOOLS_MIN(ret, 32.767);
    return ret;
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

static int vehicle_rt_sb_ins_slip_check_ranges(struct vehicle_rt_sb_ins_slip_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_slip_validity_ins_slip_is_in_range(msg->validity_ins_slip))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_slip_validity_ins_squat_is_in_range(msg->validity_ins_squat))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_slip_accuracy_ins_slip_is_in_range(msg->accuracy_ins_slip))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_slip_ins_slip_is_in_range(msg->ins_slip))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_slip_accuracy_ins_squat_is_in_range(msg->accuracy_ins_squat))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_slip_ins_squat_is_in_range(msg->ins_squat))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_slip_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_slip,
    double validity_ins_squat,
    double accuracy_ins_slip,
    double ins_slip,
    double accuracy_ins_squat,
    double ins_squat)
{
    struct vehicle_rt_sb_ins_slip_t msg;

    msg.validity_ins_slip = vehicle_rt_sb_ins_slip_validity_ins_slip_encode(validity_ins_slip);
    msg.validity_ins_squat = vehicle_rt_sb_ins_slip_validity_ins_squat_encode(validity_ins_squat);
    msg.accuracy_ins_slip = vehicle_rt_sb_ins_slip_accuracy_ins_slip_encode(accuracy_ins_slip);
    msg.ins_slip = vehicle_rt_sb_ins_slip_ins_slip_encode(ins_slip);
    msg.accuracy_ins_squat = vehicle_rt_sb_ins_slip_accuracy_ins_squat_encode(accuracy_ins_squat);
    msg.ins_squat = vehicle_rt_sb_ins_slip_ins_squat_encode(ins_squat);

    int ret = vehicle_rt_sb_ins_slip_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_slip_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_slip_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_slip,
    double *validity_ins_squat,
    double *accuracy_ins_slip,
    double *ins_slip,
    double *accuracy_ins_squat,
    double *ins_squat)
{
    struct vehicle_rt_sb_ins_slip_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_slip_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_slip_check_ranges(&msg);

    if (validity_ins_slip)
        *validity_ins_slip = vehicle_rt_sb_ins_slip_validity_ins_slip_decode(msg.validity_ins_slip);

    if (validity_ins_squat)
        *validity_ins_squat = vehicle_rt_sb_ins_slip_validity_ins_squat_decode(msg.validity_ins_squat);

    if (accuracy_ins_slip)
        *accuracy_ins_slip = vehicle_rt_sb_ins_slip_accuracy_ins_slip_decode(msg.accuracy_ins_slip);

    if (ins_slip)
        *ins_slip = vehicle_rt_sb_ins_slip_ins_slip_decode(msg.ins_slip);

    if (accuracy_ins_squat)
        *accuracy_ins_squat = vehicle_rt_sb_ins_slip_accuracy_ins_squat_decode(msg.accuracy_ins_squat);

    if (ins_squat)
        *ins_squat = vehicle_rt_sb_ins_slip_ins_squat_decode(msg.ins_squat);

    return ret;
}

uint8_t vehicle_rt_sb_ins_slip_validity_ins_slip_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_slip_validity_ins_slip_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_slip_validity_ins_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_slip_validity_ins_squat_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_slip_accuracy_ins_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_slip_ins_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_ins_slip_accuracy_ins_squat_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_slip_ins_squat_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

static int vehicle_rt_sb_ins_vel_ecef_2_check_ranges(struct vehicle_rt_sb_ins_vel_ecef_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_is_in_range(msg->validity_ins_vel_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_z_is_in_range(msg->validity_ins_vel_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_y_is_in_range(msg->ins_vel_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_z_is_in_range(msg->ins_vel_ecef_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vel_ecef_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_vel_ecef_y,
    double validity_ins_vel_ecef_z,
    double ins_vel_ecef_y,
    double ins_vel_ecef_z)
{
    struct vehicle_rt_sb_ins_vel_ecef_2_t msg;

    msg.validity_ins_vel_ecef_y = vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_encode(validity_ins_vel_ecef_y);
    msg.validity_ins_vel_ecef_z = vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_z_encode(validity_ins_vel_ecef_z);
    msg.ins_vel_ecef_y = vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_y_encode(ins_vel_ecef_y);
    msg.ins_vel_ecef_z = vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_z_encode(ins_vel_ecef_z);

    int ret = vehicle_rt_sb_ins_vel_ecef_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vel_ecef_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vel_ecef_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_vel_ecef_y,
    double *validity_ins_vel_ecef_z,
    double *ins_vel_ecef_y,
    double *ins_vel_ecef_z)
{
    struct vehicle_rt_sb_ins_vel_ecef_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vel_ecef_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vel_ecef_2_check_ranges(&msg);

    if (validity_ins_vel_ecef_y)
        *validity_ins_vel_ecef_y = vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_decode(msg.validity_ins_vel_ecef_y);

    if (validity_ins_vel_ecef_z)
        *validity_ins_vel_ecef_z = vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_z_decode(msg.validity_ins_vel_ecef_z);

    if (ins_vel_ecef_y)
        *ins_vel_ecef_y = vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_y_decode(msg.ins_vel_ecef_y);

    if (ins_vel_ecef_z)
        *ins_vel_ecef_z = vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_z_decode(msg.ins_vel_ecef_z);

    return ret;
}

uint8_t vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ecef_2_validity_ins_vel_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ecef_2_ins_vel_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_vel_ecef_1_check_ranges(struct vehicle_rt_sb_ins_vel_ecef_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_is_in_range(msg->validity_ins_vel_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_x_is_in_range(msg->accuracy_ins_vel_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_y_is_in_range(msg->accuracy_ins_vel_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_z_is_in_range(msg->accuracy_ins_vel_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ecef_1_ins_vel_ecef_x_is_in_range(msg->ins_vel_ecef_x))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vel_ecef_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_vel_ecef_x,
    double accuracy_ins_vel_ecef_x,
    double accuracy_ins_vel_ecef_y,
    double accuracy_ins_vel_ecef_z,
    double ins_vel_ecef_x)
{
    struct vehicle_rt_sb_ins_vel_ecef_1_t msg;

    msg.validity_ins_vel_ecef_x = vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_encode(validity_ins_vel_ecef_x);
    msg.accuracy_ins_vel_ecef_x = vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_x_encode(accuracy_ins_vel_ecef_x);
    msg.accuracy_ins_vel_ecef_y = vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_y_encode(accuracy_ins_vel_ecef_y);
    msg.accuracy_ins_vel_ecef_z = vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_z_encode(accuracy_ins_vel_ecef_z);
    msg.ins_vel_ecef_x = vehicle_rt_sb_ins_vel_ecef_1_ins_vel_ecef_x_encode(ins_vel_ecef_x);

    int ret = vehicle_rt_sb_ins_vel_ecef_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vel_ecef_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vel_ecef_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_vel_ecef_x,
    double *accuracy_ins_vel_ecef_x,
    double *accuracy_ins_vel_ecef_y,
    double *accuracy_ins_vel_ecef_z,
    double *ins_vel_ecef_x)
{
    struct vehicle_rt_sb_ins_vel_ecef_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vel_ecef_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vel_ecef_1_check_ranges(&msg);

    if (validity_ins_vel_ecef_x)
        *validity_ins_vel_ecef_x = vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_decode(msg.validity_ins_vel_ecef_x);

    if (accuracy_ins_vel_ecef_x)
        *accuracy_ins_vel_ecef_x = vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_x_decode(msg.accuracy_ins_vel_ecef_x);

    if (accuracy_ins_vel_ecef_y)
        *accuracy_ins_vel_ecef_y = vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_y_decode(msg.accuracy_ins_vel_ecef_y);

    if (accuracy_ins_vel_ecef_z)
        *accuracy_ins_vel_ecef_z = vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_z_decode(msg.accuracy_ins_vel_ecef_z);

    if (ins_vel_ecef_x)
        *ins_vel_ecef_x = vehicle_rt_sb_ins_vel_ecef_1_ins_vel_ecef_x_decode(msg.ins_vel_ecef_x);

    return ret;
}

uint8_t vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_vel_ecef_1_validity_ins_vel_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ecef_1_accuracy_ins_vel_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ecef_1_ins_vel_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_vel_ned_2_check_ranges(struct vehicle_rt_sb_ins_vel_ned_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_is_in_range(msg->validity_ins_vel_ned_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ned_2_accuracy_ins_vel_d_is_in_range(msg->accuracy_ins_vel_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ned_2_ins_vel_ned_d_is_in_range(msg->ins_vel_ned_d))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vel_ned_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_vel_ned_d,
    double accuracy_ins_vel_d,
    double ins_vel_ned_d)
{
    struct vehicle_rt_sb_ins_vel_ned_2_t msg;

    msg.validity_ins_vel_ned_d = vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_encode(validity_ins_vel_ned_d);
    msg.accuracy_ins_vel_d = vehicle_rt_sb_ins_vel_ned_2_accuracy_ins_vel_d_encode(accuracy_ins_vel_d);
    msg.ins_vel_ned_d = vehicle_rt_sb_ins_vel_ned_2_ins_vel_ned_d_encode(ins_vel_ned_d);

    int ret = vehicle_rt_sb_ins_vel_ned_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vel_ned_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vel_ned_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_vel_ned_d,
    double *accuracy_ins_vel_d,
    double *ins_vel_ned_d)
{
    struct vehicle_rt_sb_ins_vel_ned_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vel_ned_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vel_ned_2_check_ranges(&msg);

    if (validity_ins_vel_ned_d)
        *validity_ins_vel_ned_d = vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_decode(msg.validity_ins_vel_ned_d);

    if (accuracy_ins_vel_d)
        *accuracy_ins_vel_d = vehicle_rt_sb_ins_vel_ned_2_accuracy_ins_vel_d_decode(msg.accuracy_ins_vel_d);

    if (ins_vel_ned_d)
        *ins_vel_ned_d = vehicle_rt_sb_ins_vel_ned_2_ins_vel_ned_d_decode(msg.ins_vel_ned_d);

    return ret;
}

uint8_t vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_vel_ned_2_validity_ins_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ned_2_accuracy_ins_vel_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ned_2_ins_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_vel_ned_1_check_ranges(struct vehicle_rt_sb_ins_vel_ned_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_is_in_range(msg->validity_ins_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_e_is_in_range(msg->validity_ins_vel_ned_e))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ned_1_accuracy_ins_vel_ne_is_in_range(msg->accuracy_ins_vel_ne))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_n_is_in_range(msg->ins_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_e_is_in_range(msg->ins_vel_ned_e))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_vel_ned_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_vel_ned_n,
    double validity_ins_vel_ned_e,
    double accuracy_ins_vel_ne,
    double ins_vel_ned_n,
    double ins_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vel_ned_1_t msg;

    msg.validity_ins_vel_ned_n = vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_encode(validity_ins_vel_ned_n);
    msg.validity_ins_vel_ned_e = vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_e_encode(validity_ins_vel_ned_e);
    msg.accuracy_ins_vel_ne = vehicle_rt_sb_ins_vel_ned_1_accuracy_ins_vel_ne_encode(accuracy_ins_vel_ne);
    msg.ins_vel_ned_n = vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_n_encode(ins_vel_ned_n);
    msg.ins_vel_ned_e = vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_e_encode(ins_vel_ned_e);

    int ret = vehicle_rt_sb_ins_vel_ned_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_vel_ned_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_vel_ned_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_vel_ned_n,
    double *validity_ins_vel_ned_e,
    double *accuracy_ins_vel_ne,
    double *ins_vel_ned_n,
    double *ins_vel_ned_e)
{
    struct vehicle_rt_sb_ins_vel_ned_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_vel_ned_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_vel_ned_1_check_ranges(&msg);

    if (validity_ins_vel_ned_n)
        *validity_ins_vel_ned_n = vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_decode(msg.validity_ins_vel_ned_n);

    if (validity_ins_vel_ned_e)
        *validity_ins_vel_ned_e = vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_e_decode(msg.validity_ins_vel_ned_e);

    if (accuracy_ins_vel_ne)
        *accuracy_ins_vel_ne = vehicle_rt_sb_ins_vel_ned_1_accuracy_ins_vel_ne_decode(msg.accuracy_ins_vel_ne);

    if (ins_vel_ned_n)
        *ins_vel_ned_n = vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_n_decode(msg.ins_vel_ned_n);

    if (ins_vel_ned_e)
        *ins_vel_ned_e = vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_e_decode(msg.ins_vel_ned_e);

    return ret;
}

uint8_t vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ned_1_validity_ins_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ned_1_accuracy_ins_vel_ne_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_ins_vel_ned_1_ins_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_ins_pos_ecef_2_check_ranges(struct vehicle_rt_sb_ins_pos_ecef_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_is_in_range(msg->ins_pos_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_z_is_in_range(msg->ins_pos_ecef_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_pos_ecef_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ins_pos_ecef_y,
    double ins_pos_ecef_z)
{
    struct vehicle_rt_sb_ins_pos_ecef_2_t msg;

    msg.ins_pos_ecef_y = vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_encode(ins_pos_ecef_y);
    msg.ins_pos_ecef_z = vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_z_encode(ins_pos_ecef_z);

    int ret = vehicle_rt_sb_ins_pos_ecef_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_pos_ecef_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_pos_ecef_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ins_pos_ecef_y,
    double *ins_pos_ecef_z)
{
    struct vehicle_rt_sb_ins_pos_ecef_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_pos_ecef_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_pos_ecef_2_check_ranges(&msg);

    if (ins_pos_ecef_y)
        *ins_pos_ecef_y = vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_decode(msg.ins_pos_ecef_y);

    if (ins_pos_ecef_z)
        *ins_pos_ecef_z = vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_z_decode(msg.ins_pos_ecef_z);

    return ret;
}

int32_t vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_decode(int32_t value)
{
    return ((double)value * 0.01);
}

double vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -10000000.0);
    ret = CTOOLS_MIN(ret, 10000000.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_ecef_2_ins_pos_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -10000000.0);
    ret = CTOOLS_MIN(ret, 10000000.0);
    return ret;
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

static int vehicle_rt_sb_ins_pos_ecef_1_check_ranges(struct vehicle_rt_sb_ins_pos_ecef_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_is_in_range(msg->validity_ins_pos_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_y_is_in_range(msg->validity_ins_pos_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_z_is_in_range(msg->validity_ins_pos_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_x_is_in_range(msg->accuracy_ins_pos_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_y_is_in_range(msg->accuracy_ins_pos_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_z_is_in_range(msg->accuracy_ins_pos_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_ecef_1_ins_pos_ecef_x_is_in_range(msg->ins_pos_ecef_x))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_pos_ecef_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_pos_ecef_x,
    double validity_ins_pos_ecef_y,
    double validity_ins_pos_ecef_z,
    double accuracy_ins_pos_ecef_x,
    double accuracy_ins_pos_ecef_y,
    double accuracy_ins_pos_ecef_z,
    double ins_pos_ecef_x)
{
    struct vehicle_rt_sb_ins_pos_ecef_1_t msg;

    msg.validity_ins_pos_ecef_x = vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_encode(validity_ins_pos_ecef_x);
    msg.validity_ins_pos_ecef_y = vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_y_encode(validity_ins_pos_ecef_y);
    msg.validity_ins_pos_ecef_z = vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_z_encode(validity_ins_pos_ecef_z);
    msg.accuracy_ins_pos_ecef_x = vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_x_encode(accuracy_ins_pos_ecef_x);
    msg.accuracy_ins_pos_ecef_y = vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_y_encode(accuracy_ins_pos_ecef_y);
    msg.accuracy_ins_pos_ecef_z = vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_z_encode(accuracy_ins_pos_ecef_z);
    msg.ins_pos_ecef_x = vehicle_rt_sb_ins_pos_ecef_1_ins_pos_ecef_x_encode(ins_pos_ecef_x);

    int ret = vehicle_rt_sb_ins_pos_ecef_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_pos_ecef_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_pos_ecef_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_pos_ecef_x,
    double *validity_ins_pos_ecef_y,
    double *validity_ins_pos_ecef_z,
    double *accuracy_ins_pos_ecef_x,
    double *accuracy_ins_pos_ecef_y,
    double *accuracy_ins_pos_ecef_z,
    double *ins_pos_ecef_x)
{
    struct vehicle_rt_sb_ins_pos_ecef_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_pos_ecef_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_pos_ecef_1_check_ranges(&msg);

    if (validity_ins_pos_ecef_x)
        *validity_ins_pos_ecef_x = vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_decode(msg.validity_ins_pos_ecef_x);

    if (validity_ins_pos_ecef_y)
        *validity_ins_pos_ecef_y = vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_y_decode(msg.validity_ins_pos_ecef_y);

    if (validity_ins_pos_ecef_z)
        *validity_ins_pos_ecef_z = vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_z_decode(msg.validity_ins_pos_ecef_z);

    if (accuracy_ins_pos_ecef_x)
        *accuracy_ins_pos_ecef_x = vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_x_decode(msg.accuracy_ins_pos_ecef_x);

    if (accuracy_ins_pos_ecef_y)
        *accuracy_ins_pos_ecef_y = vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_y_decode(msg.accuracy_ins_pos_ecef_y);

    if (accuracy_ins_pos_ecef_z)
        *accuracy_ins_pos_ecef_z = vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_z_decode(msg.accuracy_ins_pos_ecef_z);

    if (ins_pos_ecef_x)
        *ins_pos_ecef_x = vehicle_rt_sb_ins_pos_ecef_1_ins_pos_ecef_x_decode(msg.ins_pos_ecef_x);

    return ret;
}

uint8_t vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_ecef_1_validity_ins_pos_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_ecef_1_accuracy_ins_pos_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_ecef_1_ins_pos_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -10000000.0);
    ret = CTOOLS_MIN(ret, 10000000.0);
    return ret;
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

static int vehicle_rt_sb_ins_pos_llh_2_check_ranges(struct vehicle_rt_sb_ins_pos_llh_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_is_in_range(msg->ins_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_altitude_is_in_range(msg->ins_pos_llh_altitude))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_pos_llh_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ins_pos_llh_longitude,
    double ins_pos_llh_altitude)
{
    struct vehicle_rt_sb_ins_pos_llh_2_t msg;

    msg.ins_pos_llh_longitude = vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_encode(ins_pos_llh_longitude);
    msg.ins_pos_llh_altitude = vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_altitude_encode(ins_pos_llh_altitude);

    int ret = vehicle_rt_sb_ins_pos_llh_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_pos_llh_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_pos_llh_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ins_pos_llh_longitude,
    double *ins_pos_llh_altitude)
{
    struct vehicle_rt_sb_ins_pos_llh_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_pos_llh_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_pos_llh_2_check_ranges(&msg);

    if (ins_pos_llh_longitude)
        *ins_pos_llh_longitude = vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_decode(msg.ins_pos_llh_longitude);

    if (ins_pos_llh_altitude)
        *ins_pos_llh_altitude = vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_altitude_decode(msg.ins_pos_llh_altitude);

    return ret;
}

int32_t vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

double vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_llh_2_ins_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -1000.0);
    ret = CTOOLS_MIN(ret, 100000.0);
    return ret;
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

static int vehicle_rt_sb_ins_pos_llh_1_check_ranges(struct vehicle_rt_sb_ins_pos_llh_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_is_in_range(msg->validity_ins_pos_llh_latitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_longitude_is_in_range(msg->validity_ins_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_altitude_is_in_range(msg->validity_ins_pos_llh_altitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_latitude_is_in_range(msg->accuracy_ins_pos_llh_latitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_longitude_is_in_range(msg->accuracy_ins_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_altitude_is_in_range(msg->accuracy_ins_pos_llh_altitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_pos_llh_1_ins_pos_llh_latitude_is_in_range(msg->ins_pos_llh_latitude))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_pos_llh_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_pos_llh_latitude,
    double validity_ins_pos_llh_longitude,
    double validity_ins_pos_llh_altitude,
    double accuracy_ins_pos_llh_latitude,
    double accuracy_ins_pos_llh_longitude,
    double accuracy_ins_pos_llh_altitude,
    double ins_pos_llh_latitude)
{
    struct vehicle_rt_sb_ins_pos_llh_1_t msg;

    msg.validity_ins_pos_llh_latitude = vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_encode(validity_ins_pos_llh_latitude);
    msg.validity_ins_pos_llh_longitude = vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_longitude_encode(validity_ins_pos_llh_longitude);
    msg.validity_ins_pos_llh_altitude = vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_altitude_encode(validity_ins_pos_llh_altitude);
    msg.accuracy_ins_pos_llh_latitude = vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_latitude_encode(accuracy_ins_pos_llh_latitude);
    msg.accuracy_ins_pos_llh_longitude = vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_longitude_encode(accuracy_ins_pos_llh_longitude);
    msg.accuracy_ins_pos_llh_altitude = vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_altitude_encode(accuracy_ins_pos_llh_altitude);
    msg.ins_pos_llh_latitude = vehicle_rt_sb_ins_pos_llh_1_ins_pos_llh_latitude_encode(ins_pos_llh_latitude);

    int ret = vehicle_rt_sb_ins_pos_llh_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_pos_llh_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_pos_llh_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_pos_llh_latitude,
    double *validity_ins_pos_llh_longitude,
    double *validity_ins_pos_llh_altitude,
    double *accuracy_ins_pos_llh_latitude,
    double *accuracy_ins_pos_llh_longitude,
    double *accuracy_ins_pos_llh_altitude,
    double *ins_pos_llh_latitude)
{
    struct vehicle_rt_sb_ins_pos_llh_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_pos_llh_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_pos_llh_1_check_ranges(&msg);

    if (validity_ins_pos_llh_latitude)
        *validity_ins_pos_llh_latitude = vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_decode(msg.validity_ins_pos_llh_latitude);

    if (validity_ins_pos_llh_longitude)
        *validity_ins_pos_llh_longitude = vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_longitude_decode(msg.validity_ins_pos_llh_longitude);

    if (validity_ins_pos_llh_altitude)
        *validity_ins_pos_llh_altitude = vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_altitude_decode(msg.validity_ins_pos_llh_altitude);

    if (accuracy_ins_pos_llh_latitude)
        *accuracy_ins_pos_llh_latitude = vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_latitude_decode(msg.accuracy_ins_pos_llh_latitude);

    if (accuracy_ins_pos_llh_longitude)
        *accuracy_ins_pos_llh_longitude = vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_longitude_decode(msg.accuracy_ins_pos_llh_longitude);

    if (accuracy_ins_pos_llh_altitude)
        *accuracy_ins_pos_llh_altitude = vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_altitude_decode(msg.accuracy_ins_pos_llh_altitude);

    if (ins_pos_llh_latitude)
        *ins_pos_llh_latitude = vehicle_rt_sb_ins_pos_llh_1_ins_pos_llh_latitude_decode(msg.ins_pos_llh_latitude);

    return ret;
}

uint8_t vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_llh_1_validity_ins_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_llh_1_accuracy_ins_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_pos_llh_1_ins_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_ins_heading_gradient_2_check_ranges(struct vehicle_rt_sb_ins_heading_gradient_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_is_in_range(msg->validity_ins_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_2_validity_ins_gradient_is_in_range(msg->validity_ins_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_heading_is_in_range(msg->accuracy_ins_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_2_ins_heading_2_is_in_range(msg->ins_heading_2))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_gradient_is_in_range(msg->accuracy_ins_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_2_ins_gradient_is_in_range(msg->ins_gradient))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_heading_gradient_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_heading,
    double validity_ins_gradient,
    double accuracy_ins_heading,
    double ins_heading_2,
    double accuracy_ins_gradient,
    double ins_gradient)
{
    struct vehicle_rt_sb_ins_heading_gradient_2_t msg;

    msg.validity_ins_heading = vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_encode(validity_ins_heading);
    msg.validity_ins_gradient = vehicle_rt_sb_ins_heading_gradient_2_validity_ins_gradient_encode(validity_ins_gradient);
    msg.accuracy_ins_heading = vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_heading_encode(accuracy_ins_heading);
    msg.ins_heading_2 = vehicle_rt_sb_ins_heading_gradient_2_ins_heading_2_encode(ins_heading_2);
    msg.accuracy_ins_gradient = vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_gradient_encode(accuracy_ins_gradient);
    msg.ins_gradient = vehicle_rt_sb_ins_heading_gradient_2_ins_gradient_encode(ins_gradient);

    int ret = vehicle_rt_sb_ins_heading_gradient_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_heading_gradient_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_heading_gradient_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_heading,
    double *validity_ins_gradient,
    double *accuracy_ins_heading,
    double *ins_heading_2,
    double *accuracy_ins_gradient,
    double *ins_gradient)
{
    struct vehicle_rt_sb_ins_heading_gradient_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_heading_gradient_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_heading_gradient_2_check_ranges(&msg);

    if (validity_ins_heading)
        *validity_ins_heading = vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_decode(msg.validity_ins_heading);

    if (validity_ins_gradient)
        *validity_ins_gradient = vehicle_rt_sb_ins_heading_gradient_2_validity_ins_gradient_decode(msg.validity_ins_gradient);

    if (accuracy_ins_heading)
        *accuracy_ins_heading = vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_heading_decode(msg.accuracy_ins_heading);

    if (ins_heading_2)
        *ins_heading_2 = vehicle_rt_sb_ins_heading_gradient_2_ins_heading_2_decode(msg.ins_heading_2);

    if (accuracy_ins_gradient)
        *accuracy_ins_gradient = vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_gradient_decode(msg.accuracy_ins_gradient);

    if (ins_gradient)
        *ins_gradient = vehicle_rt_sb_ins_heading_gradient_2_ins_gradient_decode(msg.ins_gradient);

    return ret;
}

uint8_t vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_heading_gradient_2_validity_ins_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_2_validity_ins_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_2_ins_heading_2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_2_accuracy_ins_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_2_ins_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_ins_heading_gradient_check_ranges(struct vehicle_rt_sb_ins_heading_gradient_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_is_in_range(msg->validity_ins_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_validity_ins_gradient_is_in_range(msg->validity_ins_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_accuracy_ins_heading_is_in_range(msg->accuracy_ins_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_ins_heading_is_in_range(msg->ins_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_accuracy_ins_gradient_is_in_range(msg->accuracy_ins_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_heading_gradient_ins_gradient_is_in_range(msg->ins_gradient))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_heading_gradient_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_ins_heading,
    double validity_ins_gradient,
    double accuracy_ins_heading,
    double ins_heading,
    double accuracy_ins_gradient,
    double ins_gradient)
{
    struct vehicle_rt_sb_ins_heading_gradient_t msg;

    msg.validity_ins_heading = vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_encode(validity_ins_heading);
    msg.validity_ins_gradient = vehicle_rt_sb_ins_heading_gradient_validity_ins_gradient_encode(validity_ins_gradient);
    msg.accuracy_ins_heading = vehicle_rt_sb_ins_heading_gradient_accuracy_ins_heading_encode(accuracy_ins_heading);
    msg.ins_heading = vehicle_rt_sb_ins_heading_gradient_ins_heading_encode(ins_heading);
    msg.accuracy_ins_gradient = vehicle_rt_sb_ins_heading_gradient_accuracy_ins_gradient_encode(accuracy_ins_gradient);
    msg.ins_gradient = vehicle_rt_sb_ins_heading_gradient_ins_gradient_encode(ins_gradient);

    int ret = vehicle_rt_sb_ins_heading_gradient_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_heading_gradient_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_heading_gradient_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_ins_heading,
    double *validity_ins_gradient,
    double *accuracy_ins_heading,
    double *ins_heading,
    double *accuracy_ins_gradient,
    double *ins_gradient)
{
    struct vehicle_rt_sb_ins_heading_gradient_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_heading_gradient_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_heading_gradient_check_ranges(&msg);

    if (validity_ins_heading)
        *validity_ins_heading = vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_decode(msg.validity_ins_heading);

    if (validity_ins_gradient)
        *validity_ins_gradient = vehicle_rt_sb_ins_heading_gradient_validity_ins_gradient_decode(msg.validity_ins_gradient);

    if (accuracy_ins_heading)
        *accuracy_ins_heading = vehicle_rt_sb_ins_heading_gradient_accuracy_ins_heading_decode(msg.accuracy_ins_heading);

    if (ins_heading)
        *ins_heading = vehicle_rt_sb_ins_heading_gradient_ins_heading_decode(msg.ins_heading);

    if (accuracy_ins_gradient)
        *accuracy_ins_gradient = vehicle_rt_sb_ins_heading_gradient_accuracy_ins_gradient_decode(msg.accuracy_ins_gradient);

    if (ins_gradient)
        *ins_gradient = vehicle_rt_sb_ins_heading_gradient_ins_gradient_decode(msg.ins_gradient);

    return ret;
}

uint8_t vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_heading_gradient_validity_ins_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_validity_ins_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_accuracy_ins_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_ins_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_accuracy_ins_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_heading_gradient_ins_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_ins_status_check_ranges(struct vehicle_rt_sb_ins_status_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_status_ins_status_is_in_range(msg->ins_status))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_status_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double ins_status)
{
    struct vehicle_rt_sb_ins_status_t msg;

    msg.ins_status = vehicle_rt_sb_ins_status_ins_status_encode(ins_status);

    int ret = vehicle_rt_sb_ins_status_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_status_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_status_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *ins_status)
{
    struct vehicle_rt_sb_ins_status_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_status_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_status_check_ranges(&msg);

    if (ins_status)
        *ins_status = vehicle_rt_sb_ins_status_ins_status_decode(msg.ins_status);

    return ret;
}

uint8_t vehicle_rt_sb_ins_status_ins_status_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_status_ins_status_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_status_ins_status_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

static int vehicle_rt_sb_ins_attitude_check_ranges(struct vehicle_rt_sb_ins_attitude_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_ins_attitude_validity_yaw_is_in_range(msg->validity_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_attitude_validity_pitch_is_in_range(msg->validity_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_attitude_validity_roll_is_in_range(msg->validity_roll))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_attitude_accuracy_attitude_is_in_range(msg->accuracy_attitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_attitude_attitude_yaw_is_in_range(msg->attitude_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_attitude_attitude_pitch_is_in_range(msg->attitude_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_sb_ins_attitude_attitude_roll_is_in_range(msg->attitude_roll))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_ins_attitude_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_yaw,
    double validity_pitch,
    double validity_roll,
    double accuracy_attitude,
    double attitude_yaw,
    double attitude_pitch,
    double attitude_roll)
{
    struct vehicle_rt_sb_ins_attitude_t msg;

    msg.validity_yaw = vehicle_rt_sb_ins_attitude_validity_yaw_encode(validity_yaw);
    msg.validity_pitch = vehicle_rt_sb_ins_attitude_validity_pitch_encode(validity_pitch);
    msg.validity_roll = vehicle_rt_sb_ins_attitude_validity_roll_encode(validity_roll);
    msg.accuracy_attitude = vehicle_rt_sb_ins_attitude_accuracy_attitude_encode(accuracy_attitude);
    msg.attitude_yaw = vehicle_rt_sb_ins_attitude_attitude_yaw_encode(attitude_yaw);
    msg.attitude_pitch = vehicle_rt_sb_ins_attitude_attitude_pitch_encode(attitude_pitch);
    msg.attitude_roll = vehicle_rt_sb_ins_attitude_attitude_roll_encode(attitude_roll);

    int ret = vehicle_rt_sb_ins_attitude_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_ins_attitude_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_ins_attitude_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_yaw,
    double *validity_pitch,
    double *validity_roll,
    double *accuracy_attitude,
    double *attitude_yaw,
    double *attitude_pitch,
    double *attitude_roll)
{
    struct vehicle_rt_sb_ins_attitude_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_ins_attitude_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_ins_attitude_check_ranges(&msg);

    if (validity_yaw)
        *validity_yaw = vehicle_rt_sb_ins_attitude_validity_yaw_decode(msg.validity_yaw);

    if (validity_pitch)
        *validity_pitch = vehicle_rt_sb_ins_attitude_validity_pitch_decode(msg.validity_pitch);

    if (validity_roll)
        *validity_roll = vehicle_rt_sb_ins_attitude_validity_roll_decode(msg.validity_roll);

    if (accuracy_attitude)
        *accuracy_attitude = vehicle_rt_sb_ins_attitude_accuracy_attitude_decode(msg.accuracy_attitude);

    if (attitude_yaw)
        *attitude_yaw = vehicle_rt_sb_ins_attitude_attitude_yaw_decode(msg.attitude_yaw);

    if (attitude_pitch)
        *attitude_pitch = vehicle_rt_sb_ins_attitude_attitude_pitch_decode(msg.attitude_pitch);

    if (attitude_roll)
        *attitude_roll = vehicle_rt_sb_ins_attitude_attitude_roll_decode(msg.attitude_roll);

    return ret;
}

uint8_t vehicle_rt_sb_ins_attitude_validity_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_ins_attitude_validity_yaw_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_ins_attitude_validity_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_attitude_validity_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_attitude_validity_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_ins_attitude_accuracy_attitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_ins_attitude_attitude_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_ins_attitude_attitude_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_ins_attitude_attitude_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

static int vehicle_rt_sb_output_status_check_ranges(struct vehicle_rt_sb_output_status_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_output_status_validity_status_timestamp_is_in_range(msg->validity_status_timestamp))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_analogue_1_is_in_range(msg->status_analogue_1))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_analogue_2_is_in_range(msg->status_analogue_2))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_analogue_3_is_in_range(msg->status_analogue_3))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_analogue_4_is_in_range(msg->status_analogue_4))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_pulse_output_is_in_range(msg->status_pulse_output))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_serial_output_1_is_in_range(msg->status_serial_output_1))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_serial_output_2_is_in_range(msg->status_serial_output_2))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_status_trigger_is_in_range(msg->status_trigger))
        return idx;

    idx++;

    if (!vehicle_rt_sb_output_status_gps_time_is_in_range(msg->gps_time))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_output_status_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_status_timestamp,
    double status_analogue_1,
    double status_analogue_2,
    double status_analogue_3,
    double status_analogue_4,
    double status_pulse_output,
    double status_serial_output_1,
    double status_serial_output_2,
    double status_trigger,
    double gps_time)
{
    struct vehicle_rt_sb_output_status_t msg;

    msg.validity_status_timestamp = vehicle_rt_sb_output_status_validity_status_timestamp_encode(validity_status_timestamp);
    msg.status_analogue_1 = vehicle_rt_sb_output_status_status_analogue_1_encode(status_analogue_1);
    msg.status_analogue_2 = vehicle_rt_sb_output_status_status_analogue_2_encode(status_analogue_2);
    msg.status_analogue_3 = vehicle_rt_sb_output_status_status_analogue_3_encode(status_analogue_3);
    msg.status_analogue_4 = vehicle_rt_sb_output_status_status_analogue_4_encode(status_analogue_4);
    msg.status_pulse_output = vehicle_rt_sb_output_status_status_pulse_output_encode(status_pulse_output);
    msg.status_serial_output_1 = vehicle_rt_sb_output_status_status_serial_output_1_encode(status_serial_output_1);
    msg.status_serial_output_2 = vehicle_rt_sb_output_status_status_serial_output_2_encode(status_serial_output_2);
    msg.status_trigger = vehicle_rt_sb_output_status_status_trigger_encode(status_trigger);
    msg.gps_time = vehicle_rt_sb_output_status_gps_time_encode(gps_time);

    int ret = vehicle_rt_sb_output_status_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_output_status_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_output_status_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_status_timestamp,
    double *status_analogue_1,
    double *status_analogue_2,
    double *status_analogue_3,
    double *status_analogue_4,
    double *status_pulse_output,
    double *status_serial_output_1,
    double *status_serial_output_2,
    double *status_trigger,
    double *gps_time)
{
    struct vehicle_rt_sb_output_status_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_output_status_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_output_status_check_ranges(&msg);

    if (validity_status_timestamp)
        *validity_status_timestamp = vehicle_rt_sb_output_status_validity_status_timestamp_decode(msg.validity_status_timestamp);

    if (status_analogue_1)
        *status_analogue_1 = vehicle_rt_sb_output_status_status_analogue_1_decode(msg.status_analogue_1);

    if (status_analogue_2)
        *status_analogue_2 = vehicle_rt_sb_output_status_status_analogue_2_decode(msg.status_analogue_2);

    if (status_analogue_3)
        *status_analogue_3 = vehicle_rt_sb_output_status_status_analogue_3_decode(msg.status_analogue_3);

    if (status_analogue_4)
        *status_analogue_4 = vehicle_rt_sb_output_status_status_analogue_4_decode(msg.status_analogue_4);

    if (status_pulse_output)
        *status_pulse_output = vehicle_rt_sb_output_status_status_pulse_output_decode(msg.status_pulse_output);

    if (status_serial_output_1)
        *status_serial_output_1 = vehicle_rt_sb_output_status_status_serial_output_1_decode(msg.status_serial_output_1);

    if (status_serial_output_2)
        *status_serial_output_2 = vehicle_rt_sb_output_status_status_serial_output_2_decode(msg.status_serial_output_2);

    if (status_trigger)
        *status_trigger = vehicle_rt_sb_output_status_status_trigger_decode(msg.status_trigger);

    if (gps_time)
        *gps_time = vehicle_rt_sb_output_status_gps_time_decode(msg.gps_time);

    return ret;
}

uint8_t vehicle_rt_sb_output_status_validity_status_timestamp_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_output_status_validity_status_timestamp_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_output_status_validity_status_timestamp_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_output_status_status_analogue_1_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_status_analogue_2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_status_analogue_3_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_status_analogue_4_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_status_pulse_output_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_status_serial_output_1_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_status_serial_output_2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_status_trigger_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_output_status_gps_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 604800.0);
    return ret;
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

static int vehicle_rt_sb_gps_heading_gradient_2_check_ranges(struct vehicle_rt_sb_gps_heading_gradient_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_is_in_range(msg->validity_gps_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_2_validity_gps_gradient_is_in_range(msg->validity_gps_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_heading_is_in_range(msg->accuracy_gps_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_2_gps_heading_2_is_in_range(msg->gps_heading_2))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_gradient_is_in_range(msg->accuracy_gps_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_2_gps_gradient_is_in_range(msg->gps_gradient))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_heading_gradient_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_heading,
    double validity_gps_gradient,
    double accuracy_gps_heading,
    double gps_heading_2,
    double accuracy_gps_gradient,
    double gps_gradient)
{
    struct vehicle_rt_sb_gps_heading_gradient_2_t msg;

    msg.validity_gps_heading = vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_encode(validity_gps_heading);
    msg.validity_gps_gradient = vehicle_rt_sb_gps_heading_gradient_2_validity_gps_gradient_encode(validity_gps_gradient);
    msg.accuracy_gps_heading = vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_heading_encode(accuracy_gps_heading);
    msg.gps_heading_2 = vehicle_rt_sb_gps_heading_gradient_2_gps_heading_2_encode(gps_heading_2);
    msg.accuracy_gps_gradient = vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_gradient_encode(accuracy_gps_gradient);
    msg.gps_gradient = vehicle_rt_sb_gps_heading_gradient_2_gps_gradient_encode(gps_gradient);

    int ret = vehicle_rt_sb_gps_heading_gradient_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_heading_gradient_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_heading_gradient_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_heading,
    double *validity_gps_gradient,
    double *accuracy_gps_heading,
    double *gps_heading_2,
    double *accuracy_gps_gradient,
    double *gps_gradient)
{
    struct vehicle_rt_sb_gps_heading_gradient_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_heading_gradient_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_heading_gradient_2_check_ranges(&msg);

    if (validity_gps_heading)
        *validity_gps_heading = vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_decode(msg.validity_gps_heading);

    if (validity_gps_gradient)
        *validity_gps_gradient = vehicle_rt_sb_gps_heading_gradient_2_validity_gps_gradient_decode(msg.validity_gps_gradient);

    if (accuracy_gps_heading)
        *accuracy_gps_heading = vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_heading_decode(msg.accuracy_gps_heading);

    if (gps_heading_2)
        *gps_heading_2 = vehicle_rt_sb_gps_heading_gradient_2_gps_heading_2_decode(msg.gps_heading_2);

    if (accuracy_gps_gradient)
        *accuracy_gps_gradient = vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_gradient_decode(msg.accuracy_gps_gradient);

    if (gps_gradient)
        *gps_gradient = vehicle_rt_sb_gps_heading_gradient_2_gps_gradient_decode(msg.gps_gradient);

    return ret;
}

uint8_t vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_heading_gradient_2_validity_gps_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_2_validity_gps_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_2_gps_heading_2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_2_accuracy_gps_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_2_gps_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_cumulative_distance_2_check_ranges(struct vehicle_rt_sb_cumulative_distance_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_is_in_range(msg->validity_cumulative_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_cumulative_distance_2_validity_cumulative_distance_is_in_range(msg->validity_cumulative_distance))
        return idx;

    idx++;

    if (!vehicle_rt_sb_cumulative_distance_2_cumulative_time_is_in_range(msg->cumulative_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_cumulative_distance_2_cumulative_distance_is_in_range(msg->cumulative_distance))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_cumulative_distance_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_cumulative_time,
    double validity_cumulative_distance,
    double cumulative_time,
    double cumulative_distance)
{
    struct vehicle_rt_sb_cumulative_distance_2_t msg;

    msg.validity_cumulative_time = vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_encode(validity_cumulative_time);
    msg.validity_cumulative_distance = vehicle_rt_sb_cumulative_distance_2_validity_cumulative_distance_encode(validity_cumulative_distance);
    msg.cumulative_time = vehicle_rt_sb_cumulative_distance_2_cumulative_time_encode(cumulative_time);
    msg.cumulative_distance = vehicle_rt_sb_cumulative_distance_2_cumulative_distance_encode(cumulative_distance);

    int ret = vehicle_rt_sb_cumulative_distance_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_cumulative_distance_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_cumulative_distance_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_cumulative_time,
    double *validity_cumulative_distance,
    double *cumulative_time,
    double *cumulative_distance)
{
    struct vehicle_rt_sb_cumulative_distance_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_cumulative_distance_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_cumulative_distance_2_check_ranges(&msg);

    if (validity_cumulative_time)
        *validity_cumulative_time = vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_decode(msg.validity_cumulative_time);

    if (validity_cumulative_distance)
        *validity_cumulative_distance = vehicle_rt_sb_cumulative_distance_2_validity_cumulative_distance_decode(msg.validity_cumulative_distance);

    if (cumulative_time)
        *cumulative_time = vehicle_rt_sb_cumulative_distance_2_cumulative_time_decode(msg.cumulative_time);

    if (cumulative_distance)
        *cumulative_distance = vehicle_rt_sb_cumulative_distance_2_cumulative_distance_decode(msg.cumulative_distance);

    return ret;
}

uint8_t vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_cumulative_distance_2_validity_cumulative_time_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_cumulative_distance_2_validity_cumulative_distance_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_cumulative_distance_2_cumulative_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 167772.0);
    return ret;
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

double vehicle_rt_sb_cumulative_distance_2_cumulative_distance_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 4294967.0);
    return ret;
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

static int vehicle_rt_sb_cumulative_distance_1_check_ranges(struct vehicle_rt_sb_cumulative_distance_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_is_in_range(msg->validity_cumulative_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_cumulative_distance_1_validity_cumulative_distance_is_in_range(msg->validity_cumulative_distance))
        return idx;

    idx++;

    if (!vehicle_rt_sb_cumulative_distance_1_cumulative_time_is_in_range(msg->cumulative_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_cumulative_distance_1_cumulative_distance_is_in_range(msg->cumulative_distance))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_cumulative_distance_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_cumulative_time,
    double validity_cumulative_distance,
    double cumulative_time,
    double cumulative_distance)
{
    struct vehicle_rt_sb_cumulative_distance_1_t msg;

    msg.validity_cumulative_time = vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_encode(validity_cumulative_time);
    msg.validity_cumulative_distance = vehicle_rt_sb_cumulative_distance_1_validity_cumulative_distance_encode(validity_cumulative_distance);
    msg.cumulative_time = vehicle_rt_sb_cumulative_distance_1_cumulative_time_encode(cumulative_time);
    msg.cumulative_distance = vehicle_rt_sb_cumulative_distance_1_cumulative_distance_encode(cumulative_distance);

    int ret = vehicle_rt_sb_cumulative_distance_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_cumulative_distance_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_cumulative_distance_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_cumulative_time,
    double *validity_cumulative_distance,
    double *cumulative_time,
    double *cumulative_distance)
{
    struct vehicle_rt_sb_cumulative_distance_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_cumulative_distance_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_cumulative_distance_1_check_ranges(&msg);

    if (validity_cumulative_time)
        *validity_cumulative_time = vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_decode(msg.validity_cumulative_time);

    if (validity_cumulative_distance)
        *validity_cumulative_distance = vehicle_rt_sb_cumulative_distance_1_validity_cumulative_distance_decode(msg.validity_cumulative_distance);

    if (cumulative_time)
        *cumulative_time = vehicle_rt_sb_cumulative_distance_1_cumulative_time_decode(msg.cumulative_time);

    if (cumulative_distance)
        *cumulative_distance = vehicle_rt_sb_cumulative_distance_1_cumulative_distance_decode(msg.cumulative_distance);

    return ret;
}

uint8_t vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_cumulative_distance_1_validity_cumulative_time_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_cumulative_distance_1_validity_cumulative_distance_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_cumulative_distance_1_cumulative_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 167772.0);
    return ret;
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

double vehicle_rt_sb_cumulative_distance_1_cumulative_distance_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 4294967.0);
    return ret;
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

static int vehicle_rt_sb_trigger_timestamp_check_ranges(struct vehicle_rt_sb_trigger_timestamp_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_is_in_range(msg->validity_trigger_timestamp))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trigger_timestamp_accuracy_trigger_timestamp_is_in_range(msg->accuracy_trigger_timestamp))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trigger_timestamp_trigger_number_is_in_range(msg->trigger_number))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trigger_timestamp_trigger_timestamp_type_is_in_range(msg->trigger_timestamp_type))
        return idx;

    idx++;

    if (!vehicle_rt_sb_trigger_timestamp_gps_high_resolution_time_is_in_range(msg->gps_high_resolution_time))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_trigger_timestamp_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_trigger_timestamp,
    double accuracy_trigger_timestamp,
    double trigger_number,
    double trigger_timestamp_type,
    double gps_high_resolution_time)
{
    struct vehicle_rt_sb_trigger_timestamp_t msg;

    msg.validity_trigger_timestamp = vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_encode(validity_trigger_timestamp);
    msg.accuracy_trigger_timestamp = vehicle_rt_sb_trigger_timestamp_accuracy_trigger_timestamp_encode(accuracy_trigger_timestamp);
    msg.trigger_number = vehicle_rt_sb_trigger_timestamp_trigger_number_encode(trigger_number);
    msg.trigger_timestamp_type = vehicle_rt_sb_trigger_timestamp_trigger_timestamp_type_encode(trigger_timestamp_type);
    msg.gps_high_resolution_time = vehicle_rt_sb_trigger_timestamp_gps_high_resolution_time_encode(gps_high_resolution_time);

    int ret = vehicle_rt_sb_trigger_timestamp_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_trigger_timestamp_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_trigger_timestamp_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_trigger_timestamp,
    double *accuracy_trigger_timestamp,
    double *trigger_number,
    double *trigger_timestamp_type,
    double *gps_high_resolution_time)
{
    struct vehicle_rt_sb_trigger_timestamp_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_trigger_timestamp_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_trigger_timestamp_check_ranges(&msg);

    if (validity_trigger_timestamp)
        *validity_trigger_timestamp = vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_decode(msg.validity_trigger_timestamp);

    if (accuracy_trigger_timestamp)
        *accuracy_trigger_timestamp = vehicle_rt_sb_trigger_timestamp_accuracy_trigger_timestamp_decode(msg.accuracy_trigger_timestamp);

    if (trigger_number)
        *trigger_number = vehicle_rt_sb_trigger_timestamp_trigger_number_decode(msg.trigger_number);

    if (trigger_timestamp_type)
        *trigger_timestamp_type = vehicle_rt_sb_trigger_timestamp_trigger_timestamp_type_decode(msg.trigger_timestamp_type);

    if (gps_high_resolution_time)
        *gps_high_resolution_time = vehicle_rt_sb_trigger_timestamp_gps_high_resolution_time_decode(msg.gps_high_resolution_time);

    return ret;
}

uint8_t vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_trigger_timestamp_validity_trigger_timestamp_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_trigger_timestamp_accuracy_trigger_timestamp_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_trigger_timestamp_trigger_number_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 128.0);
    return ret;
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

double vehicle_rt_sb_trigger_timestamp_trigger_timestamp_type_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_trigger_timestamp_gps_high_resolution_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 604800.0);
    return ret;
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

static int vehicle_rt_imu06_gyro_rates_check_ranges(struct vehicle_rt_imu06_gyro_rates_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_is_in_range(msg->validity_gyro_rate_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_gyro_rates_validity_gyro_rate_pitch_is_in_range(msg->validity_gyro_rate_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_gyro_rates_validity_gyro_rate_roll_is_in_range(msg->validity_gyro_rate_roll))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_gyro_rates_accuracy_gyro_rates_is_in_range(msg->accuracy_gyro_rates))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_gyro_rates_gyro_rate_yaw_is_in_range(msg->gyro_rate_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_gyro_rates_gyro_rate_pitch_is_in_range(msg->gyro_rate_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_gyro_rates_gyro_rate_roll_is_in_range(msg->gyro_rate_roll))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_imu06_gyro_rates_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gyro_rate_yaw,
    double validity_gyro_rate_pitch,
    double validity_gyro_rate_roll,
    double accuracy_gyro_rates,
    double gyro_rate_yaw,
    double gyro_rate_pitch,
    double gyro_rate_roll)
{
    struct vehicle_rt_imu06_gyro_rates_t msg;

    msg.validity_gyro_rate_yaw = vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_encode(validity_gyro_rate_yaw);
    msg.validity_gyro_rate_pitch = vehicle_rt_imu06_gyro_rates_validity_gyro_rate_pitch_encode(validity_gyro_rate_pitch);
    msg.validity_gyro_rate_roll = vehicle_rt_imu06_gyro_rates_validity_gyro_rate_roll_encode(validity_gyro_rate_roll);
    msg.accuracy_gyro_rates = vehicle_rt_imu06_gyro_rates_accuracy_gyro_rates_encode(accuracy_gyro_rates);
    msg.gyro_rate_yaw = vehicle_rt_imu06_gyro_rates_gyro_rate_yaw_encode(gyro_rate_yaw);
    msg.gyro_rate_pitch = vehicle_rt_imu06_gyro_rates_gyro_rate_pitch_encode(gyro_rate_pitch);
    msg.gyro_rate_roll = vehicle_rt_imu06_gyro_rates_gyro_rate_roll_encode(gyro_rate_roll);

    int ret = vehicle_rt_imu06_gyro_rates_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_imu06_gyro_rates_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_imu06_gyro_rates_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gyro_rate_yaw,
    double *validity_gyro_rate_pitch,
    double *validity_gyro_rate_roll,
    double *accuracy_gyro_rates,
    double *gyro_rate_yaw,
    double *gyro_rate_pitch,
    double *gyro_rate_roll)
{
    struct vehicle_rt_imu06_gyro_rates_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_imu06_gyro_rates_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_imu06_gyro_rates_check_ranges(&msg);

    if (validity_gyro_rate_yaw)
        *validity_gyro_rate_yaw = vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_decode(msg.validity_gyro_rate_yaw);

    if (validity_gyro_rate_pitch)
        *validity_gyro_rate_pitch = vehicle_rt_imu06_gyro_rates_validity_gyro_rate_pitch_decode(msg.validity_gyro_rate_pitch);

    if (validity_gyro_rate_roll)
        *validity_gyro_rate_roll = vehicle_rt_imu06_gyro_rates_validity_gyro_rate_roll_decode(msg.validity_gyro_rate_roll);

    if (accuracy_gyro_rates)
        *accuracy_gyro_rates = vehicle_rt_imu06_gyro_rates_accuracy_gyro_rates_decode(msg.accuracy_gyro_rates);

    if (gyro_rate_yaw)
        *gyro_rate_yaw = vehicle_rt_imu06_gyro_rates_gyro_rate_yaw_decode(msg.gyro_rate_yaw);

    if (gyro_rate_pitch)
        *gyro_rate_pitch = vehicle_rt_imu06_gyro_rates_gyro_rate_pitch_decode(msg.gyro_rate_pitch);

    if (gyro_rate_roll)
        *gyro_rate_roll = vehicle_rt_imu06_gyro_rates_gyro_rate_roll_decode(msg.gyro_rate_roll);

    return ret;
}

uint8_t vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_imu06_gyro_rates_validity_gyro_rate_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_imu06_gyro_rates_validity_gyro_rate_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_imu06_gyro_rates_validity_gyro_rate_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_imu06_gyro_rates_accuracy_gyro_rates_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_imu06_gyro_rates_gyro_rate_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -327.0);
    ret = CTOOLS_MIN(ret, 327.0);
    return ret;
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

double vehicle_rt_imu06_gyro_rates_gyro_rate_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -327.0);
    ret = CTOOLS_MIN(ret, 327.0);
    return ret;
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

double vehicle_rt_imu06_gyro_rates_gyro_rate_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -327.0);
    ret = CTOOLS_MIN(ret, 327.0);
    return ret;
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

static int vehicle_rt_imu06_accel_check_ranges(struct vehicle_rt_imu06_accel_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_imu06_accel_validity_accel_longitudinal_is_in_range(msg->validity_accel_longitudinal))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_accel_validity_accel_lateral_is_in_range(msg->validity_accel_lateral))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_accel_validity_accel_vertical_is_in_range(msg->validity_accel_vertical))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_accel_accuracy_accel_is_in_range(msg->accuracy_accel))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_accel_accel_longitudinal_is_in_range(msg->accel_longitudinal))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_accel_accel_lateral_is_in_range(msg->accel_lateral))
        return idx;

    idx++;

    if (!vehicle_rt_imu06_accel_accel_vertical_is_in_range(msg->accel_vertical))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_imu06_accel_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_accel_longitudinal,
    double validity_accel_lateral,
    double validity_accel_vertical,
    double accuracy_accel,
    double accel_longitudinal,
    double accel_lateral,
    double accel_vertical)
{
    struct vehicle_rt_imu06_accel_t msg;

    msg.validity_accel_longitudinal = vehicle_rt_imu06_accel_validity_accel_longitudinal_encode(validity_accel_longitudinal);
    msg.validity_accel_lateral = vehicle_rt_imu06_accel_validity_accel_lateral_encode(validity_accel_lateral);
    msg.validity_accel_vertical = vehicle_rt_imu06_accel_validity_accel_vertical_encode(validity_accel_vertical);
    msg.accuracy_accel = vehicle_rt_imu06_accel_accuracy_accel_encode(accuracy_accel);
    msg.accel_longitudinal = vehicle_rt_imu06_accel_accel_longitudinal_encode(accel_longitudinal);
    msg.accel_lateral = vehicle_rt_imu06_accel_accel_lateral_encode(accel_lateral);
    msg.accel_vertical = vehicle_rt_imu06_accel_accel_vertical_encode(accel_vertical);

    int ret = vehicle_rt_imu06_accel_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_imu06_accel_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_imu06_accel_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_accel_longitudinal,
    double *validity_accel_lateral,
    double *validity_accel_vertical,
    double *accuracy_accel,
    double *accel_longitudinal,
    double *accel_lateral,
    double *accel_vertical)
{
    struct vehicle_rt_imu06_accel_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_imu06_accel_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_imu06_accel_check_ranges(&msg);

    if (validity_accel_longitudinal)
        *validity_accel_longitudinal = vehicle_rt_imu06_accel_validity_accel_longitudinal_decode(msg.validity_accel_longitudinal);

    if (validity_accel_lateral)
        *validity_accel_lateral = vehicle_rt_imu06_accel_validity_accel_lateral_decode(msg.validity_accel_lateral);

    if (validity_accel_vertical)
        *validity_accel_vertical = vehicle_rt_imu06_accel_validity_accel_vertical_decode(msg.validity_accel_vertical);

    if (accuracy_accel)
        *accuracy_accel = vehicle_rt_imu06_accel_accuracy_accel_decode(msg.accuracy_accel);

    if (accel_longitudinal)
        *accel_longitudinal = vehicle_rt_imu06_accel_accel_longitudinal_decode(msg.accel_longitudinal);

    if (accel_lateral)
        *accel_lateral = vehicle_rt_imu06_accel_accel_lateral_decode(msg.accel_lateral);

    if (accel_vertical)
        *accel_vertical = vehicle_rt_imu06_accel_accel_vertical_decode(msg.accel_vertical);

    return ret;
}

uint8_t vehicle_rt_imu06_accel_validity_accel_longitudinal_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_imu06_accel_validity_accel_longitudinal_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_imu06_accel_validity_accel_longitudinal_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_imu06_accel_validity_accel_lateral_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_imu06_accel_validity_accel_vertical_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_imu06_accel_accuracy_accel_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_imu06_accel_accel_longitudinal_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_imu06_accel_accel_lateral_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_imu06_accel_accel_vertical_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

static int vehicle_rt_sb_speed_check_ranges(struct vehicle_rt_sb_speed_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_speed_validity_speed_is_in_range(msg->validity_speed))
        return idx;

    idx++;

    if (!vehicle_rt_sb_speed_accuracy_speed_is_in_range(msg->accuracy_speed))
        return idx;

    idx++;

    if (!vehicle_rt_sb_speed_speed_is_in_range(msg->speed))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_speed_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_speed,
    double accuracy_speed,
    double speed)
{
    struct vehicle_rt_sb_speed_t msg;

    msg.validity_speed = vehicle_rt_sb_speed_validity_speed_encode(validity_speed);
    msg.accuracy_speed = vehicle_rt_sb_speed_accuracy_speed_encode(accuracy_speed);
    msg.speed = vehicle_rt_sb_speed_speed_encode(speed);

    int ret = vehicle_rt_sb_speed_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_speed_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_speed_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_speed,
    double *accuracy_speed,
    double *speed)
{
    struct vehicle_rt_sb_speed_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_speed_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_speed_check_ranges(&msg);

    if (validity_speed)
        *validity_speed = vehicle_rt_sb_speed_validity_speed_decode(msg.validity_speed);

    if (accuracy_speed)
        *accuracy_speed = vehicle_rt_sb_speed_accuracy_speed_decode(msg.accuracy_speed);

    if (speed)
        *speed = vehicle_rt_sb_speed_speed_decode(msg.speed);

    return ret;
}

uint8_t vehicle_rt_sb_speed_validity_speed_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_speed_validity_speed_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_speed_validity_speed_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_speed_accuracy_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_speed_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -20000.0);
    ret = CTOOLS_MIN(ret, 20000.0);
    return ret;
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

static int vehicle_rt_sb_rtk_slip_check_ranges(struct vehicle_rt_sb_rtk_slip_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_rtk_slip_validity_rtk_slip_is_in_range(msg->validity_rtk_slip))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_slip_validity_rtk_squat_is_in_range(msg->validity_rtk_squat))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_slip_validity_rtk_baseline_is_in_range(msg->validity_rtk_baseline))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_slip_accuracy_rtk_baseline_is_in_range(msg->accuracy_rtk_baseline))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_slip_rtk_slip_is_in_range(msg->rtk_slip))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_slip_rtk_squat_is_in_range(msg->rtk_squat))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_slip_rtk_baseline_is_in_range(msg->rtk_baseline))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_rtk_slip_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_rtk_slip,
    double validity_rtk_squat,
    double validity_rtk_baseline,
    double accuracy_rtk_baseline,
    double rtk_slip,
    double rtk_squat,
    double rtk_baseline)
{
    struct vehicle_rt_sb_rtk_slip_t msg;

    msg.validity_rtk_slip = vehicle_rt_sb_rtk_slip_validity_rtk_slip_encode(validity_rtk_slip);
    msg.validity_rtk_squat = vehicle_rt_sb_rtk_slip_validity_rtk_squat_encode(validity_rtk_squat);
    msg.validity_rtk_baseline = vehicle_rt_sb_rtk_slip_validity_rtk_baseline_encode(validity_rtk_baseline);
    msg.accuracy_rtk_baseline = vehicle_rt_sb_rtk_slip_accuracy_rtk_baseline_encode(accuracy_rtk_baseline);
    msg.rtk_slip = vehicle_rt_sb_rtk_slip_rtk_slip_encode(rtk_slip);
    msg.rtk_squat = vehicle_rt_sb_rtk_slip_rtk_squat_encode(rtk_squat);
    msg.rtk_baseline = vehicle_rt_sb_rtk_slip_rtk_baseline_encode(rtk_baseline);

    int ret = vehicle_rt_sb_rtk_slip_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_rtk_slip_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_rtk_slip_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_rtk_slip,
    double *validity_rtk_squat,
    double *validity_rtk_baseline,
    double *accuracy_rtk_baseline,
    double *rtk_slip,
    double *rtk_squat,
    double *rtk_baseline)
{
    struct vehicle_rt_sb_rtk_slip_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_rtk_slip_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_rtk_slip_check_ranges(&msg);

    if (validity_rtk_slip)
        *validity_rtk_slip = vehicle_rt_sb_rtk_slip_validity_rtk_slip_decode(msg.validity_rtk_slip);

    if (validity_rtk_squat)
        *validity_rtk_squat = vehicle_rt_sb_rtk_slip_validity_rtk_squat_decode(msg.validity_rtk_squat);

    if (validity_rtk_baseline)
        *validity_rtk_baseline = vehicle_rt_sb_rtk_slip_validity_rtk_baseline_decode(msg.validity_rtk_baseline);

    if (accuracy_rtk_baseline)
        *accuracy_rtk_baseline = vehicle_rt_sb_rtk_slip_accuracy_rtk_baseline_decode(msg.accuracy_rtk_baseline);

    if (rtk_slip)
        *rtk_slip = vehicle_rt_sb_rtk_slip_rtk_slip_decode(msg.rtk_slip);

    if (rtk_squat)
        *rtk_squat = vehicle_rt_sb_rtk_slip_rtk_squat_decode(msg.rtk_squat);

    if (rtk_baseline)
        *rtk_baseline = vehicle_rt_sb_rtk_slip_rtk_baseline_decode(msg.rtk_baseline);

    return ret;
}

uint8_t vehicle_rt_sb_rtk_slip_validity_rtk_slip_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_slip_validity_rtk_slip_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_rtk_slip_validity_rtk_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_rtk_slip_validity_rtk_squat_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_rtk_slip_validity_rtk_baseline_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_rtk_slip_accuracy_rtk_baseline_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_rtk_slip_rtk_slip_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_rtk_slip_rtk_squat_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_rtk_slip_rtk_baseline_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 65535.0);
    return ret;
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

static int vehicle_rt_sb_rtk_attitude_check_ranges(struct vehicle_rt_sb_rtk_attitude_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_is_in_range(msg->validity_rtk_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_attitude_validity_rtk_pitch_is_in_range(msg->validity_rtk_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_attitude_validity_rtk_roll_is_in_range(msg->validity_rtk_roll))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_attitude_accuracy_rtk_attitude_is_in_range(msg->accuracy_rtk_attitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_attitude_rtk_attitude_yaw_is_in_range(msg->rtk_attitude_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_attitude_rtk_attitude_pitch_is_in_range(msg->rtk_attitude_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_sb_rtk_attitude_rtk_attitude_roll_is_in_range(msg->rtk_attitude_roll))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_rtk_attitude_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_rtk_yaw,
    double validity_rtk_pitch,
    double validity_rtk_roll,
    double accuracy_rtk_attitude,
    double rtk_attitude_yaw,
    double rtk_attitude_pitch,
    double rtk_attitude_roll)
{
    struct vehicle_rt_sb_rtk_attitude_t msg;

    msg.validity_rtk_yaw = vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_encode(validity_rtk_yaw);
    msg.validity_rtk_pitch = vehicle_rt_sb_rtk_attitude_validity_rtk_pitch_encode(validity_rtk_pitch);
    msg.validity_rtk_roll = vehicle_rt_sb_rtk_attitude_validity_rtk_roll_encode(validity_rtk_roll);
    msg.accuracy_rtk_attitude = vehicle_rt_sb_rtk_attitude_accuracy_rtk_attitude_encode(accuracy_rtk_attitude);
    msg.rtk_attitude_yaw = vehicle_rt_sb_rtk_attitude_rtk_attitude_yaw_encode(rtk_attitude_yaw);
    msg.rtk_attitude_pitch = vehicle_rt_sb_rtk_attitude_rtk_attitude_pitch_encode(rtk_attitude_pitch);
    msg.rtk_attitude_roll = vehicle_rt_sb_rtk_attitude_rtk_attitude_roll_encode(rtk_attitude_roll);

    int ret = vehicle_rt_sb_rtk_attitude_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_rtk_attitude_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_rtk_attitude_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_rtk_yaw,
    double *validity_rtk_pitch,
    double *validity_rtk_roll,
    double *accuracy_rtk_attitude,
    double *rtk_attitude_yaw,
    double *rtk_attitude_pitch,
    double *rtk_attitude_roll)
{
    struct vehicle_rt_sb_rtk_attitude_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_rtk_attitude_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_rtk_attitude_check_ranges(&msg);

    if (validity_rtk_yaw)
        *validity_rtk_yaw = vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_decode(msg.validity_rtk_yaw);

    if (validity_rtk_pitch)
        *validity_rtk_pitch = vehicle_rt_sb_rtk_attitude_validity_rtk_pitch_decode(msg.validity_rtk_pitch);

    if (validity_rtk_roll)
        *validity_rtk_roll = vehicle_rt_sb_rtk_attitude_validity_rtk_roll_decode(msg.validity_rtk_roll);

    if (accuracy_rtk_attitude)
        *accuracy_rtk_attitude = vehicle_rt_sb_rtk_attitude_accuracy_rtk_attitude_decode(msg.accuracy_rtk_attitude);

    if (rtk_attitude_yaw)
        *rtk_attitude_yaw = vehicle_rt_sb_rtk_attitude_rtk_attitude_yaw_decode(msg.rtk_attitude_yaw);

    if (rtk_attitude_pitch)
        *rtk_attitude_pitch = vehicle_rt_sb_rtk_attitude_rtk_attitude_pitch_decode(msg.rtk_attitude_pitch);

    if (rtk_attitude_roll)
        *rtk_attitude_roll = vehicle_rt_sb_rtk_attitude_rtk_attitude_roll_decode(msg.rtk_attitude_roll);

    return ret;
}

uint8_t vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_rtk_attitude_validity_rtk_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_rtk_attitude_validity_rtk_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_rtk_attitude_validity_rtk_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_rtk_attitude_accuracy_rtk_attitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_rtk_attitude_rtk_attitude_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -360.0);
    ret = CTOOLS_MIN(ret, 360.0);
    return ret;
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

double vehicle_rt_sb_rtk_attitude_rtk_attitude_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

double vehicle_rt_sb_rtk_attitude_rtk_attitude_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_gps_mcycle_lean_check_ranges(struct vehicle_rt_sb_gps_mcycle_lean_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_is_in_range(msg->validity_gps_lateral_accel))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_mcycle_lean_validity_gps_mcycle_lean_is_in_range(msg->validity_gps_mcycle_lean))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_mcycle_lean_accuracy_gps_lateral_accel_is_in_range(msg->accuracy_gps_lateral_accel))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_mcycle_lean_gps_lateral_accel_is_in_range(msg->gps_lateral_accel))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_mcycle_lean_gps_mcycle_lean_angle_is_in_range(msg->gps_mcycle_lean_angle))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_mcycle_lean_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_lateral_accel,
    double validity_gps_mcycle_lean,
    double accuracy_gps_lateral_accel,
    double gps_lateral_accel,
    double gps_mcycle_lean_angle)
{
    struct vehicle_rt_sb_gps_mcycle_lean_t msg;

    msg.validity_gps_lateral_accel = vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_encode(validity_gps_lateral_accel);
    msg.validity_gps_mcycle_lean = vehicle_rt_sb_gps_mcycle_lean_validity_gps_mcycle_lean_encode(validity_gps_mcycle_lean);
    msg.accuracy_gps_lateral_accel = vehicle_rt_sb_gps_mcycle_lean_accuracy_gps_lateral_accel_encode(accuracy_gps_lateral_accel);
    msg.gps_lateral_accel = vehicle_rt_sb_gps_mcycle_lean_gps_lateral_accel_encode(gps_lateral_accel);
    msg.gps_mcycle_lean_angle = vehicle_rt_sb_gps_mcycle_lean_gps_mcycle_lean_angle_encode(gps_mcycle_lean_angle);

    int ret = vehicle_rt_sb_gps_mcycle_lean_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_mcycle_lean_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_mcycle_lean_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_lateral_accel,
    double *validity_gps_mcycle_lean,
    double *accuracy_gps_lateral_accel,
    double *gps_lateral_accel,
    double *gps_mcycle_lean_angle)
{
    struct vehicle_rt_sb_gps_mcycle_lean_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_mcycle_lean_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_mcycle_lean_check_ranges(&msg);

    if (validity_gps_lateral_accel)
        *validity_gps_lateral_accel = vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_decode(msg.validity_gps_lateral_accel);

    if (validity_gps_mcycle_lean)
        *validity_gps_mcycle_lean = vehicle_rt_sb_gps_mcycle_lean_validity_gps_mcycle_lean_decode(msg.validity_gps_mcycle_lean);

    if (accuracy_gps_lateral_accel)
        *accuracy_gps_lateral_accel = vehicle_rt_sb_gps_mcycle_lean_accuracy_gps_lateral_accel_decode(msg.accuracy_gps_lateral_accel);

    if (gps_lateral_accel)
        *gps_lateral_accel = vehicle_rt_sb_gps_mcycle_lean_gps_lateral_accel_decode(msg.gps_lateral_accel);

    if (gps_mcycle_lean_angle)
        *gps_mcycle_lean_angle = vehicle_rt_sb_gps_mcycle_lean_gps_mcycle_lean_angle_decode(msg.gps_mcycle_lean_angle);

    return ret;
}

uint8_t vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_mcycle_lean_validity_gps_lateral_accel_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_mcycle_lean_validity_gps_mcycle_lean_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_mcycle_lean_accuracy_gps_lateral_accel_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_mcycle_lean_gps_lateral_accel_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_sb_gps_mcycle_lean_gps_mcycle_lean_angle_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_gps_status_check_ranges(struct vehicle_rt_sb_gps_status_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_status_gps_status_is_in_range(msg->gps_status))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_status_firmware_version_major_is_in_range(msg->firmware_version_major))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_status_firmware_version_intermediate_is_in_range(msg->firmware_version_intermediate))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_status_firmware_version_minor_is_in_range(msg->firmware_version_minor))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_status_gps_n_sv_is_in_range(msg->gps_n_sv))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_status_gps_n_sv_2_is_in_range(msg->gps_n_sv_2))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_status_gps_n_sv_rtk_is_in_range(msg->gps_n_sv_rtk))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_status_rtk_status_is_in_range(msg->rtk_status))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_status_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double gps_status,
    double firmware_version_major,
    double firmware_version_intermediate,
    double firmware_version_minor,
    double gps_n_sv,
    double gps_n_sv_2,
    double gps_n_sv_rtk,
    double rtk_status)
{
    struct vehicle_rt_sb_gps_status_t msg;

    msg.gps_status = vehicle_rt_sb_gps_status_gps_status_encode(gps_status);
    msg.firmware_version_major = vehicle_rt_sb_gps_status_firmware_version_major_encode(firmware_version_major);
    msg.firmware_version_intermediate = vehicle_rt_sb_gps_status_firmware_version_intermediate_encode(firmware_version_intermediate);
    msg.firmware_version_minor = vehicle_rt_sb_gps_status_firmware_version_minor_encode(firmware_version_minor);
    msg.gps_n_sv = vehicle_rt_sb_gps_status_gps_n_sv_encode(gps_n_sv);
    msg.gps_n_sv_2 = vehicle_rt_sb_gps_status_gps_n_sv_2_encode(gps_n_sv_2);
    msg.gps_n_sv_rtk = vehicle_rt_sb_gps_status_gps_n_sv_rtk_encode(gps_n_sv_rtk);
    msg.rtk_status = vehicle_rt_sb_gps_status_rtk_status_encode(rtk_status);

    int ret = vehicle_rt_sb_gps_status_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_status_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_status_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *gps_status,
    double *firmware_version_major,
    double *firmware_version_intermediate,
    double *firmware_version_minor,
    double *gps_n_sv,
    double *gps_n_sv_2,
    double *gps_n_sv_rtk,
    double *rtk_status)
{
    struct vehicle_rt_sb_gps_status_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_status_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_status_check_ranges(&msg);

    if (gps_status)
        *gps_status = vehicle_rt_sb_gps_status_gps_status_decode(msg.gps_status);

    if (firmware_version_major)
        *firmware_version_major = vehicle_rt_sb_gps_status_firmware_version_major_decode(msg.firmware_version_major);

    if (firmware_version_intermediate)
        *firmware_version_intermediate = vehicle_rt_sb_gps_status_firmware_version_intermediate_decode(msg.firmware_version_intermediate);

    if (firmware_version_minor)
        *firmware_version_minor = vehicle_rt_sb_gps_status_firmware_version_minor_decode(msg.firmware_version_minor);

    if (gps_n_sv)
        *gps_n_sv = vehicle_rt_sb_gps_status_gps_n_sv_decode(msg.gps_n_sv);

    if (gps_n_sv_2)
        *gps_n_sv_2 = vehicle_rt_sb_gps_status_gps_n_sv_2_decode(msg.gps_n_sv_2);

    if (gps_n_sv_rtk)
        *gps_n_sv_rtk = vehicle_rt_sb_gps_status_gps_n_sv_rtk_decode(msg.gps_n_sv_rtk);

    if (rtk_status)
        *rtk_status = vehicle_rt_sb_gps_status_rtk_status_decode(msg.rtk_status);

    return ret;
}

uint8_t vehicle_rt_sb_gps_status_gps_status_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_status_gps_status_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_status_gps_status_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_status_firmware_version_major_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_status_firmware_version_intermediate_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_status_firmware_version_minor_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_status_gps_n_sv_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 16.0);
    return ret;
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

double vehicle_rt_sb_gps_status_gps_n_sv_2_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 16.0);
    return ret;
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

double vehicle_rt_sb_gps_status_gps_n_sv_rtk_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 16.0);
    return ret;
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

double vehicle_rt_sb_gps_status_rtk_status_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

static int vehicle_rt_sb_gps_pos_ecef_2_check_ranges(struct vehicle_rt_sb_gps_pos_ecef_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_is_in_range(msg->gps_pos_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_z_is_in_range(msg->gps_pos_ecef_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_pos_ecef_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double gps_pos_ecef_y,
    double gps_pos_ecef_z)
{
    struct vehicle_rt_sb_gps_pos_ecef_2_t msg;

    msg.gps_pos_ecef_y = vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_encode(gps_pos_ecef_y);
    msg.gps_pos_ecef_z = vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_z_encode(gps_pos_ecef_z);

    int ret = vehicle_rt_sb_gps_pos_ecef_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_pos_ecef_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_pos_ecef_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *gps_pos_ecef_y,
    double *gps_pos_ecef_z)
{
    struct vehicle_rt_sb_gps_pos_ecef_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_pos_ecef_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_pos_ecef_2_check_ranges(&msg);

    if (gps_pos_ecef_y)
        *gps_pos_ecef_y = vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_decode(msg.gps_pos_ecef_y);

    if (gps_pos_ecef_z)
        *gps_pos_ecef_z = vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_z_decode(msg.gps_pos_ecef_z);

    return ret;
}

int32_t vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_encode(double value)
{
    return (int32_t)(value / 0.01);
}

double vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_decode(int32_t value)
{
    return ((double)value * 0.01);
}

double vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -10000000.0);
    ret = CTOOLS_MIN(ret, 10000000.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_ecef_2_gps_pos_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -10000000.0);
    ret = CTOOLS_MIN(ret, 10000000.0);
    return ret;
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

static int vehicle_rt_sb_gps_pos_ecef_1_check_ranges(struct vehicle_rt_sb_gps_pos_ecef_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_is_in_range(msg->validity_gps_pos_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_y_is_in_range(msg->validity_gps_pos_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_z_is_in_range(msg->validity_gps_pos_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_x_is_in_range(msg->accuracy_gps_pos_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_y_is_in_range(msg->accuracy_gps_pos_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_z_is_in_range(msg->accuracy_gps_pos_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_ecef_1_gps_pos_ecef_x_is_in_range(msg->gps_pos_ecef_x))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_pos_ecef_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_pos_ecef_x,
    double validity_gps_pos_ecef_y,
    double validity_gps_pos_ecef_z,
    double accuracy_gps_pos_ecef_x,
    double accuracy_gps_pos_ecef_y,
    double accuracy_gps_pos_ecef_z,
    double gps_pos_ecef_x)
{
    struct vehicle_rt_sb_gps_pos_ecef_1_t msg;

    msg.validity_gps_pos_ecef_x = vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_encode(validity_gps_pos_ecef_x);
    msg.validity_gps_pos_ecef_y = vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_y_encode(validity_gps_pos_ecef_y);
    msg.validity_gps_pos_ecef_z = vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_z_encode(validity_gps_pos_ecef_z);
    msg.accuracy_gps_pos_ecef_x = vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_x_encode(accuracy_gps_pos_ecef_x);
    msg.accuracy_gps_pos_ecef_y = vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_y_encode(accuracy_gps_pos_ecef_y);
    msg.accuracy_gps_pos_ecef_z = vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_z_encode(accuracy_gps_pos_ecef_z);
    msg.gps_pos_ecef_x = vehicle_rt_sb_gps_pos_ecef_1_gps_pos_ecef_x_encode(gps_pos_ecef_x);

    int ret = vehicle_rt_sb_gps_pos_ecef_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_pos_ecef_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_pos_ecef_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_pos_ecef_x,
    double *validity_gps_pos_ecef_y,
    double *validity_gps_pos_ecef_z,
    double *accuracy_gps_pos_ecef_x,
    double *accuracy_gps_pos_ecef_y,
    double *accuracy_gps_pos_ecef_z,
    double *gps_pos_ecef_x)
{
    struct vehicle_rt_sb_gps_pos_ecef_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_pos_ecef_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_pos_ecef_1_check_ranges(&msg);

    if (validity_gps_pos_ecef_x)
        *validity_gps_pos_ecef_x = vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_decode(msg.validity_gps_pos_ecef_x);

    if (validity_gps_pos_ecef_y)
        *validity_gps_pos_ecef_y = vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_y_decode(msg.validity_gps_pos_ecef_y);

    if (validity_gps_pos_ecef_z)
        *validity_gps_pos_ecef_z = vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_z_decode(msg.validity_gps_pos_ecef_z);

    if (accuracy_gps_pos_ecef_x)
        *accuracy_gps_pos_ecef_x = vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_x_decode(msg.accuracy_gps_pos_ecef_x);

    if (accuracy_gps_pos_ecef_y)
        *accuracy_gps_pos_ecef_y = vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_y_decode(msg.accuracy_gps_pos_ecef_y);

    if (accuracy_gps_pos_ecef_z)
        *accuracy_gps_pos_ecef_z = vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_z_decode(msg.accuracy_gps_pos_ecef_z);

    if (gps_pos_ecef_x)
        *gps_pos_ecef_x = vehicle_rt_sb_gps_pos_ecef_1_gps_pos_ecef_x_decode(msg.gps_pos_ecef_x);

    return ret;
}

uint8_t vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_ecef_1_validity_gps_pos_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_ecef_1_accuracy_gps_pos_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_ecef_1_gps_pos_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -10000000.0);
    ret = CTOOLS_MIN(ret, 10000000.0);
    return ret;
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

static int vehicle_rt_sb_gps_pos_llh_2_check_ranges(struct vehicle_rt_sb_gps_pos_llh_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_is_in_range(msg->gps_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_altitude_is_in_range(msg->gps_pos_llh_altitude))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_pos_llh_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double gps_pos_llh_longitude,
    double gps_pos_llh_altitude)
{
    struct vehicle_rt_sb_gps_pos_llh_2_t msg;

    msg.gps_pos_llh_longitude = vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_encode(gps_pos_llh_longitude);
    msg.gps_pos_llh_altitude = vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_altitude_encode(gps_pos_llh_altitude);

    int ret = vehicle_rt_sb_gps_pos_llh_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_pos_llh_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_pos_llh_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *gps_pos_llh_longitude,
    double *gps_pos_llh_altitude)
{
    struct vehicle_rt_sb_gps_pos_llh_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_pos_llh_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_pos_llh_2_check_ranges(&msg);

    if (gps_pos_llh_longitude)
        *gps_pos_llh_longitude = vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_decode(msg.gps_pos_llh_longitude);

    if (gps_pos_llh_altitude)
        *gps_pos_llh_altitude = vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_altitude_decode(msg.gps_pos_llh_altitude);

    return ret;
}

int32_t vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_encode(double value)
{
    return (int32_t)(value / 1E-7);
}

double vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_decode(int32_t value)
{
    return ((double)value * 1E-7);
}

double vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_llh_2_gps_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -1000.0);
    ret = CTOOLS_MIN(ret, 100000.0);
    return ret;
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

static int vehicle_rt_sb_gps_pos_llh_1_check_ranges(struct vehicle_rt_sb_gps_pos_llh_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_is_in_range(msg->validity_gps_pos_llh_latitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_longitude_is_in_range(msg->validity_gps_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_altitude_is_in_range(msg->validity_gps_pos_llh_altitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_is_in_range(msg->accuracy_gps_pos_llh_latitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_is_in_range(msg->accuracy_gps_pos_llh_longitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_is_in_range(msg->accuracy_gps_pos_llh_altitude))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_pos_llh_1_gps_pos_llh_latitude_is_in_range(msg->gps_pos_llh_latitude))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_pos_llh_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_pos_llh_latitude,
    double validity_gps_pos_llh_longitude,
    double validity_gps_pos_llh_altitude,
    double accuracy_gps_pos_llh_latitude,
    double accuracy_gps_pos_llh_longitude,
    double accuracy_gps_pos_llh_altitude,
    double gps_pos_llh_latitude)
{
    struct vehicle_rt_sb_gps_pos_llh_1_t msg;

    msg.validity_gps_pos_llh_latitude = vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_encode(validity_gps_pos_llh_latitude);
    msg.validity_gps_pos_llh_longitude = vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_longitude_encode(validity_gps_pos_llh_longitude);
    msg.validity_gps_pos_llh_altitude = vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_altitude_encode(validity_gps_pos_llh_altitude);
    msg.accuracy_gps_pos_llh_latitude = vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_encode(accuracy_gps_pos_llh_latitude);
    msg.accuracy_gps_pos_llh_longitude = vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_encode(accuracy_gps_pos_llh_longitude);
    msg.accuracy_gps_pos_llh_altitude = vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_encode(accuracy_gps_pos_llh_altitude);
    msg.gps_pos_llh_latitude = vehicle_rt_sb_gps_pos_llh_1_gps_pos_llh_latitude_encode(gps_pos_llh_latitude);

    int ret = vehicle_rt_sb_gps_pos_llh_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_pos_llh_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_pos_llh_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_pos_llh_latitude,
    double *validity_gps_pos_llh_longitude,
    double *validity_gps_pos_llh_altitude,
    double *accuracy_gps_pos_llh_latitude,
    double *accuracy_gps_pos_llh_longitude,
    double *accuracy_gps_pos_llh_altitude,
    double *gps_pos_llh_latitude)
{
    struct vehicle_rt_sb_gps_pos_llh_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_pos_llh_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_pos_llh_1_check_ranges(&msg);

    if (validity_gps_pos_llh_latitude)
        *validity_gps_pos_llh_latitude = vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_decode(msg.validity_gps_pos_llh_latitude);

    if (validity_gps_pos_llh_longitude)
        *validity_gps_pos_llh_longitude = vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_longitude_decode(msg.validity_gps_pos_llh_longitude);

    if (validity_gps_pos_llh_altitude)
        *validity_gps_pos_llh_altitude = vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_altitude_decode(msg.validity_gps_pos_llh_altitude);

    if (accuracy_gps_pos_llh_latitude)
        *accuracy_gps_pos_llh_latitude = vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_decode(msg.accuracy_gps_pos_llh_latitude);

    if (accuracy_gps_pos_llh_longitude)
        *accuracy_gps_pos_llh_longitude = vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_decode(msg.accuracy_gps_pos_llh_longitude);

    if (accuracy_gps_pos_llh_altitude)
        *accuracy_gps_pos_llh_altitude = vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_decode(msg.accuracy_gps_pos_llh_altitude);

    if (gps_pos_llh_latitude)
        *gps_pos_llh_latitude = vehicle_rt_sb_gps_pos_llh_1_gps_pos_llh_latitude_decode(msg.gps_pos_llh_latitude);

    return ret;
}

uint8_t vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_llh_1_validity_gps_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_longitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_llh_1_accuracy_gps_pos_llh_altitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_pos_llh_1_gps_pos_llh_latitude_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_gps_heading_gradient_check_ranges(struct vehicle_rt_sb_gps_heading_gradient_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_is_in_range(msg->validity_gps_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_validity_gps_gradient_is_in_range(msg->validity_gps_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_accuracy_gps_heading_is_in_range(msg->accuracy_gps_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_gps_heading_is_in_range(msg->gps_heading))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_accuracy_gps_gradient_is_in_range(msg->accuracy_gps_gradient))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_heading_gradient_gps_gradient_is_in_range(msg->gps_gradient))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_heading_gradient_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_heading,
    double validity_gps_gradient,
    double accuracy_gps_heading,
    double gps_heading,
    double accuracy_gps_gradient,
    double gps_gradient)
{
    struct vehicle_rt_sb_gps_heading_gradient_t msg;

    msg.validity_gps_heading = vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_encode(validity_gps_heading);
    msg.validity_gps_gradient = vehicle_rt_sb_gps_heading_gradient_validity_gps_gradient_encode(validity_gps_gradient);
    msg.accuracy_gps_heading = vehicle_rt_sb_gps_heading_gradient_accuracy_gps_heading_encode(accuracy_gps_heading);
    msg.gps_heading = vehicle_rt_sb_gps_heading_gradient_gps_heading_encode(gps_heading);
    msg.accuracy_gps_gradient = vehicle_rt_sb_gps_heading_gradient_accuracy_gps_gradient_encode(accuracy_gps_gradient);
    msg.gps_gradient = vehicle_rt_sb_gps_heading_gradient_gps_gradient_encode(gps_gradient);

    int ret = vehicle_rt_sb_gps_heading_gradient_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_heading_gradient_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_heading_gradient_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_heading,
    double *validity_gps_gradient,
    double *accuracy_gps_heading,
    double *gps_heading,
    double *accuracy_gps_gradient,
    double *gps_gradient)
{
    struct vehicle_rt_sb_gps_heading_gradient_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_heading_gradient_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_heading_gradient_check_ranges(&msg);

    if (validity_gps_heading)
        *validity_gps_heading = vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_decode(msg.validity_gps_heading);

    if (validity_gps_gradient)
        *validity_gps_gradient = vehicle_rt_sb_gps_heading_gradient_validity_gps_gradient_decode(msg.validity_gps_gradient);

    if (accuracy_gps_heading)
        *accuracy_gps_heading = vehicle_rt_sb_gps_heading_gradient_accuracy_gps_heading_decode(msg.accuracy_gps_heading);

    if (gps_heading)
        *gps_heading = vehicle_rt_sb_gps_heading_gradient_gps_heading_decode(msg.gps_heading);

    if (accuracy_gps_gradient)
        *accuracy_gps_gradient = vehicle_rt_sb_gps_heading_gradient_accuracy_gps_gradient_decode(msg.accuracy_gps_gradient);

    if (gps_gradient)
        *gps_gradient = vehicle_rt_sb_gps_heading_gradient_gps_gradient_decode(msg.gps_gradient);

    return ret;
}

uint8_t vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_heading_gradient_validity_gps_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_validity_gps_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_accuracy_gps_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_gps_heading_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -180.0);
    ret = CTOOLS_MIN(ret, 180.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_accuracy_gps_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_heading_gradient_gps_gradient_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -90.0);
    ret = CTOOLS_MIN(ret, 90.0);
    return ret;
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

static int vehicle_rt_sb_gps_vel_ecef_2_check_ranges(struct vehicle_rt_sb_gps_vel_ecef_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_is_in_range(msg->validity_gps_vel_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_z_is_in_range(msg->validity_gps_vel_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_y_is_in_range(msg->gps_vel_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_z_is_in_range(msg->gps_vel_ecef_z))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_vel_ecef_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_vel_ecef_y,
    double validity_gps_vel_ecef_z,
    double gps_vel_ecef_y,
    double gps_vel_ecef_z)
{
    struct vehicle_rt_sb_gps_vel_ecef_2_t msg;

    msg.validity_gps_vel_ecef_y = vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_encode(validity_gps_vel_ecef_y);
    msg.validity_gps_vel_ecef_z = vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_z_encode(validity_gps_vel_ecef_z);
    msg.gps_vel_ecef_y = vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_y_encode(gps_vel_ecef_y);
    msg.gps_vel_ecef_z = vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_z_encode(gps_vel_ecef_z);

    int ret = vehicle_rt_sb_gps_vel_ecef_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_vel_ecef_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_vel_ecef_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_vel_ecef_y,
    double *validity_gps_vel_ecef_z,
    double *gps_vel_ecef_y,
    double *gps_vel_ecef_z)
{
    struct vehicle_rt_sb_gps_vel_ecef_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_vel_ecef_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_vel_ecef_2_check_ranges(&msg);

    if (validity_gps_vel_ecef_y)
        *validity_gps_vel_ecef_y = vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_decode(msg.validity_gps_vel_ecef_y);

    if (validity_gps_vel_ecef_z)
        *validity_gps_vel_ecef_z = vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_z_decode(msg.validity_gps_vel_ecef_z);

    if (gps_vel_ecef_y)
        *gps_vel_ecef_y = vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_y_decode(msg.gps_vel_ecef_y);

    if (gps_vel_ecef_z)
        *gps_vel_ecef_z = vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_z_decode(msg.gps_vel_ecef_z);

    return ret;
}

uint8_t vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ecef_2_validity_gps_vel_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ecef_2_gps_vel_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_gps_vel_ecef_1_check_ranges(struct vehicle_rt_sb_gps_vel_ecef_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_is_in_range(msg->validity_gps_vel_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_x_is_in_range(msg->accuracy_gps_vel_ecef_x))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_y_is_in_range(msg->accuracy_gps_vel_ecef_y))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_z_is_in_range(msg->accuracy_gps_vel_ecef_z))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ecef_1_gps_vel_ecef_x_is_in_range(msg->gps_vel_ecef_x))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_vel_ecef_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_vel_ecef_x,
    double accuracy_gps_vel_ecef_x,
    double accuracy_gps_vel_ecef_y,
    double accuracy_gps_vel_ecef_z,
    double gps_vel_ecef_x)
{
    struct vehicle_rt_sb_gps_vel_ecef_1_t msg;

    msg.validity_gps_vel_ecef_x = vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_encode(validity_gps_vel_ecef_x);
    msg.accuracy_gps_vel_ecef_x = vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_x_encode(accuracy_gps_vel_ecef_x);
    msg.accuracy_gps_vel_ecef_y = vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_y_encode(accuracy_gps_vel_ecef_y);
    msg.accuracy_gps_vel_ecef_z = vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_z_encode(accuracy_gps_vel_ecef_z);
    msg.gps_vel_ecef_x = vehicle_rt_sb_gps_vel_ecef_1_gps_vel_ecef_x_encode(gps_vel_ecef_x);

    int ret = vehicle_rt_sb_gps_vel_ecef_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_vel_ecef_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_vel_ecef_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_vel_ecef_x,
    double *accuracy_gps_vel_ecef_x,
    double *accuracy_gps_vel_ecef_y,
    double *accuracy_gps_vel_ecef_z,
    double *gps_vel_ecef_x)
{
    struct vehicle_rt_sb_gps_vel_ecef_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_vel_ecef_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_vel_ecef_1_check_ranges(&msg);

    if (validity_gps_vel_ecef_x)
        *validity_gps_vel_ecef_x = vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_decode(msg.validity_gps_vel_ecef_x);

    if (accuracy_gps_vel_ecef_x)
        *accuracy_gps_vel_ecef_x = vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_x_decode(msg.accuracy_gps_vel_ecef_x);

    if (accuracy_gps_vel_ecef_y)
        *accuracy_gps_vel_ecef_y = vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_y_decode(msg.accuracy_gps_vel_ecef_y);

    if (accuracy_gps_vel_ecef_z)
        *accuracy_gps_vel_ecef_z = vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_z_decode(msg.accuracy_gps_vel_ecef_z);

    if (gps_vel_ecef_x)
        *gps_vel_ecef_x = vehicle_rt_sb_gps_vel_ecef_1_gps_vel_ecef_x_decode(msg.gps_vel_ecef_x);

    return ret;
}

uint8_t vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_vel_ecef_1_validity_gps_vel_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_y_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ecef_1_accuracy_gps_vel_ecef_z_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ecef_1_gps_vel_ecef_x_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_gps_vel_ned_2_check_ranges(struct vehicle_rt_sb_gps_vel_ned_2_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_is_in_range(msg->validity_gps_vel_ned_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ned_2_accuracy_gps_vel_d_is_in_range(msg->accuracy_gps_vel_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ned_2_gps_vel_ned_d_is_in_range(msg->gps_vel_ned_d))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_vel_ned_2_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_vel_ned_d,
    double accuracy_gps_vel_d,
    double gps_vel_ned_d)
{
    struct vehicle_rt_sb_gps_vel_ned_2_t msg;

    msg.validity_gps_vel_ned_d = vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_encode(validity_gps_vel_ned_d);
    msg.accuracy_gps_vel_d = vehicle_rt_sb_gps_vel_ned_2_accuracy_gps_vel_d_encode(accuracy_gps_vel_d);
    msg.gps_vel_ned_d = vehicle_rt_sb_gps_vel_ned_2_gps_vel_ned_d_encode(gps_vel_ned_d);

    int ret = vehicle_rt_sb_gps_vel_ned_2_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_vel_ned_2_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_vel_ned_2_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_vel_ned_d,
    double *accuracy_gps_vel_d,
    double *gps_vel_ned_d)
{
    struct vehicle_rt_sb_gps_vel_ned_2_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_vel_ned_2_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_vel_ned_2_check_ranges(&msg);

    if (validity_gps_vel_ned_d)
        *validity_gps_vel_ned_d = vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_decode(msg.validity_gps_vel_ned_d);

    if (accuracy_gps_vel_d)
        *accuracy_gps_vel_d = vehicle_rt_sb_gps_vel_ned_2_accuracy_gps_vel_d_decode(msg.accuracy_gps_vel_d);

    if (gps_vel_ned_d)
        *gps_vel_ned_d = vehicle_rt_sb_gps_vel_ned_2_gps_vel_ned_d_decode(msg.gps_vel_ned_d);

    return ret;
}

uint8_t vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_vel_ned_2_validity_gps_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ned_2_accuracy_gps_vel_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ned_2_gps_vel_ned_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_gps_vel_ned_1_check_ranges(struct vehicle_rt_sb_gps_vel_ned_1_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_is_in_range(msg->validity_gps_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_e_is_in_range(msg->validity_gps_vel_ned_e))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ned_1_accuracy_gps_vel_ne_is_in_range(msg->accuracy_gps_vel_ne))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_n_is_in_range(msg->gps_vel_ned_n))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_e_is_in_range(msg->gps_vel_ned_e))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_vel_ned_1_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_vel_ned_n,
    double validity_gps_vel_ned_e,
    double accuracy_gps_vel_ne,
    double gps_vel_ned_n,
    double gps_vel_ned_e)
{
    struct vehicle_rt_sb_gps_vel_ned_1_t msg;

    msg.validity_gps_vel_ned_n = vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_encode(validity_gps_vel_ned_n);
    msg.validity_gps_vel_ned_e = vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_e_encode(validity_gps_vel_ned_e);
    msg.accuracy_gps_vel_ne = vehicle_rt_sb_gps_vel_ned_1_accuracy_gps_vel_ne_encode(accuracy_gps_vel_ne);
    msg.gps_vel_ned_n = vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_n_encode(gps_vel_ned_n);
    msg.gps_vel_ned_e = vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_e_encode(gps_vel_ned_e);

    int ret = vehicle_rt_sb_gps_vel_ned_1_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_vel_ned_1_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_vel_ned_1_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_vel_ned_n,
    double *validity_gps_vel_ned_e,
    double *accuracy_gps_vel_ne,
    double *gps_vel_ned_n,
    double *gps_vel_ned_e)
{
    struct vehicle_rt_sb_gps_vel_ned_1_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_vel_ned_1_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_vel_ned_1_check_ranges(&msg);

    if (validity_gps_vel_ned_n)
        *validity_gps_vel_ned_n = vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_decode(msg.validity_gps_vel_ned_n);

    if (validity_gps_vel_ned_e)
        *validity_gps_vel_ned_e = vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_e_decode(msg.validity_gps_vel_ned_e);

    if (accuracy_gps_vel_ne)
        *accuracy_gps_vel_ne = vehicle_rt_sb_gps_vel_ned_1_accuracy_gps_vel_ne_decode(msg.accuracy_gps_vel_ne);

    if (gps_vel_ned_n)
        *gps_vel_ned_n = vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_n_decode(msg.gps_vel_ned_n);

    if (gps_vel_ned_e)
        *gps_vel_ned_e = vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_e_decode(msg.gps_vel_ned_e);

    return ret;
}

uint8_t vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ned_1_validity_gps_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ned_1_accuracy_gps_vel_ne_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_n_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

double vehicle_rt_sb_gps_vel_ned_1_gps_vel_ned_e_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -838.0);
    ret = CTOOLS_MIN(ret, 838.0);
    return ret;
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

static int vehicle_rt_sb_gps_speed_check_ranges(struct vehicle_rt_sb_gps_speed_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_is_in_range(msg->validity_gps_speed_2_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_speed_validity_gps_speed_3_d_is_in_range(msg->validity_gps_speed_3_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_speed_accuracy_gps_speed_is_in_range(msg->accuracy_gps_speed))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_speed_gps_speed_2_d_is_in_range(msg->gps_speed_2_d))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_speed_gps_speed_3_d_is_in_range(msg->gps_speed_3_d))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_speed_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_speed_2_d,
    double validity_gps_speed_3_d,
    double accuracy_gps_speed,
    double gps_speed_2_d,
    double gps_speed_3_d)
{
    struct vehicle_rt_sb_gps_speed_t msg;

    msg.validity_gps_speed_2_d = vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_encode(validity_gps_speed_2_d);
    msg.validity_gps_speed_3_d = vehicle_rt_sb_gps_speed_validity_gps_speed_3_d_encode(validity_gps_speed_3_d);
    msg.accuracy_gps_speed = vehicle_rt_sb_gps_speed_accuracy_gps_speed_encode(accuracy_gps_speed);
    msg.gps_speed_2_d = vehicle_rt_sb_gps_speed_gps_speed_2_d_encode(gps_speed_2_d);
    msg.gps_speed_3_d = vehicle_rt_sb_gps_speed_gps_speed_3_d_encode(gps_speed_3_d);

    int ret = vehicle_rt_sb_gps_speed_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_speed_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_speed_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_speed_2_d,
    double *validity_gps_speed_3_d,
    double *accuracy_gps_speed,
    double *gps_speed_2_d,
    double *gps_speed_3_d)
{
    struct vehicle_rt_sb_gps_speed_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_speed_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_speed_check_ranges(&msg);

    if (validity_gps_speed_2_d)
        *validity_gps_speed_2_d = vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_decode(msg.validity_gps_speed_2_d);

    if (validity_gps_speed_3_d)
        *validity_gps_speed_3_d = vehicle_rt_sb_gps_speed_validity_gps_speed_3_d_decode(msg.validity_gps_speed_3_d);

    if (accuracy_gps_speed)
        *accuracy_gps_speed = vehicle_rt_sb_gps_speed_accuracy_gps_speed_decode(msg.accuracy_gps_speed);

    if (gps_speed_2_d)
        *gps_speed_2_d = vehicle_rt_sb_gps_speed_gps_speed_2_d_decode(msg.gps_speed_2_d);

    if (gps_speed_3_d)
        *gps_speed_3_d = vehicle_rt_sb_gps_speed_gps_speed_3_d_decode(msg.gps_speed_3_d);

    return ret;
}

uint8_t vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_speed_validity_gps_speed_2_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_speed_validity_gps_speed_3_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_speed_accuracy_gps_speed_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_speed_gps_speed_2_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1675.0);
    return ret;
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

double vehicle_rt_sb_gps_speed_gps_speed_3_d_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1675.0);
    return ret;
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

static int vehicle_rt_sb_gps_time_check_ranges(struct vehicle_rt_sb_gps_time_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gps_time_validity_gps_time_is_in_range(msg->validity_gps_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_time_validity_gps_week_is_in_range(msg->validity_gps_week))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_time_accuracy_gps_time_is_in_range(msg->accuracy_gps_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_time_gps_time_is_in_range(msg->gps_time))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gps_time_gps_week_is_in_range(msg->gps_week))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gps_time_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gps_time,
    double validity_gps_week,
    double accuracy_gps_time,
    double gps_time,
    double gps_week)
{
    struct vehicle_rt_sb_gps_time_t msg;

    msg.validity_gps_time = vehicle_rt_sb_gps_time_validity_gps_time_encode(validity_gps_time);
    msg.validity_gps_week = vehicle_rt_sb_gps_time_validity_gps_week_encode(validity_gps_week);
    msg.accuracy_gps_time = vehicle_rt_sb_gps_time_accuracy_gps_time_encode(accuracy_gps_time);
    msg.gps_time = vehicle_rt_sb_gps_time_gps_time_encode(gps_time);
    msg.gps_week = vehicle_rt_sb_gps_time_gps_week_encode(gps_week);

    int ret = vehicle_rt_sb_gps_time_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gps_time_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gps_time_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gps_time,
    double *validity_gps_week,
    double *accuracy_gps_time,
    double *gps_time,
    double *gps_week)
{
    struct vehicle_rt_sb_gps_time_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gps_time_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gps_time_check_ranges(&msg);

    if (validity_gps_time)
        *validity_gps_time = vehicle_rt_sb_gps_time_validity_gps_time_decode(msg.validity_gps_time);

    if (validity_gps_week)
        *validity_gps_week = vehicle_rt_sb_gps_time_validity_gps_week_decode(msg.validity_gps_week);

    if (accuracy_gps_time)
        *accuracy_gps_time = vehicle_rt_sb_gps_time_accuracy_gps_time_decode(msg.accuracy_gps_time);

    if (gps_time)
        *gps_time = vehicle_rt_sb_gps_time_gps_time_decode(msg.gps_time);

    if (gps_week)
        *gps_week = vehicle_rt_sb_gps_time_gps_week_decode(msg.gps_week);

    return ret;
}

uint8_t vehicle_rt_sb_gps_time_validity_gps_time_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gps_time_validity_gps_time_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gps_time_validity_gps_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_time_validity_gps_week_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gps_time_accuracy_gps_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_gps_time_gps_time_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 604800.0);
    return ret;
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

double vehicle_rt_sb_gps_time_gps_week_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 65535.0);
    return ret;
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

static int vehicle_rt_sb_accel_check_ranges(struct vehicle_rt_sb_accel_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_accel_validity_accel_longitudinal_is_in_range(msg->validity_accel_longitudinal))
        return idx;

    idx++;

    if (!vehicle_rt_sb_accel_validity_accel_lateral_is_in_range(msg->validity_accel_lateral))
        return idx;

    idx++;

    if (!vehicle_rt_sb_accel_validity_accel_vertical_is_in_range(msg->validity_accel_vertical))
        return idx;

    idx++;

    if (!vehicle_rt_sb_accel_accuracy_accel_is_in_range(msg->accuracy_accel))
        return idx;

    idx++;

    if (!vehicle_rt_sb_accel_accel_longitudinal_is_in_range(msg->accel_longitudinal))
        return idx;

    idx++;

    if (!vehicle_rt_sb_accel_accel_lateral_is_in_range(msg->accel_lateral))
        return idx;

    idx++;

    if (!vehicle_rt_sb_accel_accel_vertical_is_in_range(msg->accel_vertical))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_accel_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_accel_longitudinal,
    double validity_accel_lateral,
    double validity_accel_vertical,
    double accuracy_accel,
    double accel_longitudinal,
    double accel_lateral,
    double accel_vertical)
{
    struct vehicle_rt_sb_accel_t msg;

    msg.validity_accel_longitudinal = vehicle_rt_sb_accel_validity_accel_longitudinal_encode(validity_accel_longitudinal);
    msg.validity_accel_lateral = vehicle_rt_sb_accel_validity_accel_lateral_encode(validity_accel_lateral);
    msg.validity_accel_vertical = vehicle_rt_sb_accel_validity_accel_vertical_encode(validity_accel_vertical);
    msg.accuracy_accel = vehicle_rt_sb_accel_accuracy_accel_encode(accuracy_accel);
    msg.accel_longitudinal = vehicle_rt_sb_accel_accel_longitudinal_encode(accel_longitudinal);
    msg.accel_lateral = vehicle_rt_sb_accel_accel_lateral_encode(accel_lateral);
    msg.accel_vertical = vehicle_rt_sb_accel_accel_vertical_encode(accel_vertical);

    int ret = vehicle_rt_sb_accel_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_accel_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_accel_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_accel_longitudinal,
    double *validity_accel_lateral,
    double *validity_accel_vertical,
    double *accuracy_accel,
    double *accel_longitudinal,
    double *accel_lateral,
    double *accel_vertical)
{
    struct vehicle_rt_sb_accel_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_accel_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_accel_check_ranges(&msg);

    if (validity_accel_longitudinal)
        *validity_accel_longitudinal = vehicle_rt_sb_accel_validity_accel_longitudinal_decode(msg.validity_accel_longitudinal);

    if (validity_accel_lateral)
        *validity_accel_lateral = vehicle_rt_sb_accel_validity_accel_lateral_decode(msg.validity_accel_lateral);

    if (validity_accel_vertical)
        *validity_accel_vertical = vehicle_rt_sb_accel_validity_accel_vertical_decode(msg.validity_accel_vertical);

    if (accuracy_accel)
        *accuracy_accel = vehicle_rt_sb_accel_accuracy_accel_decode(msg.accuracy_accel);

    if (accel_longitudinal)
        *accel_longitudinal = vehicle_rt_sb_accel_accel_longitudinal_decode(msg.accel_longitudinal);

    if (accel_lateral)
        *accel_lateral = vehicle_rt_sb_accel_accel_lateral_decode(msg.accel_lateral);

    if (accel_vertical)
        *accel_vertical = vehicle_rt_sb_accel_accel_vertical_decode(msg.accel_vertical);

    return ret;
}

uint8_t vehicle_rt_sb_accel_validity_accel_longitudinal_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_accel_validity_accel_longitudinal_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_accel_validity_accel_longitudinal_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_accel_validity_accel_lateral_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_accel_validity_accel_vertical_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_accel_accuracy_accel_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 255.0);
    return ret;
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

double vehicle_rt_sb_accel_accel_longitudinal_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_sb_accel_accel_lateral_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

double vehicle_rt_sb_accel_accel_vertical_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -65.0);
    ret = CTOOLS_MIN(ret, 65.0);
    return ret;
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

static int vehicle_rt_sb_gyro_rates_check_ranges(struct vehicle_rt_sb_gyro_rates_t *msg)
{
    int idx = 1;

    if (!vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_is_in_range(msg->validity_gyro_rate_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gyro_rates_validity_gyro_rate_pitch_is_in_range(msg->validity_gyro_rate_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gyro_rates_validity_gyro_rate_roll_is_in_range(msg->validity_gyro_rate_roll))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gyro_rates_accuracy_gyro_rates_is_in_range(msg->accuracy_gyro_rates))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gyro_rates_gyro_rate_yaw_is_in_range(msg->gyro_rate_yaw))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gyro_rates_gyro_rate_pitch_is_in_range(msg->gyro_rate_pitch))
        return idx;

    idx++;

    if (!vehicle_rt_sb_gyro_rates_gyro_rate_roll_is_in_range(msg->gyro_rate_roll))
        return idx;

    idx++;

    return 0;
}

int vehicle_rt_sb_gyro_rates_wrap_pack(
    uint8_t *outbuf, size_t outbuf_sz,
    double validity_gyro_rate_yaw,
    double validity_gyro_rate_pitch,
    double validity_gyro_rate_roll,
    double accuracy_gyro_rates,
    double gyro_rate_yaw,
    double gyro_rate_pitch,
    double gyro_rate_roll)
{
    struct vehicle_rt_sb_gyro_rates_t msg;

    msg.validity_gyro_rate_yaw = vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_encode(validity_gyro_rate_yaw);
    msg.validity_gyro_rate_pitch = vehicle_rt_sb_gyro_rates_validity_gyro_rate_pitch_encode(validity_gyro_rate_pitch);
    msg.validity_gyro_rate_roll = vehicle_rt_sb_gyro_rates_validity_gyro_rate_roll_encode(validity_gyro_rate_roll);
    msg.accuracy_gyro_rates = vehicle_rt_sb_gyro_rates_accuracy_gyro_rates_encode(accuracy_gyro_rates);
    msg.gyro_rate_yaw = vehicle_rt_sb_gyro_rates_gyro_rate_yaw_encode(gyro_rate_yaw);
    msg.gyro_rate_pitch = vehicle_rt_sb_gyro_rates_gyro_rate_pitch_encode(gyro_rate_pitch);
    msg.gyro_rate_roll = vehicle_rt_sb_gyro_rates_gyro_rate_roll_encode(gyro_rate_roll);

    int ret = vehicle_rt_sb_gyro_rates_check_ranges(&msg);
    if (ret) {
        return ret;
    }

    ret = vehicle_rt_sb_gyro_rates_pack(outbuf, &msg, outbuf_sz);
    if (8 != ret) {
        return -1;
    }

    return 0;
}

int vehicle_rt_sb_gyro_rates_wrap_unpack(
    uint8_t *inbuf, size_t inbuf_sz,
    double *validity_gyro_rate_yaw,
    double *validity_gyro_rate_pitch,
    double *validity_gyro_rate_roll,
    double *accuracy_gyro_rates,
    double *gyro_rate_yaw,
    double *gyro_rate_pitch,
    double *gyro_rate_roll)
{
    struct vehicle_rt_sb_gyro_rates_t msg;
    memset(&msg, 0, sizeof(msg));

    if (vehicle_rt_sb_gyro_rates_unpack(&msg, inbuf, inbuf_sz)) {
        return -1;
    }

    int ret = vehicle_rt_sb_gyro_rates_check_ranges(&msg);

    if (validity_gyro_rate_yaw)
        *validity_gyro_rate_yaw = vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_decode(msg.validity_gyro_rate_yaw);

    if (validity_gyro_rate_pitch)
        *validity_gyro_rate_pitch = vehicle_rt_sb_gyro_rates_validity_gyro_rate_pitch_decode(msg.validity_gyro_rate_pitch);

    if (validity_gyro_rate_roll)
        *validity_gyro_rate_roll = vehicle_rt_sb_gyro_rates_validity_gyro_rate_roll_decode(msg.validity_gyro_rate_roll);

    if (accuracy_gyro_rates)
        *accuracy_gyro_rates = vehicle_rt_sb_gyro_rates_accuracy_gyro_rates_decode(msg.accuracy_gyro_rates);

    if (gyro_rate_yaw)
        *gyro_rate_yaw = vehicle_rt_sb_gyro_rates_gyro_rate_yaw_decode(msg.gyro_rate_yaw);

    if (gyro_rate_pitch)
        *gyro_rate_pitch = vehicle_rt_sb_gyro_rates_gyro_rate_pitch_decode(msg.gyro_rate_pitch);

    if (gyro_rate_roll)
        *gyro_rate_roll = vehicle_rt_sb_gyro_rates_gyro_rate_roll_decode(msg.gyro_rate_roll);

    return ret;
}

uint8_t vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_encode(double value)
{
    return (uint8_t)(value);
}

double vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_decode(uint8_t value)
{
    return ((double)value);
}

double vehicle_rt_sb_gyro_rates_validity_gyro_rate_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gyro_rates_validity_gyro_rate_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gyro_rates_validity_gyro_rate_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, 0.0);
    ret = CTOOLS_MIN(ret, 1.0);
    return ret;
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

double vehicle_rt_sb_gyro_rates_accuracy_gyro_rates_clamp(double val)
{
    double ret = val;


    return ret;
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

double vehicle_rt_sb_gyro_rates_gyro_rate_yaw_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -327.0);
    ret = CTOOLS_MIN(ret, 327.0);
    return ret;
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

double vehicle_rt_sb_gyro_rates_gyro_rate_pitch_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -327.0);
    ret = CTOOLS_MIN(ret, 327.0);
    return ret;
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

double vehicle_rt_sb_gyro_rates_gyro_rate_roll_clamp(double val)
{
    double ret = val;
    ret = CTOOLS_MAX(ret, -327.0);
    ret = CTOOLS_MIN(ret, 327.0);
    return ret;
}

bool vehicle_rt_sb_gyro_rates_gyro_rate_roll_is_in_range(int16_t value)
{
    return ((value >= -32700) && (value <= 32700));
}

#undef CTOOLS_MAX
#undef CTOOLS_MIN
