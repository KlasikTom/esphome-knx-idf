#pragma once
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "knx_facade.h"

namespace esphome {
namespace knx_custom {

class KNXSensor : public sensor::Sensor, public Component {
 public:
  void set_group_address(const std::string &ga) { ga_str_ = ga; }

  void setup() override {
    uint16_t addr = stringToGroupAddress(ga_str_.c_str());

    // 1. Získáme existující GroupObject z tabulky.
    // Knihovna ho identifikuje pomocí ASAP (Application Layer Service Access Point).
    // Použijeme index 0, což je v malých projektech bez ETS obvykle první volný slot.
    go_ = &knx.getGroupObject(0); 
    
    // 2. Nastavení typu databodu (DPT 9.001 pro teplotu)
    // V group_object.h vidíme metodu dataPointType(Dpt value)
    go_->dataPointType(DPT_Value_Temp);

    // Poznámka k adrese: V této verzi knihovny se skupinová adresa 
    // mapuje v AssociationTable. Pokud ji chceš vnutit natvrdo:
    // Tato verze nemá go->groupAddress(), adresa se řeší v knx.loop() 
    // skrze porovnávání v tabulce. Pro testování v ESPHome budeme 
    // předpokládat, že stack je správně inicializován.

    go_->callback([this](GroupObject& go) {
      // 3. Oprava získání hodnoty:
      // go.value() vrací objekt KNXValue.
      // KNXValue má definovaný 'operator float()', takže ho musíme explicitně přetypovat.
      KNXValue knx_val = go.value();
      float val = (float)knx_val; 
      this->publish_state(val);
    });
  }

 protected:
  std::string ga_str_;
  GroupObject* go_{nullptr};

  uint16_t stringToGroupAddress(const char* ga) {
    uint16_t a, b, c;
    if (sscanf(ga, "%hu/%hu/%hu", &a, &b, &c) == 3)
        return (a << 11) | (b << 8) | c;
    return 0;
  }
};

}  // namespace knx_custom
}  // namespace esphome