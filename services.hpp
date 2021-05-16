#pragma once

#include "objectset.hpp"
#include "util.hpp"

#include <wbemdisp.h>
#include <iostream>

#include <boost/smart_ptr/intrusive_ptr.hpp>

DEFINE_GetIIDForInterface(ISWbemServices)

class SWbemServices : public IUnknownImpl<IDispatch> {
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
    if (name == "ExecQuery") {
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
    // ExecQuery is the only implemented method.
    boost::intrusive_ptr<SWbemObjectSet> enumPtr(
        new SWbemObjectSet(), /*add_ref=*/true);  // client must call Release
    if (enumPtr == nullptr) {
      return E_OUTOFMEMORY;
    }
    V_VT(pVarResult) = VT_DISPATCH;
    V_DISPATCH(pVarResult) = enumPtr.get();
    return S_OK;
  }
};

inline void intrusive_ptr_add_ref(SWbemServices *obj) {
  obj->AddRef();
}
inline void intrusive_ptr_release(SWbemServices *obj) {
  obj->Release();
}
