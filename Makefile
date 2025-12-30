WINELIB = /opt/wine-devel/lib64/wine
WINELIB32 = /opt/wine-devel/lib/wine
WINEINC = /opt/wine-devel/include/wine
WINEGCC = /opt/wine-devel/bin/winegcc
WINEBUILD = /opt/wine-devel/bin/winebuild

WINE_INCLUDES = -I$(WINEINC)/windows -I$(WINEINC)/msvcrt

i386_CC = clang
i386_CCFLAGS = \
    -I/usr/include/libusb-1.0 \
    -m32 \
    -O2 \
    -D__WINESRC__ \
    -DWINE_UNIX_LIB \
    -D_WIN64 \
    -pipe \
    -fcf-protection=none \
    -fvisibility=hidden \
    -fno-stack-protector \
    -fno-strict-aliasing \
    -fPIC \
    -fasynchronous-unwind-tables \
    -Wall \
    -Wempty-body \
    -Wignored-qualifiers \
    -Winit-self \
    -Wno-pragma-pack \
    -Wno-deprecated-declarations \
    -Wstrict-prototypes \
    -Wtype-limits \
    -Wunused-but-set-parameter \
    -Wvla \
    -Wwrite-strings \
    -Wpointer-arith
i386_LDFLAGS = \
    -m32 \
    -shared \
    -Wl,-Bsymbolic \
    -Wl,-soname,$(LIBNAME).so \
    -Wl,-z,defs \
    -L$(WINELIB32)/i386-unix -l:ntdll.so \
    -lusb-1.0

x86_64_CC = clang
x86_64_CCFLAGS = \
    -I/usr/include/libusb-1.0 \
    -m64 \
    -O2 \
    -D__WINESRC__ \
    -DWINE_UNIX_LIB \
    -D_WIN64 \
    -pipe \
    -fcf-protection=none \
    -fvisibility=hidden \
    -fno-stack-protector \
    -fno-strict-aliasing \
    -fPIC \
    -fasynchronous-unwind-tables \
    -Wall \
    -Wempty-body \
    -Wignored-qualifiers \
    -Winit-self \
    -Wno-pragma-pack \
    -Wno-deprecated-declarations \
    -Wstrict-prototypes \
    -Wtype-limits \
    -Wunused-but-set-parameter \
    -Wvla \
    -Wwrite-strings \
    -Wpointer-arith
x86_64_LDFLAGS = \
    -shared \
    -Wl,-Bsymbolic \
    -Wl,-soname,$(LIBNAME).so \
    -Wl,-z,defs \
    -L$(WINELIB)/x86_64-unix -l:ntdll.so \
    -lusb-1.0

WIN_LIBS = -lwinecrt0 -lucrtbase -lkernel32 -lntdll -loleaut32

i386_CROSSCC = clang
i386_CROSSCCFLAGS = \
    -O2 \
    -D__STDC__ \
    -D__WINE_PE_BUILD \
    -D_UCRT \
    -D__WINESRC__ \
    -target i686-windows \
    -fuse-ld=lld \
    --no-default-config \
    -fno-strict-aliasing \
    -fno-omit-frame-pointer \
    -Wall \
    -Wempty-body \
    -Wignored-qualifiers \
    -Winit-self \
    -Wno-pragma-pack \
    -Wno-microsoft-enum-forward-reference \
    -Wstrict-prototypes \
    -Wtype-limits \
    -Wunused-but-set-parameter \
    -Wvla \
    -Wwrite-strings \
    -Wpointer-arith \
    -Wabsolute-value \
    -Wenum-conversion \
    $(WINE_INCLUDES)
i386_CROSSLDFLAGS = \
    -b i686-windows \
    -Wl,--wine-builtin \
    -Wl,/safeseh:NO \
    --no-default-config \
    -L$(WINELIB)/i386-windows $(WIN_LIBS)
i386_DIR = i386-windows

