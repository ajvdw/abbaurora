import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import (
    CONF_ID,
    CONF_SENSOR,
    ICON_EMPTY,
)
from . import CONF_ABBAURORA_ID, ABBAurora

CONF_ABB_VERSION = "version"
CONF_ABB_STATUS = "status"
CONF_ABB_IDENTIFICATION = "identification"
AUTO_LOAD = ["abbaurora"]

TYPES = {
    CONF_ABB_VERSION,
    CONF_ABB_STATUS,
    CONF_ABB_IDENTIFICATION,
}

async def to_code(config):
    paren = await cg.get_variable(config[CONF_ABBAURORA_ID])

    for type, _ in TYPES.items():
        if type in config:
            conf = config[type]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(paren, f"set_{type}")(sens))