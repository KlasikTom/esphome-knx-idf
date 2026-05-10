#pragma once
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "knx_facade.h"
#include "esp32_platform.h"

static const char* const TAG_KNX = "knx";

namespace esphome {
namespace knx_custom {

class KNXComponent : public Component {
 public:
  void set_individual_address(const std::string &address) { address_str_ = address; }

  void setup() override {
    ESP_LOGI(TAG_KNX, "Inicializace KNX stacku...");

    uint16_t addr = stringToAddress(address_str_.c_str());
    if (addr == 0) {
      ESP_LOGE(TAG_KNX, "Neplatna individualni adresa: %s", address_str_.c_str());
      this->mark_failed();
      return;
    }

    // KNX stack se inicializuje přes globální instanci 'knx' (definovánu v knx_facade.cpp)
    // ARDUINO_ARCH_ESP32 + MASK_VERSION=0x07B0 → KnxFacade<KnxEsp32Platform, Bau07B0>
    knx.bau().deviceObject().individualAddress(addr);
    knx.start();

    ESP_LOGI(TAG_KNX, "KNX stack spusten, adresa: %s (0x%04X)", address_str_.c_str(), addr);
  }

  void loop() override {
    knx.loop();
  }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  std::string address_str_;

  static uint16_t stringToAddress(const char* s) {
    unsigned int area, line, member;
    if (sscanf(s, "%u.%u.%u", &area, &line, &member) == 3) {
      if (area <= 15 && line <= 15 && member <= 255)
        return (uint16_t)((area << 12) | (line << 8) | member);
    }
    return 0;
  }
};

}  // namespace knx_custom
}  // namespace esphome
