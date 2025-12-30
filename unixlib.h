/* This part is taken from Wine's include/wine/unixlib.h */

#ifndef __UNIXLIB_H
#define __UNIXLIB_H

#include <wchar.h>
#include <stdarg.h>
#include <stdint.h>

#ifndef WINE_NTSTATUS_DECLARED
#define WINE_NTSTATUS_DECLARED
typedef long NTSTATUS;
#endif

#define STATUS_SUCCESS ((NTSTATUS)0x00000000)
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001)

typedef uint64_t unixlib_handle_t;

#ifdef WINE_UNIX_LIB

typedef NTSTATUS (*unixlib_entry_t)(void* args);

extern __attribute__((visibility("default")))
const unixlib_entry_t __wine_unix_call_funcs[];
extern __attribute__((visibility("default")))
const unixlib_entry_t __wine_unix_call_wow64_funcs[];

#else

extern unixlib_handle_t __wine_unixlib_handle;
extern NTSTATUS(WINAPI* __wine_unix_call_dispatcher)(unixlib_handle_t,
                                                     unsigned int, void*);
extern NTSTATUS WINAPI __wine_init_unix_call(void);

#define WINE_UNIX_CALL(code, args) \
  __wine_unix_call_dispatcher(__wine_unixlib_handle, (code), (args))

#endif

/* Original code */

struct param_get_usb_serial {
  char* buf;
  uint32_t* size;
};

#endif

