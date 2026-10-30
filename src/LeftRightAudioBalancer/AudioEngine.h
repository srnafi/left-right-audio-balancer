#pragma once
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <string>
#include <vector>
#include "AppState.h"

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    bool Initialize(HWND notifyHwnd = nullptr);
    void Release();

    void RefreshDeviceList(std::vector<DeviceItem>& outList);
    bool BindDevice(const std::wstring& id);

    void GetLevels(float& outLeft, float& outRight, float& outMaster);
    void SetLeft(float vol);
    void SetRight(float vol);
    void SetMaster(float vol);

    void GetPeaks(float& outLeft, float& outRight);

    std::wstring GetCurrentDeviceId() const { return m_currentId; }
    std::wstring GetCurrentDeviceName() const { return m_deviceName; }
    bool IsCurrentBluetooth() const { return m_isBluetooth; }

private:
    void ReleaseDevice();
    void BindEndpoint(IMMDevice* pDev);

    IMMDeviceEnumerator* m_pEnumerator = nullptr;
    IMMDevice* m_pDevice = nullptr;
    IAudioEndpointVolume* m_pVolume = nullptr;
    IAudioMeterInformation* m_pMeter = nullptr;

    std::wstring m_currentId;
    std::wstring m_deviceName;
    bool m_isBluetooth = false;
    UINT m_channels = 2;
    HWND m_notifyHwnd = nullptr;
};
