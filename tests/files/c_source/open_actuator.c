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

#include "open_actuator.h"

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

static inline uint8_t pack_right_shift_u16(
    uint16_t value,
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

int open_actuator_control_cmd_pack(
    uint8_t *dst_p,
    const struct open_actuator_control_cmd_t *src_p,
    size_t size)
{
    uint16_t position_cmd_64;
    uint16_t torque_command_8;

    if (size < 7u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 7);

    dst_p[0] |= pack_left_shift_u8(src_p->crc8_cmd1, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->target_mode, 0u, 0x07u);
    dst_p[1] |= pack_left_shift_u8(src_p->target_motor_id_cmd1, 4u, 0x30u);
    position_cmd_64 = (uint16_t)src_p->position_cmd_64;
    dst_p[2] |= pack_left_shift_u16(position_cmd_64, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(position_cmd_64, 8u, 0xffu);
    torque_command_8 = (uint16_t)src_p->torque_command_8;
    dst_p[4] |= pack_left_shift_u16(torque_command_8, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(torque_command_8, 8u, 0x03u);
    dst_p[5] |= pack_left_shift_u8(src_p->torque_close_loop_max_32, 2u, 0xfcu);
    dst_p[6] |= pack_left_shift_u8(src_p->counter_cmd1, 0u, 0x0fu);

    return (7);
}

int open_actuator_control_cmd_unpack(
    struct open_actuator_control_cmd_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t position_cmd_64;
    uint16_t torque_command_8;

    if (size < 7u) {
        return (-EINVAL);
    }

    dst_p->crc8_cmd1 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->target_mode = unpack_right_shift_u8(src_p[1], 0u, 0x07u);
    dst_p->target_motor_id_cmd1 = unpack_right_shift_u8(src_p[1], 4u, 0x30u);
    position_cmd_64 = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    position_cmd_64 |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->position_cmd_64 = (int16_t)position_cmd_64;
    torque_command_8 = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    torque_command_8 |= unpack_left_shift_u16(src_p[5], 8u, 0x03u);

    if ((torque_command_8 & (1u << 9)) != 0u) {
        torque_command_8 |= 0xfc00u;
    }

    dst_p->torque_command_8 = (int16_t)torque_command_8;
    dst_p->torque_close_loop_max_32 = unpack_right_shift_u8(src_p[5], 2u, 0xfcu);
    dst_p->counter_cmd1 = unpack_right_shift_u8(src_p[6], 0u, 0x0fu);

    return (0);
}

int open_actuator_control_cmd_init(struct open_actuator_control_cmd_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct open_actuator_control_cmd_t));

    return 0;
}

uint8_t open_actuator_control_cmd_crc8_cmd1_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_control_cmd_crc8_cmd1_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_control_cmd_crc8_cmd1_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t open_actuator_control_cmd_target_mode_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_control_cmd_target_mode_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_control_cmd_target_mode_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

uint8_t open_actuator_control_cmd_target_motor_id_cmd1_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_control_cmd_target_motor_id_cmd1_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_control_cmd_target_motor_id_cmd1_is_in_range(uint8_t value)
{
    return (value <= 3u);
}

int16_t open_actuator_control_cmd_position_cmd_64_encode(double value)
{
    return (int16_t)(value / 0.0154286);
}

double open_actuator_control_cmd_position_cmd_64_decode(int16_t value)
{
    return ((double)value * 0.0154286);
}

bool open_actuator_control_cmd_position_cmd_64_is_in_range(int16_t value)
{
    return ((value >= -29167) && (value <= 29167));
}

int16_t open_actuator_control_cmd_torque_command_8_encode(double value)
{
    return (int16_t)(value / 0.0166667);
}

double open_actuator_control_cmd_torque_command_8_decode(int16_t value)
{
    return ((double)value * 0.0166667);
}

bool open_actuator_control_cmd_torque_command_8_is_in_range(int16_t value)
{
    return ((value >= -480) && (value <= 480));
}

uint8_t open_actuator_control_cmd_torque_close_loop_max_32_encode(double value)
{
    return (uint8_t)(value / 0.186666);
}

double open_actuator_control_cmd_torque_close_loop_max_32_decode(uint8_t value)
{
    return ((double)value * 0.186666);
}

bool open_actuator_control_cmd_torque_close_loop_max_32_is_in_range(uint8_t value)
{
    return (value <= 43u);
}

uint8_t open_actuator_control_cmd_counter_cmd1_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_control_cmd_counter_cmd1_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_control_cmd_counter_cmd1_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

int open_actuator_limits_cmd_pack(
    uint8_t *dst_p,
    const struct open_actuator_limits_cmd_t *src_p,
    size_t size)
{
    if (size < 6u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 6);

    dst_p[0] |= pack_left_shift_u8(src_p->crc8_cmd2, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->counter_cmd2, 4u, 0xf0u);
    dst_p[2] |= pack_left_shift_u16(src_p->velocity_limit, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(src_p->velocity_limit, 8u, 0xffu);
    dst_p[4] |= pack_left_shift_u16(src_p->accel_limit, 0u, 0xffu);
    dst_p[5] |= pack_right_shift_u16(src_p->accel_limit, 8u, 0xffu);

    return (6);
}

int open_actuator_limits_cmd_unpack(
    struct open_actuator_limits_cmd_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 6u) {
        return (-EINVAL);
    }

    dst_p->crc8_cmd2 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->counter_cmd2 = unpack_right_shift_u8(src_p[1], 4u, 0xf0u);
    dst_p->velocity_limit = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    dst_p->velocity_limit |= unpack_left_shift_u16(src_p[3], 8u, 0xffu);
    dst_p->accel_limit = unpack_right_shift_u16(src_p[4], 0u, 0xffu);
    dst_p->accel_limit |= unpack_left_shift_u16(src_p[5], 8u, 0xffu);

    return (0);
}

int open_actuator_limits_cmd_init(struct open_actuator_limits_cmd_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct open_actuator_limits_cmd_t));

    return 0;
}

