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

#ifndef ABS_H
#define ABS_H

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
#define ABS_BREMSE_33_FRAME_ID (0x343u)
#define ABS_BREMSE_10_FRAME_ID (0x140u)
#define ABS_BREMSE_11_FRAME_ID (0x141u)
#define ABS_BREMSE_12_FRAME_ID (0x142u)
#define ABS_BREMSE_13_FRAME_ID (0x143u)
#define ABS_DRS_RX_ID0_FRAME_ID (0x75u)
#define ABS_MM5_10_TX1_FRAME_ID (0x70u)
#define ABS_MM5_10_TX2_FRAME_ID (0x80u)
#define ABS_MM5_10_TX3_FRAME_ID (0x576u)
#define ABS_BREMSE_2_FRAME_ID (0x24au)
#define ABS_ABS_SWITCH_FRAME_ID (0x24cu)
#define ABS_BREMSE_30_FRAME_ID (0x340u)
#define ABS_BREMSE_31_FRAME_ID (0x341u)
#define ABS_BREMSE_32_FRAME_ID (0x342u)
#define ABS_BREMSE_51_FRAME_ID (0x541u)
#define ABS_BREMSE_52_FRAME_ID (0x542u)
#define ABS_BREMSE_50_FRAME_ID (0x560u)
#define ABS_BREMSE_53_FRAME_ID (0x5c0u)

/* Frame lengths in bytes. */
#define ABS_BREMSE_33_LENGTH (8u)
#define ABS_BREMSE_10_LENGTH (8u)
#define ABS_BREMSE_11_LENGTH (8u)
#define ABS_BREMSE_12_LENGTH (8u)
#define ABS_BREMSE_13_LENGTH (8u)
#define ABS_DRS_RX_ID0_LENGTH (8u)
#define ABS_MM5_10_TX1_LENGTH (8u)
#define ABS_MM5_10_TX2_LENGTH (8u)
#define ABS_MM5_10_TX3_LENGTH (8u)
#define ABS_BREMSE_2_LENGTH (8u)
#define ABS_ABS_SWITCH_LENGTH (8u)
#define ABS_BREMSE_30_LENGTH (8u)
#define ABS_BREMSE_31_LENGTH (8u)
#define ABS_BREMSE_32_LENGTH (8u)
#define ABS_BREMSE_51_LENGTH (8u)
#define ABS_BREMSE_52_LENGTH (8u)
#define ABS_BREMSE_50_LENGTH (8u)
#define ABS_BREMSE_53_LENGTH (8u)

/* Extended or standard frame types. */
#define ABS_BREMSE_33_IS_EXTENDED (0)
#define ABS_BREMSE_10_IS_EXTENDED (0)
#define ABS_BREMSE_11_IS_EXTENDED (0)
#define ABS_BREMSE_12_IS_EXTENDED (0)
#define ABS_BREMSE_13_IS_EXTENDED (0)
#define ABS_DRS_RX_ID0_IS_EXTENDED (0)
#define ABS_MM5_10_TX1_IS_EXTENDED (0)
#define ABS_MM5_10_TX2_IS_EXTENDED (0)
#define ABS_MM5_10_TX3_IS_EXTENDED (0)
#define ABS_BREMSE_2_IS_EXTENDED (0)
#define ABS_ABS_SWITCH_IS_EXTENDED (0)
#define ABS_BREMSE_30_IS_EXTENDED (0)
#define ABS_BREMSE_31_IS_EXTENDED (0)
#define ABS_BREMSE_32_IS_EXTENDED (0)
#define ABS_BREMSE_51_IS_EXTENDED (0)
#define ABS_BREMSE_52_IS_EXTENDED (0)
#define ABS_BREMSE_50_IS_EXTENDED (0)
#define ABS_BREMSE_53_IS_EXTENDED (0)

