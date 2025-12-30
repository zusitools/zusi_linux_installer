#!/bin/bash
set -e
set -o pipefail

[ $# -eq 2 ] || (echo "Expected 2 arguments: 1) wine 2) Path to Zusi setup exe"; exit 1)

scriptDir=$(dirname "$(readlink -f "$0")")

[ -f "${scriptDir}/i386-unix/zwbemdisp.so" ] || (echo "${scriptDir}/i386-unix/zwbemdisp.so not found"; exit 1)
[ -f "${scriptDir}/i386-windows/zwbemdisp.dll" ] || (echo "${scriptDir}/i386-windows/zwbemdisp.dll not found"; exit 1)
[ -f "${scriptDir}/x86_64-unix/zwbemdisp.so" ] || (echo "${scriptDir}/x86_64-unix/zwbemdisp.so not found"; exit 1)
[ -f "${scriptDir}/x86_64-windows/zwbemdisp.dll" ] || (echo "${scriptDir}/x86_64-windows/zwbemdisp.dll not found"; exit 1)
[ -f "${scriptDir}/i386-windows/zusi_linux_installer.exe" ] || (echo "${scriptDir}/i386-windows/zusi_linux_installer.exe not found"; exit 1)

# Only "built-in" DLLs can have a unixlib.
# When loading a DLL (e.g. C:\Windows\System32\wbem\zwbemdisp.dll), Wine will normally search
# for a corresponding DLL in /usr/lib/wine/i386-windows and .so in /usr/lib/wine/x86_64-unix
# and load that instead.
# Fortunately, we can extend the search path with our own folders using WINEDLLPATH.
WINEDLLPATH=${scriptDir} "$1" "${scriptDir}/i386-windows/zusi_linux_installer.exe" $("$1" winepath -w "$2")
