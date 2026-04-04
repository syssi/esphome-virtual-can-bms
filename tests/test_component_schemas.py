"""Schema structure tests for virtual_can_bms ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.virtual_can_bms as hub  # noqa: E402


class TestHubConstants:
    def test_conf_sensor_ids_defined(self):
        assert hub.CONF_CHARGE_VOLTAGE_ID == "charge_voltage_id"
        assert hub.CONF_CHARGE_CURRENT_LIMIT_ID == "charge_current_limit_id"
        assert hub.CONF_DISCHARGE_CURRENT_LIMIT_ID == "discharge_current_limit_id"
        assert hub.CONF_STATE_OF_CHARGE_ID == "state_of_charge_id"
        assert hub.CONF_STATE_OF_HEALTH_ID == "state_of_health_id"
        assert hub.CONF_BATTERY_VOLTAGE_ID == "battery_voltage_id"
        assert hub.CONF_BATTERY_CURRENT_ID == "battery_current_id"
        assert hub.CONF_BATTERY_TEMPERATURE_ID == "battery_temperature_id"

    def test_sensors_list_completeness(self):
        assert hub.CONF_CHARGE_VOLTAGE_ID in hub.SENSORS
        assert hub.CONF_STATE_OF_CHARGE_ID in hub.SENSORS
        assert hub.CONF_BATTERY_VOLTAGE_ID in hub.SENSORS
        assert len(hub.SENSORS) == 10