/* Frame cycle times in milliseconds. */
#define ABS_BREMSE_33_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_10_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_11_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_12_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_13_CYCLE_TIME_MS (10u)
#define ABS_DRS_RX_ID0_CYCLE_TIME_MS (10u)
#define ABS_MM5_10_TX1_CYCLE_TIME_MS (10u)
#define ABS_MM5_10_TX2_CYCLE_TIME_MS (10u)
#define ABS_MM5_10_TX3_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_2_CYCLE_TIME_MS (10u)
#define ABS_ABS_SWITCH_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_30_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_31_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_32_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_51_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_52_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_50_CYCLE_TIME_MS (10u)
#define ABS_BREMSE_53_CYCLE_TIME_MS (10u)

/* Signal choices. */
#define ABS_BREMSE_53_DIAG_FL_SIGNAL_OK_CHOICE (0u)
#define ABS_BREMSE_53_DIAG_FL_LINE_ERROR_CHOICE (1u)
#define ABS_BREMSE_53_DIAG_FL_SIGNAL_ERROR_CHOICE (2u)

#define ABS_BREMSE_53_DIAG_FR_SIGNAL_OK_CHOICE (0u)
#define ABS_BREMSE_53_DIAG_FR_LINE_ERROR_CHOICE (1u)
#define ABS_BREMSE_53_DIAG_FR_SIGNAL_ERROR_CHOICE (2u)

#define ABS_BREMSE_53_DIAG_RL_SIGNAL_OK_CHOICE (0u)
#define ABS_BREMSE_53_DIAG_RL_LINE_ERROR_CHOICE (1u)
#define ABS_BREMSE_53_DIAG_RL_SIGNAL_ERROR_CHOICE (2u)

#define ABS_BREMSE_53_DIAG_RR_SIGNAL_OK_CHOICE (0u)
#define ABS_BREMSE_53_DIAG_RR_LINE_ERROR_CHOICE (1u)
#define ABS_BREMSE_53_DIAG_RR_SIGNAL_ERROR_CHOICE (2u)

#define ABS_BREMSE_53_ABS_FAULT_INFO_NO_FAULTS_STORED_CHOICE (0u)
#define ABS_BREMSE_53_ABS_FAULT_INFO_INACTIVE_FAULTS_STORED_CHOICE (1u)
#define ABS_BREMSE_53_ABS_FAULT_INFO_ACTIVE_FAULTS_STORED_CHOICE (2u)

/**
 * Signals in message BREMSE_33.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_33_t {
    /**
     * Radgeschwindigkeit / wheel speed absCtrl FL
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_fl;

    /**
     * Radgeschwindigkeit / wheel speed absCtrl FR
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_fr;

    /**
     * Radgeschwindigkeit / wheel speed absCtrl RL
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_rl;

    /**
     * Radgeschwindigkeit / wheel speed absCtrl RR
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_rr;
};

/**
 * Signals in message BREMSE_10.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_10_t {
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
};

/**
 * Signals in message BREMSE_11.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_11_t {
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
};

/**
 * Signals in message BREMSE_12.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_12_t {
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
};

/**
 * Signals in message BREMSE_13.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_13_t {
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
};

/**
 * Signals in message DRS_RX_ID0.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_drs_rx_id0_t {
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
};

/**
 * Signals in message MM5_10_TX1.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_mm5_10_tx1_t {
    /**
     * Measured yaw rate around the Z axle.
     *
     * Range: 0..65534 (-163.84..163.83 °/s)
     * Scale: 0.005
     * Offset: -163.84
     */
    uint16_t yaw_rate;

    /**
     * Measured lateral acceleration.
     *
     * Range: 0..65534.06817557760953987369082 (-4.1768..4.1765 g)
     * Scale: 0.000127465
     * Offset: -4.1768
     */
    uint16_t ay1;
};

/**
 * Signals in message MM5_10_TX2.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_mm5_10_tx2_t {
    /**
     * Measured roll rate around the X axle.
     *
     * Range: 0..65535 (-163.84..163.835 °/s)
     * Scale: 0.005
     * Offset: -163.84
     */
    uint16_t roll_rate;

    /**
     * Measured longitudional acceleration.
     *
     * Range: 0..65534.06817557760953987369082 (-4.1768..4.1765 g)
     * Scale: 0.000127465
     * Offset: -4.1768
     */
    uint16_t ax1;
};

