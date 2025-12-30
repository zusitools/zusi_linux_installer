#define COBJMACROS

#define INITGUID
#include <initguid.h>

#include <olectl.h>
#include <shlobj.h>
#include <strsafe.h>
#include <windows.h>
#include <wbemdisp.h>

#include "unixlib.h"

BSTR g_pnp_device_id;

// ISWbemObject

struct swbem_object {
  IDispatch IDispatch_iface;
  BSTR device_id;
};

static inline struct swbem_object* swbem_object_impl_from_IDispatch(
    IDispatch* iface) {
  return CONTAINING_RECORD(iface, struct swbem_object, IDispatch_iface);
}

static HRESULT WINAPI swbem_object_QueryInterface(IDispatch* iface, REFIID riid,
                                                  void** ppv) {
  if (IsEqualGUID(&IID_IUnknown, riid) || IsEqualGUID(&IID_IDispatch, riid)) {
    *ppv = iface;
    return S_OK;
  }

  return E_NOINTERFACE;
}

static ULONG WINAPI swbem_object_AddRef(IDispatch* iface) {
  return 2;
}

static ULONG WINAPI swbem_object_Release(IDispatch* iface) {
  return 1;
}

static HRESULT WINAPI swbem_object_GetTypeInfoCount(IDispatch* iface,
                                                    UINT* pctinfo) {
  OutputDebugStringA("swbem_object_GetTypeInfoCount");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_object_GetTypeInfo(IDispatch* iface, UINT iTInfo,
                                               LCID lcid, ITypeInfo** ppTInfo) {
  OutputDebugStringA("swbem_object_GetTypeInfo");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_object_GetIDsOfNames(IDispatch* iface, REFIID riid,
                                                 LPOLESTR* rgszNames,
                                                 UINT cNames, LCID lcid,
                                                 DISPID* rgDispId) {
  OutputDebugStringA("swbem_object_GetIDsOfNames");
  if (cNames != 1) {
    return E_NOTIMPL;
  }
  if (wcsncmp(rgszNames[0], L"DeviceID", 8) == 0) {
    *rgDispId = 42;
    return S_OK;
  } else if (wcsncmp(rgszNames[0], L"PnPDeviceID", 11) == 0) {
    *rgDispId = 43;
    return S_OK;
  } else {
    return E_FAIL;
  }
}

static HRESULT WINAPI swbem_object_Invoke(IDispatch* iface, DISPID dispIdMember,
                                          REFIID riid, LCID lcid, WORD wFlags,
                                          DISPPARAMS* pDispParams,
                                          VARIANT* pVarResult,
                                          EXCEPINFO* pExcepInfo,
                                          UINT* puArgErr) {
  struct swbem_object* This = swbem_object_impl_from_IDispatch(iface);
  // DeviceID and PnpDeviceID are the only methods implemented.
  OutputDebugStringA("swbem_object_Invoke");
  V_VT(pVarResult) = VT_BSTR;
  V_BSTR(pVarResult) =
      SysAllocString(dispIdMember == 42 ? This->device_id : g_pnp_device_id);
  return S_OK;
}

static const IDispatchVtbl swbem_object_vtbl = {
    swbem_object_QueryInterface,    //
    swbem_object_AddRef,            //
    swbem_object_Release,           //
    swbem_object_GetTypeInfoCount,  //
    swbem_object_GetTypeInfo,       //
    swbem_object_GetIDsOfNames,     //
    swbem_object_Invoke             //
};

// IEnumVARIANT

struct enum_variant {
  IEnumVARIANT IEnumVARIANT_iface;
  size_t index;
};

static inline struct enum_variant* enum_variant_impl_from_IEnumVARIANT(
    IEnumVARIANT* iface) {
  return CONTAINING_RECORD(iface, struct enum_variant, IEnumVARIANT_iface);
}

static HRESULT WINAPI enum_variant_QueryInterface(IEnumVARIANT* iface,
                                                  REFIID riid, void** ppv) {
  *ppv = NULL;

  if (IsEqualGUID(riid, &IID_IEnumVARIANT)) {
    *ppv = iface;
  } else {
    return E_NOINTERFACE;
  }

  return S_OK;
}

static ULONG WINAPI enum_variant_AddRef(IEnumVARIANT* iface) {
  return 2;
}

static ULONG WINAPI enum_variant_Release(IEnumVARIANT* iface) {
  return 1;
}

static HRESULT WINAPI enum_variant_Next(IEnumVARIANT* iface, ULONG celt,
                                        VARIANT* rgVar, ULONG* pCeltFetched) {
  struct swbem_object* object;
  wchar_t* buf;
  size_t bufsize;
  struct enum_variant* This = enum_variant_impl_from_IEnumVARIANT(iface);
  OutputDebugStringA("enum_variant_Next");
  if (This->index >= 26) {
    return S_FALSE;
  }

  if (!(object = calloc(1, sizeof(*object)))) {
    return E_OUTOFMEMORY;
  }

  object->IDispatch_iface.lpVtbl = &swbem_object_vtbl;

  bufsize = 2;
  if (!(buf = calloc(bufsize + 1, sizeof(*buf)))) {
    return E_OUTOFMEMORY;
  }
  buf[0] = L'A' + This->index;
  buf[1] = L':';
  buf[2] = L'\0';
  object->device_id = SysAllocStringLen(buf, bufsize);

  ++This->index;
  V_VT(rgVar) = VT_DISPATCH;
  V_DISPATCH(rgVar) = &object->IDispatch_iface;
  return S_OK;
}

static HRESULT WINAPI enum_variant_Skip(IEnumVARIANT* This, ULONG celt) {
  OutputDebugStringA("enum_variant_Skip");
  return E_NOTIMPL;
}

static HRESULT WINAPI enum_variant_Reset(IEnumVARIANT* This) {
  OutputDebugStringA("enum_variant_Reset");
  return E_NOTIMPL;
}

static HRESULT WINAPI enum_variant_Clone(IEnumVARIANT* This,
                                         IEnumVARIANT** ppEnum) {
  OutputDebugStringA("enum_variant_Clone");
  return E_NOTIMPL;
}

static const IEnumVARIANTVtbl enum_variant_vtbl = {
    enum_variant_QueryInterface,  //
    enum_variant_AddRef,          //
    enum_variant_Release,         //
    enum_variant_Next,            //
    enum_variant_Skip,            //
    enum_variant_Reset,           //
    enum_variant_Clone            //
};

// ISWbemObjectSet

struct swbem_object_set {
  IDispatch IDispatch_iface;
};

static HRESULT WINAPI swbem_object_set_QueryInterface(IDispatch* iface,
                                                      REFIID riid, void** ppv) {
  if (IsEqualGUID(&IID_IUnknown, riid) || IsEqualGUID(&IID_IDispatch, riid)) {
    *ppv = iface;
    return S_OK;
  }

  return E_NOINTERFACE;
}

static ULONG WINAPI swbem_object_set_AddRef(IDispatch* iface) {
  return 2;
}

static ULONG WINAPI swbem_object_set_Release(IDispatch* iface) {
  return 1;
}

static HRESULT WINAPI swbem_object_set_GetTypeInfoCount(IDispatch* iface,
                                                        UINT* pctinfo) {
  OutputDebugStringA("swbem_object_set_GetTypeInfoCount");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_object_set_GetTypeInfo(IDispatch* iface,
                                                   UINT iTInfo, LCID lcid,
                                                   ITypeInfo** ppTInfo) {
  OutputDebugStringA("swbem_object_set_GetTypeInfo");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_object_set_GetIDsOfNames(IDispatch* iface,
                                                     REFIID riid,
                                                     LPOLESTR* rgszNames,
                                                     UINT cNames, LCID lcid,
                                                     DISPID* rgDispId) {
  OutputDebugStringA("swbem_object_set_GetIDsOfNames");
  if (cNames != 1) {
    return E_NOTIMPL;
  }
  if (wcsncmp(rgszNames[0], L"_NewEnum", 8) == 0) {
    *rgDispId = 42;
    return S_OK;
  } else {
    return E_FAIL;
  }
}

static HRESULT WINAPI swbem_object_set_Invoke(
    IDispatch* iface, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
    DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo,
    UINT* puArgErr) {
  struct enum_variant* enum_variant;
  OutputDebugStringA("swbem_object_set_Invoke");

  // _NewEnum is the only implemented method.

  if (!(enum_variant = calloc(1, sizeof(*enum_variant)))) {
    return E_OUTOFMEMORY;
  }

  enum_variant->IEnumVARIANT_iface.lpVtbl = &enum_variant_vtbl;
  enum_variant->index = 0;
  V_VT(pVarResult) = VT_UNKNOWN;
  V_UNKNOWN(pVarResult) = (IUnknown*)&enum_variant->IEnumVARIANT_iface;
  return S_OK;
}

static const IDispatchVtbl swbem_object_set_vtbl = {
    swbem_object_set_QueryInterface,    //
    swbem_object_set_AddRef,            //
    swbem_object_set_Release,           //
    swbem_object_set_GetTypeInfoCount,  //
    swbem_object_set_GetTypeInfo,       //
    swbem_object_set_GetIDsOfNames,     //
    swbem_object_set_Invoke             //
};

// ISWbemServices

struct swbem_services {
  IDispatch IDispatch_iface;
};

static HRESULT WINAPI swbem_services_QueryInterface(IDispatch* iface,
                                                    REFIID riid, void** ppv) {
  if (IsEqualGUID(&IID_IUnknown, riid) || IsEqualGUID(&IID_IDispatch, riid)) {
    *ppv = iface;
    return S_OK;
  }

  return E_NOINTERFACE;
}

static ULONG WINAPI swbem_services_AddRef(IDispatch* iface) {
  return 2;
}

static ULONG WINAPI swbem_services_Release(IDispatch* iface) {
  return 1;
}

static HRESULT WINAPI swbem_services_GetTypeInfoCount(IDispatch* iface,
                                                      UINT* pctinfo) {
  OutputDebugStringA("swbem_services_GetTypeInfoCount");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_services_GetTypeInfo(IDispatch* iface, UINT iTInfo,
                                                 LCID lcid,
                                                 ITypeInfo** ppTInfo) {
  OutputDebugStringA("swbem_services_GetTypeInfo");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_services_GetIDsOfNames(IDispatch* iface,
                                                   REFIID riid,
                                                   LPOLESTR* rgszNames,
                                                   UINT cNames, LCID lcid,
                                                   DISPID* rgDispId) {
  OutputDebugStringA("swbem_services_GetIDsOfNames");
  if (cNames != 1) {
    return E_NOTIMPL;
  }
  if (wcsncmp(rgszNames[0], L"ExecQuery", 9) == 0) {
    *rgDispId = 42;
    return S_OK;
  } else {
    return E_FAIL;
  }
}

static HRESULT WINAPI swbem_services_Invoke(
    IDispatch* iface, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
    DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo,
    UINT* puArgErr) {
  // ExecQuery is the only implemented method.
  struct swbem_object_set* object_set;
  OutputDebugStringA("swbem_services_Invoke");
  if (!(object_set = calloc(1, sizeof(*object_set)))) {
    return E_OUTOFMEMORY;
  }

  object_set->IDispatch_iface.lpVtbl = &swbem_object_set_vtbl;
  V_VT(pVarResult) = VT_DISPATCH;
  V_DISPATCH(pVarResult) = &object_set->IDispatch_iface;
  return S_OK;
}

static const IDispatchVtbl swbem_services_vtbl = {
    swbem_services_QueryInterface,    //
    swbem_services_AddRef,            //
    swbem_services_Release,           //
    swbem_services_GetTypeInfoCount,  //
    swbem_services_GetTypeInfo,       //
    swbem_services_GetIDsOfNames,     //
    swbem_services_Invoke             //
};

// ISWbemLocator

struct swbem_locator {
  IDispatch IDispatch_iface;
};

static HRESULT WINAPI swbem_locator_QueryInterface(IDispatch* iface,
                                                   REFIID riid, void** ppv) {
  if (IsEqualGUID(&IID_IUnknown, riid) || IsEqualGUID(&IID_IDispatch, riid)) {
    *ppv = iface;
    return S_OK;
  }

  return E_NOINTERFACE;
}

static ULONG WINAPI swbem_locator_AddRef(IDispatch* iface) {
  return 2;
}

static ULONG WINAPI swbem_locator_Release(IDispatch* iface) {
  return 1;
}

static HRESULT WINAPI swbem_locator_GetTypeInfoCount(IDispatch* iface,
                                                     UINT* pctinfo) {
  OutputDebugStringA("swbem_locator_GetTypeInfoCount");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_locator_GetTypeInfo(IDispatch* iface, UINT iTInfo,
                                                LCID lcid,
                                                ITypeInfo** ppTInfo) {
  OutputDebugStringA("swbem_locator_GetTypeInfo");
  return E_NOTIMPL;
}

static HRESULT WINAPI swbem_locator_GetIDsOfNames(IDispatch* iface, REFIID riid,
                                                  LPOLESTR* rgszNames,
                                                  UINT cNames, LCID lcid,
                                                  DISPID* rgDispId) {
  OutputDebugStringA("swbem_locator_GetIDsOfNames");
  if (cNames != 1) {
    return E_NOTIMPL;
  }
  if (wcsncmp(rgszNames[0], L"ConnectServer", 13) == 0) {
    *rgDispId = 42;
    return S_OK;
  } else {
    return E_FAIL;
  }
}

static HRESULT WINAPI swbem_locator_Invoke(
    IDispatch* iface, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
    DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo,
    UINT* puArgErr) {
  struct swbem_services* services;

  // ConnectServer is the only implemented method.
  OutputDebugStringA("swbem_locator_Invoke");
  if (!(services = calloc(1, sizeof(*services)))) {
    return E_OUTOFMEMORY;
  }

  services->IDispatch_iface.lpVtbl = &swbem_services_vtbl;
  V_VT(pVarResult) = VT_DISPATCH;
  V_DISPATCH(pVarResult) = &services->IDispatch_iface;

  return S_OK;
}

static const IDispatchVtbl swbem_locator_vtbl = {
    swbem_locator_QueryInterface,    //
    swbem_locator_AddRef,            //
    swbem_locator_Release,           //
    swbem_locator_GetTypeInfoCount,  //
    swbem_locator_GetTypeInfo,       //
    swbem_locator_GetIDsOfNames,     //
    swbem_locator_Invoke             //
};

// IClassFactory

static HRESULT WINAPI class_factory_QueryInterface(IClassFactory* iface,
                                                   REFIID iid, void** out) {
  if (IsEqualGUID(iid, &IID_IUnknown) || IsEqualGUID(iid, &IID_IClassFactory)) {
    *out = iface;
    IClassFactory_AddRef(iface);
    return S_OK;
  }

  *out = NULL;
  return E_NOINTERFACE;
}

static ULONG WINAPI class_factory_AddRef(IClassFactory* iface) {
  return 2;
}

static ULONG WINAPI class_factory_Release(IClassFactory* iface) {
  return 1;
}

static HRESULT fill_pnp_device_id(void) {
  // Retrieve USB serial from Unix lib.
  uint32_t bufsize = 16;
  char* buf;
  if (!(buf = calloc(bufsize, sizeof(*buf)))) {
    return E_OUTOFMEMORY;
  }

  struct param_get_usb_serial params;
  params.buf = buf;
  params.size = &bufsize;
  if (WINE_UNIX_CALL(0, &params) != 0) {
    printf("Unix call failed\n");
    return E_FAIL;
  }

  if (params.size <= 0) {
    printf("Unix call returned size <= 0\n");
    return E_FAIL;
  }

  // Replace leading " " by "_".
  char* tmp = buf;
  while (*tmp == ' ') {
    *tmp = '_';
    ++tmp;
  }

  // Convert to wide string.
  size_t buf2size = bufsize;
  wchar_t* buf2;
  if (!(buf2 = calloc(buf2size, sizeof(*buf2)))) {
    return E_OUTOFMEMORY;
  }

  size_t convertedChars = 0;
  mbstowcs_s(&convertedChars, buf2, buf2size, buf, _TRUNCATE);

  // Build PnP Device ID with serial in it.
  size_t buf3size = 68;
  wchar_t* buf3;
  if (!(buf3 = calloc(buf3size, sizeof(*buf3)))) {
    return E_OUTOFMEMORY;
  }
  buf3size = _snwprintf(
      buf3, buf3size,
      L"USBSTOR\\Disk&Ven_Generic&Prod_Flash_Disk&Rev_8.07\\%s&0", buf2);
  g_pnp_device_id = SysAllocStringLen(buf3, buf3size);
  return S_OK;
}

static HRESULT WINAPI swbemlocator_class_factory_CreateInstance(
    IClassFactory* iface, IUnknown* outer, REFIID riid, void** out) {
  OutputDebugStringA("swbemlocator_class_factory_CreateInstance");

  *out = NULL;
  if (outer != NULL) {
    return CLASS_E_NOAGGREGATION;
  }

  HRESULT hr;
  if (FAILED(hr = fill_pnp_device_id())) {
    return hr;
  }

  // Create instance.
  struct swbem_locator* locator;
  if (!(locator = calloc(1, sizeof(*locator)))) {
    return E_OUTOFMEMORY;
  }

  locator->IDispatch_iface.lpVtbl = &swbem_locator_vtbl;

  if (FAILED(hr = swbem_locator_QueryInterface(&locator->IDispatch_iface, riid,
                                               out))) {
    printf("QueryInterface failed\n");
    free(locator);
  }

  return hr;
}

static HRESULT WINAPI class_factory_LockServer(IClassFactory* iface,
                                               BOOL lock) {
  return S_OK;
}

static const IClassFactoryVtbl class_factory_vtbl = {
    class_factory_QueryInterface,               //
    class_factory_AddRef,                       //
    class_factory_Release,                      //
    swbemlocator_class_factory_CreateInstance,  //
    class_factory_LockServer,                   //
};

static IClassFactory classfactory = {&class_factory_vtbl};

// DllGetClassObject

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppReturn) {
  OutputDebugStringA("DllGetClassObject");
  *ppReturn = NULL;

  if (IsEqualCLSID(rclsid, &CLSID_SWbemLocator)) {
    return IClassFactory_QueryInterface(&classfactory, riid, ppReturn);
  }

  return CLASS_E_CLASSNOTAVAILABLE;
}

// DllMain

BOOL WINAPI DllMain(HINSTANCE hModule, DWORD ul_reason_for_call,
                    LPVOID lpReserved) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      DisableThreadLibraryCalls(hModule);
      __wine_init_unix_call();
      break;
    default:
      break;
  }
  return TRUE;
}
