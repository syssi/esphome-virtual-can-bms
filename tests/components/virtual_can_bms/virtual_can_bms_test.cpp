#include <gtest/gtest.h>
#include "esphome/components/sensor/sensor.h"
#include "common.h"

namespace esphome::virtual_can_bms::testing {

class VirtualCanBmsTest : public ::testing::Test {
 protected:
  MockCanbus canbus_;
  TestableVirtualCanBms bms_;

  sensor::Sensor charge_voltage_;
  sensor::Sensor charge_current_limit_;
  sensor::Sensor discharge_current_limit_;
  sensor::Sensor discharge_voltage_limit_;
  sensor::Sensor state_of_charge_;
  sensor::Sensor state_of_health_;
  sensor::Sensor hires_state_of_charge_;
  sensor::Sensor battery_voltage_;
  sensor::Sensor battery_current_;
  sensor::Sensor battery_temperature_;

  void SetUp() override {
    bms_.set_canbus(&canbus_);
    bms_.set_charge_voltage_sensor(&charge_voltage_);
    bms_.set_charge_current_limit_sensor(&charge_current_limit_);
    bms_.set_discharge_current_limit_sensor(&discharge_current_limit_);
    bms_.set_discharge_voltage_limit_sensor(&discharge_voltage_limit_);
    bms_.set_state_of_charge_sensor(&state_of_charge_);
    bms_.set_state_of_health_sensor(&state_of_health_);
    bms_.set_hires_state_of_charge_sensor(&hires_state_of_charge_);
    bms_.set_battery_voltage_sensor(&battery_voltage_);
    bms_.set_battery_current_sensor(&battery_current_);
    bms_.set_battery_temperature_sensor(&battery_temperature_);
  }
};

// ── Frame 0x0351 ──────────────────────────────────────────────────────────────

TEST_F(VirtualCanBmsTest, Frame0x0351ChargeVoltage) {
  charge_voltage_.publish_state(54.4f);
  charge_current_limit_.publish_state(50.0f);
  discharge_current_limit_.publish_state(50.0f);
  discharge_voltage_limit_.publish_state(46.0f);
  bms_.send_frame_0x0351_();

  // ChargeVoltage = 54.4 * 10 = 544 = 0x0220 → [0x20, 0x02]
  ASSERT_EQ(canbus_.frames[0x0351].data.size(), 8u);
  EXPECT_EQ(canbus_.frames[0x0351].data[0], 0x20);
  EXPECT_EQ(canbus_.frames[0x0351].data[1], 0x02);
}

TEST_F(VirtualCanBmsTest, Frame0x0351ChargeCurrent) {
  charge_voltage_.publish_state(54.4f);
  charge_current_limit_.publish_state(50.0f);
  discharge_current_limit_.publish_state(50.0f);
  discharge_voltage_limit_.publish_state(46.0f);
  bms_.send_frame_0x0351_();

  // MaxChargingCurrent = 50.0 * 10 = 500 = 0x01F4 → [0xF4, 0x01]
  EXPECT_EQ(canbus_.frames[0x0351].data[2], 0xF4);
  EXPECT_EQ(canbus_.frames[0x0351].data[3], 0x01);
}

TEST_F(VirtualCanBmsTest, Frame0x0351DischargeCurrent) {
  charge_voltage_.publish_state(54.4f);
  charge_current_limit_.publish_state(50.0f);
  discharge_current_limit_.publish_state(50.0f);
  discharge_voltage_limit_.publish_state(46.0f);
  bms_.send_frame_0x0351_();

  // MaxDischargingCurrent = 50.0 * 10 = 500 = 0x01F4 → [0xF4, 0x01]
  EXPECT_EQ(canbus_.frames[0x0351].data[4], 0xF4);
  EXPECT_EQ(canbus_.frames[0x0351].data[5], 0x01);
}

TEST_F(VirtualCanBmsTest, Frame0x0351DischargeVoltageLimit) {
  charge_voltage_.publish_state(54.4f);
  charge_current_limit_.publish_state(50.0f);
  discharge_current_limit_.publish_state(50.0f);
  discharge_voltage_limit_.publish_state(46.0f);
  bms_.send_frame_0x0351_();

  // DischargeVoltageLimit = 46.0 * 10 = 460 = 0x01CC → [0xCC, 0x01]
  EXPECT_EQ(canbus_.frames[0x0351].data[6], 0xCC);
  EXPECT_EQ(canbus_.frames[0x0351].data[7], 0x01);
}

TEST_F(VirtualCanBmsTest, Frame0x0351SkippedOnNaN) {
  charge_voltage_.publish_state(NAN);
  charge_current_limit_.publish_state(50.0f);
  discharge_current_limit_.publish_state(50.0f);
  discharge_voltage_limit_.publish_state(46.0f);
  bms_.send_frame_0x0351_();

  EXPECT_EQ(canbus_.frames.count(0x0351), 0u);
}

// ── Frame 0x0355 ──────────────────────────────────────────────────────────────

TEST_F(VirtualCanBmsTest, Frame0x0355StateOfCharge) {
  state_of_charge_.publish_state(85.0f);
  state_of_health_.publish_state(100.0f);
  hires_state_of_charge_.publish_state(85.50f);
  bms_.send_frame_0x0355_();

  // StateOfCharge = 85 = 0x0055 → [0x55, 0x00]
  ASSERT_EQ(canbus_.frames[0x0355].data.size(), 6u);
  EXPECT_EQ(canbus_.frames[0x0355].data[0], 0x55);
  EXPECT_EQ(canbus_.frames[0x0355].data[1], 0x00);
}

TEST_F(VirtualCanBmsTest, Frame0x0355StateOfHealth) {
  state_of_charge_.publish_state(85.0f);
  state_of_health_.publish_state(100.0f);
  hires_state_of_charge_.publish_state(85.50f);
  bms_.send_frame_0x0355_();

  // StateOfHealth = 100 = 0x0064 → [0x64, 0x00]
  EXPECT_EQ(canbus_.frames[0x0355].data[2], 0x64);
  EXPECT_EQ(canbus_.frames[0x0355].data[3], 0x00);
}

TEST_F(VirtualCanBmsTest, Frame0x0355HiresStateOfCharge) {
  state_of_charge_.publish_state(85.0f);
  state_of_health_.publish_state(100.0f);
  hires_state_of_charge_.publish_state(85.50f);
  bms_.send_frame_0x0355_();

  // HiresSOC = 85.50 * 100 = 8550 = 0x2166 → [0x66, 0x21]
  EXPECT_EQ(canbus_.frames[0x0355].data[4], 0x66);
  EXPECT_EQ(canbus_.frames[0x0355].data[5], 0x21);
}

TEST_F(VirtualCanBmsTest, Frame0x0355HiresInvalidWhenNotConfigured) {
  TestableVirtualCanBms bms_no_hires;
  bms_no_hires.set_canbus(&canbus_);
  bms_no_hires.set_state_of_charge_sensor(&state_of_charge_);
  bms_no_hires.set_state_of_health_sensor(&state_of_health_);
  state_of_charge_.publish_state(85.0f);
  state_of_health_.publish_state(100.0f);
  bms_no_hires.send_frame_0x0355_();

  // HiresSOC = 65535 = 0xFFFF → [0xFF, 0xFF]
  ASSERT_EQ(canbus_.frames[0x0355].data.size(), 6u);
  EXPECT_EQ(canbus_.frames[0x0355].data[4], 0xFF);
  EXPECT_EQ(canbus_.frames[0x0355].data[5], 0xFF);
}

TEST_F(VirtualCanBmsTest, Frame0x0355SkippedOnNaN) {
  state_of_charge_.publish_state(NAN);
  state_of_health_.publish_state(100.0f);
  bms_.send_frame_0x0355_();

  EXPECT_EQ(canbus_.frames.count(0x0355), 0u);
}

// ── Frame 0x0356 ──────────────────────────────────────────────────────────────

TEST_F(VirtualCanBmsTest, Frame0x0356BatteryVoltage) {
  battery_voltage_.publish_state(52.5f);
  battery_current_.publish_state(-3.1f);
  battery_temperature_.publish_state(25.0f);
  bms_.send_frame_0x0356_();

  // BatteryVoltage = 52.5 * 100 = 5250 = 0x1482 → [0x82, 0x14]
  ASSERT_EQ(canbus_.frames[0x0356].data.size(), 6u);
  EXPECT_EQ(canbus_.frames[0x0356].data[0], 0x82);
  EXPECT_EQ(canbus_.frames[0x0356].data[1], 0x14);
}

TEST_F(VirtualCanBmsTest, Frame0x0356BatteryCurrentDischarging) {
  battery_voltage_.publish_state(52.5f);
  battery_current_.publish_state(-3.1f);
  battery_temperature_.publish_state(25.0f);
  bms_.send_frame_0x0356_();

  // BatteryCurrent = -3.1 * 10 = -31 = 0xFFE1 → [0xE1, 0xFF]
  EXPECT_EQ(canbus_.frames[0x0356].data[2], 0xE1);
  EXPECT_EQ(canbus_.frames[0x0356].data[3], 0xFF);
}

TEST_F(VirtualCanBmsTest, Frame0x0356BatteryCurrentCharging) {
  battery_voltage_.publish_state(52.5f);
  battery_current_.publish_state(10.0f);
  battery_temperature_.publish_state(25.0f);
  bms_.send_frame_0x0356_();

  // BatteryCurrent = 10.0 * 10 = 100 = 0x0064 → [0x64, 0x00]
  EXPECT_EQ(canbus_.frames[0x0356].data[2], 0x64);
  EXPECT_EQ(canbus_.frames[0x0356].data[3], 0x00);
}

TEST_F(VirtualCanBmsTest, Frame0x0356BatteryTemperature) {
  battery_voltage_.publish_state(52.5f);
  battery_current_.publish_state(-3.1f);
  battery_temperature_.publish_state(25.0f);
  bms_.send_frame_0x0356_();

  // BatteryTemperature = 25.0 * 10 = 250 = 0x00FA → [0xFA, 0x00]
  EXPECT_EQ(canbus_.frames[0x0356].data[4], 0xFA);
  EXPECT_EQ(canbus_.frames[0x0356].data[5], 0x00);
}

TEST_F(VirtualCanBmsTest, Frame0x0356SkippedWhenAllSensorsAbsent) {
  TestableVirtualCanBms bms_no_bat;
  bms_no_bat.set_canbus(&canbus_);
  bms_no_bat.send_frame_0x0356_();

  EXPECT_EQ(canbus_.frames.count(0x0356), 0u);
}

TEST_F(VirtualCanBmsTest, Frame0x0356SkippedOnNaN) {
  battery_voltage_.publish_state(NAN);
  battery_current_.publish_state(-3.1f);
  battery_temperature_.publish_state(25.0f);
  bms_.send_frame_0x0356_();

  EXPECT_EQ(canbus_.frames.count(0x0356), 0u);
}

// ── Frame 0x035A ──────────────────────────────────────────────────────────────

TEST_F(VirtualCanBmsTest, Frame0x035AAlwaysZero) {
  bms_.send_frame_0x035a_();

  ASSERT_EQ(canbus_.frames[0x035A].data.size(), 8u);
  for (auto byte : canbus_.frames[0x035A].data) {
    EXPECT_EQ(byte, 0x00);
  }
}

}  // namespace esphome::virtual_can_bms::testing
