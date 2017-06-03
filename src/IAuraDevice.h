#pragma once

#include <atlbase.h>


MIDL_INTERFACE("EBA0F532-3726-465B-9D21-E05C70C7263D")
IAuraDevice : public IDispatch
{
public:
  virtual /* [id] */ HRESULT STDMETHODCALLTYPE QuerySDKVersionNumber(
    /* [retval][out] */ BSTR *versionNum) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE QueryDeviceNameList(
    /* [retval][out] */ SAFEARRAY * *deviceNameList) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE QueryEffectCapList(
    /* [out] */ SAFEARRAY * *effectIdList,
    /* [out] */ SAFEARRAY * *effectNameList) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE QueryLedList(
    /* [out] */ SAFEARRAY * *ledIdList,
    /* [out] */ SAFEARRAY * *ledNameList) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE QueryCurrentEffect(
    /* [retval][out] */ BSTR *effectId) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetEffect(
    /* [in] */ BSTR effectId) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE QueryHslFromLed(
    /* [in] */ BSTR ledId,
    /* [out] */ float *h,
    /* [out] */ float *s,
    /* [out] */ float *l) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetHslToLed(
    /* [in] */ BSTR ledId,
    /* [in] */ float h,
    /* [in] */ float s,
    /* [in] */ float l) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE QuerySwitchState(
    /* [retval][out] */ VARIANT_BOOL *isOn) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetSwitchState(
    /* [in] */ VARIANT_BOOL on) = 0;

  virtual /* [id] */ HRESULT STDMETHODCALLTYPE Apply(void) = 0;

};

const IID IID_IAuraDevice;

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)
MIDL_DEFINE_GUID(IID, IID_IAuraDevice, 0xEBA0F532, 0x3726, 0x465B, 0x9D, 0x21, 0xE0, 0x5C, 0x70, 0xC7, 0x26, 0x3D);
