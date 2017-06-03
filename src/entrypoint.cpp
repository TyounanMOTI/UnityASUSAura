#include "IUnityInterface.h"
#include <memory>
#include "AuraDevice.h"
#include "debug.h"
#include "runtime_error.h"

namespace {
  std::unique_ptr<aura_device> device;
  std::unique_ptr<debug> dbg;
}

#ifdef __cplusplus
extern "C" {
#endif

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API set_debug_log_func(debug::log_func func)
  {
    dbg = std::make_unique<debug>(func);
  }

  int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API is_available() {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
    } catch (std::exception const&) {
      return FALSE;
    }
    return TRUE;
  }

  int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API query_switch_state() {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      return device->get_switch_state();
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
      return FALSE;
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API set_switch_state(int state) {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      device->set_switch_state((state == TRUE) ? true : false);
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
    }
  }

  int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API get_device_name_list(BSTR* out_names, int max_list_length)
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      auto names = device->get_device_name_list();
      for (int i = 0; i < min(names.size(), max_list_length); ++i) {
        out_names[i] = SysAllocString(names[i].c_str());  // 呼び出し側で解放する
      }
      return (int)min(names.size(), max_list_length);
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
      return 0;
    }
  }

  int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API get_effect_list(BSTR* out_ids, BSTR* out_names, int max_list_length)
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      auto id_name_list = device->get_effect_list();
      auto length = static_cast<int>(min(id_name_list.first.size(), max_list_length));
      for (int i = 0; i < length; ++i) {
        out_ids[i] = SysAllocString(id_name_list.first[i].c_str());  // 呼び出し側で解放する
        out_names[i] = SysAllocString(id_name_list.second[i].c_str());  // 呼び出し側で解放する
      }
      return length;
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
      return 0;
    }
  }

  int UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API get_LED_list(BSTR* out_ids, BSTR* out_names, int max_list_length)
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      auto id_name_list = device->get_LED_list();
      auto length = static_cast<int>(min(id_name_list.first.size(), max_list_length));
      for (int i = 0; i < length; ++i) {
        out_ids[i] = SysAllocString(id_name_list.first[i].c_str());  // 呼び出し側で解放する
        out_names[i] = SysAllocString(id_name_list.second[i].c_str());  // 呼び出し側で解放する
      }
      return length;
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
      return 0;
    }
  }

  BSTR UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API get_current_effect()
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      auto name = device->get_current_effect();
      return SysAllocString(name.c_str());  // 呼び出し側で解放する
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API set_effect(BSTR effect_id)
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      device->set_effect(effect_id);
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API get_HSL(BSTR LED_id, float* h, float* s, float* l)
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      auto hsl = device->get_HSL(LED_id);
      *h = std::get<0>(hsl);
      *s = std::get<1>(hsl);
      *l = std::get<2>(hsl);
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API set_HSL(BSTR LED_id, float h, float s, float l)
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      device->set_HSL(LED_id, std::make_tuple(h, s, l));
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
    }
  }

  void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API apply()
  {
    try {
      if (!device) {
        device = std::make_unique<aura_device>();
      }
      device->apply();
    } catch (runtime_error const& e) {
      if (dbg) {
        dbg->log(e.get_message());
      }
    }
  }

#ifdef __cplusplus
}
#endif
