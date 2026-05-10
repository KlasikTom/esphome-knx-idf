#pragma once
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/wifi/wifi_component.h"
#include <Arduino.h>
#include "knx.h"
#include "arduino_platform.h"

static const char* const TAG_KNX = "knx";

namespace esphome {
namespace knx_custom {

// Globální flag - true jakmile knx.start() proběhlo
// Používají ho KNXSensor a další komponenty
static bool knx_stack_started = false;

class KNXComponent : public Component {
 public:
  void set_individual_address(const std::string &address) { address_str_ = address; }

  void setup() override {
    // Serial musí být nastaven hned - KNX loguje od první operace
    ArduinoPlatform::SerialDebug = &Serial;

    addr_ = stringToAddress(address_str_.c_str());
    if (addr_ == 0) {
      ESP_LOGE(TAG_KNX, "Neplatna individualni adresa: %s", address_str_.c_str());
      this->mark_failed();
    }
  }

  void loop() override {
    if (!knx_stack_started) {
      if (!wifi::global_wifi_component || !wifi::global_wifi_component->is_connected())
        return;

      if (wifi_connected_ms_ == 0) { wifi_connected_ms_ = millis(); return; }
      if (millis() - wifi_connected_ms_ < 500) return;

      ESP_LOGI(TAG_KNX, "Startuji KNX (adresa: %s)...", address_str_.c_str());
      knx.bau().deviceObject().individualAddress(addr_);
      knx.readMemory();
      knx.start();
      knx_stack_started = true;
      ESP_LOGI(TAG_KNX, "KNX spusten, configured=%d", knx.configured());
      return;
    }

    knx.loop();
  }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  std::string address_str_;
  uint16_t addr_{0};
  uint32_t wifi_connected_ms_{0};

  static uint16_t stringToAddress(const char* s) {
    unsigned int a, l, m;
    if (sscanf(s, "%u.%u.%u", &a, &l, &m) == 3 && a <= 15 && l <= 15 && m <= 255)
      return (uint16_t)((a << 12) | (l << 8) | m);
    return 0;
  }
};

}  // namespace knx_custom
}  // namespace esphome
