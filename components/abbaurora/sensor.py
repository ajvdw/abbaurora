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
    LAST_RESET_TYPE_NEVER,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_NONE,
    UNIT_AMPERE,
    UNIT_CELCIUS,
    UNIT_EMPTY,
    UNIT_VOLT,
    UNIT_WATT_HOURS,
    UNIT_WATT,
)
from . import ABBAURORA, CONF_ABBAURORA_ID

AUTO_LOAD = ["abbaurora"]


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ABBAURORA_ID): cv.use_id(ABBAURORA),

        cv.Optional("v_in_1"): sensor.sensor_schema(
            UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE, STATE_CLASS_NONE
        ),
        cv.Optional("v_in_2"): sensor.sensor_schema(
            UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE, STATE_CLASS_NONE
        ),
        cv.Optional("i_in_1"): sensor.sensor_schema(
            UNIT_AMPERE, ICON_EMPTY, 1, DEVICE_CLASS_CURRENT, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("i_in_2"): sensor.sensor_schema(
            UNIT_AMPERE, ICON_EMPTY, 1, DEVICE_CLASS_CURRENT, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("power_in_1): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("power_in_2"): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("power_in_total"): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("grid_power"): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 1, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
	),
        cv.Optional("temperature_inverter"): sensor.sensor_schema(
            UNIT_CELCIUS, ICON_EMPTY, 1, DEVICE_CLASS_TEMPERATURE, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("temperature_booster"): sensor.sensor_schema(
            UNIT_CELSIUS, ICON_EMPTY, 1, DEVICE_CLASS_TEMPERATURE, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("grid_voltage"): sensor.sensor_schema(
            UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("cumulated_energy_today"): sensor.sensor_schema( 
            UNIT_WATT_HOURS, ICON_EMPTY, 0, DEVICE_CLASS_ENERGY, STATE_CLASS_MEASUREMENT
        ),
        cv.Optional("cumulated_energy_total"): sensor.sensor_schema( 
            UNIT_WATT_HOURS, ICON_EMPTY, 0, DEVICE_CLASS_ENERGY, STATE_CLASS_MEASUREMENT, LAST_RESET_TYPE_NEVER,
        ),
    }
).extend(cv.COMPONENT_SCHEMA)

