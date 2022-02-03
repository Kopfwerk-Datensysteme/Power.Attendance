#include "fingerprintreader.h"
#include "database.h"
#include "winbio.h"

#pragma comment(lib, "winbio.lib")
#pragma comment(lib, "advapi32.lib")

class WinBioSession {
   public:
    WinBioSession() {
        if (WinBioOpenSession(WINBIO_TYPE_FINGERPRINT, WINBIO_POOL_SYSTEM, WINBIO_FLAG_DEFAULT, nullptr, 0, WINBIO_DB_DEFAULT, &handle) != S_OK) {
            throw QException();
        }
        if (WinBioCancel(handle) != S_OK) {
            throw QException();
        }
    }
    ~WinBioSession() {
        WinBioCloseSession(handle);
    }
    WINBIO_SESSION_HANDLE handle;
};

void HandleMissingFingerprintSensor() {
    ShowMessage("Es ist leider kein Fingerabdrucksensor auf Ihrem System verfügbar!");
    throw QException();
}

QString GetFreeBiometricId() {
    QSet<QString> freeBiometricIds;
    for (size_t i = 1; i <= 254; i++) {
        freeBiometricIds.insert(QString::number(i));
    }
    auto userList = GetUsers();
    for (auto& user : userList) {
        freeBiometricIds.remove(user.biometricId);
    }
    if (freeBiometricIds.empty()) {
        throw QException();
    }
    return *freeBiometricIds.begin();
}

WINBIO_IDENTITY GetCurrentUserIdentity()
{
    WINBIO_IDENTITY identity;
    HANDLE tokenHandle;
    DWORD returnedBytes;
    struct {
        TOKEN_USER tokenUser;
        BYTE buffer[SECURITY_MAX_SID_SIZE];
    } tokenInfoBuffer;
    ZeroMemory(&identity, sizeof(WINBIO_IDENTITY));
    identity.Type = WINBIO_ID_TYPE_NULL;
    if (!OpenProcessToken(
        GetCurrentProcess(),
        TOKEN_READ,
        &tokenHandle))
    {
        throw QException();
    }
    ZeroMemory(&tokenInfoBuffer, sizeof(tokenInfoBuffer));
    if (!GetTokenInformation(
        tokenHandle,
        TokenUser,
        &tokenInfoBuffer.tokenUser,
        sizeof(tokenInfoBuffer),
        &returnedBytes))
    {
        CloseHandle(tokenHandle);
        throw QException();
    }
    CopySid(
        SECURITY_MAX_SID_SIZE,
        identity.Value.AccountSid.Data,
        tokenInfoBuffer.tokenUser.User.Sid
    );
    identity.Value.AccountSid.Size = GetLengthSid(tokenInfoBuffer.tokenUser.User.Sid);
    identity.Type = WINBIO_ID_TYPE_SID;
    CloseHandle(tokenHandle);
    return identity;
}

QList<WINBIO_UNIT_ID> EnumerateFingerprintSensors()
{
    HRESULT hr;
    QList<WINBIO_UNIT_ID> fingerprintSensors;
    WINBIO_UNIT_SCHEMA* unitSchemaArray;
    std::size_t unitCount;
    hr = WinBioEnumBiometricUnits(WINBIO_TYPE_FINGERPRINT, &unitSchemaArray, &unitCount);
    if (hr == S_OK) {
        for (std::size_t index = 0; index < unitCount; index++) {
            fingerprintSensors.append(unitSchemaArray[index].UnitId);
        }
    }
    WinBioFree(unitSchemaArray);
    return fingerprintSensors;
}

void DeleteBiometricIdFromWindows(QString biometricId) {
    WINBIO_IDENTITY identity = GetCurrentUserIdentity();
    auto sensorList = EnumerateFingerprintSensors();
    WinBioSession session;
    for (auto sensor : sensorList) {
        HRESULT hr;
        hr = WinBioDeleteTemplate(session.handle, sensor, &identity, biometricId.toInt());
        if (hr != S_OK && hr != WINBIO_E_DATABASE_NO_SUCH_RECORD) {
            throw QException();
        }
    }

}

