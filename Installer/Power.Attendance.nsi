# https://nsis.sourceforge.io/Docs/Contents.html
# https://nsis.sourceforge.io/A_simple_installer_with_start_menu_shortcut_and_uninstaller

# This installs the necessary files, creates a start menu shortcut, builds an uninstaller, and
# adds uninstall information to the registry for Add/Remove Programs
  
!include "MUI.nsh"
!include LogicLib.nsh
Unicode true

#------------------------------------------------------------------------------

!define APPNAME     "Power.Attendance"
!define COMPANYNAME "KOPFWERK"
!define DESCRIPTION "Verwaltung zur Ausgabe von Winkel- und Wäschestücken"
# These three must be integers
!define VERSIONMAJOR 1
!define VERSIONMINOR 1
!define VERSIONBUILD 0
# These will be displayed by the "Click here for support information" link in "Add/Remove Programs"
# It is possible to use "mailto:" links in here to open the email client
!define HELPURL   "http://www.kopfwerk.at/tipps-tricks/" 	# "Support Information" link
!define UPDATEURL "http://www.kopfwerk.at/kundenbereich/" 	# "Product Updates" link
!define ABOUTURL  "http://www.kopfwerk.at/" 			# "Publisher" link
# This is the size (in kB) of all the files copied into "Program Files"
!define INSTALLSIZE 72000

VIAddVersionKey /LANG=0 "ProductName" 		"${APPNAME}"
VIAddVersionKey /LANG=0 "CompanyName" 		"${COMPANYNAME} Datensysteme GmbH."
VIAddVersionKey /LANG=0 "Comments" 		    "${DESCRIPTION}"
VIAddVersionKey /LANG=0 "LegalTrademarks" 	"${APPNAME} ist eine Marke von ${COMPANYNAME} Datensysteme GmbH."
VIAddVersionKey /LANG=0 "LegalCopyright" 	"©2019-20 ${COMPANYNAME} Datensysteme GmbH."
VIAddVersionKey /LANG=0 "FileDescription" 	"${APPNAME} - ${DESCRIPTION}"
VIAddVersionKey /LANG=0 "FileVersion" 		"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}"
VIAddVersionKey /LANG=0 "ProductVersion" 	"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}"
VIFileVersion    ${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}.0
VIProductVersion ${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}.0

#------------------------------------------------------------------------------

RequestExecutionLevel admin ;Require admin rights on NT6+ (When UAC is turned on)
 
InstallDir "$PROGRAMFILES64\${APPNAME}"

# This will be in the installer/uninstaller's title bar
Name "${APPNAME}"
Icon "logo.ico"
outFile "${APPNAME}.Setup.exe"
 
# rtf or txt file - remember if it is txt, it must be in the DOS text format (\r\n)
# LicenseData "license.rtf"

# Just three pages - welcome, install location, and installation
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "German"


!macro VerifyUserIsAdmin
UserInfo::GetAccountType
pop $0
${If} $0 != "admin" ;Require admin rights on NT4+
        messageBox mb_iconstop "Es sind Administratorberechtigungen nötig!"
        setErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
        quit
${EndIf}
!macroend
 
#------------------------------------------------------------------------------
# Installer

function .onInit
	setShellVarContext all
	!insertmacro VerifyUserIsAdmin
functionEnd

