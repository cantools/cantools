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

#ifndef OPEN_ACTUATOR_H
#define OPEN_ACTUATOR_H

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
#define OPEN_ACTUATOR_CONTROL_CMD_FRAME_ID (0xfau)
#define OPEN_ACTUATOR_LIMITS_CMD_FRAME_ID (0xfbu)
#define OPEN_ACTUATOR_CONTROL_STATUS_FRAME_ID (0xfcu)
#define OPEN_ACTUATOR_SYSTEM_STATUS_FRAME_ID (0xfdu)
#define OPEN_ACTUATOR_TORQUE_SENSOR_DATA_FRAME_ID (0x71u)

/* Frame lengths in bytes. */
#define OPEN_ACTUATOR_CONTROL_CMD_LENGTH (7u)
#define OPEN_ACTUATOR_LIMITS_CMD_LENGTH (6u)
#define OPEN_ACTUATOR_CONTROL_STATUS_LENGTH (4u)
#define OPEN_ACTUATOR_SYSTEM_STATUS_LENGTH (3u)
#define OPEN_ACTUATOR_TORQUE_SENSOR_DATA_LENGTH (3u)

/* Extended or standard frame types. */
#define OPEN_ACTUATOR_CONTROL_CMD_IS_EXTENDED (0)
#define OPEN_ACTUATOR_LIMITS_CMD_IS_EXTENDED (0)
#define OPEN_ACTUATOR_CONTROL_STATUS_IS_EXTENDED (0)
#define OPEN_ACTUATOR_SYSTEM_STATUS_IS_EXTENDED (0)
#define OPEN_ACTUATOR_TORQUE_SENSOR_DATA_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */


/* Signal choices. */
#define OPEN_ACTUATOR_CONTROL_CMD_TARGET_MODE_OFF_CHOICE (0u)
#define OPEN_ACTUATOR_CONTROL_CMD_TARGET_MODE_ASSIST_CHOICE (1u)
#define OPEN_ACTUATOR_CONTROL_CMD_TARGET_MODE_POSITION_RELATIVE_CHOICE (2u)
#define OPEN_ACTUATOR_CONTROL_CMD_TARGET_MODE_TORQUE_CHOICE (3u)
#define OPEN_ACTUATOR_CONTROL_CMD_TARGET_MODE_POSITION_ABSOLUTE_CHOICE (4u)

/* Frame Names. */
#define OPEN_ACTUATOR_CONTROL_CMD_NAME "ControlCmd"
#define OPEN_ACTUATOR_LIMITS_CMD_NAME "LimitsCmd"
#define OPEN_ACTUATOR_CONTROL_STATUS_NAME "ControlStatus"
#define OPEN_ACTUATOR_SYSTEM_STATUS_NAME "SystemStatus"
#define OPEN_ACTUATOR_TORQUE_SENSOR_DATA_NAME "TorqueSensorData"

/* Signal Names. */
#define OPEN_ACTUATOR_CONTROL_CMD_CRC8_CMD1_NAME "CRC8_CMD1"
#define OPEN_ACTUATOR_CONTROL_CMD_TARGET_MODE_NAME "TargetMode"
#define OPEN_ACTUATOR_CONTROL_CMD_TARGET_MOTOR_ID_CMD1_NAME "TargetMotorID_CMD1"
#define OPEN_ACTUATOR_CONTROL_CMD_POSITION_CMD_64_NAME "PositionCmd_64"
#define OPEN_ACTUATOR_CONTROL_CMD_TORQUE_COMMAND_8_NAME "TorqueCommand_8"
#define OPEN_ACTUATOR_CONTROL_CMD_TORQUE_CLOSE_LOOP_MAX_32_NAME "TorqueCloseLoopMax_32"
#define OPEN_ACTUATOR_CONTROL_CMD_COUNTER_CMD1_NAME "Counter_CMD1"
#define OPEN_ACTUATOR_LIMITS_CMD_CRC8_CMD2_NAME "CRC8_CMD2"
#define OPEN_ACTUATOR_LIMITS_CMD_COUNTER_CMD2_NAME "Counter_CMD2"
#define OPEN_ACTUATOR_LIMITS_CMD_VELOCITY_LIMIT_NAME "VelocityLimit"
#define OPEN_ACTUATOR_LIMITS_CMD_ACCEL_LIMIT_NAME "AccelLimit"
#define OPEN_ACTUATOR_CONTROL_STATUS_CRC8_STAT1_NAME "CRC8_STAT1"
#define OPEN_ACTUATOR_CONTROL_STATUS_COUNTER_STAT1_NAME "Counter_STAT1"
#define OPEN_ACTUATOR_CONTROL_STATUS_TORQUE_ACTUAL_NAME "TorqueActual"
#define OPEN_ACTUATOR_CONTROL_STATUS_TORQUE_CLOSE_LOOP_ACTUAL_NAME "TorqueCloseLoopActual"
#define OPEN_ACTUATOR_SYSTEM_STATUS_CRC8_STAT2_NAME "CRC8_STAT2"
#define OPEN_ACTUATOR_SYSTEM_STATUS_COUNTER_STAT2_NAME "Counter_STAT2"
#define OPEN_ACTUATOR_SYSTEM_STATUS_CHIP_TEMP_NAME "ChipTemp"
#define OPEN_ACTUATOR_TORQUE_SENSOR_DATA_CRC8_DATA1_NAME "CRC8_DATA1"
#define OPEN_ACTUATOR_TORQUE_SENSOR_DATA_COUNTER_DATA1_NAME "Counter_DATA1"
#define OPEN_ACTUATOR_TORQUE_SENSOR_DATA_TORQUE_SENSE_NAME "TorqueSense"

