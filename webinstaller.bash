#!/bin/bash
set -e
set -o pipefail

if [ $# -ne 1 ]; then
  echo "Nutzung: webinstaller.bash /pfad/zu/ZusiWebInstallerLauncher.exe" >&2
  exit 1
fi

scriptDir=$(dirname "$(readlink -f "$0")")

setupPath=$1
setupDir=$(dirname "$setupPath")
stickRootDir=$(realpath "$setupDir/..")
setupBasename=$(basename "$setupPath")

cd "${setupDir}"
setupWinPath=$(wine winepath -w "${setupBasename}")
wbemdispPath=$(wine winepath -u "C:\\Windows\\System32\\wbem\\wbemdisp.dll")

echo "Bitte 'wine winecfg' aufrufen und unter 'Drives' eine neue virtuelle Festplatte mit folgenden Einstellungen anlegen:" >&2
echo " - Path: <Pfad, an dem der Zusi-USB-Stick eingehängt ist>" >&2
echo " - Type: Floppy disk  [zum Anzeigen dieses Feldes auf \"Show Advanced\" klicken]." >&2
echo "(Nach Ende der Installation kann die virtuelle Festplatte wieder entfernt werden.)" >&2
echo >&2
echo "Bitte mit Enter-Taste bestätigen, dass die Einstellungen so sind wie beschrieben." >&2
read

mv "${wbemdispPath}" "${wbemdispPath}.backup"
cp "${scriptDir}/wbemdisp.dll.so" "${wbemdispPath}" || {
  mv "${wbemdispPath}.backup" "${wbemdispPath}"
  exit 1
}
wine "${setupBasename}" || {
  mv "${wbemdispPath}.backup" "${wbemdispPath}"
  exit 1
}
