import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    CONF_SENSOR,
    ICON_EMPTY,
)
from . import CONF_ABBAURORA_ID, ABBAurora, ABBAURORA_COMPONENT_SCHEMA

CONF_ABB_VERSION = "version"
CONF_ABB_STATUS = "status"
CONF_ABB_IDENTIFICATION = "identification"
AUTO_LOAD = ["abbaurora"]

TYPES = {
    CONF_ABB_VERSION: ,
    CONF_ABB_STATUS,
    CONF_ABB_IDENTIFICATION,
}


CONFIG_SCHEMA = ABBAURORA_COMPONENT_SCHEMA.extend(
    {
        cv.Optional( type ): text_sensor.TEXT_SENSOR_SCHEMA.extend( { cv.GenerateID(): cv.declare_id(text_sensor.TextSensor) } )
        for type in TYPES
    }
)

async def to_code(config):
    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            sens = await text_sensor.new_text_sensor(conf)
  