/**
 * Signals in message ControlCmd.
 *
 * All signal values are as on the CAN bus.
 */
struct open_actuator_control_cmd_t {
    /**
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t crc8_cmd1;

    /**
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t target_mode;

    /**
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t target_motor_id_cmd1;

    /**
     * Output relative position.
     * Alternative usage - absolute output position
     * Factor = 64_const / 200steps / 256microsteps *360deg / FinalGearRatio / GearboxRatio
     *
     * Range: -29167..29167 (-450..450 deg)
     * Scale: 0.0154286
     * Offset: 0
     */
    int16_t position_cmd_64;

    /**
     * Factor:
     * 8_const * 1A/1000mA * MotorRatedTorque / MotorRatedCurrent * GearboxRatio * FinalGearRatio
     *
     * Range: -480..480 (-8..8 N*m)
     * Scale: 0.0166667
     * Offset: 0
     */
    int16_t torque_command_8;

    /**
     * For TorqueCmd > 0
     * Max positive close loop torque on top of TorqueCmd (outward torque) and below 0 (centering torque).
     * For TorqueCmd < 0;
     * Max negative close loop torque on top of TorqueCmd (outward torque) and above 0 (centering torque).
     * Factor:
     * 32_const * 1A/1000mA * MotorRatedTorque / MotorRatedCurrent * GearboxRatio * FinalGearRatio
     *
     * Range: 0..43 (0..8 N*m)
     * Scale: 0.186666
     * Offset: 0
     */
    uint8_t torque_close_loop_max_32;

    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t counter_cmd1;
};

/**
 * Signals in message LimitsCmd.
 *
 * All signal values are as on the CAN bus.
 */
struct open_actuator_limits_cmd_t {
    /**
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t crc8_cmd2;

    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t counter_cmd2;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint16_t velocity_limit;

    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint16_t accel_limit;
};

/**
 * Signals in message ControlStatus.
 *
 * All signal values are as on the CAN bus.
 */
struct open_actuator_control_status_t {
    /**
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t crc8_stat1;

    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t counter_stat1;

    /**
     * Range: -512..512 (-8..8 N*m)
     * Scale: 0.015625
     * Offset: 0
     */
    int16_t torque_actual;

