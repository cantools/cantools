// Introducing google test because cannot get nala to build with bazel - too much effort for too little reward
// to create interopability between python, c, make, and bazel
#include <gtest/gtest.h>

#include "motohawk.h"
#include "signed.h"
#include "css__electronics_sae_j1939_demo.h"

// Copy struct pack/unpack in test_basic.motohawk_example_message_t
TEST(motohawk_dbc, struct_unpack) {
    uint8_t buffer[] = "\xc0\x06\xe0\x00\x00\x00\x00\x00";
    ExampleMessage m(buffer, 8u);

    // Confirm unpacking values is successful
    EXPECT_EQ(1, m.Enable.Raw());
    EXPECT_EQ(32, m.AverageRadius.Raw());
    EXPECT_EQ(55, m.Temperature.Raw());

    // Confirm buffer to_string() accuracy
    auto str = m.to_string();
    EXPECT_EQ(str, "c006e00000000000");
    
    // Confirm input buffer equal to buffer accessor
    auto output_buffer = m.buffer();
    for (size_t i = 0; i < 8; ++i) {
        EXPECT_EQ(buffer[i], output_buffer[i]);
    }

    // Confirm after setting new signals values that original buffer pointer is modified
    EXPECT_TRUE(m.set_Enable(0));
    EXPECT_TRUE(m.set_AverageRadius(0.5));
    EXPECT_TRUE(m.set_Temperature(249));
    output_buffer = m.buffer();
    for (size_t i = 0; i < 8; ++i) {
        EXPECT_EQ(buffer[i], output_buffer[i]);
    }
}

TEST(motohawk_dbc, struct_pack) {
    ExampleMessage m;

    // Set Enable and confirm
    EXPECT_TRUE(m.set_Enable(1));
    EXPECT_EQ(1, m.Enable.Real());

    // Set Radius and confirm Enable and AverageRadius signals
    EXPECT_TRUE(m.set_AverageRadius(0.5));
    EXPECT_EQ(1, m.Enable.Real());
    EXPECT_EQ(0.5, m.AverageRadius.Real());

    // Set Temperature and confirm Enable, AverageRadius, and Temperature signals
    EXPECT_TRUE(m.set_Temperature(250));
    EXPECT_EQ(1, m.Enable.Real());
    EXPECT_EQ(0.5, m.AverageRadius.Real());
    EXPECT_EQ(250, m.Temperature.Real());
}

TEST(signed_dbc, struct_pack) {
    Message64 m;

    EXPECT_TRUE(m.set_s64(-5));
    EXPECT_EQ(-5, m.s64.Raw());
    EXPECT_EQ(-5, m.s64.Real());
    EXPECT_EQ("fbffffffffffffff", m.to_string());

    m.clear();
    EXPECT_EQ("0000000000000000", m.to_string());
}

TEST(css__electronics_sae_j1939_demo, spns) {
    EEC1 eec1;
    CCVS1 ccvs1;

    EXPECT_EQ(eec1.EngineSpeed.SPN(), 190);
    EXPECT_EQ(ccvs1.WheelBasedVehicleSpeed.SPN(), 84);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
