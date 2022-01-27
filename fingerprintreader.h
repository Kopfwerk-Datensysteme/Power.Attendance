#pragma once

#include "common.h"
#include "windows.h"
#include "winbio.h"

class FingerprintReader
{
public:
    FingerprintReader();
    ~FingerprintReader();
    static std::string GetFingerprintSample();
    static void FreeBioPointers(WINBIO_SESSION_HANDLE sessionHandle, WINBIO_BIR* sample);
};
