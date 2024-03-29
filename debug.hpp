#pragma once

#include "util.hpp"

#include <windows.h>

#include <sstream>

// https://stackoverflow.com/a/14422777
#define STRINGIFY2(m) #m
#define STRINGIFY(m) STRINGIFY2(m)

#define DEBUG(args)                                                       \
  {                                                                       \
    do {                                                                  \
      std::stringstream msg;                                              \
      msg << "[T " << GetCurrentThreadId() << "] " << __PRETTY_FUNCTION__ \
          << ":" STRINGIFY(__LINE__) ": " << args << "\r\n";              \
      OutputDebugString(reinterpret_cast<const wchar_t *>(                \
          NarrowToWideString(msg.str()).c_str()));                        \
    } while (0);                                                          \
  }

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &os, const GUID &guid) {
  os << std::uppercase;
  os.width(8);
  os << std::hex << guid.Data1 << '-';

  os.width(4);
  os << std::hex << guid.Data2 << '-';

  os.width(4);
  os << std::hex << guid.Data3 << '-';

  os.width(2);
  os << std::hex << static_cast<short>(guid.Data4[0])
     << static_cast<short>(guid.Data4[1]) << '-'
     << static_cast<short>(guid.Data4[2]) << static_cast<short>(guid.Data4[3])
     << static_cast<short>(guid.Data4[4]) << static_cast<short>(guid.Data4[5])
     << static_cast<short>(guid.Data4[6]) << static_cast<short>(guid.Data4[7]);
  os << std::nouppercase;
  return os;
}
