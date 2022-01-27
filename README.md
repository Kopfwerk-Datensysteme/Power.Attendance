# POWER.ATTENDANCE

## Development Setup

1. Install Visual Studio Community 2019 using this [link](https://visualstudio.microsoft.com/de/vs/older-downloads/)
2. Download the Qt installer using this [website](https://www.qt.io/download)
3. Install Qt Creator and Qt 5.15.2 MSVC2019 64bit using the Qt Installer
4. Open the **POWER.ATTENDANCE** project file located under **./Power.Attendance.pro** in Qt Creator to start development

## Installer Building
1. Install NSIS using this [link](https://nsis.sourceforge.io/Download)
2. Set the new product version in **./version.h** (VER_FILEVERSION, VER_FILEVERSION_STR, VER_PRODUCTVERSION, VER_PRODUCTVERSION_STR) and in **./Installer/Power.Attendance.nsi** (VERSIONMAJOR, VERSIONMINOR, VERSIONBUILD)
3. Then change to the **./Installer/** directory and run **./build_installer.bat**
4. The installer is now located in the same directory and is called **Power.Attendance.Setup.exe**

## NFC Reader Selection
The found NFC reader names are logged into the **.log** files present in the database directory.
If there is at least one NFC reader found on the system, the following logic is applied to select the chosen NFC reader:
- one NFC reader is found: select the single found NFC reader for operation
- more than one NFC reader is found: search for the file **nfc_reader_config.txt** in the database directory and choose the NFC reader name that is contained there for operation if the NFC reader and the file exist, otherwise use the first found NFC reader for operation as fallback
