import os
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import CORE

knx_ns = cg.esphome_ns.namespace('knx_custom')
KNXComponent = knx_ns.class_('KNXComponent', cg.Component)

CONF_INDIVIDUAL_ADDRESS = "individual_address"
MULTI_CONF = False

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(KNXComponent),
    cv.Required(CONF_INDIVIDUAL_ADDRESS): cv.string,
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_individual_address(config[CONF_INDIVIDUAL_ADDRESS]))

    curr_dir = os.path.dirname(os.path.abspath(__file__))
    cg.add_build_flag(f"-I{curr_dir}")

    # KNX stack defines
    cg.add_build_flag("-DMASK_VERSION=0x57B0")
    cg.add_build_flag("-DKNX_NO_AUTOMATION")
    cg.add_build_flag("-DKNX_NO_SPI")
    cg.add_build_flag("-DKNX_NO_AUTOMATIC_GLOBAL_INSTANCE")

    if CORE.is_esp32:
        cg.add_build_flag("-DARDUINO_ARCH_ESP32")

    cg.add_library("EEPROM", None)
    cg.add_library("WiFi", None)


__all__ = ['knx_ns', 'KNXComponent']
