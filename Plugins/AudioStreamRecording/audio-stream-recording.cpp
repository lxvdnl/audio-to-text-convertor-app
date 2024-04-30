#include "audio-stream-recording.hpp"

audioStreamRecording::audioStreamRecording(std::string filePath)
    : pMySink(nullptr),
      stopRecording(false),
      audioRecordingFile(nullptr),
      CLSID_MMDeviceEnumerator(__uuidof(MMDeviceEnumerator)),
      IID_IMMDeviceEnumerator(__uuidof(IMMDeviceEnumerator)),
      IID_IAudioClient(__uuidof(IAudioClient)),
      IID_IAudioCaptureClient(__uuidof(IAudioCaptureClient)) {
    int size_needed =
        MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0);

    WCHAR* fileName = new WCHAR[(size_needed + 1) * 2];
    MultiByteToWideChar(CP_UTF8, 0, filePath.c_str(), -1, fileName,
                        size_needed);
    fileName[size_needed] = L'\0';

    MMIOINFO mi = {0};
    audioRecordingFile =
        mmioOpenW((LPWSTR)fileName, &mi, MMIO_WRITE | MMIO_CREATE);
    delete[] fileName;
    pMySink = new MyAudioSink;
}

audioStreamRecording::~audioStreamRecording() {
    mmioClose(audioRecordingFile, 0);
    CoUninitialize();
}

HRESULT audioStreamRecording::MyAudioSink::CopyData(BYTE* pData,
                                                    UINT32 NumFrames,
                                                    WAVEFORMATEX* pwfx,
                                                    HMMIO audioRecordingFile) {
    HRESULT hr = S_OK;

    if (!NumFrames) {
        wprintf(L"IAudioCaptureClient::GetBuffer said to read 0 frames\n");
        return E_UNEXPECTED;
    }

    LONG lBytesToWrite = NumFrames * pwfx->nBlockAlign;
#pragma prefast(                      \
    suppress                          \
    : __WARNING_INCORRECT_ANNOTATION, \
      "IAudioCaptureClient::GetBuffer SAL annotation implies a 1-byte buffer")
    LONG lBytesWritten = mmioWrite(
        audioRecordingFile, reinterpret_cast<PCHAR>(pData), lBytesToWrite);

    return S_OK;
}

HRESULT audioStreamRecording::Record() {
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator* pEnumerator = nullptr;
    IMMDevice* pDevice = nullptr;
    IAudioClient* pAudioClient = nullptr;
    IAudioCaptureClient* pCaptureClient = nullptr;
    WAVEFORMATEX* pwfx = nullptr;
    UINT32 packetLength = 0;
    BYTE* pData;
    DWORD flags;

    MMCKINFO ckRIFF = {0};
    MMCKINFO ckData = {0};

    hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        std::cerr << "CoInitializeEx failed with HRESULT: 0x" << std::hex << hr
                  << std::endl;
        return hr;
    }

    hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                          IID_IMMDeviceEnumerator, (void**)&pEnumerator);
    if (FAILED(hr)) {
        std::cerr << "CoCreateInstance failed with HRESULT: 0x" << std::hex
                  << hr << std::endl;
        return hr;
    }

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
    if (FAILED(hr)) {
        std::cerr
            << "pEnumerator->GetDefaultAudioEndpoint failed with HRESULT: 0x"
            << std::hex << hr << std::endl;
        return hr;
    }

    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL, nullptr,
                           (void**)&pAudioClient);
    if (FAILED(hr)) {
        std::cerr << "pDevice->Activate failed with HRESULT: 0x" << std::hex
                  << hr << std::endl;
        return hr;
    }

    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) {
        std::cerr << "pAudioClient->GetMixFormat failed with HRESULT: 0x"
                  << std::hex << hr << std::endl;
        return hr;
    }

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  AUDCLNT_STREAMFLAGS_LOOPBACK,
                                  hnsRequestedDuration, 0, pwfx, nullptr);
    if (FAILED(hr)) {
        std::cerr << "pAudioClient->Initialize failed with HRESULT: 0x"
                  << std::hex << hr << std::endl;
        return hr;
    }

    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    if (FAILED(hr)) {
        std::cerr << "pAudioClient->GetBufferSize failed with HRESULT: 0x"
                  << std::hex << hr << std::endl;
        return hr;
    }

    hr = pAudioClient->GetService(IID_IAudioCaptureClient,
                                  (void**)&pCaptureClient);
    if (FAILED(hr)) {
        std::cerr << "pAudioClient->GetService failed with HRESULT: 0x"
                  << std::hex << hr << std::endl;
        return hr;
    }

    hr = WriteWaveHeader(pwfx, &ckRIFF, &ckData);
    if (FAILED(hr)) {
        std::cerr << "WriteWaveHeader failed with HRESULT: 0x" << std::hex << hr
                  << std::endl;
        return hr;
    }

    hnsActualDuration =
        (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();
    if (FAILED(hr)) {
        std::cerr << "pAudioClient->Start failed with HRESULT: 0x" << std::hex
                  << hr << std::endl;
        return hr;
    }

    auto start = std::chrono::steady_clock::now();

    std::cout << "Recording...\n";

    while (!stopRecording) {
        Sleep(hnsActualDuration / REFTIMES_PER_MILLISEC / 2);

        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        if (FAILED(hr)) {
            std::cerr
                << "pCaptureClient->GetNextPacketSize failed with HRESULT: 0x"
                << std::hex << hr << std::endl;
            return hr;
        }

        while (packetLength) {
            hr = pCaptureClient->GetBuffer(&pData, &numFramesAvailable, &flags,
                                           nullptr, nullptr);
            if (FAILED(hr)) {
                std::cerr << "pCaptureClient->GetBuffer failed with HRESULT: 0x"
                          << std::hex << hr << std::endl;
                return hr;
            }

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
                pData = nullptr;
            }

            hr = pMySink->CopyData(pData, numFramesAvailable, pwfx,
                                   (HMMIO)audioRecordingFile);
            if (FAILED(hr)) {
                std::cerr << "pMySink->CopyData failed with HRESULT: 0x"
                          << std::hex << hr << std::endl;
                return hr;
            }

            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            if (FAILED(hr)) {
                std::cerr
                    << "pCaptureClient->ReleaseBuffer failed with HRESULT: 0x"
                    << std::hex << hr << std::endl;
                return hr;
            }

            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            if (FAILED(hr)) {
                std::cerr << "pCaptureClient->GetNextPacketSize failed with "
                             "HRESULT: 0x"
                          << std::hex << hr << std::endl;
                return hr;
            }
        }
        if (std::chrono::steady_clock::now() - start >
            std::chrono::seconds(MAX_RECORDING_TIME_SEC))
            Stop();
    }
    std::cout << "Recording stopped\n";

    hr = pAudioClient->Stop();
    if (FAILED(hr)) {
        std::cerr << "pAudioClient->Stop failed with HRESULT: 0x" << std::hex
                  << hr << std::endl;
        return hr;
    }

    hr = FinishWaveFile(&ckData, &ckRIFF);
    if (FAILED(hr)) {
        std::cerr << "FinishWaveFile failed with HRESULT: 0x" << std::hex << hr
                  << std::endl;
        return hr;
    }

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)

    return hr;
}

