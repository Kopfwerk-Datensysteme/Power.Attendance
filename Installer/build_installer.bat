REM setup the environment
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\VsDevCmd.bat" (call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\Common7\Tools\VsDevCmd.bat" -arch=x64) else (call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat" -arch=x64)

REM create a release folder if it does not exist
if not exist "..\Release" mkdir "..\Release"

REM build a release version
cd ..\Release
"C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe" ..\Power.Attendance.pro -spec win32-msvc "CONFIG+=qtquickcompiler"
nmake release
cd ..\Installer

REM delete deploy_files folder
rmdir .\deploy_files /s/q

REM create a new deploy_files folder
mkdir .\deploy_files

REM copy the latest built executable to the deploy_files folder
copy ..\Release\release\Power.Attendance.exe .\deploy_files\Power.Attendance.exe /y

REM add all required Qt DLLs to the deploy_files folder
C:\Qt\5.15.2\msvc2019_64\bin\windeployqt.exe .\deploy_files\Power.Attendance.exe

REM copy the fernwartung.exe to the deploy_files folder
copy .\fernwartung.exe .\deploy_files\fernwartung.exe /y

REM build installer out of all files present in the current directory
"C:\Program Files (x86)\NSIS\makensis.exe" Power.Attendance.nsi
