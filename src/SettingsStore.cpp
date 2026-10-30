#include "SettingsStore.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <fstream>
#include <sstream>
#include <filesystem>

#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

std::wstring SettingsStore::GetSettingsFilePath() {
    wchar_t appData[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appData))) {
        std::filesystem::path p(appData);
        p /= L"LeftRightAudioBalancer";
        p /= L"settings.json";
        return p.wstring();
    }
    return L"settings.json";
}

std::wstring SettingsStore::GetStartupShortcutPath() {
    wchar_t startup[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_STARTUP, nullptr, 0, startup))) {
        std::filesystem::path p(startup);
        p /= L"LeftRightAudioBalancer.lnk";
        return p.wstring();
    }
    return L"";
}

bool SettingsStore::IsStartOnBootEnabled() {
    std::wstring path = GetStartupShortcutPath();
    if (path.empty()) return false;
    return std::filesystem::exists(path);
}

bool SettingsStore::SetStartOnBoot(bool enabled) {
    std::wstring shortcutPath = GetStartupShortcutPath();
    if (shortcutPath.empty()) return false;

    if (!enabled) {
        if (std::filesystem::exists(shortcutPath)) {
            std::filesystem::remove(shortcutPath);
        }
        return true;
    }

    wchar_t exePath[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, exePath, MAX_PATH)) return false;

    wchar_t dirPath[MAX_PATH];
    wcscpy_s(dirPath, exePath);
    PathRemoveFileSpecW(dirPath);

    IShellLinkW* psl = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl);
    if (SUCCEEDED(hr)) {
        psl->SetPath(exePath);
        psl->SetWorkingDirectory(dirPath);
        psl->SetDescription(L"Left Right Audio Balancer");

        IPersistFile* ppf = nullptr;
        hr = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
        if (SUCCEEDED(hr)) {
            hr = ppf->Save(shortcutPath.c_str(), TRUE);
            ppf->Release();
        }
        psl->Release();
    }
    return SUCCEEDED(hr);
}

PersistentSettings SettingsStore::Load() {
    PersistentSettings s;
    std::wstring filePath = GetSettingsFilePath();
    if (!std::filesystem::exists(filePath)) return s;

    std::wifstream f(filePath);
    if (!f.is_open()) return s;

    std::wstring line;
    while (std::getline(f, line)) {
        if (line.find(L"\"Locked\"") != std::wstring::npos) {
            s.locked = (line.find(L"true") != std::wstring::npos);
        } else if (line.find(L"\"Linked\"") != std::wstring::npos) {
            s.linked = (line.find(L"true") != std::wstring::npos);
        } else if (line.find(L"\"StartOnBoot\"") != std::wstring::npos) {
            s.startOnBoot = (line.find(L"true") != std::wstring::npos);
        } else if (line.find(L"\"WindowLeft\"") != std::wstring::npos) {
            size_t colon = line.find(L':');
            if (colon != std::wstring::npos) {
                try {
                    s.windowLeft = (int)std::stof(line.substr(colon + 1));
                    s.hasPosition = true;
                } catch (...) {}
            }
        } else if (line.find(L"\"WindowTop\"") != std::wstring::npos) {
            size_t colon = line.find(L':');
            if (colon != std::wstring::npos) {
                try {
                    s.windowTop = (int)std::stof(line.substr(colon + 1));
                    s.hasPosition = true;
                } catch (...) {}
            }
        } else if (line.find(L"\"DeviceId\"") != std::wstring::npos) {
            size_t firstQuote = line.find(L'\"', line.find(L':'));
            if (firstQuote != std::wstring::npos) {
                size_t secondQuote = line.find(L'\"', firstQuote + 1);
                if (secondQuote != std::wstring::npos) {
                    s.deviceId = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                }
            }
        }
    }
    return s;
}

void SettingsStore::Save(const PersistentSettings& s) {
    std::wstring filePath = GetSettingsFilePath();
    try {
        std::filesystem::path dir = std::filesystem::path(filePath).parent_path();
        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        std::wofstream f(filePath);
        if (!f.is_open()) return;

        f << L"{\n";
        f << L"  \"DeviceId\": \"" << s.deviceId << L"\",\n";
        f << L"  \"Locked\": " << (s.locked ? L"true" : L"false") << L",\n";
        f << L"  \"Linked\": " << (s.linked ? L"true" : L"false") << L",\n";
        f << L"  \"StartOnBoot\": " << (s.startOnBoot ? L"true" : L"false") << L",\n";
        if (s.hasPosition) {
            f << L"  \"WindowLeft\": " << s.windowLeft << L",\n";
            f << L"  \"WindowTop\": " << s.windowTop << L"\n";
        } else {
            f << L"  \"WindowLeft\": null,\n";
            f << L"  \"WindowTop\": null\n";
        }
        f << L"}\n";
    } catch (...) {}
}

std::wstring SettingsStore::PerformShortcutCheck() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);

    wchar_t dirPath[MAX_PATH];
    wcscpy_s(dirPath, exePath);
    PathRemoveFileSpecW(dirPath);

    std::wstring shortcutPath = GetStartupShortcutPath();
    bool created = SetStartOnBoot(true);

    bool match = false;
    wchar_t targetPath[MAX_PATH] = { 0 };

    if (created && std::filesystem::exists(shortcutPath)) {
        IShellLinkW* psl = nullptr;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl))) {
            IPersistFile* ppf = nullptr;
            if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (void**)&ppf))) {
                if (SUCCEEDED(ppf->Load(shortcutPath.c_str(), STGM_READ))) {
                    WIN32_FIND_DATAW wfd;
                    if (SUCCEEDED(psl->GetPath(targetPath, MAX_PATH, &wfd, SLGP_UNCPRIORITY))) {
                        match = (_wcsicmp(targetPath, exePath) == 0);
                    }
                }
                ppf->Release();
            }
            psl->Release();
        }
    }

    // Clean up test shortcut
    SetStartOnBoot(false);

    std::wstringstream ss;
    ss << L"process      " << exePath << L"\n";
    ss << L"base dir     " << dirPath << L"\n";
    ss << L"shortcut     " << shortcutPath << L"\n";
    ss << L"created      " << (created ? L"True" : L"False") << L"\n";
    ss << L"target       " << targetPath << L"\n";
    ss << L"match        " << (match ? L"True" : L"False") << L"\n";
    ss << L"verdict      " << (match ? L"PASS" : L"FAIL") << L"\n";

    return ss.str();
}
