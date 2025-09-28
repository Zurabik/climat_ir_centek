import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate, remote_base
from esphome.const import CONF_ID, CONF_PROTOCOL

CONF_CENTEK_CODE = "centek_code"
CONF_BIT_LENGTH = "bit_length"
CONF_HEADER_MARK = "header_mark"
CONF_HEADER_SPACE = "header_space"
CONF_BIT_MARK = "bit_mark"
CONF_ONE_SPACE = "one_space"
CONF_ZERO_SPACE = "zero_space"

CENTEK_IR_CLIMATE_PROTOCOLS = {
    "CENTEK_A": "centek_a",
    "CENTEK_B": "centek_b",
    "CENTEK_YKR-T/121E": "centek_ykr-t/121e",
}

centek_ir_climate_ns = cg.esphome_ns.namespace('centek_ir_climate')
CentekIRClimate = centek_ir_climate_ns.class_('CentekIRClimate', climate.Climate, cg.Component)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(CentekIRClimate),
    cv.Required(CONF_PROTOCOL): cv.enum(CENTEK_IR_CLIMATE_PROTOCOLS, upper=True),
    cv.Required(CONF_CENTEK_CODE): cv.hex_uint32_t,
    cv.Optional(CONF_BIT_LENGTH, default=32): cv.int_range(min=8, max=64),
    cv.Optional(CONF_HEADER_MARK, default=3500): cv.positive_time_period_microseconds,
    cv.Optional(CONF_HEADER_SPACE, default=1750): cv.positive_time_period_microseconds,
    cv.Optional(CONF_BIT_MARK, default=500): cv.positive_time_period_microseconds,
    cv.Optional(CONF_ONE_SPACE, default=1250): cv.positive_time_period_microseconds,
    cv.Optional(CONF_ZERO_SPACE, default=500): cv.positive_time_period_microseconds,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
    
    cg.add(var.set_protocol(config[CONF_PROTOCOL]))
    cg.add(var.set_centek_code(config[CONF_CENTEK_CODE]))
    cg.add(var.set_bit_length(config[CONF_BIT_LENGTH]))
    cg.add(var.set_header_mark(config[CONF_HEADER_MARK]))
    cg.add(var.set_header_space(config[CONF_HEADER_SPACE]))
    cg.add(var.set_bit_mark(config[CONF_BIT_MARK]))
    cg.add(var.set_one_space(config[CONF_ONE_SPACE]))
    cg.add(var.set_zero_space(config[CONF_ZERO_SPACE]))