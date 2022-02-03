#pragma once

#include "common.h"

void DeleteBiometricIdFromWindows(QString biometricId);

QString GetBiometricIdForFingerprint();

QString RegisterBiometricIdForFingerprint();
