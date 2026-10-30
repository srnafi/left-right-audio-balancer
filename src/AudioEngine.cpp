#include "AudioEngine.h"
#include <functiondiscoverykeys_devpkey.h>
#include <algorithm>

#pragma comment(lib, "ole32.lib")

AudioEngine::AudioEngine() {}

AudioEngine::~AudioEngine() {
    Release();
}

bool AudioEngine::Initialize(HWND notifyHwnd) {
    m_notifyHwnd = notifyHwnd;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&m_pEnumerator
    );
    if (FAILED(hr)) return false;

    IMMDevice* pDef = nullptr;
    if (SUCCEEDED(m_pEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDef))) {
        BindEndpoint(pDef);
        pDef->Release();
    }
    return true;
}

void AudioEngine::ReleaseDevice() {
    if (m_pMeter) { m_pMeter->Release(); m_pMeter = nullptr; }
    if (m_pVolume) { m_pVolume->Release(); m_pVolume = nullptr; }
    if (m_pDevice) { m_pDevice->Release(); m_pDevice = nullptr; }
}

void AudioEngine::Release() {
    ReleaseDevice();
    if (m_pEnumerator) { m_pEnumerator->Release(); m_pEnumerator = nullptr; }
}

void AudioEngine::BindEndpoint(IMMDevice* pDev) {
    ReleaseDevice();
    if (!pDev) return;

    m_pDevice = pDev;
    m_pDevice->AddRef();

    LPWSTR strId = nullptr;
    if (SUCCEEDED(m_pDevice->GetId(&strId))) {
        m_currentId = strId;
        CoTaskMemFree(strId);
    }

    m_deviceName = L"Speakers";
    m_isBluetooth = false;

    IPropertyStore* pProps = nullptr;
    if (SUCCEEDED(m_pDevice->OpenPropertyStore(STGM_READ, &pProps))) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &pv))) {
            if (pv.vt == VT_LPWSTR && pv.pwszVal) {
                m_deviceName = pv.pwszVal;
            }
            PropVariantClear(&pv);
        }
        pProps->Release();
    }

    std::wstring lower = m_deviceName;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
    if (lower.find(L"bluetooth") != std::wstring::npos ||
        lower.find(L"hands-free") != std::wstring::npos ||
        lower.find(L"wh-") != std::wstring::npos ||
        lower.find(L"wf-") != std::wstring::npos ||
        lower.find(L"airpods") != std::wstring::npos ||
        lower.find(L"buds") != std::wstring::npos) {
        m_isBluetooth = true;
    }

    m_pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**)&m_pVolume);
    m_pDevice->Activate(__uuidof(IAudioMeterInformation), CLSCTX_ALL, nullptr, (void**)&m_pMeter);

    if (m_pVolume) {
        m_pVolume->GetChannelCount(&m_channels);
    }
}

bool AudioEngine::BindDevice(const std::wstring& id) {
    if (!m_pEnumerator) return false;
    IMMDevice* pDev = nullptr;
    if (SUCCEEDED(m_pEnumerator->GetDevice(id.c_str(), &pDev))) {
        BindEndpoint(pDev);
        pDev->Release();
        return true;
    }
    return false;
}

void AudioEngine::RefreshDeviceList(std::vector<DeviceItem>& outList) {
    outList.clear();
    if (!m_pEnumerator) return;

    IMMDeviceCollection* pCol = nullptr;
    if (FAILED(m_pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pCol))) return;

    UINT count = 0;
    pCol->GetCount(&count);
    for (UINT i = 0; i < count; ++i) {
        IMMDevice* pDev = nullptr;
        if (SUCCEEDED(pCol->Item(i, &pDev))) {
            DeviceItem item;
            LPWSTR strId = nullptr;
            if (SUCCEEDED(pDev->GetId(&strId))) {
                item.id = strId;
                CoTaskMemFree(strId);
            }

            IPropertyStore* pProps = nullptr;
            if (SUCCEEDED(pDev->OpenPropertyStore(STGM_READ, &pProps))) {
                PROPVARIANT pv;
                PropVariantInit(&pv);
                if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &pv))) {
                    if (pv.vt == VT_LPWSTR && pv.pwszVal) {
                        item.name = pv.pwszVal;
                    }
                    PropVariantClear(&pv);
                }
                pProps->Release();
            }

            std::wstring lower = item.name;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::towlower);
            if (lower.find(L"bluetooth") != std::wstring::npos ||
                lower.find(L"hands-free") != std::wstring::npos ||
                lower.find(L"wh-") != std::wstring::npos ||
                lower.find(L"wf-") != std::wstring::npos ||
                lower.find(L"airpods") != std::wstring::npos ||
                lower.find(L"buds") != std::wstring::npos) {
                item.isBluetooth = true;
            }

            outList.push_back(item);
            pDev->Release();
        }
    }
    pCol->Release();
}

void AudioEngine::GetLevels(float& outLeft, float& outRight, float& outMaster) {
    if (!m_pVolume) return;
    m_pVolume->GetMasterVolumeLevelScalar(&outMaster);
    if (m_channels >= 2) {
        m_pVolume->GetChannelVolumeLevelScalar(0, &outLeft);
        m_pVolume->GetChannelVolumeLevelScalar(1, &outRight);
    } else {
        outLeft = outRight = outMaster;
    }
}

void AudioEngine::SetLeft(float vol) {
    if (!m_pVolume) return;
    vol = std::clamp(vol, 0.0f, 1.0f);
    if (m_channels >= 2) {
        m_pVolume->SetChannelVolumeLevelScalar(0, vol, nullptr);
    } else {
        m_pVolume->SetMasterVolumeLevelScalar(vol, nullptr);
    }
}

void AudioEngine::SetRight(float vol) {
    if (!m_pVolume) return;
    vol = std::clamp(vol, 0.0f, 1.0f);
    if (m_channels >= 2) {
        m_pVolume->SetChannelVolumeLevelScalar(1, vol, nullptr);
    } else {
        m_pVolume->SetMasterVolumeLevelScalar(vol, nullptr);
    }
}

void AudioEngine::SetMaster(float vol) {
    if (!m_pVolume) return;
    vol = std::clamp(vol, 0.0f, 1.0f);
    m_pVolume->SetMasterVolumeLevelScalar(vol, nullptr);
}

void AudioEngine::GetPeaks(float& outLeft, float& outRight) {
    outLeft = 0.0f;
    outRight = 0.0f;
    if (!m_pMeter) return;

    if (m_channels >= 2) {
        float peaks[2] = { 0.0f, 0.0f };
        if (SUCCEEDED(m_pMeter->GetChannelsPeakValues(2, peaks))) {
            outLeft = peaks[0];
            outRight = peaks[1];
        }
    } else {
        float peak = 0.0f;
        if (SUCCEEDED(m_pMeter->GetPeakValue(&peak))) {
            outLeft = outRight = peak;
        }
    }
}