uint8_t open_actuator_limits_cmd_crc8_cmd2_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_limits_cmd_crc8_cmd2_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_limits_cmd_crc8_cmd2_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t open_actuator_limits_cmd_counter_cmd2_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_limits_cmd_counter_cmd2_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_limits_cmd_counter_cmd2_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

uint16_t open_actuator_limits_cmd_velocity_limit_encode(double value)
{
    return (uint16_t)(value);
}

double open_actuator_limits_cmd_velocity_limit_decode(uint16_t value)
{
    return ((double)value);
}

bool open_actuator_limits_cmd_velocity_limit_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

uint16_t open_actuator_limits_cmd_accel_limit_encode(double value)
{
    return (uint16_t)(value);
}

double open_actuator_limits_cmd_accel_limit_decode(uint16_t value)
{
    return ((double)value);
}

bool open_actuator_limits_cmd_accel_limit_is_in_range(uint16_t value)
{
    (void)value;

    return (true);
}

int open_actuator_control_status_pack(
    uint8_t *dst_p,
    const struct open_actuator_control_status_t *src_p,
    size_t size)
{
    uint16_t torque_actual;

    if (size < 4u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 4);

    dst_p[0] |= pack_left_shift_u8(src_p->crc8_stat1, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->counter_stat1, 4u, 0xf0u);
    torque_actual = (uint16_t)src_p->torque_actual;
    dst_p[2] |= pack_left_shift_u16(torque_actual, 0u, 0xffu);
    dst_p[3] |= pack_right_shift_u16(torque_actual, 8u, 0x03u);
    dst_p[3] |= pack_left_shift_u8(src_p->torque_close_loop_actual, 2u, 0xfcu);

    return (4);
}

int open_actuator_control_status_unpack(
    struct open_actuator_control_status_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t torque_actual;

    if (size < 4u) {
        return (-EINVAL);
    }

    dst_p->crc8_stat1 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->counter_stat1 = unpack_right_shift_u8(src_p[1], 4u, 0xf0u);
    torque_actual = unpack_right_shift_u16(src_p[2], 0u, 0xffu);
    torque_actual |= unpack_left_shift_u16(src_p[3], 8u, 0x03u);

    if ((torque_actual & (1u << 9)) != 0u) {
        torque_actual |= 0xfc00u;
    }

    dst_p->torque_actual = (int16_t)torque_actual;
    dst_p->torque_close_loop_actual = unpack_right_shift_u8(src_p[3], 2u, 0xfcu);

    return (0);
}

int open_actuator_control_status_init(struct open_actuator_control_status_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct open_actuator_control_status_t));

    return 0;
}

uint8_t open_actuator_control_status_crc8_stat1_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_control_status_crc8_stat1_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_control_status_crc8_stat1_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t open_actuator_control_status_counter_stat1_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_control_status_counter_stat1_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_control_status_counter_stat1_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

int16_t open_actuator_control_status_torque_actual_encode(double value)
{
    return (int16_t)(value / 0.015625);
}

double open_actuator_control_status_torque_actual_decode(int16_t value)
{
    return ((double)value * 0.015625);
}

bool open_actuator_control_status_torque_actual_is_in_range(int16_t value)
{
    return ((value >= -512) && (value <= 512));
}

