#include "fingerprintreader.h"

FingerprintReader::FingerprintReader() {}

FingerprintReader::~FingerprintReader() {}

std::string FingerprintReader::GetFingerprintSample()
{
    // define important variables
    std::string id = "";
    HRESULT resultHandle = 0;
    WINBIO_SESSION_HANDLE sessionHandle = 0;
    WINBIO_BIR* sample = nullptr;
    // open a bio session
    resultHandle = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT,
        WINBIO_POOL_SYSTEM,
        WINBIO_FLAG_DEFAULT | WINBIO_FLAG_RAW,
        nullptr,
        0,
        WINBIO_DB_DEFAULT,
        &sessionHandle
    );
    if (resultHandle != S_OK) {
        if (resultHandle == E_ACCESSDENIED) {
            id = "E_ACCESSDENIED";
        }
        FreeBioPointers(sessionHandle, sample);
        return id;
    }
    WINBIO_UNIT_ID unitId;
    size_t sampleSize;
    WINBIO_REJECT_DETAIL rejectDetail;
    resultHandle = WinBioCaptureSample(
        sessionHandle,
        WINBIO_PURPOSE_IDENTIFY,
        WINBIO_DATA_FLAG_PROCESSED,
        &unitId,
        &sample,
        &sampleSize,
        &rejectDetail
    );
    if (resultHandle == S_OK && sample != nullptr) {
        id = std::string((char*)(sample + sample->StandardDataBlock.Offset), sample->StandardDataBlock.Size);
    } else {
        if (resultHandle == WINBIO_E_INVALID_DEVICE_STATE) {
            id = "WINBIO_E_INVALID_DEVICE_STATE";
        }
    }
    FreeBioPointers(sessionHandle, sample);
    return id;
}

void FingerprintReader::FreeBioPointers(WINBIO_SESSION_HANDLE sessionHandle, WINBIO_BIR* sample) {
    WinBioFree(sample);
    WinBioCloseSession(sessionHandle);
}