/**
 * Signals in message MM5_10_TX3.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_mm5_10_tx3_t {
    /**
     * Measured vertical acceleration.
     *
     * Range: 0..65534.06817557760953987369082 (-4.1768..4.1765 g)
     * Scale: 0.000127465
     * Offset: -4.1768
     */
    uint16_t az;
};

/**
 * Signals in message BREMSE_2.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_2_t {
    /**
     * Radgeschwindigkeit / wheel speed direct FL
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_fl_bremse2;

    /**
     * Radgeschwindigkeit / wheel speed direct FR
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_fr_bremse2;

    /**
     * Radgeschwindigkeit / wheel speed direct RL
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_rl_bremse2;

    /**
     * Radgeschwindigkeit / wheel speed direct RR
     *
     * Range: 0..6400 (0..100 m/s)
     * Scale: 0.015625
     * Offset: 0
     */
    uint16_t whlspeed_rr_bremse2;
};

/**
 * Signals in message ABS_Switch.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_abs_switch_t {
    /**
     * Channel to send the swich position via CAN to the ABS.
     *
     * Range: 0..11 (0..11 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t abs_switchposition;
};

/**
 * Signals in message BREMSE_30.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_30_t {
    /**
     * Dummy signal in empty message.
     */
    uint8_t dummy;
};

/**
 * Signals in message BREMSE_31.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_31_t {
    /**
     * Range: -
     * Scale: 1
     * Offset: 0
     */
    uint16_t idle_time;
};

