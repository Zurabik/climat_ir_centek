import esphome.codegen as cg
from esphome.components import climate_ir

AUTO_LOAD = ["climate_ir"]
CODEOWNERS = ["@bazuchan"]

Centek09_ns = cg.esphome_ns.namespace("Centek09")
Centek09Climate = Centek09_ns.class_("Centek09Climate", climate_ir.ClimateIR)

CONFIG_SCHEMA = climate_ir.climate_ir_with_receiver_schema(Centek09Climate)


async def to_code(config):
    await climate_ir.new_climate_ir(config)