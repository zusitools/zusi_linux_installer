#pragma once

#include <string>

#define NOTIMPL                                                           \
  {                                                                       \
    std::cerr << "Call to unimplemented function " << __PRETTY_FUNCTION__ \
              << "\n";                                                    \
    return E_NOTIMPL;                                                     \
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
