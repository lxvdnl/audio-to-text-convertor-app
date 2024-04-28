#ifndef AUDIOSTREAMRECORDING_HPP
#define AUDIOSTREAMRECORDING_HPP

#pragma once

#include <Windows.h>
#include <audioclient.h>
#include <ctime>
#include <mmdeviceapi.h>
#include <mmsystem.h>
#include <string>

#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MILLISEC 10000
#define MAX_RECORDING_TIME_SEC 30

#define EXIT_ON_ERROR(hres) \
    if (FAILED(hres)) { \
        goto Exit; \
    }
#define SAFE_RELEASE(punk) \
    if ((punk) != NULL) { \
        (punk)->Release(); \
        (punk) = NULL; \
    }

class audioStreamRecording
{
    BOOL stopRecording;
    HMMIO audioRecordingFile;

    const CLSID CLSID_MMDeviceEnumerator;
    const IID IID_IMMDeviceEnumerator;
    const IID IID_IAudioClient;
    const IID IID_IAudioCaptureClient;

    class MyAudioSink
    {
    public:
        HRESULT CopyData(BYTE* pData,
                         UINT32 NumFrames,
                         WAVEFORMATEX* pwfx,
                         HMMIO audioRecordingFile);
    };
    MyAudioSink* pMySink;

    HRESULT WriteWaveHeader(LPCWAVEFORMATEX pwfx, MMCKINFO* pckRIFF, MMCKINFO* pckData);
    HRESULT FinishWaveFile(MMCKINFO* pckRIFF, MMCKINFO* pckData);

public:
    audioStreamRecording(std::string filePath);
    ~audioStreamRecording();

    HRESULT Record();
    HRESULT Stop();
};

#endif // AUDIOSTREAMRECORDING_HPP
