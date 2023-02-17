#pragma once

#include "util.hpp"
#include "enum.hpp"
#include "serial.hpp"

#include <wbemdisp.h>

#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <algorithm>
#include <string>

namespace {

std::u16string PadSerial(std::u16string serial) {
  if (serial.size() < 8) {
    serial.insert(serial.size(), 8 - serial.size(), L'_');
  }
  std::replace(serial.begin(), serial.end(), static_cast<char16_t>(L' '),
               static_cast<char16_t>(L'_'));
  return serial;
}

}  // namespace

class SWbemObjectSet : public IUnknownImpl<IDispatch> {
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
    if (name == "_NewEnum") {
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
    // _NewEnum is the only implemented method.
    std::vector<boost::intrusive_ptr<SWbemObject>> objects;
    static std::u16string pnpDeviceId{
        std::u16string{
            u"USBSTOR\\Disk&Ven_Generic&Prod_Flash_Disk&Rev_8.07\\"} +
        PadSerial(GetUsbSerial()) + std::u16string{u"&0"}};
    objects.reserve(26);
    for (char16_t driveLetter = u'A'; driveLetter <= u'Z'; ++driveLetter) {
      objects.push_back(
          new SWbemObject(driveLetter + std::u16string(u":"), pnpDeviceId));
    }

    boost::intrusive_ptr<EnumVariant> enumPtr(
        new EnumVariant(std::move(objects)),
        /*add_ref=*/true);  // client must call Release
    if (enumPtr == nullptr) {
      return E_OUTOFMEMORY;
    }
    V_VT(pVarResult) = VT_UNKNOWN;
    V_UNKNOWN(pVarResult) = enumPtr.get();
    return S_OK;
  }
};

inline void intrusive_ptr_add_ref(SWbemObjectSet *obj) {
  obj->AddRef();
}
inline void intrusive_ptr_release(SWbemObjectSet *obj) {
  obj->Release();
}
