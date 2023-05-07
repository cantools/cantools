/* Include the generated files first to test that all required header
   files are included. */
#include "files/c_source/vehicle.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "nala.h"

TEST(valid_initial_value)
{
    struct vehicle_rt_imu06_accel_t vehicle_rt_imu06_accel;
    int result_valid = vehicle_rt_imu06_accel_init(&vehicle_rt_imu06_accel);

    ASSERT_EQ(result_valid, 0);
    ASSERT_EQ(vehicle_rt_imu06_accel.accel_lateral, -30000);
    ASSERT_EQ(vehicle_rt_imu06_accel.accel_longitudinal, 32767);
    ASSERT_EQ(vehicle_rt_imu06_accel.accel_vertical, 16120);
    ASSERT_EQ(vehicle_rt_imu06_accel.accuracy_accel, 127);
    ASSERT_EQ(vehicle_rt_imu06_accel.validity_accel_lateral, 1);
    ASSERT_EQ(vehicle_rt_imu06_accel.validity_accel_longitudinal, 0);
    ASSERT_EQ(vehicle_rt_imu06_accel.validity_accel_vertical, 0);
}

TEST(absence_of_initial_value)
{
    struct vehicle_rt_imu06_accel_t* ptr = NULL;
    int result_invalid = vehicle_rt_imu06_accel_init(ptr);

    ASSERT_EQ(result_invalid, -1);
}

TEST(nullptr_ptr_initial_value)
{
    struct vehicle_rt_dl1_mk3_speed_t vehicle_rt_dl1_mk3_speed;
    int result_valid = vehicle_rt_dl1_mk3_speed_init(&vehicle_rt_dl1_mk3_speed);

    ASSERT_EQ(result_valid, 0);
    ASSERT_EQ(vehicle_rt_dl1_mk3_speed.accuracy_speed, 0);
    ASSERT_EQ(vehicle_rt_dl1_mk3_speed.speed, 0);
    ASSERT_EQ(vehicle_rt_dl1_mk3_speed.validity_speed, 0);
}
