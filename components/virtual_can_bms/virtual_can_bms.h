#pragma once

#include "esphome/core/component.h"
#include "esphome/components/canbus/canbus.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace virtual_can_bms {

struct SmaCanMessage0x0351 {
  uint16_t ChargeVoltage;          // U16
  int16_t MaxChargingCurrent;      // S16
  int16_t MaxDischargingCurrent;   // S16
  uint16_t DischargeVoltageLimit;  // U16
};

struct SmaCanMessage0x0355 {
  uint16_t StateOfCharge;         // U16
  uint16_t StateOfHealth;         // U16
  uint16_t StateOfChargeHighRes;  // U16
};

struct SmaCanMessage0x0356 {
  int16_t BatteryVoltage;      // S16
  int16_t BatteryCurrent;      // S16
  int16_t BatteryTemperature;  // S16
};

struct SmaCanMessage0x035A {
  uint32_t AlarmBitmask;    // 32 Bits
  uint32_t WarningBitmask;  // 32 Bits
};

struct SmaCanMessage0x035E {
  char Model[8];
};

struct SmaCanMessage0x035F {
  uint16_t CellChemistry;
  uint8_t HardwareVersion[2];
  uint16_t NominalCapacity;
  uint8_t SoftwareVersion[2];
};

struct SmaCanMessage0x0370 {
  char Manufacturer[8];
};

struct SmaCanMessage0x0373 {
  uint16_t MinCellvoltage;  // v * 1000.0f
  uint16_t MaxCellvoltage;  // v * 1000.0f
  uint16_t MinTemperature;  // v * 273.15f
  uint16_t MaxTemperature;  // v * 273.15f
};

class VirtualCanBms : public PollingComponent {
 public:
  void set_canbus(canbus::Canbus *canbus) { this->canbus = canbus; }

  void set_charge_voltage_sensor(sensor::Sensor *charge_voltage_sensor) {
    charge_voltage_sensor_ = charge_voltage_sensor;
  }
  void set_charge_current_limit_sensor(sensor::Sensor *charge_current_limit_sensor) {
    charge_current_limit_sensor_ = charge_current_limit_sensor;
  }
  void set_discharge_current_limit_sensor(sensor::Sensor *discharge_current_limit_sensor) {
    discharge_current_limit_sensor_ = discharge_current_limit_sensor;
  }
  void set_discharge_voltage_limit_sensor(sensor::Sensor *discharge_voltage_limit_sensor) {
    discharge_voltage_limit_sensor_ = discharge_voltage_limit_sensor;
  }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_state_of_health_sensor(sensor::Sensor *state_of_health_sensor) {
    state_of_health_sensor_ = state_of_health_sensor;
  }
  void set_hires_state_of_charge_sensor(sensor::Sensor *hires_state_of_charge_sensor) {
    hires_state_of_charge_sensor_ = hires_state_of_charge_sensor;
  }
  void set_battery_voltage_sensor(sensor::Sensor *battery_voltage_sensor) {
    battery_voltage_sensor_ = battery_voltage_sensor;
  }
  void set_battery_current_sensor(sensor::Sensor *battery_current_sensor) {
    battery_current_sensor_ = battery_current_sensor;
  }
  void set_battery_temperature_sensor(sensor::Sensor *battery_temperature_sensor) {
    battery_temperature_sensor_ = battery_temperature_sensor;
  }

  void dump_config() override;

  void update() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

  canbus::Canbus *canbus;

 protected:
  sensor::Sensor *charge_voltage_sensor_;
  sensor::Sensor *charge_current_limit_sensor_;
  sensor::Sensor *discharge_current_limit_sensor_;
  sensor::Sensor *discharge_voltage_limit_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *state_of_health_sensor_;
  sensor::Sensor *hires_state_of_charge_sensor_;
  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *battery_current_sensor_;
  sensor::Sensor *battery_temperature_sensor_;

  void send_frame_0x0351_();
  void send_frame_0x0355_();
  void send_frame_0x0356_();
  void send_frame_0x035a_();
  void send_frame_0x035b_();
  void send_frame_0x035e_();
  void send_frame_0x035f_();

  void publish_state_(sensor::Sensor *sensor, float value);
};

}  // namespace virtual_can_bms
}  // namespace esphome
