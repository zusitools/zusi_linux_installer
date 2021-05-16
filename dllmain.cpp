#include "class_factory.hpp"
#include "swbemlocator.hpp"

#define INITGUID
#include <initguid.h>

#include "debug.hpp"

#include <boost/nowide/stackstring.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>

#include <olectl.h>
#include <shlobj.h>
#include <strsafe.h>
#include <windows.h>
#include <wbemdisp.h>

#include <optional>
#include <variant>
#include <iostream>

static_assert(sizeof(wchar_t) == sizeof(char16_t));

// data
HINSTANCE g_hInst;
UINT g_DllRefCount;
DWORD g_mainThreadId;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call,
                      LPVOID /*lpReserved*/) {
  switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
      g_hInst = (HINSTANCE)hModule;
      g_mainThreadId = GetCurrentThreadId();
      DisableThreadLibraryCalls(reinterpret_cast<HMODULE>(hModule));
      break;
    default:
      break;
  }
  return TRUE;
}

/*---------------------------------------------------------------*/
// DllCanUnloadNow()
/*---------------------------------------------------------------*/
STDAPI DllCanUnloadNow(VOID) {
  return (g_DllRefCount ? S_FALSE : S_OK);
}

/*---------------------------------------------------------------*/
// DllGetClassObject()
/*---------------------------------------------------------------*/
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppReturn) {
  *ppReturn = NULL;

  if (IsEqualCLSID(rclsid, CLSID_SWbemLocator)) {
    boost::intrusive_ptr<SWbemLocatorClassFactory> pClassFactory(
        new SWbemLocatorClassFactory(), false);
    if (pClassFactory == NULL) {
      return E_OUTOFMEMORY;
    }

    return pClassFactory->QueryInterface(riid, ppReturn);
  }
  (void)riid;

#if 0
#endif
  return CLASS_E_CLASSNOTAVAILABLE;
}

// ----
//
SWbemLocatorClassFactory::SWbemLocatorClassFactory() {
  g_DllRefCount++;
}

SWbemLocatorClassFactory::~SWbemLocatorClassFactory() {
  g_DllRefCount--;
}

STDMETHODIMP SWbemLocatorClassFactory::CreateInstance(LPUNKNOWN pUnknown,
                                                      REFIID riid,
                                                      LPVOID *ppObject) {
  *ppObject = NULL;
  if (pUnknown != NULL)
    return CLASS_E_NOAGGREGATION;
  try {
    boost::intrusive_ptr<SWbemLocator> pLocator(new SWbemLocator(), false);
    if (pLocator == NULL) {
      return E_OUTOFMEMORY;
    }

    return pLocator->QueryInterface(riid, ppObject);
  } catch (const std::exception &e) {
    return E_FAIL;
  }
}
