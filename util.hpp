#pragma once

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
std::string CrudeBstrToString(BSTR src) {
  std::string result;
  if (src != nullptr) {
    while (*src) {
      result += *(const char*)(src);
      ++src;
    }
  }
  return result;
}