QString GetAdditionalInformation(WINBIO_REJECT_DETAIL rejectDetail) {
    QString message;
    if (rejectDetail == WINBIO_FP_TOO_HIGH) {
        message += "Bitte legen Sie den Finger weiter unten auf den Scanner!";
    } else if (rejectDetail == WINBIO_FP_TOO_LOW) {
        message += "Bitte legen Sie den Finger weiter oben auf den Scanner!";
    } else if (rejectDetail == WINBIO_FP_TOO_LEFT) {
        message += "Bitte legen Sie den Finger weiter rechts auf den Scanner!";
    } else if (rejectDetail == WINBIO_FP_TOO_RIGHT) {
        message += "Bitte legen Sie den Finger weiter links auf den Scanner!";
    } else if (rejectDetail == WINBIO_FP_TOO_FAST) {
        message += "Bitte tippen Sie langsamer auf den Scanner!";
    } else if (rejectDetail == WINBIO_FP_TOO_SLOW) {
        message += "Bitte tippen Sie schneller auf den Scanner!";
    } else if (rejectDetail == WINBIO_FP_POOR_QUALITY) {
        message += "";
    } else if (rejectDetail == WINBIO_FP_TOO_SKEWED) {
        message += "Bitte legen Sie den Finger gerade auf den Scanner";
    } else if (rejectDetail == WINBIO_FP_TOO_SHORT) {
        message += "Bitte tippen Sie langsamer auf den Scanner!";
    } else if (rejectDetail == WINBIO_FP_MERGE_FAILURE) {
        message += "";
    }
    return message;
}

QString GetBiometricIdForFingerprint() {
    auto sensorList = EnumerateFingerprintSensors();
    if (sensorList.size() == 0) {
        HandleMissingFingerprintSensor();
    }
    WinBioSession session;
    HRESULT hr;
    WINBIO_UNIT_ID unitId;
    WINBIO_IDENTITY identity;
    WINBIO_BIOMETRIC_SUBTYPE subFactor;
    WINBIO_REJECT_DETAIL rejectDetail;
    auto msgBox = GetNonModalMessageBox("Bitte legen Sie ihren Finger wiederholt auf den Fingerabdrucksensor bis diese Meldung verschwindet!");
    while (true) {
        hr = WinBioIdentify(session.handle, &unitId, &identity, &subFactor, &rejectDetail);
        if (hr == S_OK) {
            QString biometricId = QString::number(subFactor);
            msgBox->close();
            return biometricId;
        } else if (hr == WINBIO_E_UNKNOWN_ID) {
            msgBox->close();
            ShowMessage("Dieser Fingerabdruck ist keinem Benutzer zugeordnet!");
            throw QException();
        }
    }
}

QString RegisterFingerprintForBiometricId() {
    auto sensorList = EnumerateFingerprintSensors();
    if (sensorList.size() == 0) {
        HandleMissingFingerprintSensor();
    }
    WinBioSession session;
    HRESULT hr;
    WINBIO_UNIT_ID sensorId;
    if (sensorList.size() == 1) {
        sensorId = sensorList[0];
    } else {
        auto msgBox = GetNonModalMessageBox("Tippen Sie bitte auf den Fingerabdrucksensor, den Sie für die Registrierung verwenden möchten!");
        hr = WinBioLocateSensor(session.handle, &sensorId);
        msgBox->close();
        if (hr != S_OK) {
            throw QException();
        }
    }
    QString biometricId = GetFreeBiometricId();
    DeleteBiometricIdFromWindows(biometricId);
    hr = WinBioEnrollBegin(session.handle, biometricId.toInt(), sensorId);
    if (hr != S_OK) {
        if (hr == WINBIO_E_DATABASE_FULL) {
            ShowMessage("Die Datenbank zur Speicherung der Fingerabdrücke ist voll!");
        }
        throw QException();
    }
    WINBIO_REJECT_DETAIL rejectDetail = 0;
    for (std::size_t i = 1;; i++ ) {
        QString additionalInformation = GetAdditionalInformation(rejectDetail);
        if (!additionalInformation.isEmpty()) {
            additionalInformation = "\n\n" + additionalInformation;
        }
        auto msgBox = GetNonModalMessageBox("Legen Sie bitte Ihren Finger auf den Fingerabrucksensor, um den " + QString::number(i) + ". Abdruck zu scannen!" + additionalInformation);
        hr = WinBioEnrollCapture(session.handle, &rejectDetail);
        msgBox->close();
        if (hr == S_OK) {
            break;
        }
    }
    WINBIO_IDENTITY identity;
    BOOLEAN isNewTemplate;
    hr = WinBioEnrollCommit(session.handle, &identity, &isNewTemplate);
    if (hr != S_OK) {
        throw QException();
    }
    return biometricId;
}