/**
 * Signals in message BREMSE_32.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_32_t {
    /**
     * Fill level of the fluid reservoir of the front axle.
     *
     * Range: 0..200 (0..10 cm3)
     * Scale: 0.05
     * Offset: 0
     */
    uint8_t acc_fa;

    /**
     * Fill level of the fluid reservoir of the rear axle.
     *
     * Range: 0..200 (0..10 cm3)
     * Scale: 0.05
     * Offset: 0
     */
    uint8_t acc_ra;

    /**
     * Bit matrix
     * Bit0 ( 1) Signal Reduced Monitored
     * Bit1 ( 2) Reduced Accuracy
     * Bit2 ( 4) Interfered
     * Bit3 ( 8) Suspicious Plausibility
     * Bit4 (16) Suspicious Lost
     * Bit5 (32) Not Initialized
     * Bit6 (64) Invalid Generic
     * Bit7 (128) Invalid Individual
     *
     * Range: 0..32 (0..32 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t wheel_quality_fl;

    /**
     * Bit matrix
     * Bit0 ( 1) Signal Reduced Monitored
     * Bit1 ( 2) Reduced Accuracy
     * Bit2 ( 4) Interfered
     * Bit3 ( 8) Suspicious Plausibility
     * Bit4 (16) Suspicious Lost
     * Bit5 (32) Not Initialized
     * Bit6 (64) Invalid Generic
     * Bit7 (128) Invalid Individual
     *
     * Range: 0..32 (0..32 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t wheel_quality_fr;

    /**
     * Bit matrix
     * Bit0 ( 1) Signal Reduced Monitored
     * Bit1 ( 2) Reduced Accuracy
     * Bit2 ( 4) Interfered
     * Bit3 ( 8) Suspicious Plausibility
     * Bit4 (16) Suspicious Lost
     * Bit5 (32) Not Initialized
     * Bit6 (64) Invalid Generic
     * Bit7 (128) Invalid Individual
     *
     * Range: 0..32 (0..32 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t wheel_quality_rl;

    /**
     * Bit matrix
     * Bit0 ( 1) Signal Reduced Monitored
     * Bit1 ( 2) Reduced Accuracy
     * Bit2 ( 4) Interfered
     * Bit3 ( 8) Suspicious Plausibility
     * Bit4 (16) Suspicious Lost
     * Bit5 (32) Not Initialized
     * Bit6 (64) Invalid Generic
     * Bit7 (128) Invalid Individual
     *
     * Range: 0..32 (0..32 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t wheel_quality_rr;
};

/**
 * Signals in message BREMSE_51.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_51_t {
    /**
     * Used longitudional acceleration value in the ABS.
     *
     * Range: 0..65535 (-4.1768..4.1736697 g)
     * Scale: 0.00012742
     * Offset: -4.1768
     */
    uint16_t ax1_abs_int;

    /**
     * Used lateral acceleration value in the ABS.
     *
     * Range: 0..65557.21236854496939256003767 (-4.1768..4.1765 g)
     * Scale: 0.00012742
     * Offset: -4.1768
     */
    uint16_t ay1_abs_int;

    /**
     * external info to e.g. MS6 which dbc has to be used. This index increments on changes that make the MS6 interface incompatible to the predecessor CAN interface implementation
     *
     * Range: 0..63 (0..63 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t if_variant;

    /**
     * external info to e.g. MS6 which dbc has to be used. This index increments with added features (rest of MS6 interface stays intact.)
     *
     * Range: 0..63 (0..63 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t if_revision;

    /**
     * external info to e.g. MS6 which dbc has to be used. Checksum
     *
     * Range: 0..15 (0..15 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t if_chksum;
};

/**
 * Signals in message BREMSE_52.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_52_t {
    /**
     * 1=SW version; 2=BB#; 3,4=application name; 5=application date (UTC); 6=deviceType (SW CAN ident, ABS M5=2, ABS M6=3); 7=Serial#
     *
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t mplx_sw_info;

    /**
     * version 1.0 as 0x01(upper), version 100.20 as 0x64(upper)
     *
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t sw_version_high_upper;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bb_dig1;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_01;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_08;

    /**
     * year
     *
     * Range: 0..99 (0..99 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_date_01;

    /**
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t sw_can_ident;

    /**
     * Range: 0..99 (0..99 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t hu_date_year;

    /**
     * version 1.0 as 0x00(lower), version 100.20 as 0x14(lower)
     *
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t sw_version_high_lower;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bb_dig2;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_02;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_09;

    /**
     * month
     *
     * Range: 1..12 (1..12 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_date_02;

    /**
     * Range: 1..12 (1..12 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t hu_date_month;

    /**
     * version 1.0 as 0x01(upper), version 100.20 as 0x64(upper)
     *
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t sw_version_mid_upper;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bb_dig3;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_03;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_10;

    /**
     * day
     *
     * Range: 1..31 (1..31 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_date_03;

    /**
     * Range: 1..31 (1..31 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t hu_date_day;

    /**
     * version 1.0 as 0x00(lower), version 100.20 as 0x14(lower)
     *
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t sw_version_mid_lower;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bb_dig4;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_04;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_11;

    /**
     * hour
     *
     * Range: 0..24 (0..24 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_date_04;

    /**
     * Range: 0..99999 (0..99999 -)
     * Scale: 1
     * Offset: 0
     */
    uint32_t ecu_serial;

    /**
     * version 1.0 as 0x01(upper), version 100.20 as 0x64(upper)
     *
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t sw_version_low_upper;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bb_dig5;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_05;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_12;

    /**
     * minute
     *
     * Range: 0..59 (0..59 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_date_05;

    /**
     * version 1.0 as 0x00(lower), version 100.20 as 0x14(lower)
     *
     * Range: 0..255 (0..255 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t sw_version_low_lower;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bb_dig6;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_06;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_13;

    /**
     * seconds
     *
     * Range: 0..59 (0..59 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_date_06;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bb_dig7;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_07;

    /**
     * Range: 0..255 (0..255 ASCII)
     * Scale: 1
     * Offset: 0
     */
    uint8_t appl_id_14;
};

