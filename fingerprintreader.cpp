#include "fingerprintreader.h"
#include "winbio_types.h"
#include "winbio_err.h"

#pragma comment(lib, "winbio.lib")
#pragma comment(lib, "advapi32.lib")

typedef HRESULT(__stdcall *PF_WinBioOpenSession) (
    __in WINBIO_BIOMETRIC_TYPE Factor,
    __in WINBIO_POOL_TYPE PoolType,
    __in WINBIO_SESSION_FLAGS Flags,
    __in_ecount_opt(UnitCount) WINBIO_UNIT_ID *UnitArray,
    __in_opt SIZE_T UnitCount,
    __in_opt GUID *DatabaseId,
    __out WINBIO_SESSION_HANDLE *SessionHandle);
typedef HRESULT(__stdcall *PF_WinBioCloseSession) (__in WINBIO_SESSION_HANDLE SessionHandle);
typedef HRESULT(__stdcall *PF_WinBioLocateSensor) (
    __in WINBIO_SESSION_HANDLE SessionHandle,
    __out WINBIO_UNIT_ID *UnitId);
typedef HRESULT(__stdcall *PF_WinBioIdentify) (
    __in WINBIO_SESSION_HANDLE SessionHandle,
    __out WINBIO_UNIT_ID *UnitId,
    __out WINBIO_IDENTITY *Identity,
    __out WINBIO_BIOMETRIC_SUBTYPE *SubFactor,
    __out WINBIO_REJECT_DETAIL *RejectDetail);
typedef HRESULT(__stdcall *PF_WinBioEnrollBegin) (
    __in WINBIO_SESSION_HANDLE SessionHandle,
    __in WINBIO_BIOMETRIC_SUBTYPE SubFactor,
    __in WINBIO_UNIT_ID UnitId);
typedef  HRESULT(__stdcall *PF_WinBioEnumBiometricUnits) (
    __in WINBIO_BIOMETRIC_TYPE Factor,
    __deref_out_ecount(*UnitCount) WINBIO_UNIT_SCHEMA **UnitSchemaArray,
    __out SIZE_T *UnitCount);
typedef HRESULT(__stdcall *PF_WinBioEnrollCapture) (
    __in WINBIO_SESSION_HANDLE SessionHandle,
    __out WINBIO_REJECT_DETAIL *RejectDetail);
typedef HRESULT(__stdcall *PF_WinBioEnrollCommit) (
    __in WINBIO_SESSION_HANDLE SessionHandle,
    __out WINBIO_IDENTITY *Identity,
    __out BOOLEAN *IsNewTemplate);
typedef HRESULT(__stdcall *PF_WinBioEnumEnrollments) (
    __in WINBIO_SESSION_HANDLE SessionHandle,
    __in WINBIO_UNIT_ID UnitId,
    __in WINBIO_IDENTITY *Identity,
    __deref_out_ecount(*SubFactorCount) WINBIO_BIOMETRIC_SUBTYPE **SubFactorArray,
    __out SIZE_T *SubFactorCount);
typedef HRESULT(__stdcall *PF_WinBioDeleteTemplate) (
    __in WINBIO_SESSION_HANDLE SessionHandle,
    __in WINBIO_UNIT_ID UnitId,
    __in WINBIO_IDENTITY *Identity,
    __in WINBIO_BIOMETRIC_SUBTYPE SubFactor);
typedef HRESULT(__stdcall *PF_WinBioFree) (__in PVOID Address);

static const USHORT FPMAXUSER   = 25;
static const USHORT FPMAXFINGER = 8;

