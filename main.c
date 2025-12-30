#include <windows.h>
#include <stdio.h>

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: zusi_linux_installer.exe path\\to\\setup.exe.\n");
    return 1;
  }

  // ---
  // Copy DLL into system folder.
  // ---
  char sourceFile[MAX_PATH];
  GetModuleFileNameA(NULL, sourceFile, MAX_PATH);
  char* lastSep = strrchr(sourceFile, '\\');
  if (lastSep) {
    // Remove the executable name to get the current directory.
    *lastSep = '\0';
  }

  snprintf(sourceFile, MAX_PATH, "%s\\zwbemdisp.dll", sourceFile);

  const char* destFile = "C:\\windows\\system32\\wbem\\zwbemdisp.dll";
  if (CopyFileA(sourceFile, destFile, FALSE)) {
    printf("File copied successfully.\n");
  } else {
    printf("Failed to copy file. Error: %lu\n", GetLastError());
    return 1;
  }

  // ---
  // Registry patch.
  // ---
  const char* registryPath =
      "CLSID\\{76A64158-CB41-11D1-8B02-00600806D9B6}\\InprocServer32";
  const char* origRegValue = "C:\\windows\\system32\\wbem\\wbemdisp.dll";
  const char* newRegValue = "C:\\windows\\system32\\wbem\\zwbemdisp.dll";

  HKEY hKey;
  HRESULT hr;

  hr = RegOpenKeyExA(HKEY_CLASSES_ROOT, registryPath, 0, KEY_WRITE, &hKey);
  if (hr != ERROR_SUCCESS) {
    printf("Failed to open registry key: %d\n", hr);
    return 1;
  }

  hr = RegSetValueExA(hKey, "", 0, REG_SZ, (const BYTE*)newRegValue,
                      strlen(newRegValue) + 1);
  if (hr != ERROR_SUCCESS) {
    printf("Failed to write registry value: %d\n", hr);
    RegCloseKey(hKey);
    return 1;
  }

  // ---
  // Ask user to adjust settings.
  // ---
  int msgResult =
      MessageBoxA(NULL,
                  "Bitte 'wine winecfg' aufrufen und unter 'Drives' eine neue "
                  "virtuelle Festplatte mit folgenden Einstellungen anlegen:\n"
                  " - Laufwerksbuchstabe: egal\n"
                  " - Path: <Pfad, an dem der Zusi-USB-Stick eingehaengt ist>\n"
                  " - Type: Floppy disk  [zum Anzeigen dieses Feldes auf "
                  "\"Show Advanced\" klicken].\n"
                  "Auch dann, wenn Wine schon eine virtuelle Festplatte mit "
                  "demselben \"Path\" angelegt hat!\n"
                  "Nach Ende der Installation kann die virtuelle Festplatte "
                  "wieder entfernt werden.\n"
                  "//\n"
                  "Please run 'wine winecfg' and in the 'Drives' tab create a "
                  "new virtual hard disk with the following settings:\n"
                  " - Drive letter: choose any\n"
                  " - Path: <Mount point of the Zusi USB stick>\n"
                  " - Type: Floppy disk  [to show this field, click on \"Show "
                  "Advanced\"].\n"
                  "Do this even if Wine has already created a virtual hard "
                  "disk with the same \"Path\"!\n"
                  "After the installation has finished, you can remove the "
                  "virtual hard disk.\n"
                  "\n"
                  "Sind die Einstellungen so wie beschrieben?\n"
                  "//\n"
                  "Are the settings as described?",
                  "Zusi-Linux-Installer", MB_YESNO | MB_ICONQUESTION);
  if (msgResult == IDNO) {
    printf("User declined to proceed.\n");
    goto cleanup;
  }

  // ---
  // Start the program passed as argument.
  // ---

  STARTUPINFOA si = {sizeof(si)};
  PROCESS_INFORMATION pi;
  hr =
      CreateProcessA(argv[1], NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
  if (hr == 0) {
    printf("Failed to start program %s: %d\n", argv[1], hr);
    goto cleanup;
  }

  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  // Wait for the user to confirm the end of the installation
  // because ZusiWebInstallerLauncher uses ShellExecute to launch the installer,
  // then terminates itself.
  MessageBoxA(
      NULL,
      "Bitte mit OK bestaetigen, wenn die Installation von Zusi beendet ist.\n"
      "//\n"
      "Please confirm with OK when the Zusi installation process has finished.",
      "Zusi-Linux-Installer", MB_OK | MB_ICONINFORMATION);

cleanup:
  // Restore the original registry value.
  RegSetValueExA(hKey, "", 0, REG_SZ, (const BYTE*)origRegValue,
                 strlen(origRegValue) + 1);
  RegCloseKey(hKey);

  // Remove the file from the target location.
  if (DeleteFileA(destFile)) {
    printf("File removed successfully.\n");
  } else {
    printf("Failed to remove file. Error: %lu\n", GetLastError());
    return 1;
  }

  return 0;
}
