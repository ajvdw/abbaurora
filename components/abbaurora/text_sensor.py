import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    CONF_SENSOR,
    ICON_EMPTY,
)
from . import ABBAURORA, CONF_ABBAURORA_ID

AUTO_LOAD = ["abbaurora"]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ABBAURORA_ID): cv.use_id(ABBAURORA),
        cv.Optional("identification"): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
            }
        ),
        cv.Optional("version"): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
            }
        ),
        cv.Optional("status"): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
            }
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


