#pragma once
#include "esphome/core/component.h"
#include "knx_facade.h"
#include "esp_platform.h"

namespace esphome {
namespace knx_custom {

class KNXComponent : public Component {
 public:
  void set_individual_address(const std::string &address) { address_str_ = address; }

  void setup() override {
    ESP_LOGI("knx", "Inicializace KNX...");
    
    static KnxEsp32Platform platform_inst;
    // Oprava: platform() vrací referenci, do které se přiřazuje
    knx.platform() = platform_inst; 

    uint16_t addr = stringToAddress(address_str_.c_str());

    knx.bau().deviceObject().individualAddress(addr);    

    knx.start();
    ESP_LOGI("knx", "KNX Stack spuštěn s adresou %s", address_str_.c_str());
  }

  void loop() override {
    knx.loop();
  }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  std::string address_str_;

  uint16_t stringToAddress(const char* areaLineMember) {
    uint16_t area, line, member;
    if (sscanf(areaLineMember, "%hu.%hu.%hu", &area, &line, &member) == 3)
        return (area << 12) | (line << 8) | member;
    return 0;
  }
};

}  // namespace knx_custom
}  // namespace esphome
