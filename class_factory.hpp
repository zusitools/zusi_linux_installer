#pragma once

#include "iunknown_impl.hpp"

extern HINSTANCE g_hInst;
extern UINT g_DllRefCount;

DEFINE_GetIIDForInterface(IClassFactory)

class SWbemLocatorClassFactory : public IUnknownImpl<IClassFactory> {
 public:
  SWbemLocatorClassFactory();
  virtual ~SWbemLocatorClassFactory();

  // IClassFactory methods
  STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, LPVOID *);
  STDMETHODIMP LockServer(BOOL) {
    return E_NOTIMPL;
  };
};

inline void intrusive_ptr_add_ref(SWbemLocatorClassFactory *obj) {
  obj->AddRef();
}
inline void intrusive_ptr_release(SWbemLocatorClassFactory *obj) {
  obj->Release();
}
