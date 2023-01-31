#!/bin/bash
set -e
set -o pipefail

if [ $# -ne 1 ]; then
  echo "Nutzung: setup.bash /pfad/zu/usb-stick/Vollversion/Setup_Zusi3_Hobby_XXXX-XX-XX.exe" >&2
  echo "oder: setup.bash /pfad/ZusiWebInstallerLauncher.exe" >&2
  exit 1
fi

scriptDir=$(dirname "$(readlink -f "$0")")

setupPath=$1
setupDir=$(dirname "$setupPath")
setupBasename=$(basename "$setupPath")

cd "${setupDir}"
# Call "wine", not "wine64". This will redirect to C:\\Windows\\SysWoW64 on a 64-bit WINEPREFIX.
# This is what we want, since the installer is a 32-bit application.
wbemdispPath=$(wine winepath -u "C:\\Windows\\System32\\wbem\\wbemdisp.dll")
wbemdispPathBackup=${wbemdispPath}.backup-$(date "+%Y%m%d")
wbemdispReplacementPath="${scriptDir}/wbemdisp32.dll.so"

echo "Bitte 'wine winecfg' aufrufen und unter 'Drives' eine neue virtuelle Festplatte mit folgenden Einstellungen anlegen:" >&2
echo " - Path: <Pfad, an dem der Zusi-USB-Stick eingehängt ist>" >&2
echo " - Type: Floppy disk  [zum Anzeigen dieses Feldes auf \"Show Advanced\" klicken]." >&2
echo "(Nach Ende der Installation kann die virtuelle Festplatte wieder entfernt werden.)" >&2
echo >&2
echo "Bitte mit Enter-Taste bestätigen, dass die Einstellungen so sind wie beschrieben." >&2
read -r

cp "${wbemdispPath}" "${wbemdispPathBackup}"
function cleanup {
  mv "${wbemdispPathBackup}" "${wbemdispPath}"
}
trap cleanup EXIT

cp "${wbemdispReplacementPath}" "${wbemdispPath}"
wine "${setupBasename}"