    /**
     * Range: 0..64 (0..8 N*m)
     * Scale: 0.125
     * Offset: 0
     */
    uint8_t torque_close_loop_actual;
};

/**
 * Signals in message SystemStatus.
 *
 * All signal values are as on the CAN bus.
 */
struct open_actuator_system_status_t {
    /**
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t crc8_stat2;

    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t counter_stat2;

    /**
     * Range: 0..255 (-60..195 C)
     * Scale: 1
     * Offset: -60
     */
    uint8_t chip_temp;
};

/**
 * Signals in message TorqueSensorData.
 *
 * All signal values are as on the CAN bus.
 */
struct open_actuator_torque_sensor_data_t {
    /**
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t crc8_data1;

    /**
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t counter_data1;

    /**
     * Strain gauge torque measured
     *
     * Range: -2000..2000 (-20..20 N*m)
     * Scale: 0.01
     * Offset: 0
     */
    int16_t torque_sense;
};

/**
 * Pack message ControlCmd.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int open_actuator_control_cmd_pack(
    uint8_t *dst_p,
    const struct open_actuator_control_cmd_t *src_p,
    size_t size);

/**
 * Unpack message ControlCmd.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int open_actuator_control_cmd_unpack(
    struct open_actuator_control_cmd_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from ControlCmd.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int open_actuator_control_cmd_init(struct open_actuator_control_cmd_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_cmd_crc8_cmd1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_cmd_crc8_cmd1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_cmd_crc8_cmd1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_cmd_target_mode_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_cmd_target_mode_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_cmd_target_mode_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_cmd_target_motor_id_cmd1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_cmd_target_motor_id_cmd1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_cmd_target_motor_id_cmd1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t open_actuator_control_cmd_position_cmd_64_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_cmd_position_cmd_64_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_cmd_position_cmd_64_is_in_range(int16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t open_actuator_control_cmd_torque_command_8_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_cmd_torque_command_8_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_cmd_torque_command_8_is_in_range(int16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_cmd_torque_close_loop_max_32_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_cmd_torque_close_loop_max_32_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_cmd_torque_close_loop_max_32_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_cmd_counter_cmd1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_cmd_counter_cmd1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_cmd_counter_cmd1_is_in_range(uint8_t value);

/**
 * Pack message LimitsCmd.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int open_actuator_limits_cmd_pack(
    uint8_t *dst_p,
    const struct open_actuator_limits_cmd_t *src_p,
    size_t size);

/**
 * Unpack message LimitsCmd.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int open_actuator_limits_cmd_unpack(
    struct open_actuator_limits_cmd_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from LimitsCmd.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int open_actuator_limits_cmd_init(struct open_actuator_limits_cmd_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_limits_cmd_crc8_cmd2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_limits_cmd_crc8_cmd2_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_limits_cmd_crc8_cmd2_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_limits_cmd_counter_cmd2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_limits_cmd_counter_cmd2_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_limits_cmd_counter_cmd2_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t open_actuator_limits_cmd_velocity_limit_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_limits_cmd_velocity_limit_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_limits_cmd_velocity_limit_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t open_actuator_limits_cmd_accel_limit_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_limits_cmd_accel_limit_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_limits_cmd_accel_limit_is_in_range(uint16_t value);

/**
 * Pack message ControlStatus.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int open_actuator_control_status_pack(
    uint8_t *dst_p,
    const struct open_actuator_control_status_t *src_p,
    size_t size);

/**
 * Unpack message ControlStatus.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int open_actuator_control_status_unpack(
    struct open_actuator_control_status_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from ControlStatus.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int open_actuator_control_status_init(struct open_actuator_control_status_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_status_crc8_stat1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_status_crc8_stat1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_status_crc8_stat1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_status_counter_stat1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_status_counter_stat1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_status_counter_stat1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t open_actuator_control_status_torque_actual_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_status_torque_actual_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_status_torque_actual_is_in_range(int16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_control_status_torque_close_loop_actual_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_control_status_torque_close_loop_actual_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_control_status_torque_close_loop_actual_is_in_range(uint8_t value);

/**
 * Pack message SystemStatus.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int open_actuator_system_status_pack(
    uint8_t *dst_p,
    const struct open_actuator_system_status_t *src_p,
    size_t size);

/**
 * Unpack message SystemStatus.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int open_actuator_system_status_unpack(
    struct open_actuator_system_status_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from SystemStatus.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int open_actuator_system_status_init(struct open_actuator_system_status_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_system_status_crc8_stat2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_system_status_crc8_stat2_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_system_status_crc8_stat2_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_system_status_counter_stat2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_system_status_counter_stat2_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_system_status_counter_stat2_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_system_status_chip_temp_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_system_status_chip_temp_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_system_status_chip_temp_is_in_range(uint8_t value);

/**
 * Pack message TorqueSensorData.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int open_actuator_torque_sensor_data_pack(
    uint8_t *dst_p,
    const struct open_actuator_torque_sensor_data_t *src_p,
    size_t size);

/**
 * Unpack message TorqueSensorData.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int open_actuator_torque_sensor_data_unpack(
    struct open_actuator_torque_sensor_data_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Init message fields to default values from TorqueSensorData.
 *
 * @param[in] msg_p Message to init.
 *
 * @return zero(0) on success or (-1) in case of nullptr argument.
 */
int open_actuator_torque_sensor_data_init(struct open_actuator_torque_sensor_data_t *msg_p);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_torque_sensor_data_crc8_data1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_torque_sensor_data_crc8_data1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_torque_sensor_data_crc8_data1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t open_actuator_torque_sensor_data_counter_data1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_torque_sensor_data_counter_data1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_torque_sensor_data_counter_data1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t open_actuator_torque_sensor_data_torque_sense_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double open_actuator_torque_sensor_data_torque_sense_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool open_actuator_torque_sensor_data_torque_sense_is_in_range(int16_t value);


#ifdef __cplusplus
}
#endif

#endif
