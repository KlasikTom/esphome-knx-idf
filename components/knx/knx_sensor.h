#pragma once
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "knx_facade.h"

static const char* const TAG_KNX_SENSOR = "knx.sensor";

namespace esphome {
namespace knx_custom {

class KNXComponent;  // forward declaration

class KNXSensor : public sensor::Sensor, public Component {
 public:
  void set_group_address(const std::string &ga) { ga_str_ = ga; }
  void set_knx_component(KNXComponent* /*knx_comp*/) { /* referenci nepotřebujeme, knx je globální */ }

  void setup() override {
    // KNX GroupObject index: každý sensor dostane unikátní slot
    // V produkci se mapuje přes ETS AssociationTable.
    // Zde pro jednoduché projekty přidělujeme staticky.
    go_index_ = next_go_index_++;

    GroupObject& go = knx.getGroupObject(go_index_);

    // DPT 9.001 = teplota v °C (2-byte float)
    go.dataPointType(Dpt(9, 1));

    go.callback([this](GroupObject& go) {
      KNXValue val = go.value();
      float f = (float)val;
      ESP_LOGD(TAG_KNX_SENSOR, "KNX callback GA index %d: %.2f", go_index_, f);
      this->publish_state(f);
    });

    ESP_LOGI(TAG_KNX_SENSOR, "KNXSensor setup: GA='%s', GO index=%d", ga_str_.c_str(), go_index_);
  }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI - 1.0f; }

 protected:
  std::string ga_str_;
  uint16_t go_index_{0};

  // Statický čítač pro přidělování GO indexů
  static uint16_t next_go_index_;
};

uint16_t KNXSensor::next_go_index_ = 0;

}  // namespace knx_custom
}  // namespace esphome
