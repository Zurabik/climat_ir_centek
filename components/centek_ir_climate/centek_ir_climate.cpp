#include "centek_ir_climate.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace centek_ir_climate {

void CentekIRClimate::setup() {
  this->target_temperature = 22.0;
  this->mode = climate::CLIMATE_MODE_OFF;
  this->fan_mode = climate::CLIMATE_FAN_AUTO;
  this->swing_mode = climate::CLIMATE_SWING_OFF;
}

climate::ClimateTraits CentekIRClimate::traits() {
  auto traits = climate::ClimateTraits();
  
  traits.set_supports_current_temperature(false);
  traits.set_visual_min_temperature(16);
  traits.set_visual_max_temperature(30);
  traits.set_visual_temperature_step(1);
  
  traits.set_supported_modes({
    climate::CLIMATE_MODE_OFF,
    climate::CLIMATE_MODE_HEAT,
    climate::CLIMATE_MODE_COOL,
    climate::CLIMATE_MODE_DRY,
    climate::CLIMATE_MODE_FAN_ONLY,
    climate::CLIMATE_MODE_AUTO,
  });
  
  traits.set_supported_fan_modes({
    climate::CLIMATE_FAN_AUTO,
    climate::CLIMATE_FAN_LOW,
    climate::CLIMATE_FAN_MEDIUM,
    climate::CLIMATE_FAN_HIGH,
  });
  
  traits.set_supported_swing_modes({
    climate::CLIMATE_SWING_OFF,
    climate::CLIMATE_SWING_VERTICAL,
    climate::CLIMATE_SWING_HORIZONTAL,
    climate::CLIMATE_SWING_BOTH,
  });
  
  return traits;
}

void CentekIRClimate::control(const climate::ClimateCall &call) {
  bool changed = false;
  
  // Обработка режима питания
  if (call.get_mode().has_value()) {
    auto new_mode = *call.get_mode();
    if (this->mode != new_mode) {
      this->mode = new_mode;
      this->power_ = (this->mode != climate::CLIMATE_MODE_OFF);
      changed = true;
    }
  }
  
  // Обработка температуры
  if (call.get_target_temperature().has_value()) {
    auto new_temp = *call.get_target_temperature();
    if (this->target_temperature != new_temp) {
      this->target_temperature = new_temp;
      this->target_temperature_ = new_temp;
      changed = true;
    }
  }
  
  // Обработка скорости вентилятора
  if (call.get_fan_mode().has_value()) {
    auto new_fan_mode = *call.get_fan_mode();
    if (this->fan_mode != new_fan_mode) {
      this->fan_mode = new_fan_mode;
      this->fan_mode_ = new_fan_mode;
      changed = true;
    }
  }
  
  // Обработка swing режима
  if (call.get_swing_mode().has_value()) {
    auto new_swing_mode = *call.get_swing_mode();
    if (this->swing_mode != new_swing_mode) {
      this->swing_mode = new_swing_mode;
      this->swing_mode_ = new_swing_mode;
      changed = true;
    }
  }
  
  // Отправляем IR команду только если что-то изменилось
  if (changed) {
    this->send_ir_command_();
  }
  
  this->publish_state();
}

void CentekIRClimate::send_ir_command_() {
  if (transmitter_ == nullptr) {
    ESP_LOGE("centek", "Transmitter not set!");
    return;
  }
  
  auto transmit = transmitter_->transmit();
  auto *data = transmit.get_data();
  
  // Заголовок пакета для Centek
  data->item(header_mark_, header_space_);
  
  // Базовый код протокола Centek
  uint32_t command = centek_code_;
  
  // Бит питания
  if (power_) {
    command |= 0x10000000;
  } else {
    // Если питание выключено, отправляем команду выключения
    command &= ~0x10000000;
  }
  
  // Режим работы Centek
  switch (this->mode) {  // Используем this->mode вместо mode_
    case climate::CLIMATE_MODE_HEAT:
      command |= 0x01000000;
      break;
    case climate::CLIMATE_MODE_COOL:
      command |= 0x02000000;
      break;
    case climate::CLIMATE_MODE_DRY:
      command |= 0x03000000;
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      command |= 0x04000000;
      break;
    case climate::CLIMATE_MODE_AUTO:
      command |= 0x05000000;
      break;
    case climate::CLIMATE_MODE_OFF:
      // Для выключения режим обычно не меняется, только питание
      break;
  }
  
  // Температура для Centek (16-30°C → 0x0-0xE)
  if (this->target_temperature >= 16 && this->target_temperature <= 30) {
    uint8_t temp_bits = (uint8_t)(this->target_temperature - 16) & 0x0F;
    command |= (temp_bits << 16);
  }
  
  // Скорость вентилятора Centek
  switch (this->fan_mode) {  // Используем this->fan_mode вместо fan_mode_
    case climate::CLIMATE_FAN_AUTO:
      command |= 0x00;
      break;
    case climate::CLIMATE_FAN_LOW:
      command |= 0x10;
      break;
    case climate::CLIMATE_FAN_MEDIUM:
      command |= 0x20;
      break;
    case climate::CLIMATE_FAN_HIGH:
      command |= 0x30;
      break;
  }
  
  // Swing mode для Centek
  switch (this->swing_mode) {  // Используем this->swing_mode вместо swing_mode_
    case climate::CLIMATE_SWING_VERTICAL:
      command |= 0x0400;
      break;
    case climate::CLIMATE_SWING_HORIZONTAL:
      command |= 0x0800;
      break;
    case climate::CLIMATE_SWING_BOTH:
      command |= 0x0C00;
      break;
    case climate::CLIMATE_SWING_OFF:
    default:
      command &= ~0x0C00;  // Выключаем swing
      break;
  }
  
  // Передача битов протокола Centek
  for (uint8_t i = bit_length_; i > 0; i--) {
    bool bit = (command >> (i - 1)) & 1;
    data->item(bit_mark_, bit ? one_space_ : zero_space_);
  }
  
  // Завершающий бит
  data->item(bit_mark_, 0);
  
  transmit.perform();
  ESP_LOGD("centek", "Sent Centek command: 0x%08X, mode=%d, temp=%.1f, fan=%d, swing=%d", 
           command, (int)this->mode, this->target_temperature, (int)this->fan_mode, (int)this->swing_mode);
}

}  // namespace centek_ir_climate
}  // namespace esphome