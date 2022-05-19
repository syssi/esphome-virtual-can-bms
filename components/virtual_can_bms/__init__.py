import esphome.codegen as cg
from esphome.components import canbus, sensor
from esphome.components.canbus import CONF_CANBUS_ID, CanbusComponent
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["canbus", "sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_CHARGE_VOLTAGE_ID = "charge_voltage_id"
CONF_CHARGE_CURRENT_LIMIT_ID = "charge_current_limit_id"
CONF_DISCHARGE_CURRENT_LIMIT_ID = "discharge_current_limit_id"
CONF_DISCHARGE_VOLTAGE_LIMIT_ID = "discharge_voltage_limit_id"
CONF_STATE_OF_CHARGE_ID = "state_of_charge_id"
CONF_STATE_OF_HEALTH_ID = "state_of_health_id"
CONF_HIRES_STATE_OF_CHARGE_ID = "hires_state_of_charge_id"
CONF_BATTERY_VOLTAGE_ID = "battery_voltage_id"
CONF_BATTERY_CURRENT_ID = "battery_current_id"
CONF_BATTERY_TEMPERATURE_ID = "battery_temperature_id"

SENSORS = [
    CONF_CHARGE_VOLTAGE_ID,
    CONF_CHARGE_CURRENT_LIMIT_ID,
    CONF_DISCHARGE_CURRENT_LIMIT_ID,
    CONF_DISCHARGE_VOLTAGE_LIMIT_ID,
    CONF_STATE_OF_CHARGE_ID,
    CONF_STATE_OF_HEALTH_ID,
    CONF_HIRES_STATE_OF_CHARGE_ID,
    CONF_BATTERY_VOLTAGE_ID,
    CONF_BATTERY_CURRENT_ID,
    CONF_BATTERY_TEMPERATURE_ID,
]

virtual_can_bms_ns = cg.esphome_ns.namespace("virtual_can_bms")
VirtualCanBms = virtual_can_bms_ns.class_(
    "VirtualCanBms",
    cg.PollingComponent,
    canbus.CanbusComponent,
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(VirtualCanBms),
            cv.GenerateID(CONF_CANBUS_ID): cv.use_id(CanbusComponent),
            cv.Required(CONF_CHARGE_VOLTAGE_ID): cv.use_id(sensor.Sensor),
            cv.Required(CONF_CHARGE_CURRENT_LIMIT_ID): cv.use_id(sensor.Sensor),
            cv.Required(CONF_DISCHARGE_CURRENT_LIMIT_ID): cv.use_id(sensor.Sensor),
            cv.Required(CONF_DISCHARGE_VOLTAGE_LIMIT_ID): cv.use_id(sensor.Sensor),
            cv.Required(CONF_STATE_OF_CHARGE_ID): cv.use_id(sensor.Sensor),
            cv.Required(CONF_STATE_OF_HEALTH_ID): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_HIRES_STATE_OF_CHARGE_ID): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_BATTERY_VOLTAGE_ID): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_BATTERY_CURRENT_ID): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_BATTERY_TEMPERATURE_ID): cv.use_id(sensor.Sensor),
        }
    ).extend(cv.polling_component_schema("1s")),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    canbus_component = await cg.get_variable(config[CONF_CANBUS_ID])
    cg.add(var.set_canbus(canbus_component))

    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await cg.get_variable(conf)
            cg.add(getattr(var, f"set_{key.replace('_id', '')}_sensor")(sens))