static const HINSTANCE winBioLib                                    = LoadLibraryA("winbio.dll");
static const PF_WinBioOpenSession WinBioOpenSession                 = (PF_WinBioOpenSession) GetProcAddress(winBioLib, "WinBioOpenSession");
static const PF_WinBioCloseSession WinBioCloseSession               = (PF_WinBioCloseSession) GetProcAddress(winBioLib, "WinBioCloseSession");
static const PF_WinBioLocateSensor WinBioLocateSensor               = (PF_WinBioLocateSensor) GetProcAddress(winBioLib, "WinBioLocateSensor");
static const PF_WinBioIdentify WinBioIdentify                       = (PF_WinBioIdentify) GetProcAddress(winBioLib, "WinBioIdentify");
static const PF_WinBioEnrollBegin WinBioEnrollBegin                 = (PF_WinBioEnrollBegin) GetProcAddress(winBioLib, "WinBioEnrollBegin");
static const PF_WinBioEnumBiometricUnits WinBioEnumBiometricUnits	= (PF_WinBioEnumBiometricUnits) GetProcAddress(winBioLib, "WinBioEnumBiometricUnits");
static const PF_WinBioEnrollCapture WinBioEnrollCapture             = (PF_WinBioEnrollCapture) GetProcAddress(winBioLib, "WinBioEnrollCapture");
static const PF_WinBioEnrollCommit WinBioEnrollCommit               = (PF_WinBioEnrollCommit) GetProcAddress(winBioLib, "WinBioEnrollCommit");
static const PF_WinBioEnumEnrollments WinBioEnumEnrollments         = (PF_WinBioEnumEnrollments) GetProcAddress(winBioLib, "WinBioEnumEnrollments");
static const PF_WinBioDeleteTemplate WinBioDeleteTemplate           = (PF_WinBioDeleteTemplate) GetProcAddress(winBioLib, "WinBioDeleteTemplate");
static const PF_WinBioFree WinBioFree                               = (PF_WinBioFree) GetProcAddress(winBioLib, "WinBioFree");

struct SFPSensor {
        ULONG _ulUnitId;
        QString _sDescription;
        QString _sManufacturer;
        QString _sModel;
};

bool IsDigitString(QString str) {
    QRegExp re("\\d*");
    return re.exactMatch(str);
}

QString	GetSystemErrorMessage(DWORD nErr)
{
    LPVOID lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        nErr,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language ( = ONLY language available, since windos is MONOlingual)
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
    );

    QString strRet = QString::fromStdString(std::string((char*)lpMsgBuf));

    // Free the buffer.
    LocalFree( lpMsgBuf );

    return strRet;
}

std::vector<QString> LoadTextFileArr(QString sInFName)
{
    std::vector<QString> vsRslt;
    std::ifstream ifstr(sInFName.toStdString(), std::ios_base::in);
    if (ifstr.is_open()) {
        for (std::string ssLine; std::getline(ifstr, ssLine); ) {
            QString sLine = QString(ssLine.c_str());
            vsRslt.push_back(sLine);
        }
        ifstr.close();
    }
    return vsRslt;
}

BOOL StoreTextFile(QString sInFName, QString sInData)
{
    std::ofstream ofstr(sInFName.toStdString(), std::ios_base::out);
    if (ofstr.is_open()) {
        ofstr << sInData.toStdString();
        ofstr.close();
        return TRUE;
    } else {
        return FALSE;
    }
}

static BOOL StoreTextFileArr(QString sInFName, std::vector<QString> vsInData)
{
    QString sData;
    for (QString s : vsInData)
        sData += (!sData.isEmpty() ? "\r\n" : "") + s;
    return StoreTextFile(sInFName, sData);
}

std::vector<std::tuple<QString, USHORT, SFPSensor>> WBFLoadUserMetadata() {
    std::vector<std::tuple<QString, USHORT, SFPSensor>> vUserNames;
    std::vector<QString> vsLines = LoadTextFileArr(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + "KWFPREG2.DAT");
    // Zeile 0 ist für die Flags
    for (USHORT i = 1; i < vsLines.size(); i++) {
        QStringList vsCols = vsLines[i].split("|");
        if (vsCols.size() != 6 && IsDigitString(vsCols[1]) && IsDigitString(vsCols[2])) {
            ShowMessage("Finger-Registrierung: Fehler beim Laden der Registrierung!");
            return vUserNames;
        }
        vUserNames.push_back(std::tuple<QString, USHORT, SFPSensor>(vsCols[0], atoi(vsCols[1].toStdString().c_str()),
                            { static_cast<ULONG>(atol(vsCols[2].toStdString().c_str())), vsCols[3], vsCols[4], vsCols[5] }));
    }
    return vUserNames;
}

