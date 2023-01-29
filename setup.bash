#!/bin/bash
set -e
set -o pipefail

if [ $# -ne 1 ]; then
  echo "Nutzung: setup.bash /pfad/zu/usb-stick/Vollversion/Setup_Zusi3_Hobby_XXXX-XX-XX.exe" >&2
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

if ! echo "$setupWinPath" | grep -q ".:\\\\Vollversion\\\\"; then
  echo "Das Setup ist für Wine nicht unter dem Pfad X:\\Vollversion\\Setup_Zusi3_Hobby_XXXX-XX-XX.exe" >&2
  echo "für einen beliebigen Laufwerksbuchstaben X ansprechbar." >&2
  echo "(aktueller Pfad unter Wine: ${setupWinPath})." >&2
  echo >&2
  echo "Bitte 'wine winecfg' aufrufen und unter 'Drives' eine neue virtuelle Festplatte anlegen," >&2
  echo "deren Pfad auf '${stickRootDir}' verweist. Danach dieses Skript neu starten." >&2
  echo "(Nach Ende der Installation kann die virtuelle Festplatte wieder entfernt werden.)" >&2
  exit 1
fi

mv "${wbemdispPath}" "${wbemdispPath}.backup"
cp "${scriptDir}/wbemdisp.dll.so" "${wbemdispPath}" || {
  mv "${wbemdispPath}.backup" "${wbemdispPath}"
  exit 1
}
wine "${setupBasename}" || {
  mv "${wbemdispPath}.backup" "${wbemdispPath}"
  exit 1
}
