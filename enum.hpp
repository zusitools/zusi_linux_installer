#pragma once

#include "util.hpp"
#include "object.hpp"

#include <wbemcli.h>

#include <boost/smart_ptr/intrusive_ptr.hpp>

DEFINE_GetIIDForInterface(IEnumVARIANT)

class EnumVariant : public IUnknownImpl<IEnumVARIANT> {
 public:
  EnumVariant(std::vector<boost::intrusive_ptr<SWbemObject>> objects)
      : _objects(std::move(objects)) {
  }

  virtual HRESULT STDMETHODCALLTYPE Next(ULONG celt, VARIANT *rgVar,
                                         ULONG *pCeltFetched) {
    if (_index >= _objects.size()) {
      return S_FALSE;
    }

    V_VT(rgVar) = VT_DISPATCH;
    auto *object = _objects.at(_index).get();
    ++_index;
    intrusive_ptr_add_ref(object);
    V_DISPATCH(rgVar) = object;
    return S_OK;
  }

  virtual HRESULT STDMETHODCALLTYPE Skip(ULONG celt) NOTIMPL

  virtual HRESULT STDMETHODCALLTYPE Reset() NOTIMPL

  virtual HRESULT STDMETHODCALLTYPE Clone(IEnumVARIANT **ppEnum) NOTIMPL
 private:
  std::vector<boost::intrusive_ptr<SWbemObject>> _objects;
  size_t _index{0u};
};

inline void intrusive_ptr_add_ref(EnumVariant *obj) {
  obj->AddRef();
}
inline void intrusive_ptr_release(EnumVariant *obj) {
  obj->Release();
}
