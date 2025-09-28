#pragma once

#include "esphome.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/remote_base/remote_base.h"
#include "esphome/components/remote_transmitter/remote_transmitter.h"

namespace esphome {
namespace centek_ir_climate {

class CentekIRClimate : public climate::Climate, public Component {
 public:
  void set_transmitter(remote_transmitter::RemoteTransmitterComponent *transmitter) { 
    transmitter_ = transmitter; 
  }
  void set_protocol(const std::string &protocol) { protocol_ = protocol; }
  void set_centek_code(uint32_t code) { centek_code_ = code; }
  void set_bit_length(uint8_t length) { bit_length_ = length; }
  void set_header_mark(uint32_t mark) { header_mark_ = mark; }
  void set_header_space(uint32_t space) { header_space_ = space; }
  void set_bit_mark(uint32_t mark) { bit_mark_ = mark; }
  void set_one_space(uint32_t space) { one_space_ = space; }
  void set_zero_space(uint32_t space) { zero_space_ = space; }

  void setup() override;
  climate::ClimateTraits traits() override;
  void control(const climate::ClimateCall &call) override;

 protected:
  void send_ir_command_();
  
  std::string protocol_;
  uint32_t centek_code_;
  uint8_t bit_length_;
  uint32_t header_mark_, header_space_;
  uint32_t bit_mark_, one_space_, zero_space_;
  remote_transmitter::RemoteTransmitterComponent *transmitter_;
  
  // Убрали дублирующие переменные, используем унаследованные от climate::Climate
  bool power_{false};
};

}  // namespace centek_ir_climate
}  // namespace esphome