BOOL WBFStoreUserMetadata(std::vector<std::tuple<QString, USHORT, SFPSensor>> vIn)
{
    // Lese Zeile 0 (=flags)
    std::vector<QString> vsLinesSrc = LoadTextFileArr(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + "KWFPREG2.DAT");

    std::vector<QString> vsLinesDst;
    if (vsLinesSrc.size() > 0)	vsLinesDst.push_back(vsLinesSrc[0]);
    else						vsLinesDst.push_back("kwfpffffffff");

    for (USHORT i = 0; i < vIn.size(); i++) {
        vsLinesDst.push_back(std::get<0>(vIn[i]) + "|" +
                          QString::number(std::get<1>(vIn[i])) + "|" +
                          QString::number(std::get<2>(vIn[i])._ulUnitId) + "|" +
                          std::get<2>(vIn[i])._sDescription + "|" +
                          std::get<2>(vIn[i])._sManufacturer + "|" +
                          std::get<2>(vIn[i])._sModel);
    }
    return StoreTextFileArr(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QDir::separator() + "KWFPREG2.DAT", vsLinesDst);
}

BOOL WBFEnumerateSensors(std::vector<SFPSensor>& vOutSensors)
{
    vOutSensors.clear();

    // Declare variables.
    PWINBIO_UNIT_SCHEMA unitSchema = NULL;
    SIZE_T unitCount = 0;

    // Enumerate the installed biometric units.
    HRESULT hr = WinBioEnumBiometricUnits(WINBIO_TYPE_FINGERPRINT, &unitSchema, &unitCount);
    if (FAILED(hr)) {
        ShowMessage("Fehler bei der Erkennung der Sensoren!");
        if (unitSchema != NULL)
            WinBioFree(unitSchema);
        return FALSE;
    }

    for (USHORT i = 0; i < unitCount; ++i) {
        vOutSensors.push_back(SFPSensor{ unitSchema[i].UnitId,
                              QString::fromStdWString(std::wstring(unitSchema[i].Description)),
                              QString::fromStdWString(std::wstring(unitSchema[i].Manufacturer)),
                              QString::fromStdWString(std::wstring(unitSchema[i].Model)) });
    }

    if (unitSchema != NULL) {
        WinBioFree(unitSchema);
    }
    return TRUE;
}

BOOL WBFGetCurrentUserIdentity(PWINBIO_IDENTITY Identity)
{
    // Declare variables.
    HRESULT hr = S_OK;
    HANDLE tokenHandle = NULL;
    DWORD bytesReturned = 0;
    struct {
        TOKEN_USER tokenUser;
        BYTE buffer[SECURITY_MAX_SID_SIZE];
    } tokenInfoBuffer;

    // Zero the input identity and specify the type.
    ZeroMemory(Identity, sizeof(WINBIO_IDENTITY));
    Identity->Type = WINBIO_ID_TYPE_NULL;

    // Open the access token associated with the current process
    if (!OpenProcessToken(
        GetCurrentProcess(),            // Process handle
        TOKEN_READ,                     // Read access only
        &tokenHandle))                  // Access token handle
    {
        DWORD win32Status = GetLastError();
        wprintf_s(L"Cannot open token handle: %d\n", win32Status);
        hr = HRESULT_FROM_WIN32(win32Status);
        if (tokenHandle != NULL) CloseHandle(tokenHandle);
        return FALSE;
    }

    // Zero the tokenInfoBuffer structure.
    ZeroMemory(&tokenInfoBuffer, sizeof(tokenInfoBuffer));

    // Retrieve information about the access token. In this case, retrieve a SID.
    if (!GetTokenInformation(
        tokenHandle,                    // Access token handle
        TokenUser,                      // User for the token
        &tokenInfoBuffer.tokenUser,     // Buffer to fill
        sizeof(tokenInfoBuffer),        // Size of the buffer
        &bytesReturned))                // Size needed
    {
        DWORD win32Status = GetLastError();
        wprintf_s(L"Cannot query token information: %d\n", win32Status);
        hr = HRESULT_FROM_WIN32(win32Status);
        if (tokenHandle != NULL) CloseHandle(tokenHandle);
        return FALSE;
    }

    // Copy the SID from the tokenInfoBuffer structure to the
    // WINBIO_IDENTITY structure.
    CopySid(
        SECURITY_MAX_SID_SIZE,
        Identity->Value.AccountSid.Data,
        tokenInfoBuffer.tokenUser.User.Sid
        );

    // Specify the size of the SID and assign WINBIO_ID_TYPE_SID
    // to the type member of the WINBIO_IDENTITY structure.
    Identity->Value.AccountSid.Size = GetLengthSid(tokenInfoBuffer.tokenUser.User.Sid);
    Identity->Type = WINBIO_ID_TYPE_SID;

    if (tokenHandle != NULL) CloseHandle(tokenHandle);
    return hr == S_OK;
}

