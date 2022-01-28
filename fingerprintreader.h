#pragma once

#include "common.h"
#include "winbio.h"

QString GetFingerprintSample();
void FreeBioPointers(WINBIO_SESSION_HANDLE sessionHandle, WINBIO_BIR* sample);
