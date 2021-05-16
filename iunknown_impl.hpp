#pragma once

#include <initguid.h>
#include <unknwn.h>
#include <windows.h>

#include <type_traits>

template <typename Interface>
constexpr const IID &GetIIDForInterface();

#define CONCAT(a, b) a##b
#define DEFINE_GetIIDForInterface(IInterface)             \
  template <>                                             \
  constexpr const IID &GetIIDForInterface<IInterface>() { \
    return CONCAT(IID_, IInterface);                      \
  }

DEFINE_GetIIDForInterface(IUnknown)

template <typename T>
struct Empty {};

// type is Empty<T> if T is a base class of some element of Us
// and T otherwise.
template <typename T, typename... Us>
struct FilterOutBaseClasses {
  using type = std::conditional_t<
      ((!std::is_same_v<T, Us> && std::is_base_of_v<T, Us>) || ...), Empty<T>,
      T>;
};

// Base class that implements IUnknown functionality (AddRef, Release,
// QueryInterface).
template <typename... Interfaces>
class IUnknownImpl
    // Avoid "ambiguous base" error by filtering out interfaces that
    // are a base class of another interface.
    : public FilterOutBaseClasses<Interfaces, Interfaces...>::type... {
 public:
  virtual ~IUnknownImpl() = default;

  DWORD STDMETHODCALLTYPE AddRef() override {
    return ++m_ObjRefCount;
  }

  DWORD STDMETHODCALLTYPE Release() override {
    if (--m_ObjRefCount == 0) {
      delete this;
      return 0;
    }

    return m_ObjRefCount;
  }

  template <typename Interface, typename... Tail>
  HRESULT QueryInterfaceHelper(const IID &iid, void **ret) {
    if (IsEqualIID(iid, GetIIDForInterface<Interface>())) {
      this->AddRef();
      if constexpr (std::is_same_v<Interface, IUnknown>) {
        *ret = this;
      } else {
        *ret = static_cast<Interface *>(this);
      }
      return S_OK;
    }
    if constexpr (sizeof...(Tail) > 0) {
      return QueryInterfaceHelper<Tail...>(iid, ret);
    } else {
      *ret = nullptr;
      return E_NOINTERFACE;
    }
  }

  HRESULT STDMETHODCALLTYPE QueryInterface(const IID &riid,
                                           void **ppReturn) override {
    return QueryInterfaceHelper<IUnknown, Interfaces...>(riid, ppReturn);
  }

 private:
  int m_ObjRefCount{1};
};
