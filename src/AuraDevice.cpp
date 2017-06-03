#include "AuraDevice.h"
#include "runtime_error.h"
#include <memory>

class noncopyable
{
protected:
  constexpr noncopyable() = default;
  ~noncopyable() = default;
  noncopyable(noncopyable const&) = delete;
  noncopyable& operator=(noncopyable const&) = delete;
};

class safe_array : public noncopyable
{
public:
  ~safe_array()
  {
    if (array_ptr) {
      SafeArrayDestroy(array_ptr);
    }
  }

  long size() const
  {
    if (!array_ptr) {
      return 0;
    }

    long lower_bound, upper_bound;
    auto hr = SafeArrayGetLBound(array_ptr, 1, &lower_bound);
    if (FAILED(hr)) {
      throw runtime_error(L"ASUS Aura: 配列要素数の取得に失敗しました。");
    }
    hr = SafeArrayGetUBound(array_ptr, 1, &upper_bound);
    if (FAILED(hr)) {
      throw runtime_error(L"ASUS Aura: 配列要素数の取得に失敗しました。");
    }

    return upper_bound - lower_bound + 1;
  }

  SAFEARRAY* data() noexcept
  {
    return array_ptr;
  }

  SAFEARRAY** operator&()
  {
    return &array_ptr;
  }

private:
  SAFEARRAY* array_ptr = nullptr;
};

class safe_array_access : public noncopyable
{
public:
  explicit safe_array_access(safe_array& array)
    : array(array)
  {
    auto hr = SafeArrayAccessData(array.data(), (void**)&bstr);
    if (FAILED(hr)) {
      throw runtime_error(L"ASUS Aura: 配列へのアクセスに失敗しました。");
    }
  }

  std::vector<std::wstring> generate_data()
  {
    auto size = array.size();
    std::vector<std::wstring> retval(size);
    for (long i = 0; i < size; ++i) {
      retval[i] = std::wstring(bstr[i], SysStringLen(bstr[i]));
    }
    return retval;
  }

  ~safe_array_access()
  {
    SafeArrayUnaccessData(array.data());
  }

private:
  safe_array& array;
  BSTR* bstr;
};

aura_device::aura_device()
{
  CLSID clsid;
  HRESULT hr = CLSIDFromProgID(OLESTR("AsRogAuraService.AuraDevice"), &clsid);
  hr = CoCreateInstance(clsid, NULL, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&device));

  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: 初期化に失敗しました。");
  }
}

bool aura_device::get_switch_state()
{
  VARIANT_BOOL state;
  auto hr = device->QuerySwitchState(&state);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: スイッチの状態取得に失敗しました。");
  }
  return state;
}

void aura_device::set_switch_state(bool state)
{
  auto input = state ? VARIANT_TRUE : VARIANT_FALSE;
  auto hr = device->SetSwitchState(input);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: スイッチの状態変更に失敗しました。");
  }
}

std::vector<std::wstring> aura_device::get_device_name_list()
{
  safe_array device_names;
  auto hr = device->QueryDeviceNameList(&device_names);

  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: デバイス一覧の取得に失敗しました。");
  }

  safe_array_access access(device_names);
  return access.generate_data();
}

aura_device::id_name_list aura_device::get_effect_list()
{
  safe_array ids;
  safe_array names;
  auto hr = device->QueryEffectCapList(&ids, &names);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: エフェクト一覧の取得に失敗しました。");
  }

  safe_array_access ids_access(ids);
  safe_array_access names_access(names);
  return std::make_pair(ids_access.generate_data(), names_access.generate_data());
}

aura_device::id_name_list aura_device::get_LED_list()
{
  safe_array ids, names;
  auto hr = device->QueryLedList(&ids, &names);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: LED一覧の取得に失敗しました。");
  }

  safe_array_access ids_access(ids), names_access(names);
  return std::make_pair(ids_access.generate_data(), names_access.generate_data());
}

std::wstring aura_device::get_current_effect()
{
  BSTR effect_id;
  auto hr = device->QueryCurrentEffect(&effect_id);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: 現在のエフェクト取得に失敗しました。");
  }

  return std::wstring(effect_id, SysStringLen(effect_id));
}

void aura_device::set_effect(std::wstring effect_id)
{
  BSTR effect_id_str = SysAllocStringLen(effect_id.data(), (UINT)effect_id.size());
  auto hr = device->SetEffect(effect_id_str);
  SysFreeString(effect_id_str);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: エフェクトの設定に失敗しました。");
  }
}

aura_device::HSL aura_device::get_HSL(std::wstring LED_id)
{
  HSL hsl;
  auto id_str = SysAllocStringLen(LED_id.data(), (UINT)LED_id.size());
  auto hr = device->QueryHslFromLed(
    id_str,
    &std::get<0>(hsl),
    &std::get<1>(hsl),
    &std::get<2>(hsl));
  SysFreeString(id_str);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: 現在の色の取得に失敗しました。");
  }
  return hsl;
}

void aura_device::set_HSL(std::wstring LED_id, HSL hsl)
{
  auto id_str = SysAllocStringLen(LED_id.data(), (UINT)LED_id.size());
  auto hr = device->SetHslToLed(
    id_str,
    std::get<0>(hsl),
    std::get<1>(hsl),
    std::get<2>(hsl)
  );
  SysFreeString(id_str);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: 色の設定に失敗しました。");
  }
}

void aura_device::apply()
{
  auto hr = device->Apply();
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: 変更の適用に失敗しました。");
  }
}
