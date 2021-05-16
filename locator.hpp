#pragma once

#include "services.hpp"

#include <wbemcli.h>
#include <iostream>

DEFINE_GetIIDForInterface(IWbemLocator)

class WbemLocator : public IUnknownImpl<IWbemLocator> {
 public:
  HRESULT STDMETHODCALLTYPE ConnectServer(BSTR, BSTR, BSTR, BSTR, LONG, BSTR,
                                          IWbemContext *,
                                          IWbemServices **services) override {
    boost::intrusive_ptr<WbemServices> servicesPtr(
        new WbemServices(), /*add_ref=*/true);  // client must call Release
    if (servicesPtr == nullptr) {
      return E_OUTOFMEMORY;
    }
    *services = servicesPtr.get();

    return S_OK;
  }
};

inline void intrusive_ptr_add_ref(WbemLocator *obj) {
  obj->AddRef();
}
inline void intrusive_ptr_release(WbemLocator *obj) {
  obj->Release();
}
