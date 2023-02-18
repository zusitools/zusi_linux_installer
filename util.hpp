#pragma once

#include <windows.h>
#include <oleauto.h>

#include <string>
#include <cstdio>

#define NOTIMPL                                            \
  {                                                        \
    fprintf(stderr, "Call to unimplemented function %s\n", \
            __PRETTY_FUNCTION__);                          \
    return E_NOTIMPL;                                      \
  }

/**
 * BSTR to string, sufficient for our purposes since we only deal with ASCII
 * chars.
 */
inline std::string CrudeBstrToString(BSTR src) {
  std::string result;
  if (src != nullptr) {
    while (*src) {
      result += *(const char *)(src);
      ++src;
    }
  }
  return result;
}

inline std::u16string NarrowToWideString(const std::string &source) {
  std::u16string result(
      // determine needed size
      MultiByteToWideChar(CP_UTF8, 0, &source[0], (int)source.size(), nullptr,
                          0),
      0);
  static_assert(sizeof(wchar_t) == sizeof(char16_t));
  MultiByteToWideChar(CP_UTF8, 0, &source[0], (int)source.size(),
                      reinterpret_cast<wchar_t *>(&result[0]), result.size());
  return result;
}