/**
 * Signals in message BREMSE_50.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_50_t {
    /**
     * Calculated rear axle brake pressure if the front pressure is at 50 bar.
     *
     * Range: 0..1000 (0..100 Bar)
     * Scale: 0.1
     * Offset: 0
     */
    uint16_t brake_bal_at50;

    /**
     * Recommended rear axle brake pressure if the front pressure is at 50 bar.
     *
     * Range: 0..100 (0..100 Bar)
     * Scale: 1
     * Offset: 0
     */
    uint8_t brake_bal_at50_advice;

    /**
     * Percental brake balance on the front axle.
     *
     * Range: 0..1000 (0..100 %)
     * Scale: 0.1
     * Offset: 0
     */
    uint16_t brake_bal_pct;

    /**
     * Recommended percental brake balance on the front axle.
     *
     * Range: 0..100 (0..100 %)
     * Scale: 1
     * Offset: 0
     */
    uint8_t brake_bal_pct_advice;
};

/**
 * Signals in message BREMSE_53.
 *
 * All signal values are as on the CAN bus.
 */
struct abs_bremse_53_t {
    /**
     * Used switch position of the ABS.
     *
     * Range: 1..12 (1..12 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t switch_position;

    /**
     * Brake pressure on the front axle.
     *
     * Range: -2785.058977719528178243774574..27850.58977719528178243774574 (-42.5..425 bar)
     * Scale: 0.01526
     * Offset: 0
     */
    int16_t p_fa;

    /**
     * Bit for the brake light switch.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bls;

    /**
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t bremse_53_cnt;

    /**
     * Bit will jump to 1, if the ABS control is deactivated by a fault.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t abs_malfunction;

    /**
     * Bit will jump to 1, when the ABS control is active.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t abs_active;

    /**
     * Bit will jump to 1, when the EBD is deactivated due to a fault.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t ebd_lamp;

    /**
     * Bit will jump to 1, when the ABS control is deactivated due to a fault, switch to the off position or while working with RaceABS.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t abs_lamp;

    /**
     * Value to show faults related to the wheel speed sensor.
     * 0 - Signal ok, 1 - Wiring related fault, 2 - Signal related fault
     *
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_fl;

    /**
     * Value to show faults related to the wheel speed sensor.
     * 0 - Signal ok, 1 - Wiring related fault, 2 - Signal related fault
     *
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_fr;

    /**
     * Value to show faults related to the wheel speed sensor.
     * 0 - Signal ok, 1 - Wiring related fault, 2 - Signal related fault
     *
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_rl;

    /**
     * Value to show faults related to the wheel speed sensor.
     * 0 - Signal ok, 1 - Wiring related fault, 2 - Signal related fault
     *
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_rr;

    /**
     * Bit to show, if a ABS error related to the hydraulic unit is present
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_abs_unit;

    /**
     * Bit to show, if a ABS error related to the fuse or power supply of the ABS valves is present.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_fuse_valve;

    /**
     * Bit to show, if a ABS error related to the fuse or power supply of the ABS pump is present.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_fuse_pump;

    /**
     * Bit to show, if the pressure sensor FA is working properly. An error is pressent, if the bit is 1.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_p_fa;

    /**
     * Bit to show, if the pressure sensor RA is working properly. An error is pressent, if the bit is 1.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_p_ra;

    /**
     * Bit to show, if the yaw rate sensor is working properly. An error is pressent, if the bit is 1.
     *
     * Range: 0..1 (0..1 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t diag_yrs;

    /**
     * Bit matrix to show if a fault or a active fault is stored in the ABS. Bit will also show minor errors which do  not shut down the ABS controller.
     *
     * Range: 0..3 (0..3 -)
     * Scale: 1
     * Offset: 0
     */
    uint8_t abs_fault_info;

    /**
     * Brake pressure on the rear axle.
     *
     * Range: -2785.058977719528178243774574..27850.58977719528178243774574 (-42.5..425 bar)
     * Scale: 0.01526
     * Offset: 0
     */
    int16_t p_ra;
};

