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
      throw runtime_error(L"ASUS Aura: �z��v�f���̎擾�Ɏ��s���܂����B");
    }
    hr = SafeArrayGetUBound(array_ptr, 1, &upper_bound);
    if (FAILED(hr)) {
      throw runtime_error(L"ASUS Aura: �z��v�f���̎擾�Ɏ��s���܂����B");
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
      throw runtime_error(L"ASUS Aura: �z��ւ̃A�N�Z�X�Ɏ��s���܂����B");
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
    throw runtime_error(L"ASUS Aura: �������Ɏ��s���܂����B");
  }
}

bool aura_device::get_switch_state()
{
  VARIANT_BOOL state;
  auto hr = device->QuerySwitchState(&state);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: �X�C�b�`�̏�Ԏ擾�Ɏ��s���܂����B");
  }
  return state;
}

void aura_device::set_switch_state(bool state)
{
  auto input = state ? VARIANT_TRUE : VARIANT_FALSE;
  auto hr = device->SetSwitchState(input);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: �X�C�b�`�̏�ԕύX�Ɏ��s���܂����B");
  }
}

std::vector<std::wstring> aura_device::get_device_name_list()
{
  safe_array device_names;
  auto hr = device->QueryDeviceNameList(&device_names);

  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: �f�o�C�X�ꗗ�̎擾�Ɏ��s���܂����B");
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
    throw runtime_error(L"ASUS Aura: �G�t�F�N�g�ꗗ�̎擾�Ɏ��s���܂����B");
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
    throw runtime_error(L"ASUS Aura: LED�ꗗ�̎擾�Ɏ��s���܂����B");
  }

  safe_array_access ids_access(ids), names_access(names);
  return std::make_pair(ids_access.generate_data(), names_access.generate_data());
}

std::wstring aura_device::get_current_effect()
{
  BSTR effect_id;
  auto hr = device->QueryCurrentEffect(&effect_id);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: ���݂̃G�t�F�N�g�擾�Ɏ��s���܂����B");
  }

  return std::wstring(effect_id, SysStringLen(effect_id));
}

void aura_device::set_effect(std::wstring effect_id)
{
  BSTR effect_id_str = SysAllocStringLen(effect_id.data(), (UINT)effect_id.size());
  auto hr = device->SetEffect(effect_id_str);
  SysFreeString(effect_id_str);
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: �G�t�F�N�g�̐ݒ�Ɏ��s���܂����B");
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
    throw runtime_error(L"ASUS Aura: ���݂̐F�̎擾�Ɏ��s���܂����B");
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
    throw runtime_error(L"ASUS Aura: �F�̐ݒ�Ɏ��s���܂����B");
  }
}

void aura_device::apply()
{
  auto hr = device->Apply();
  if (FAILED(hr)) {
    throw runtime_error(L"ASUS Aura: �ύX�̓K�p�Ɏ��s���܂����B");
  }
}
