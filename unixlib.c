#include "unixlib.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <libusb.h>

// https://github.com/gregkh/usbutils/blob/master/sysfs.c

#define USB_MAX_DEPTH 7

#define SYSFS_DEV_ATTR_PATH "/sys/bus/usb/devices/%s/%s"

int get_sysfs_name(char* buf, size_t size, libusb_device* dev) {
  int len = 0;
  uint8_t bnum = libusb_get_bus_number(dev);
  uint8_t pnums[USB_MAX_DEPTH];
  int num_pnums;

  buf[0] = '\0';

  num_pnums = libusb_get_port_numbers(dev, pnums, sizeof(pnums));
  if (num_pnums == LIBUSB_ERROR_OVERFLOW) {
    return -1;
  } else if (num_pnums == 0) {
    /* Special-case root devices */
    return snprintf(buf, size, "usb%d", bnum);
  }

  len += snprintf(buf, size, "%d-", bnum);
  for (int i = 0; i < num_pnums; i++)
    len += snprintf(buf + len, size - len, i ? ".%d" : "%d", pnums[i]);

  return len;
}

int read_sysfs_prop(char* buf, size_t size, char* sysfs_name,
                    const char* propname) {
  int n, fd;
  char path[PATH_MAX];

  buf[0] = '\0';
  snprintf(path, sizeof(path), SYSFS_DEV_ATTR_PATH, sysfs_name, propname);
  fprintf(stderr, " - reading from %s\n", path);
  fd = open(path, O_RDONLY);

  if (fd == -1) {
    return 0;
  }

  n = read(fd, buf, size);

  if (n > 0) {
    buf[n - 1] = '\0';  // Turn newline into null terminator
  }

  close(fd);
  return n;
}

NTSTATUS get_usb_serial(void* args) {
  struct param_get_usb_serial* a = (struct param_get_usb_serial*)args;
  char* buf = a->buf;
  uint32_t* serial_size = a->size;
  size_t max_serial_size = *serial_size;

  *serial_size = 0;

  libusb_device** list;
  struct libusb_device_descriptor desc = {};

  libusb_init(NULL);
  libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_INFO);
  ssize_t size = libusb_get_device_list(NULL, &list);
  for (int i = 0; i < size; i++) {
    libusb_device* dev = list[i];
    libusb_get_device_descriptor(dev, &desc);
    libusb_get_device_descriptor(dev, &desc);
    fprintf(stderr, "device %d: class %d, manufacturer %d, product %d\n", i,
            (uint32_t)desc.bDeviceClass, (uint32_t)desc.iManufacturer,
            (uint32_t)desc.iProduct);
    fprintf(stderr, " - bus %d, device %d\n", libusb_get_bus_number(dev),
            libusb_get_device_address(dev));
    for (int j = 0; j < desc.bNumConfigurations; ++j) {
      fprintf(stderr, " - configuration %d\n", j);
      struct libusb_config_descriptor* config;
      if (libusb_get_config_descriptor(dev, j, &config) != 0) {
        continue;
      }
      for (int k = 0; k < config->bNumInterfaces; k++) {
        fprintf(stderr, "  - interface %d\n", k);
        for (int l = 0; l < config->interface[k].num_altsetting; l++) {
          fprintf(stderr, "   - altsetting %d\n", l);
          if (config->interface[k].altsetting[l].bInterfaceClass ==
              LIBUSB_CLASS_MASS_STORAGE) {
            char sysfs_name[1024];
            if (get_sysfs_name(sysfs_name, sizeof(sysfs_name), dev) >= 0) {
              if (*serial_size != 0) {
                fprintf(stderr,
                        "WARNING: More than one USB mass storage device found, "
                        "taking first one.\n");
              } else {
                *serial_size =
                    read_sysfs_prop(buf, max_serial_size, sysfs_name, "serial");
              }
            }
          }
        }
      }
    }
  }

  libusb_free_device_list(list, 1);
  libusb_exit(NULL);

  if (*serial_size == 0) {
    fprintf(stderr, "ERROR: No USB stick found.\n");
  }

  return STATUS_SUCCESS;
}

#ifdef _WIN64

typedef uint32_t PTR32;

static inline void* ULongToPtr(uint32_t ul) {
  return (void*)(uint64_t)ul;
}

NTSTATUS wow64_get_usb_serial(void* args) {
  struct {
    PTR32 buf;
    PTR32 size;
  }* params32 = args;

  struct param_get_usb_serial params = {
      .buf = ULongToPtr(params32->buf),
      .size = ULongToPtr(params32->size),
  };

  return get_usb_serial(&params);
}

const unixlib_entry_t __wine_unix_call_wow64_funcs[] = {
    wow64_get_usb_serial,
};

#endif

const unixlib_entry_t __wine_unix_call_funcs[] = {
    get_usb_serial,
};