/**
 * Pack message BREMSE_33.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_33_pack(
    uint8_t *dst_p,
    const struct abs_bremse_33_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_33.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_33_unpack(
    struct abs_bremse_33_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_33_whlspeed_fl_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_33_whlspeed_fl_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_33_whlspeed_fl_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_33_whlspeed_fr_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_33_whlspeed_fr_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_33_whlspeed_fr_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_33_whlspeed_rl_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_33_whlspeed_rl_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_33_whlspeed_rl_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_33_whlspeed_rr_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_33_whlspeed_rr_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_33_whlspeed_rr_is_in_range(uint16_t value);

/**
 * Pack message BREMSE_10.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_10_pack(
    uint8_t *dst_p,
    const struct abs_bremse_10_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_10.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_10_unpack(
    struct abs_bremse_10_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Pack message BREMSE_11.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_11_pack(
    uint8_t *dst_p,
    const struct abs_bremse_11_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_11.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_11_unpack(
    struct abs_bremse_11_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Pack message BREMSE_12.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_12_pack(
    uint8_t *dst_p,
    const struct abs_bremse_12_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_12.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_12_unpack(
    struct abs_bremse_12_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Pack message BREMSE_13.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_13_pack(
    uint8_t *dst_p,
    const struct abs_bremse_13_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_13.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_13_unpack(
    struct abs_bremse_13_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Pack message DRS_RX_ID0.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_drs_rx_id0_pack(
    uint8_t *dst_p,
    const struct abs_drs_rx_id0_t *src_p,
    size_t size);

/**
 * Unpack message DRS_RX_ID0.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_drs_rx_id0_unpack(
    struct abs_drs_rx_id0_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Pack message MM5_10_TX1.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_mm5_10_tx1_pack(
    uint8_t *dst_p,
    const struct abs_mm5_10_tx1_t *src_p,
    size_t size);

/**
 * Unpack message MM5_10_TX1.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_mm5_10_tx1_unpack(
    struct abs_mm5_10_tx1_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_mm5_10_tx1_yaw_rate_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_mm5_10_tx1_yaw_rate_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_mm5_10_tx1_yaw_rate_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_mm5_10_tx1_ay1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_mm5_10_tx1_ay1_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_mm5_10_tx1_ay1_is_in_range(uint16_t value);

/**
 * Pack message MM5_10_TX2.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_mm5_10_tx2_pack(
    uint8_t *dst_p,
    const struct abs_mm5_10_tx2_t *src_p,
    size_t size);

/**
 * Unpack message MM5_10_TX2.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_mm5_10_tx2_unpack(
    struct abs_mm5_10_tx2_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_mm5_10_tx2_roll_rate_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_mm5_10_tx2_roll_rate_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_mm5_10_tx2_roll_rate_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_mm5_10_tx2_ax1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_mm5_10_tx2_ax1_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_mm5_10_tx2_ax1_is_in_range(uint16_t value);

/**
 * Pack message MM5_10_TX3.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_mm5_10_tx3_pack(
    uint8_t *dst_p,
    const struct abs_mm5_10_tx3_t *src_p,
    size_t size);

/**
 * Unpack message MM5_10_TX3.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_mm5_10_tx3_unpack(
    struct abs_mm5_10_tx3_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_mm5_10_tx3_az_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_mm5_10_tx3_az_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_mm5_10_tx3_az_is_in_range(uint16_t value);

/**
 * Pack message BREMSE_2.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_2_pack(
    uint8_t *dst_p,
    const struct abs_bremse_2_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_2.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_2_unpack(
    struct abs_bremse_2_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_2_whlspeed_fl_bremse2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_2_whlspeed_fl_bremse2_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_2_whlspeed_fl_bremse2_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_2_whlspeed_fr_bremse2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_2_whlspeed_fr_bremse2_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_2_whlspeed_fr_bremse2_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_2_whlspeed_rl_bremse2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_2_whlspeed_rl_bremse2_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_2_whlspeed_rl_bremse2_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_2_whlspeed_rr_bremse2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_2_whlspeed_rr_bremse2_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_2_whlspeed_rr_bremse2_is_in_range(uint16_t value);

/**
 * Pack message ABS_Switch.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_abs_switch_pack(
    uint8_t *dst_p,
    const struct abs_abs_switch_t *src_p,
    size_t size);

/**
 * Unpack message ABS_Switch.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_abs_switch_unpack(
    struct abs_abs_switch_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_abs_switch_abs_switchposition_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_abs_switch_abs_switchposition_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_abs_switch_abs_switchposition_is_in_range(uint8_t value);

/**
 * Pack message BREMSE_30.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_30_pack(
    uint8_t *dst_p,
    const struct abs_bremse_30_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_30.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_30_unpack(
    struct abs_bremse_30_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Pack message BREMSE_31.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_31_pack(
    uint8_t *dst_p,
    const struct abs_bremse_31_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_31.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_31_unpack(
    struct abs_bremse_31_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_31_idle_time_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_31_idle_time_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_31_idle_time_is_in_range(uint16_t value);

/**
 * Pack message BREMSE_32.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_32_pack(
    uint8_t *dst_p,
    const struct abs_bremse_32_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_32.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_32_unpack(
    struct abs_bremse_32_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_32_acc_fa_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_32_acc_fa_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_32_acc_fa_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_32_acc_ra_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_32_acc_ra_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_32_acc_ra_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_32_wheel_quality_fl_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_32_wheel_quality_fl_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_32_wheel_quality_fl_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_32_wheel_quality_fr_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_32_wheel_quality_fr_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_32_wheel_quality_fr_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_32_wheel_quality_rl_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_32_wheel_quality_rl_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_32_wheel_quality_rl_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_32_wheel_quality_rr_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_32_wheel_quality_rr_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_32_wheel_quality_rr_is_in_range(uint8_t value);

/**
 * Pack message BREMSE_51.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_51_pack(
    uint8_t *dst_p,
    const struct abs_bremse_51_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_51.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_51_unpack(
    struct abs_bremse_51_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_51_ax1_abs_int_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_51_ax1_abs_int_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_51_ax1_abs_int_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_51_ay1_abs_int_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_51_ay1_abs_int_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_51_ay1_abs_int_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_51_if_variant_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_51_if_variant_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_51_if_variant_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_51_if_revision_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_51_if_revision_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_51_if_revision_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_51_if_chksum_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_51_if_chksum_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_51_if_chksum_is_in_range(uint8_t value);

/**
 * Pack message BREMSE_52.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_52_pack(
    uint8_t *dst_p,
    const struct abs_bremse_52_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_52.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_52_unpack(
    struct abs_bremse_52_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_mplx_sw_info_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_mplx_sw_info_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_mplx_sw_info_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_sw_version_high_upper_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_sw_version_high_upper_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_sw_version_high_upper_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_bb_dig1_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_bb_dig1_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_bb_dig1_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_01_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_01_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_01_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_08_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_08_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_08_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_date_01_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_date_01_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_date_01_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_sw_can_ident_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_sw_can_ident_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_sw_can_ident_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_hu_date_year_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_hu_date_year_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_hu_date_year_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_sw_version_high_lower_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_sw_version_high_lower_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_sw_version_high_lower_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_bb_dig2_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_bb_dig2_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_bb_dig2_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_02_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_02_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_02_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_09_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_09_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_09_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_date_02_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_date_02_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_date_02_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_hu_date_month_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_hu_date_month_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_hu_date_month_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_sw_version_mid_upper_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_sw_version_mid_upper_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_sw_version_mid_upper_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_bb_dig3_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_bb_dig3_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_bb_dig3_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_03_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_03_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_03_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_10_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_10_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_10_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_date_03_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_date_03_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_date_03_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_hu_date_day_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_hu_date_day_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_hu_date_day_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_sw_version_mid_lower_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_sw_version_mid_lower_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_sw_version_mid_lower_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_bb_dig4_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_bb_dig4_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_bb_dig4_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_04_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_04_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_04_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_11_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_11_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_11_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_date_04_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_date_04_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_date_04_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint32_t abs_bremse_52_ecu_serial_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_ecu_serial_decode(uint32_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_ecu_serial_is_in_range(uint32_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_sw_version_low_upper_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_sw_version_low_upper_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_sw_version_low_upper_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_bb_dig5_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_bb_dig5_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_bb_dig5_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_05_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_05_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_05_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_12_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_12_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_12_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_date_05_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_date_05_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_date_05_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_sw_version_low_lower_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_sw_version_low_lower_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_sw_version_low_lower_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_bb_dig6_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_bb_dig6_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_bb_dig6_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_06_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_06_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_06_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_13_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_13_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_13_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_date_06_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_date_06_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_date_06_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_bb_dig7_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_bb_dig7_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_bb_dig7_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_07_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_07_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_07_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_52_appl_id_14_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_52_appl_id_14_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_52_appl_id_14_is_in_range(uint8_t value);

/**
 * Pack message BREMSE_50.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_50_pack(
    uint8_t *dst_p,
    const struct abs_bremse_50_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_50.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_50_unpack(
    struct abs_bremse_50_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_50_brake_bal_at50_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_50_brake_bal_at50_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_50_brake_bal_at50_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_50_brake_bal_at50_advice_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_50_brake_bal_at50_advice_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_50_brake_bal_at50_advice_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint16_t abs_bremse_50_brake_bal_pct_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_50_brake_bal_pct_decode(uint16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_50_brake_bal_pct_is_in_range(uint16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_50_brake_bal_pct_advice_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_50_brake_bal_pct_advice_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_50_brake_bal_pct_advice_is_in_range(uint8_t value);

/**
 * Pack message BREMSE_53.
 *
 * @param[out] dst_p Buffer to pack the message into.
 * @param[in] src_p Data to pack.
 * @param[in] size Size of dst_p.
 *
 * @return Size of packed data, or negative error code.
 */
