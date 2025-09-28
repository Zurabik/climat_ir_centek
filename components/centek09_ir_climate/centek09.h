#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome {
namespace Centek09 {

// Support for Centek09 air conditioners with YKR-T/121E remote

// Temperature
const float YKR_T_121E_TEMP_MIN = 16.0;
const float YKR_T_121E_TEMP_MAX = 32.0;

class Centek09Climate : public climate_ir::ClimateIR {
 public:
  Centek09Climate()
      : climate_ir::ClimateIR(YKR_T_121E_TEMP_MIN, YKR_T_121E_TEMP_MAX, 1.0f, true, true,
                              {climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM,
                               climate::CLIMATE_FAN_HIGH, climate::CLIMATE_FAN_TURBO, climate::CLIMATE_FAN_SILENCE},
                              {climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_VERTICAL,
                               climate::CLIMATE_SWING_HORIZONTAL, climate::CLIMATE_SWING_BOTH}) {}

 protected:
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;
  /// Handle received IR Buffer
  bool on_receive(remote_base::RemoteReceiveData data) override;
};

}  // namespace Centek09
}  // namespace esphome