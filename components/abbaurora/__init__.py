import esphome.codegen as cg
import esphome.config_validation as cv

from esphome.components import sensor,uart
from esphome.cpp_helpers import gpio_pin_expression
from esphome.const import (
    CONF_FLOW_CONTROL_PIN,
    CONF_ID,
    CONF_ADDRESS,
    CONF_SENSORS,
    CONF_TEXT_SENSORS,
)
from esphome import pins

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

DEPENDENCIES = ["uart"]

AUTO_LOAD = ["sensor", "text_sensor"]

CONF_ABBAURORA_ID = "abbaurora_id"

abbaurora_ns = cg.esphome_ns.namespace("esphome::abbaurora")
ABBAurora = abbaurora_ns.class_("ABBAuroraComponent", uart.UARTDevice, cg.PollingComponent )

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(ABBAurora),
            cv.Required(CONF_FLOW_CONTROL_PIN): pins.gpio_output_pin_schema,
            cv.Optional(CONF_ADDRESS, default=2): cv.int_range(min=0, max=1023),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA)
)

async def to_code(config):
    cg.add_global(abbaurora_ns.using)
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    if CONF_FLOW_CONTROL_PIN in config:
        pin = await gpio_pin_expression(config[CONF_FLOW_CONTROL_PIN])
        cg.add(var.set_flow_control_pin(pin))

    if CONF_ADDRESS in config:
        cg.add(var.set_address(config[CONF_ADDRESS]))

    if CONF_ABB_CUMULATED_ENERGY_TOTAL in config:
        sens = await sensor(config[CONF_ABB_CUMULATED_ENERGY_TOTAL])
        cg.add(var.set_cumulated_energy_total_sensor(sens))

    for conf in config[CONF_SENSORS]:
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await sensor.register_sensor(var, conf)

    for conf in config[CONF_TEXT_SENSORS]:
        var = cg.new_Pvariable(conf[CONF_ID])
        await cg.register_component(var, conf)
        await text_sensor.register_text_sensor(var, conf)

#CONF_ABB_V_IN_1 = "v_in_1"
#CONF_ABB_V_IN_2 = "v_in_2"
#CONF_ABB_I_IN_1 = "i_in_1"
#CONF_ABB_I_IN_2 = "i_in_2"
#CONF_ABB_POWER_IN_1 = "power_in_1"
#CONF_ABB_POWER_IN_2 = "power_in_2"
#CONF_ABB_POWER_IN_TOTAL = "power_in_total"
#CONF_ABB_GRID_POWER = "grid_power"
#CONF_ABB_TEMPERATURE_INVERTER= "temperature_inverter"
#CONF_ABB_TEMPERATURE_BOOSTER = "temperature_booster"
#CONF_ABB_GRID_VOLTAGE = "grid_voltage"
#CONF_ABB_CUMULATED_ENERGY_TODAY = "cumulated_energy_today" 

   

