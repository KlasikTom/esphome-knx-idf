#pragma once
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "knx_facade.h"

static const char* const TAG_KNX_SENSOR = "knx.sensor";

namespace esphome {
namespace knx_custom {

class KNXComponent;

class KNXSensor : public sensor::Sensor, public Component {
 public:
  void set_group_address(const std::string &ga) { ga_str_ = ga; }
  void set_knx_component(KNXComponent* /*unused*/) {}

  void setup() override {
    go_index_ = next_go_index_++;
    GroupObject& go = knx.getGroupObject(go_index_);
    go.dataPointType(Dpt(9, 1));  // DPT 9.001 = temperature °C
    go.callback([this](GroupObject& go) {
      KNXValue val = go.value();
      float f = (float)val;
      ESP_LOGD(TAG_KNX_SENSOR, "GA '%s' GO[%d]: %.2f", ga_str_.c_str(), go_index_, f);
      this->publish_state(f);
    });
    ESP_LOGI(TAG_KNX_SENSOR, "KNXSensor: GA='%s', GO index=%d", ga_str_.c_str(), go_index_);
  }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI - 1.0f; }

 protected:
  std::string ga_str_;
  uint16_t go_index_{0};
  static uint16_t next_go_index_;
};

uint16_t KNXSensor::next_go_index_ = 0;

}  // namespace knx_custom
}  // namespace esphome