HRESULT audioStreamRecording::Stop() {
    stopRecording = true;
    return S_OK;
}

HRESULT audioStreamRecording::WriteWaveHeader(LPCWAVEFORMATEX pwfx,
                                              MMCKINFO* pckRIFF,
                                              MMCKINFO* pckData) {
    MMRESULT result;

    pckRIFF->ckid = MAKEFOURCC('R', 'I', 'F', 'F');
    pckRIFF->fccType = MAKEFOURCC('W', 'A', 'V', 'E');

    result = mmioCreateChunk(audioRecordingFile, pckRIFF, MMIO_CREATERIFF);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioCreateChunk(\"RIFF/WAVE\") failed: MMRESULT = 0x%08x",
                result);
        return E_FAIL;
    }

    MMCKINFO chunk;
    chunk.ckid = MAKEFOURCC('f', 'm', 't', ' ');
    result = mmioCreateChunk(audioRecordingFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioCreateChunk(\"fmt \") failed: MMRESULT = 0x%08x", result);
        return E_FAIL;
    }

    LONG lBytesInWfx = sizeof(WAVEFORMATEX) + pwfx->cbSize;
    LONG lBytesWritten = mmioWrite(
        audioRecordingFile,
        reinterpret_cast<PCHAR>(const_cast<LPWAVEFORMATEX>(pwfx)), lBytesInWfx);
    if (lBytesWritten != lBytesInWfx) {
        wprintf(L"mmioWrite(fmt data) wrote %u bytes; expected %u bytes",
                lBytesWritten, lBytesInWfx);
        return E_FAIL;
    }

    result = mmioAscend(audioRecordingFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioAscend(\"fmt \" failed: MMRESULT = 0x%08x", result);
        return E_FAIL;
    }

    chunk.ckid = MAKEFOURCC('f', 'a', 'c', 't');
    result = mmioCreateChunk(audioRecordingFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioCreateChunk(\"fmt \") failed: MMRESULT = 0x%08x", result);
        return E_FAIL;
    }

    DWORD frames = 0;
    lBytesWritten = mmioWrite(audioRecordingFile,
                              reinterpret_cast<PCHAR>(&frames), sizeof(frames));
    if (lBytesWritten != sizeof(frames)) {
        wprintf(L"mmioWrite(fact data) wrote %u bytes; expected %u bytes",
                lBytesWritten, (UINT32)sizeof(frames));
        return E_FAIL;
    }

    result = mmioAscend(audioRecordingFile, &chunk, 0);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioAscend(\"fact\" failed: MMRESULT = 0x%08x", result);
        return E_FAIL;
    }

    pckData->ckid = MAKEFOURCC('d', 'a', 't', 'a');
    result = mmioCreateChunk(audioRecordingFile, pckData, 0);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioCreateChunk(\"data\") failed: MMRESULT = 0x%08x", result);
        return E_FAIL;
    }

    return S_OK;
}

HRESULT audioStreamRecording::FinishWaveFile(MMCKINFO* pckRIFF,
                                             MMCKINFO* pckData) {
    MMRESULT result;

    result = mmioAscend(audioRecordingFile, pckData, 0);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioAscend(\"data\" failed: MMRESULT = 0x%08x", result);
        return E_FAIL;
    }

    result = mmioAscend(audioRecordingFile, pckRIFF, 0);
    if (MMSYSERR_NOERROR != result) {
        wprintf(L"mmioAscend(\"RIFF/WAVE\" failed: MMRESULT = 0x%08x", result);
        return E_FAIL;
    }

    return S_OK;
}
