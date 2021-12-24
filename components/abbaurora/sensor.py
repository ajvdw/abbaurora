import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    ICON_EMPTY,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    STATE_CLASS_NONE,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_VOLT,
    UNIT_WATT_HOURS,
    UNIT_WATT,
)
from . import CONF_ABBAURORA_ID

CONF_ABB_V_IN_1 = "v_in_1"
CONF_ABB_V_IN_2 = "v_in_2"
CONF_ABB_I_IN_1 = "i_in_1"
CONF_ABB_I_IN_2 = "i_in_2"
CONF_ABB_POWER_IN_1 = "power_in_1"
CONF_ABB_POWER_IN_2 = "power_in_2"
CONF_ABB_POWER_IN_TOTAL = "power_in_total"
CONF_ABB_GRID_POWER = "grid_power"
CONF_ABB_TEMPERATURE_INVERTER= "temperature_inverter"
CONF_ABB_TEMPERATURE_BOOSTER = "temperature_booster"
CONF_ABB_GRID_VOLTAGE = "grid_voltage"
CONF_ABB_CUMULATED_ENERGY_TODAY = "cumulated_energy_today" 
CONF_ABB_CUMULATED_ENERGY_TOTAL = "cumulated_energy_total"

AUTO_LOAD = ["abbaurora"]


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ABBAURORA_ID): cv.use_id(ABBAurora),

        cv.Optional(CONF_ABB_V_IN_1): sensor.sensor_schema(
            UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE, STATE_CLASS_NONE
        ),
        cv.Optional(CONF_ABB_V_IN_2): sensor.sensor_schema(
            UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE, STATE_CLASS_NONE
        ),
        cv.Optional(CONF_ABB_I_IN_1): sensor.sensor_schema(
            UNIT_AMPERE, ICON_EMPTY, 1, DEVICE_CLASS_CURRENT, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_I_IN_2): sensor.sensor_schema(
            UNIT_AMPERE, ICON_EMPTY, 1, DEVICE_CLASS_CURRENT, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_POWER_IN_1): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_POWER_IN_2): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_POWER_IN_TOTAL): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_GRID_POWER): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
	    ),
        cv.Optional(CONF_ABB_TEMPERATURE_INVERTER): sensor.sensor_schema(
            UNIT_CELSIUS, ICON_EMPTY, 1, DEVICE_CLASS_TEMPERATURE, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_TEMPERATURE_BOOSTER): sensor.sensor_schema(
            UNIT_CELSIUS, ICON_EMPTY, 1, DEVICE_CLASS_TEMPERATURE, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_GRID_VOLTAGE): sensor.sensor_schema(
            UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_CUMULATED_ENERGY_TODAY): sensor.sensor_schema( 
            UNIT_WATT_HOURS, ICON_EMPTY, 0, DEVICE_CLASS_ENERGY, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional(CONF_ABB_CUMULATED_ENERGY_TOTAL): sensor.sensor_schema( 
            UNIT_WATT_HOURS, ICON_EMPTY, 0, DEVICE_CLASS_ENERGY, STATE_CLASS_TOTAL_INCREASING
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    hub = await cg.get_variable(config[CONF_ABBAURORA_ID])
    var = cg.new_Pvariable(config[CONF_ID], hub)
    await cg.register_component(var, config)
    await text_sensor.register_text_sensor(var, config)

    cg.add(hub.register_textsensor_component(var))



