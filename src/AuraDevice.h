#pragma once

#include <string>
#include <vector>
#include <tuple>
#include "IAuraDevice.h"

class aura_device
{
public:
  using id_name_list = std::pair<std::vector<std::wstring>, std::vector<std::wstring>>;
  using HSL = std::tuple<float, float, float>;

  aura_device();

  bool get_switch_state();
  void set_switch_state(bool state);

  std::vector<std::wstring> get_device_name_list();
  id_name_list get_effect_list();
  id_name_list get_LED_list();

  std::wstring get_current_effect();
  void set_effect(std::wstring effect_id);
  HSL get_HSL(std::wstring LED_id);
  void set_HSL(std::wstring LED_id, HSL hsl);

  void apply();

private:
  CComPtr<IAuraDevice> device;
};
