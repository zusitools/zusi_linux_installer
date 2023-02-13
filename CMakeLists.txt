project(WbemDisp LANGUAGES CXX)
cmake_minimum_required(VERSION 3.0)

# build with:
# cmake -DCMAKE_CXX_COMPILER='/path/to/wine-build64/tools/winegcc/wineg++' -DCMAKE_CXX_FLAGS='--wine-objdir /path/to/wine-build32 --winebuild /path/to/wine-build64/tools/winebuild/winebuild -m32 -I/path/to/wine-source/include --target i686-linux-gnu'
#
# needs an i386 cross compiler and libusb 32-bit

add_library(wbemdisp SHARED
  dllmain.cpp
  serial.cpp
  )

target_compile_definitions(wbemdisp PRIVATE -DUNICODE -D_UNICODE -DNOMINMAX -DWIN32_LEAN_AND_MEAN)
target_compile_options(wbemdisp PRIVATE -Wall -Wextra -Wno-pedantic -Werror -Wno-error=unused-function -Wno-error=deprecated-declarations -Wno-unused-parameter -fshort-wchar)
target_link_options(wbemdisp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/dllmain.spec)
target_include_directories(wbemdisp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} /usr/include/libusb-1.0)
target_link_libraries(wbemdisp PRIVATE usb-1.0 oleaut32)
set_target_properties(wbemdisp PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(wbemdisp PROPERTIES CXX_STANDARD 17)
set_target_properties(wbemdisp PROPERTIES VISIBILITY_INLINES_HIDDEN ON)
set_target_properties(wbemdisp PROPERTIES POSITION_INDEPENDENT_CODE OFF)
set_target_properties(wbemdisp PROPERTIES PREFIX "")
set_target_properties(wbemdisp PROPERTIES SUFFIX ".dll.so")
install(TARGETS wbemdisp DESTINATION bin)
