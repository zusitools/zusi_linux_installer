#pragma once

#include "util.hpp"

#include <wbemdisp.h>

#include <boost/smart_ptr/intrusive_ptr.hpp>

class SWbemObject : public IUnknownImpl<IDispatch> {
 public:
  SWbemObject(std::u16string deviceId, std::u16string pnpDeviceId)
      : _deviceId(std::move(deviceId)), _pnpDeviceId(std::move(pnpDeviceId)) {
  }

  virtual HRESULT STDMETHODCALLTYPE
  GetTypeInfoCount(UINT *pctinfo) override NOTIMPL

  virtual HRESULT STDMETHODCALLTYPE
  GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo) override NOTIMPL

  virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid,
                                                  LPOLESTR *rgszNames,
                                                  UINT cNames, LCID lcid,
                                                  DISPID *rgDispId) override {
    if (cNames != 1) {
      return E_NOTIMPL;
    }
    const auto &name = CrudeBstrToString(rgszNames[0]);
    if (name == "DeviceID") {
      *rgDispId = 42;
    } else if (name == "PnPDeviceID") {
      *rgDispId = 43;
    } else {
      return E_FAIL;
    }
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid,
                                           LCID lcid, WORD wFlags,
                                           DISPPARAMS *pDispParams,
                                           VARIANT *pVarResult,
                                           EXCEPINFO *pExcepInfo,
                                           UINT *puArgErr) override {
    // DeviceID and PnpDeviceID are the only methods implemented.
    V_VT(pVarResult) = VT_BSTR;
    const auto &result = (dispIdMember == 42) ? _deviceId : _pnpDeviceId;
    V_BSTR(pVarResult) = SysAllocStringLen(result.data(), result.size());
    return S_OK;
  }

 private:
  std::u16string _deviceId;
  std::u16string _pnpDeviceId;
};

inline void intrusive_ptr_add_ref(SWbemObject *obj) {
  obj->AddRef();
}
inline void intrusive_ptr_release(SWbemObject *obj) {
  obj->Release();
}