x86_64_CROSSCC = clang
x86_64_CROSSCCFLAGS = \
    -O2 \
    -D__STDC__ \
    -D__WINE_PE_BUILD \
    -D_UCRT \
    -D__WINESRC__ \
    -target x86_64-windows \
    -fuse-ld=lld \
    --no-default-config \
    -fno-strict-aliasing \
    -Wall \
    -Wempty-body \
    -Wignored-qualifiers \
    -Winit-self \
    -Wno-pragma-pack \
    -Wno-microsoft-enum-forward-reference \
    -Wstrict-prototypes \
    -Wtype-limits \
    -Wunused-but-set-parameter \
    -Wvla \
    -Wwrite-strings \
    -Wpointer-arith \
    -Wabsolute-value \
    -Wenum-conversion \
    -Wformat-overflow \
    -Wnonnull \
    -mcx16 \
    -mcmodel=small \
    $(WINE_INCLUDES)
x86_64_CROSSLDFLAGS = \
    -b x86_64-windows \
    -Wl,--wine-builtin \
    --no-default-config \
    -L$(WINELIB)/x86_64-windows $(WIN_LIBS)
x86_64_DIR = x86_64-windows

UNIX_i386_DIR = i386-unix
UNIX_x86_64_DIR = x86_64-unix
LIBNAME = zwbemdisp
EXENAME = zusi_linux_installer

SRCS = \
    dllmain.c \

all: libs exes

libs: $(UNIX_i386_DIR)/$(LIBNAME).so $(UNIX_x86_64_DIR)/$(LIBNAME).so $(i386_DIR)/$(LIBNAME).dll $(x86_64_DIR)/$(LIBNAME).dll

exes: $(i386_DIR)/$(EXENAME).exe

$(i386_DIR) $(x86_64_DIR) $(UNIX_i386_DIR) $(UNIX_x86_64_DIR):
	mkdir -p $@

# Unix libraries
# 32 bit
$(UNIX_i386_DIR)/unixlib.o: unixlib.c | $(UNIX_i386_DIR)
	$(x86_64_CC) -c -o $@ $< $(i386_CCFLAGS)

$(UNIX_i386_DIR)/$(LIBNAME).so: $(UNIX_i386_DIR)/unixlib.o
	$(x86_64_CC) -o $@ $^ $(i386_LDFLAGS)

# 64 bit
$(UNIX_x86_64_DIR)/unixlib.o: unixlib.c | $(UNIX_x86_64_DIR)
	$(x86_64_CC) -c -o $@ $< $(x86_64_CCFLAGS)

$(UNIX_x86_64_DIR)/$(LIBNAME).so: $(UNIX_x86_64_DIR)/unixlib.o
	$(x86_64_CC) -o $@ $^ $(x86_64_LDFLAGS)

# Windows libraries
# 32 bit
$(i386_DIR)/%.o: %.c | $(i386_DIR)
	$(i386_CROSSCC) -c -o $@ $< $(i386_CROSSCCFLAGS)

$(i386_DIR)/$(LIBNAME).a: $(LIBNAME).spec
	$(WINEBUILD) -w --implib -o $@ -b i686-windows --export $^

$(i386_DIR)/$(LIBNAME).dll: $(LIBNAME).spec $(addprefix $(i386_DIR)/, $(SRCS:.c=.o))
	$(WINEGCC) -o $@ $^ -shared $(i386_CROSSLDFLAGS)

# 64 bit
$(x86_64_DIR)/%.o: %.c | $(x86_64_DIR)
	$(x86_64_CROSSCC) -c -o $@ $< $(x86_64_CROSSCCFLAGS)

$(x86_64_DIR)/$(LIBNAME).a: $(LIBNAME).spec
	$(WINEBUILD) -w --implib -o $@ -b x86_64-windows --export $^

$(x86_64_DIR)/$(LIBNAME).dll: $(LIBNAME).spec $(addprefix $(x86_64_DIR)/, $(SRCS:.c=.o))
	$(WINEGCC) -o $@ $^ -shared $(x86_64_CROSSLDFLAGS)

# Helper application (32 bit so that WOW64 redirection works just like for ZusiWebInstaller).
$(i386_DIR)/$(EXENAME).o: main.c
	$(i386_CROSSCC) -c -o $@ $< $(i386_CROSSCCFLAGS)

$(i386_DIR)/$(EXENAME).exe: $(i386_DIR)/$(EXENAME).o
	$(WINEGCC) -o $@ $^ $(i386_CROSSLDFLAGS)

clean::
	rm -rf $(i386_DIR) $(x86_64_DIR) $(UNIX_i386_DIR) $(UNIX_x86_64_DIR)