section "install"
	# Files for the install directory - to build the installer, these should be 
	# in the same directory as the install script (this file)
	setOutPath $INSTDIR
	
	# alte Version von InstallMate entfernen
	SetRegView 64
	DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{BDABFABD-79B3-4354-8330-DB5A0B7C360E}"
	
	# install Visual C++ Redistributable
	File "deploy_files\vc_redist.x64.exe" 	
	ClearErrors
	ExecWait '"$INSTDIR\vc_redist.x64.exe" /passive /norestart'	
	SetRegView default

	# Files added here should be removed by the uninstaller (see section "uninstall")
	File /r "deploy_files\iconengines"
	File /r "deploy_files\imageformats"
	File /r "deploy_files\platforms"
	File /r "deploy_files\styles"
	File /r "deploy_files\translations"
 	File "deploy_files\D3Dcompiler_47.dll"
 	File "deploy_files\fernwartung.exe"
 	File "deploy_files\libEGL.dll"
 	File "deploy_files\libGLESV2.dll"
 	File "deploy_files\opengl32sw.dll"
    File "deploy_files\${APPNAME}.exe"
 	File "deploy_files\Qt5Core.dll"
 	File "deploy_files\Qt5Gui.dll"
 	File "deploy_files\Qt5Svg.dll"
 	File "deploy_files\Qt5Widgets.dll"
	File "logo.ico"

	# Uninstaller - See function un.onInit and section "uninstall" for configuration
	writeUninstaller "$INSTDIR\uninstall.exe"
 
	# Start Menu
	createDirectory "$SMPROGRAMS\${COMPANYNAME}"
        createShortCut "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk" "$INSTDIR\${APPNAME}.exe" "" "$INSTDIR\logo.ico"
	createShortCut "$SMPROGRAMS\${COMPANYNAME}\Fernwartung.lnk" "$INSTDIR\fernwartung.exe" "" "$INSTDIR\fernwartung.exe"
 
	# Registry information for add/remove programs
	# liegt bei mir unter: HKEY_LOCAL_MACHINE\SOFTWARE\WOW6432Node\Microsoft\Windows\CurrentVersion\Uninstall\KOPFWERK PowerSteri
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayName" "${APPNAME} - ${DESCRIPTION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayIcon" "$\"$INSTDIR\logo.ico$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "Publisher" "$\"${COMPANYNAME}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayVersion" "$\"${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}$\""
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "VersionMajor" ${VERSIONMAJOR}
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "VersionMinor" ${VERSIONMINOR}
	# There is no option for modifying or repairing the install
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoModify" 1
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoRepair" 1
	# Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "EstimatedSize" ${INSTALLSIZE}
sectionEnd
 
#------------------------------------------------------------------------------
# Uninstaller

function un.onInit
	SetShellVarContext all
 
	#Verify the uninstaller - last chance to back out
	MessageBox MB_OKCANCEL "Soll ${APPNAME} wirklich entfernt werden?" IDOK next
		Abort
	next:
	!insertmacro VerifyUserIsAdmin
functionEnd
 
section "uninstall"
	# Remove Start Menu launcher
	delete "$SMPROGRAMS\${COMPANYNAME}\${APPNAME}.lnk"
	delete "$SMPROGRAMS\${COMPANYNAME}\Fernwartung.lnk"
	# Try to remove the Start Menu folder - this will only happen if it is empty
	rmDir "$SMPROGRAMS\${COMPANYNAME}"
 
	# Remove files
	rmdir /r "$INSTDIR\iconengines"
	rmdir /r "$INSTDIR\imageformats"
	rmdir /r "$INSTDIR\platforms"
	rmdir /r "$INSTDIR\styles"
	rmdir /r "$INSTDIR\translations"
 	delete "$INSTDIR\D3Dcompiler_47.dll"
 	delete "$INSTDIR\fernwartung.exe"
 	delete "$INSTDIR\libEGL.dll"
 	delete "$INSTDIR\libGLESV2.dll"
 	delete "$INSTDIR\opengl32sw.dll"
    delete "$INSTDIR\${APPNAME}.exe"
 	delete "$INSTDIR\Qt5Core.dll"
 	delete "$INSTDIR\Qt5Gui.dll"
 	delete "$INSTDIR\Qt5Svg.dll"
 	delete "$INSTDIR\Qt5Widgets.dll"
 	delete "$INSTDIR\vc_redist.x64.exe"
 	delete "$INSTDIR\logo.ico"
 
	# Always delete uninstaller as the last action
	delete $INSTDIR\uninstall.exe
 
	# Try to remove the install directory - this will only happen if it is empty
	rmDir /r $INSTDIR
 
	# Remove uninstaller information from the registry
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
sectionEnd