BOOL WBFEnroll(USHORT unInUser, USHORT unInFinger, ULONG& ulIoScannerId)
{
    if (unInUser >= FPMAXUSER || unInFinger >= FPMAXFINGER) {
        ShowMessage("Ungültiger Benutzer oder Finger!");
        return FALSE;
    }
    UCHAR ucSubType = unInUser * FPMAXFINGER + unInFinger + 1;	// 1..

    // gibt es überhaupt einen Sensor?
    std::vector<SFPSensor> vSensors;
    if (!WBFEnumerateSensors(vSensors) || vSensors.size() == 0) {
        ShowMessage("Es wurde kein Sensor gefunden!");
        return FALSE;
    }

    // Connect to the system pool.
    WINBIO_SESSION_HANDLE sessionHandle = NULL;
    HRESULT hr = WinBioOpenSession(
        WINBIO_TYPE_FINGERPRINT,    // Service provider
        WINBIO_POOL_SYSTEM,         // Pool type
        WINBIO_FLAG_DEFAULT,        // Configuration and access
        NULL,                       // Array of biometric unit IDs
        0,                          // Count of biometric unit IDs
        NULL,                       // Database ID
        &sessionHandle);            // [out] Session handle
    if (FAILED(hr)) {
        ShowMessage("Es kann keine Verbindung zu WBF hergestellt werden!");
        return FALSE;
    }

    // Locate a sensor.
    if (ulIoScannerId == 999) {
        ShowMessage("Tippen Sie bitte auf den gewünschten Sensor");
        if (WinBioLocateSensor(sessionHandle, &ulIoScannerId)<0) {
            ShowMessage("Der Fingerprintsensor wurde nicht erkannt!");
            if (sessionHandle != NULL)
                WinBioCloseSession(sessionHandle);
            return FALSE;
        }
    }

    // Ein möglicherweise bestehendes template entfernen
    WINBIO_IDENTITY identity = { 0 };
    hr = WBFGetCurrentUserIdentity(&identity);
    if (FAILED(hr)) {
        ShowMessage("Fehler bei der Windowsuser-Identität!");
        if (sessionHandle != NULL)
            WinBioCloseSession(sessionHandle);
        return FALSE;
    }
    hr = WinBioDeleteTemplate(sessionHandle, ulIoScannerId, &identity, ucSubType);
    // Jetzt gehts wirklich los
    if (ucSubType == WINBIO_SUBTYPE_NO_INFORMATION ||	//+++
        ucSubType == WINBIO_SUBTYPE_ANY ||
        ulIoScannerId == 0)
        ShowMessage("Fehlerhafte Daten!");
    hr = WinBioEnrollBegin(sessionHandle, ucSubType, ulIoScannerId);
    if (FAILED(hr)) {
        // WINBIO_E_DUPLICATE_ENROLLMENT    ((HRESULT)0x8009801CL)
        ShowMessage("Fehler(EnrollBegin): " + GetSystemErrorMessage(hr));
        if (sessionHandle != NULL)
            WinBioCloseSession(sessionHandle);
        return FALSE;
    }

    QString asFingerNames[] = { "linken Zeigefinger", "linken Mittelfinger", "linken Ringfinger", "linken kleinen Finger", "linken Daumen",
        "rechten Zeigefinger", "rechten Mittelfinger", "rechten Ringfinger", "rechten kleinen Finger", "rechten Daumen" };

    USHORT i = 1;
    do {
        ShowMessage("Bitte " + asFingerNames[unInFinger] + " für Abdruck " + QString::number(i) + " auflegen.");
        WINBIO_REJECT_DETAIL rejectDetail = 0;
        hr = WinBioEnrollCapture(sessionHandle, &rejectDetail);
        if (hr == WINBIO_I_MORE_DATA)
            i++;
        else if (hr == WINBIO_E_BAD_CAPTURE)
            ShowMessage("Schlechter Abdruck. Bitte nochmals.");
        else if (FAILED(hr)) {
            ShowMessage("Fehler(EnrollCapture): " + GetSystemErrorMessage(hr));
            if (sessionHandle != NULL)
                WinBioCloseSession(sessionHandle);
            return FALSE;
        }
    } while (hr == WINBIO_I_MORE_DATA || hr == WINBIO_E_BAD_CAPTURE);

    BOOLEAN bIsNew = TRUE;
    hr = WinBioEnrollCommit(sessionHandle, &identity, &bIsNew);
    if (FAILED(hr)) {
        if (hr == WINBIO_E_DUPLICATE_TEMPLATE)
            ShowMessage("Dieser Finger wurde bereits eingelesen!\nBitte den Vorgang mit einem anderen Finger wiederholen.");
        else
            ShowMessage("Fehler(EnrollCommit): " + GetSystemErrorMessage(hr));
        if (sessionHandle != NULL)
            WinBioCloseSession(sessionHandle);
        return FALSE;
    }

    if (sessionHandle != NULL)
        WinBioCloseSession(sessionHandle);
    return TRUE;
}

