import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Definice namespace
knx_ns = cg.esphome_ns.namespace('knx_custom')
KNXComponent = knx_ns.class_('KNXComponent', cg.Component)

CONF_INDIVIDUAL_ADDRESS = "individual_address"

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(KNXComponent),
    cv.Required(CONF_INDIVIDUAL_ADDRESS): cv.string,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    cg.add(var.set_individual_address(config[CONF_INDIVIDUAL_ADDRESS]))

    # Přidání závislostí na Arduino knihovnách
    cg.add_library("SPI", None)
    cg.add_library("Wire", None)
    cg.add_library("WiFi", None)
    cg.add_library("Network", None)
    cg.add_library("EEPROM", None)
    cg.add_library("Preferences", None)

    cg.add_build_flag("-DMASK_VERSION=0x07B0")
    cg.add_build_flag("-DKNX_NO_AUTOMATION")
    cg.add_build_flag("-DARDUINO_ARCH_ESP32")
    # Přidáme i flag pro ESP-IDF, aby knx knihovna věděla, že neběžíme na Arduinu
    # cg.add_build_flag("-DESP_PLATFORM")

    # Přidání cest k souborům
    import os
    curr_dir = os.path.dirname(__file__)
    cg.add_build_flag("-I" + curr_dir)
    # Cesta ke vnitřní složce knx/
    cg.add_build_flag("-I" + os.path.join(curr_dir, "knx"))

__all__ = ['knx_ns', 'KNXComponent']