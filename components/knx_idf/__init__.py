import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

# Definice názvu komponenty v C++ (namespace)
knx_idf_ns = cg.esphome_ns.namespace('knx_idf')
KNXComponent = knx_idf_ns.class_('KNXComponent', cg.Component)

# Základní schéma konfigurace v YAML
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(KNXComponent),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Přidání lokálních souborů do kompilace
    cg.add_build_flag("-I")
    # Tady řekneme kompilátoru, kde hledat knx/knx.h
    # ESPHome automaticky přidá cestu ke komponentě do Include path
    
    # Pokud potřebuješ specifické IDF flagy
    cg.add_build_flag("-DCORE_DEBUG_LEVEL=5")