uint8_t open_actuator_control_status_torque_close_loop_actual_encode(double value)
{
    return (uint8_t)(value / 0.125);
}

double open_actuator_control_status_torque_close_loop_actual_decode(uint8_t value)
{
    return ((double)value * 0.125);
}

bool open_actuator_control_status_torque_close_loop_actual_is_in_range(uint8_t value)
{
    return (value <= 64u);
}

int open_actuator_system_status_pack(
    uint8_t *dst_p,
    const struct open_actuator_system_status_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u8(src_p->crc8_stat2, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->counter_stat2, 4u, 0xf0u);
    dst_p[2] |= pack_left_shift_u8(src_p->chip_temp, 0u, 0xffu);

    return (3);
}

int open_actuator_system_status_unpack(
    struct open_actuator_system_status_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->crc8_stat2 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->counter_stat2 = unpack_right_shift_u8(src_p[1], 4u, 0xf0u);
    dst_p->chip_temp = unpack_right_shift_u8(src_p[2], 0u, 0xffu);

    return (0);
}

int open_actuator_system_status_init(struct open_actuator_system_status_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct open_actuator_system_status_t));

    return 0;
}

uint8_t open_actuator_system_status_crc8_stat2_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_system_status_crc8_stat2_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_system_status_crc8_stat2_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t open_actuator_system_status_counter_stat2_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_system_status_counter_stat2_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_system_status_counter_stat2_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

uint8_t open_actuator_system_status_chip_temp_encode(double value)
{
    return (uint8_t)(value - -60.0);
}

double open_actuator_system_status_chip_temp_decode(uint8_t value)
{
    return ((double)value + -60.0);
}

bool open_actuator_system_status_chip_temp_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

int open_actuator_torque_sensor_data_pack(
    uint8_t *dst_p,
    const struct open_actuator_torque_sensor_data_t *src_p,
    size_t size)
{
    uint16_t torque_sense;

    if (size < 3u) {
        return (-EINVAL);
    }

    memset(&dst_p[0], 0, 3);

    dst_p[0] |= pack_left_shift_u8(src_p->crc8_data1, 0u, 0xffu);
    dst_p[1] |= pack_left_shift_u8(src_p->counter_data1, 0u, 0x0fu);
    torque_sense = (uint16_t)src_p->torque_sense;
    dst_p[1] |= pack_left_shift_u16(torque_sense, 4u, 0xf0u);
    dst_p[2] |= pack_right_shift_u16(torque_sense, 4u, 0xffu);

    return (3);
}

int open_actuator_torque_sensor_data_unpack(
    struct open_actuator_torque_sensor_data_t *dst_p,
    const uint8_t *src_p,
    size_t size)
{
    uint16_t torque_sense;

    if (size < 3u) {
        return (-EINVAL);
    }

    dst_p->crc8_data1 = unpack_right_shift_u8(src_p[0], 0u, 0xffu);
    dst_p->counter_data1 = unpack_right_shift_u8(src_p[1], 0u, 0x0fu);
    torque_sense = unpack_right_shift_u16(src_p[1], 4u, 0xf0u);
    torque_sense |= unpack_left_shift_u16(src_p[2], 4u, 0xffu);

    if ((torque_sense & (1u << 11)) != 0u) {
        torque_sense |= 0xf000u;
    }

    dst_p->torque_sense = (int16_t)torque_sense;

    return (0);
}

int open_actuator_torque_sensor_data_init(struct open_actuator_torque_sensor_data_t *msg_p)
{
    if (msg_p == NULL) return -1;

    memset(msg_p, 0, sizeof(struct open_actuator_torque_sensor_data_t));

    return 0;
}

uint8_t open_actuator_torque_sensor_data_crc8_data1_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_torque_sensor_data_crc8_data1_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_torque_sensor_data_crc8_data1_is_in_range(uint8_t value)
{
    (void)value;

    return (true);
}

uint8_t open_actuator_torque_sensor_data_counter_data1_encode(double value)
{
    return (uint8_t)(value);
}

double open_actuator_torque_sensor_data_counter_data1_decode(uint8_t value)
{
    return ((double)value);
}

bool open_actuator_torque_sensor_data_counter_data1_is_in_range(uint8_t value)
{
    return (value <= 15u);
}

int16_t open_actuator_torque_sensor_data_torque_sense_encode(double value)
{
    return (int16_t)(value / 0.01);
}

double open_actuator_torque_sensor_data_torque_sense_decode(int16_t value)
{
    return ((double)value * 0.01);
}

bool open_actuator_torque_sensor_data_torque_sense_is_in_range(int16_t value)
{
    return ((value >= -2000) && (value <= 2000));
}
