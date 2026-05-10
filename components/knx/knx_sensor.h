#pragma once
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "knx.h"
#include "knx_component.h"

static const char* const TAG_KNX_SENSOR = "knx.sensor";

namespace esphome {
namespace knx_custom {

class KNXSensor : public sensor::Sensor, public Component {
 public:
  void set_group_address(const std::string &ga) { ga_str_ = ga; }

  void setup() override {
    ESP_LOGI(TAG_KNX_SENSOR, "KNXSensor '%s' setup", ga_str_.c_str());
  }

  void loop() override {
    if (registered_) return;
    // Bez ETS knx.configured() = false, ale GO fungují i tak
    // Zaregistruj jakmile je KNX stack spuštěn (knx.started())
    if (!knx_stack_started) return;

    go_index_ = next_go_index_++;
    GroupObject& go = knx.getGroupObject(go_index_);
    go.dataPointType(Dpt(9, 1));  // DPT 9.001 = teplota °C
    go.callback([this](GroupObject& go_obj) {
      float f = (float)(KNXValue)go_obj.value();
      ESP_LOGI(TAG_KNX_SENSOR, "'%s' GO[%d]: %.2f °C", ga_str_.c_str(), go_index_, f);
      this->publish_state(f);
    });
    registered_ = true;
    ESP_LOGI(TAG_KNX_SENSOR, "'%s' zaregistrovan jako GO[%d]", ga_str_.c_str(), go_index_);
  }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI - 1.0f; }

 protected:
  std::string ga_str_;
  bool registered_{false};
  uint16_t go_index_{0};
  static uint16_t next_go_index_;
};

uint16_t KNXSensor::next_go_index_ = 0;

}  // namespace knx_custom
}  // namespace esphome
