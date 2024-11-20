/* Include the generated files first to test that all required header
   files are included. */
#include "vehicle.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "test_framework/unity.h"

void valid_initial_value(void) {
    struct vehicle_rt_imu06_accel_t vehicle_rt_imu06_accel;
    int result_valid = vehicle_rt_imu06_accel_init(&vehicle_rt_imu06_accel);

    TEST_ASSERT_EQUAL(result_valid, 0);
    TEST_ASSERT_EQUAL(vehicle_rt_imu06_accel.accel_lateral, -30000);
    TEST_ASSERT_EQUAL(vehicle_rt_imu06_accel.accel_longitudinal, 32767);
    TEST_ASSERT_EQUAL(vehicle_rt_imu06_accel.accel_vertical, 16120);
    TEST_ASSERT_EQUAL(vehicle_rt_imu06_accel.accuracy_accel, 127);
    TEST_ASSERT_EQUAL(vehicle_rt_imu06_accel.validity_accel_lateral, 1);
    TEST_ASSERT_EQUAL(vehicle_rt_imu06_accel.validity_accel_longitudinal, 0);
    TEST_ASSERT_EQUAL(vehicle_rt_imu06_accel.validity_accel_vertical, 0);
}

void absence_of_initial_value(void) {
    struct vehicle_rt_imu06_accel_t* ptr = NULL;
    int result_invalid = vehicle_rt_imu06_accel_init(ptr);

    TEST_ASSERT_EQUAL(result_invalid, -1);
}

void nullptr_ptr_initial_value(void) {
    struct vehicle_rt_dl1_mk3_speed_t vehicle_rt_dl1_mk3_speed;
    int result_valid = vehicle_rt_dl1_mk3_speed_init(&vehicle_rt_dl1_mk3_speed);

    TEST_ASSERT_EQUAL(result_valid, 0);
    TEST_ASSERT_EQUAL(vehicle_rt_dl1_mk3_speed.accuracy_speed, 0);
    TEST_ASSERT_EQUAL(vehicle_rt_dl1_mk3_speed.speed, 0);
    TEST_ASSERT_EQUAL(vehicle_rt_dl1_mk3_speed.validity_speed, 0);
}
