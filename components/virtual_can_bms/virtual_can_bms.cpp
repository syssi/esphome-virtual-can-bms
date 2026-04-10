#include "virtual_can_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace virtual_can_bms {

static const char *const TAG = "virtual_can_bms";

void VirtualCanBms::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void VirtualCanBms::dump_config() { ESP_LOGCONFIG(TAG, "VirtualCanBms:"); }

void VirtualCanBms::update() {
  this->send_frame_0x0351_();
  this->send_frame_0x0355_();
  this->send_frame_0x0356_();
  this->send_frame_0x035a_();
  this->send_frame_0x035e_();
}

// Required
void VirtualCanBms::send_frame_0x0351_() {
  static SmaCanMessage0x0351 message;

  if (this->charge_voltage_sensor_ == nullptr || this->charge_current_limit_sensor_ == nullptr ||
      this->discharge_current_limit_sensor_ == nullptr || this->discharge_voltage_limit_sensor_ == nullptr) {
    ESP_LOGW(TAG,
             "One of the required sensors (charge_voltage_id, charge_current_limit_id, discharge_current_limit_id, "
             "discharge_voltage_limit_id) missing. Unable to populate 0x0351 frame. Skipped");
    return;
  }

  float charge_voltage = this->charge_voltage_sensor_->get_state();
  float charge_current_limit = this->charge_current_limit_sensor_->get_state();
  float discharge_current_limit = this->discharge_current_limit_sensor_->get_state();
  float discharge_voltage_limit = this->discharge_voltage_limit_sensor_->get_state();
  if (std::isnan(charge_voltage) || std::isnan(charge_current_limit) || std::isnan(discharge_current_limit) ||
      std::isnan(discharge_voltage_limit)) {
    ESP_LOGW(TAG, "One of the required sensor states is NaN. Unable to populate 0x0351 frame. Skipped");
    return;
  }

  message.ChargeVoltage = (charge_voltage * 10.0f);                   // 41V * 10 ... 64V * 10 = 410...640
  message.MaxChargingCurrent = (charge_current_limit * 10.0f);        // 0A * 10 ... 1200A * 10 = 0...12000
  message.MaxDischargingCurrent = (discharge_current_limit * 10.0f);  // 0A * 10 ... 1200A * 10 = 0...12000
  message.DischargeVoltageLimit = (discharge_voltage_limit * 10.0f);  // 41V * 10 ... 48V * 10 = 410...480

  auto *ptr = reinterpret_cast<uint8_t *>(&message);
  this->canbus->send_data(0x0351, false, false, std::vector<uint8_t>(ptr, ptr + sizeof message));
}

// Required
void VirtualCanBms::send_frame_0x0355_() {
  static SmaCanMessage0x0355 message;

  if (this->state_of_charge_sensor_ == nullptr || this->state_of_health_sensor_ == nullptr) {
    ESP_LOGW(TAG, "One of the required sensors (state_of_charge_id, state_of_health_id) "
                  "missing. Unable to populate 0x0355 frame. Skipped");
    return;
  }

  float hires_state_of_charge = 65535;  // Invalid unsigned. Unused fields of used frames must set to "invalid"
  if (this->hires_state_of_charge_sensor_ != nullptr) {
    hires_state_of_charge = this->hires_state_of_charge_sensor_->get_state();
  }

  float state_of_charge = this->state_of_charge_sensor_->get_state();
  float state_of_health = this->state_of_health_sensor_->get_state();
  if (std::isnan(state_of_charge) || std::isnan(state_of_health) || std::isnan(hires_state_of_charge)) {
    ESP_LOGW(TAG, "One of the required sensor states is NaN. Unable to populate 0x0355 frame. Skipped");
    return;
  }

  message.StateOfCharge = state_of_charge;                          // 0%...100%
  message.StateOfHealth = state_of_health;                          // 0%...100%
  message.StateOfChargeHighRes = (hires_state_of_charge * 100.0f);  // 0.00%...100.00%

  auto *ptr = reinterpret_cast<uint8_t *>(&message);
  this->canbus->send_data(0x0355, false, false, std::vector<uint8_t>(ptr, ptr + sizeof message));
}

// Optional
void VirtualCanBms::send_frame_0x0356_() {
  static SmaCanMessage0x0356 message;

  // If all sensors are unavailable it's on purpose
  if (this->battery_voltage_sensor_ == nullptr && this->battery_current_sensor_ == nullptr &&
      this->battery_temperature_sensor_ == nullptr) {
    ESP_LOGVV(TAG, "Sensor battery_voltage_id, battery_current_id and battery_temperature_id not configured. "
                   "Skipping frame 0x0356");
    return;
  }

  // If one of the sensors is missing it's a fault
  if (this->battery_voltage_sensor_ == nullptr || this->battery_current_sensor_ == nullptr ||
      this->battery_temperature_sensor_ == nullptr) {
    ESP_LOGW(TAG, "One of the required sensors (battery_voltage_id, battery_current_id, battery_temperature_id) "
                  "missing. Unable to populate 0x0356 frame. Skipped");
    return;
  }

  float battery_voltage = this->battery_voltage_sensor_->get_state();
  float battery_current = this->battery_current_sensor_->get_state();
  float battery_temperature = this->battery_temperature_sensor_->get_state();
  if (std::isnan(battery_voltage) || std::isnan(battery_current) || std::isnan(battery_temperature)) {
    ESP_LOGW(TAG, "One of the required sensor states is NaN. Unable to populate 0x0356 frame. Skipped");
    return;
  }

  message.BatteryVoltage = (battery_voltage * 100.0f);         // V
  message.BatteryCurrent = (battery_current * 10.0f);          // A
  message.BatteryTemperature = (battery_temperature * 10.0f);  // °C

  auto *ptr = reinterpret_cast<uint8_t *>(&message);
  this->canbus->send_data(0x0356, false, false, std::vector<uint8_t>(ptr, ptr + sizeof message));
}

// Required
void VirtualCanBms::send_frame_0x035a_() {
  static SmaCanMessage0x035A message;

  message.AlarmBitmask = 0x00000000;
  message.WarningBitmask = 0x00000000;

  auto *ptr = reinterpret_cast<uint8_t *>(&message);
  this->canbus->send_data(0x035A, false, false, std::vector<uint8_t>(ptr, ptr + sizeof message));
}

void VirtualCanBms::send_frame_0x035e_() {
  static SmaCanMessage0x035E message;

  if(this->battery_name_.empty())
    return;

  this->battery_name_.copy(message.BatteryName, 8);

  auto *ptr = reinterpret_cast<uint8_t *>(&message);
  this->canbus->send_data(0x035E, false, false, std::vector<uint8_t>(ptr, ptr + sizeof message));
}

}  // namespace virtual_can_bms
}  // namespace esphome
