#ifndef AUDIOSTREAMRECORDINGWIN_HPP
#define AUDIOSTREAMRECORDINGWIN_HPP

#pragma once

#include <Windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <mmsystem.h>

#include <atomic>
#include <string>

#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MILLISEC 10000
#define MAX_RECORDING_TIME_SEC 18000

#define SAFE_RELEASE(punk)   \
    if ((punk) != nullptr) { \
        (punk)->Release();   \
        (punk) = nullptr;    \
    }

class audioStreamRecordingWin {
    class MyAudioSink {
       public:
        HRESULT CopyData(BYTE* pData, UINT32 NumFrames, WAVEFORMATEX* pwfx,
                         HMMIO audioRecordingFile);
    };
    MyAudioSink* pMySink;
    std::atomic<bool> stopRecording;
    HMMIO audioRecordingFile;

    const CLSID CLSID_MMDeviceEnumerator;
    const IID IID_IMMDeviceEnumerator;
    const IID IID_IAudioClient;
    const IID IID_IAudioCaptureClient;

    HRESULT WriteWaveHeader(LPCWAVEFORMATEX pwfx, MMCKINFO* pckRIFF,
                            MMCKINFO* pckData);
    HRESULT FinishWaveFile(MMCKINFO* pckRIFF, MMCKINFO* pckData);

   public:
    audioStreamRecordingWin(std::string filePath);
    ~audioStreamRecordingWin();

    HRESULT Record();
    HRESULT Stop();
};

#endif  // AUDIOSTREAMRECORDINGWIN_HPP