int abs_bremse_53_pack(
    uint8_t *dst_p,
    const struct abs_bremse_53_t *src_p,
    size_t size);

/**
 * Unpack message BREMSE_53.
 *
 * @param[out] dst_p Object to unpack the message into.
 * @param[in] src_p Message to unpack.
 * @param[in] size Size of src_p.
 *
 * @return zero(0) or negative error code.
 */
int abs_bremse_53_unpack(
    struct abs_bremse_53_t *dst_p,
    const uint8_t *src_p,
    size_t size);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_switch_position_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_switch_position_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_switch_position_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t abs_bremse_53_p_fa_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_p_fa_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_p_fa_is_in_range(int16_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_bls_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_bls_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_bls_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_bremse_53_cnt_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_bremse_53_cnt_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_bremse_53_cnt_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_abs_malfunction_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_abs_malfunction_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_abs_malfunction_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_abs_active_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_abs_active_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_abs_active_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_ebd_lamp_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_ebd_lamp_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_ebd_lamp_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_abs_lamp_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_abs_lamp_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_abs_lamp_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_fl_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_fl_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_fl_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_fr_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_fr_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_fr_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_rl_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_rl_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_rl_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_rr_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_rr_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_rr_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_abs_unit_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_abs_unit_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_abs_unit_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_fuse_valve_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_fuse_valve_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_fuse_valve_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_fuse_pump_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_fuse_pump_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_fuse_pump_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_p_fa_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_p_fa_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_p_fa_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_p_ra_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_p_ra_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_p_ra_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_diag_yrs_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_diag_yrs_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_diag_yrs_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
uint8_t abs_bremse_53_abs_fault_info_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_abs_fault_info_decode(uint8_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_abs_fault_info_is_in_range(uint8_t value);

/**
 * Encode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to encode.
 *
 * @return Encoded signal.
 */
int16_t abs_bremse_53_p_ra_encode(double value);

/**
 * Decode given signal by applying scaling and offset.
 *
 * @param[in] value Signal to decode.
 *
 * @return Decoded signal.
 */
double abs_bremse_53_p_ra_decode(int16_t value);

/**
 * Check that given signal is in allowed range.
 *
 * @param[in] value Signal to check.
 *
 * @return true if in range, false otherwise.
 */
bool abs_bremse_53_p_ra_is_in_range(int16_t value);


#ifdef __cplusplus
}
#endif

#endif
