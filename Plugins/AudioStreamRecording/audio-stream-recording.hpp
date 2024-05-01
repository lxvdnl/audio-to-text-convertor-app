#ifndef AUDIOSTREAMRECORDING_HPP
#define AUDIOSTREAMRECORDING_HPP

#pragma once

#include <Windows.h>
#include <atomic>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <string>

#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MILLISEC 10000
#define MAX_RECORDING_TIME_SEC 18000

#define SAFE_RELEASE(punk) \
    if ((punk) != nullptr) { \
        (punk)->Release(); \
        (punk) = nullptr; \
    }

class audioStreamRecording
{
    class MyAudioSink
    {
    public:
        HRESULT CopyData(BYTE* pData,
                         UINT32 NumFrames,
                         WAVEFORMATEX* pwfx,
                         HMMIO audioRecordingFile);
    };
    MyAudioSink* pMySink;
    std::atomic<bool> stopRecording;
    HMMIO audioRecordingFile;

    const CLSID CLSID_MMDeviceEnumerator;
    const IID IID_IMMDeviceEnumerator;
    const IID IID_IAudioClient;
    const IID IID_IAudioCaptureClient;

    HRESULT WriteWaveHeader(LPCWAVEFORMATEX pwfx, MMCKINFO* pckRIFF, MMCKINFO* pckData);
    HRESULT FinishWaveFile(MMCKINFO* pckRIFF, MMCKINFO* pckData);

public:
    audioStreamRecording(std::string filePath);
    ~audioStreamRecording();

    HRESULT Record();
    HRESULT Stop();
};

#endif // AUDIOSTREAMRECORDING_HPP