void RegisterFingerprintForId(QString biometricId) {
    // ist der User dabei? Oder neuen anlegen?
    // Benutzername auf eine ID mappen
    USHORT unUserIdx = 999;	// invalid
    USHORT unMaxUserId = 0;
    std::vector<std::tuple<QString, USHORT, SFPSensor>> vUserNames = WBFLoadUserMetadata();
    for (USHORT i = 0; i < vUserNames.size(); i++) {
        if (std::get<1>(vUserNames[i]) > unMaxUserId)	unMaxUserId = std::get<1>(vUserNames[i]);
        if (std::get<0>(vUserNames[i]) == biometricId) unUserIdx = i;
    }
    USHORT unUserId = 0;
    if (unUserIdx != 999)	unUserId = std::get<1>(vUserNames[unUserIdx]);
    else					unUserId = vUserNames.size() > 0 ? unMaxUserId + 1 : 0;

    ULONG ulScannerId = 999;	// invalid -> wird beim ersten Aufruf von FPEnroll ausgewählt
    BOOL bIsOK = FALSE;
    for (USHORT unFinger = 0; unFinger < 3; unFinger++) {
        if (WBFEnroll(unUserId, unFinger, ulScannerId))
            bIsOK = TRUE;	// ein Finger soll zumindest gelesen worden sein.
        else
            break;
    }

    if (bIsOK) {
        if (unUserIdx == 999)
            vUserNames.push_back(std::tuple<QString, USHORT, SFPSensor>(biometricId, unUserId,
            SFPSensor({ ulScannerId, "", "", "" })));	// TODO
        else
            vUserNames[unUserIdx] = std::tuple<QString, USHORT, SFPSensor>(biometricId, unUserId,
            SFPSensor({ ulScannerId, "", "", "" }));	// TODO

        if (!WBFStoreUserMetadata(vUserNames)) {
            ShowMessage("Finger-Registrierung: Fehler beim Speichern der Registrierung!");
            bIsOK = FALSE;
        }
        if (bIsOK) ShowMessage("Die Finger-Registrierung wurde erfolgreich durchgeführt!");
    }
    if (!bIsOK) {
        throw QException();
    }
}

QString GetIdForFingerprint() {
    return "";
}


int main(int argc, char* argv[])
{
    RegisterFingerprintForId("Hannes");
}
