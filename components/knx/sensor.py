import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import CONF_ID
from . import knx_ns, KNXComponent

KNXSensor = knx_ns.class_('KNXSensor', sensor.Sensor, cg.Component)

CONF_GROUP_ADDRESS = "group_address"
CONF_KNX_ID = "knx_id"

DEPENDENCIES = ['knx']

CONFIG_SCHEMA = sensor.sensor_schema(KNXSensor).extend({
    cv.Required(CONF_GROUP_ADDRESS): cv.string,
    cv.GenerateID(CONF_KNX_ID): cv.use_id(KNXComponent),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await sensor.register_sensor(var, config)
    cg.add(var.set_group_address(config[CONF_GROUP_ADDRESS]))
    knx_comp = await cg.get_variable(config[CONF_KNX_ID])
    cg.add(var.set_knx_component(knx_comp))
