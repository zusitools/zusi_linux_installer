#pragma once

#include "services.hpp"
#include "util.hpp"
#include "debug.hpp"

#include <wbemdisp.h>

DEFINE_GetIIDForInterface(IDispatch)
DEFINE_GetIIDForInterface(ISWbemLocator)

class SWbemLocator : public IUnknownImpl<IDispatch> {
 public:
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
    if (name == "ConnectServer") {
      *rgDispId = 42;
      return S_OK;
    } else {
      return E_FAIL;
    }
  }

  virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid,
                                           LCID lcid, WORD wFlags,
                                           DISPPARAMS *pDispParams,
                                           VARIANT *pVarResult,
                                           EXCEPINFO *pExcepInfo,
                                           UINT *puArgErr) override {
    // ConnectServer is the only implemented method.
    boost::intrusive_ptr<SWbemServices> servicesPtr(
        new SWbemServices(), /*add_ref=*/true);  // client must call Release
    if (servicesPtr == nullptr) {
      return E_OUTOFMEMORY;
    }
    V_VT(pVarResult) = VT_DISPATCH;
    V_DISPATCH(pVarResult) = servicesPtr.get();

    return S_OK;
  }
};

inline void intrusive_ptr_add_ref(SWbemLocator *obj) {
  obj->AddRef();
}
inline void intrusive_ptr_release(SWbemLocator *obj) {
  obj->Release();
}
