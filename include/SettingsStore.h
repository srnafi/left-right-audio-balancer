#pragma once
#include <windows.h>
#include <string>

struct PersistentSettings {
    std::wstring deviceId;
    bool locked = false;
    bool linked = true;
    bool startOnBoot = false;
    int windowLeft = -1;
    int windowTop = -1;
    bool hasPosition = false;
};

class SettingsStore {
public:
    static std::wstring GetSettingsFilePath();
    static std::wstring GetStartupShortcutPath();

    static PersistentSettings Load();
    static void Save(const PersistentSettings& settings);

    static bool IsStartOnBootEnabled();
    static bool SetStartOnBoot(bool enabled);

    static std::wstring PerformShortcutCheck();
};
