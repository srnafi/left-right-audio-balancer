#include "AvrcpHelper.h"
#include <shellapi.h>
#include <string>

static const wchar_t* AVRCP_KEY_PATH = L"SYSTEM\\CurrentControlSet\\Control\\Bluetooth\\Audio\\AVRCP\\CT";
static const wchar_t* AVRCP_VAL_NAME = L"DisableAbsoluteVolume";

bool AvrcpHelper::IsApplied() {
    HKEY hKey = nullptr;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, AVRCP_KEY_PATH, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD val = 0;
        DWORD size = sizeof(val);
        bool applied = (RegQueryValueExW(hKey, AVRCP_VAL_NAME, nullptr, nullptr, (LPBYTE)&val, &size) == ERROR_SUCCESS && val == 1);
        RegCloseKey(hKey);
        return applied;
    }
    return false;
}

int AvrcpHelper::WriteRegistry(bool enable) {
    HKEY hKey = nullptr;
    DWORD disposition = 0;
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, AVRCP_KEY_PATH, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, &disposition) == ERROR_SUCCESS) {
        DWORD val = enable ? 1 : 0;
        LONG res = RegSetValueExW(hKey, AVRCP_VAL_NAME, 0, REG_DWORD, (const BYTE*)&val, sizeof(val));
        RegCloseKey(hKey);
        return (res == ERROR_SUCCESS) ? 0 : 2;
    }
    return 1;
}

bool AvrcpHelper::Apply(bool enable) {
    // Try in-process write first (succeeds if running elevated)
    if (WriteRegistry(enable) == 0) {
        return IsApplied() == enable;
    }

    // Elevate via self-execution with runas
    wchar_t exePath[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, exePath, MAX_PATH)) return false;

    std::wstring params = enable ? L"--set-absolute-volume 1" : L"--set-absolute-volume 0";

    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = exePath;
    sei.lpParameters = params.c_str();
    sei.nShow = SW_HIDE;
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;

    if (ShellExecuteExW(&sei)) {
        if (sei.hProcess) {
            WaitForSingleObject(sei.hProcess, 30000);
            CloseHandle(sei.hProcess);
        }
    }

    return IsApplied() == enable;
}
