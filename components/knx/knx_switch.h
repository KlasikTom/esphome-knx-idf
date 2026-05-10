#pragma once
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "knx.h"
#include "knx_component.h"

static const char* const TAG_KNX_SWITCH = "knx.switch";

namespace esphome {
namespace knx_custom {

class KNXSwitch : public switch_::Switch, public Component {
 public:
  void set_group_address(const std::string &ga) { ga_str_ = ga; }

  void setup() override {
    ESP_LOGI(TAG_KNX_SWITCH, "KNXSwitch '%s' setup", ga_str_.c_str());
  }

  void loop() override {
    if (registered_) return;
    if (!knx_stack_started) return;

    go_index_ = next_go_index_++;
    GroupObject& go = knx.getGroupObject(go_index_);
    go.dataPointType(Dpt(1, 1));  // DPT 1.001 = bool on/off
    go.callback([this](GroupObject& go_obj) {
      bool val = (bool)(KNXValue)go_obj.value();
      ESP_LOGI(TAG_KNX_SWITCH, "'%s' GO[%d]: %s", ga_str_.c_str(), go_index_, val ? "ZAP" : "VYP");
      // Synchronizuj stav ESPHome switche bez zpětného odeslání
      this->state = val;
      this->publish_state(val);
    });
    registered_ = true;
    ESP_LOGI(TAG_KNX_SWITCH, "'%s' zaregistrovan jako GO[%d]", ga_str_.c_str(), go_index_);
  }

  float get_setup_priority() const override { return setup_priority::AFTER_WIFI - 1.0f; }

 protected:
  // Voláno když uživatel přepne switch v HA/ESPHome UI
  void write_state(bool state) override {
    if (!knx_stack_started) return;
    GroupObject& go = knx.getGroupObject(go_index_);
    KNXValue val(state);
    go.value(val, Dpt(1, 1));
    ESP_LOGI(TAG_KNX_SWITCH, "'%s' → KNX: %s", ga_str_.c_str(), state ? "ZAP" : "VYP");
    publish_state(state);
  }

  std::string ga_str_;
  bool registered_{false};
  uint16_t go_index_{0};
  static uint16_t next_go_index_;
};

uint16_t KNXSwitch::next_go_index_ = 0;  // pokračuje za KNXSensor indexy

}  // namespace knx_custom
}  // namespace esphome