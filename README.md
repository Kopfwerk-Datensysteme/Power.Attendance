# Power.Attendance

## Development Setup
1. Install Visual Studio Community 2019 using this [link](https://visualstudio.microsoft.com/de/vs/older-downloads/)
2. Download the Qt installer using this [website](https://www.qt.io/download)
3. Install Qt Creator and Qt 5.15.2 MSVC2019 64bit using the Qt Installer
4. Open the **Power.Attendance** project file located under **./Power.Attendance.pro** in Qt Creator to start development

## Installer Building
1. Install NSIS using this [link](https://nsis.sourceforge.io/Download)
2. Set the new product version in **./Installer/Power.Attendance.nsi** (VERSIONMAJOR, VERSIONMINOR, VERSIONBUILD)
3. Then change to the **./Installer/** directory and run **./build_installer.bat**
4. The installer is now located in the same directory and is called **Power.Attendance.Setup.exe**

## Features
1. On initial startup the program checks if there is an sqlite database file present in path **C:\Users\$USER\AppData\Roaming\KOPFWERK\Power.Attendance\database.sqlite**, otherwise the program creates one with the schema defined in **SetupSchemaIfNecessary()**
2. Please keep in mind that this file is not on a network share and only lying on the local computer
3. The program also uses the local Windows Biometric Service (WBS) database located under **C:\Windows\System32\WinBioDatabase** to store fingerprint samples, it is also not lying on a network share, to delete entries the corresponding WBS must be stopped
4. Whenever a user is created/changed/deleted, the program updates the local WBS database automatically
5. By using the WBS, all fingerprint sensors that have a Windows Biometric Framework driver are supported
6. Keep in mind that fingerprint samples can only be identified with the same reader that has been used to create the samples
7. To change users, an admin password must be provided which is "1234" by default, it can be changed under "Admin -> Passwort ändern ..."
8. Under "Anwesenheit -> Kontrollieren ..." the registered timestamps of users can be searched by name, matriculation number, from date and to date, the dates can be left open by disabling the respective checkboxes
9. The attendance can be exported in a .csv file for external processing with the button "Export"
10. On the main screen, a user can click "Zeitstempel hinzufügen", put the registered finger on the fingerprint sensor and when it was identified successfully a timestamp will be added to the database for this user
11. If the finger was not identified for a registered user, please try again
12. Under "Benutzer -> Ändern ..." an admin password request dialog will pop up and after entering the correct password, the user table will be shown
13. The user table shows the user ID, the user name and the user matriculation number, the table can be searched by name or matriculation number and can also be exported by clicking the button "Export"
14. A user can be created, deleted or changed
15. On creation the user needs to provide the fingerprint, the name and the matriculation number
16. The fingerprint, matriculation number and name of a user can be changed
17. Whenever a user is deleted, there is a question if you really want to delete this user and after confirming the user is deleted from the local program database and the WBS database
18. The WBS only allows to store fingerprints for the currently logged in user, therefore the users in this application are differentiated by the subfactor argument of the WBS function calls, which normally should differ between the individual fingers of a single user but in this application's case it is used to differ between the program's actual users and some fingers
19. As the subfactor argument is only a byte big and 0 and 255 are reserved values, this program can at most support 254 concurrent users
20. The actual supported maximum user count might be much smaller as some fingerprint sensor drivers only support at most 10 concurrent fingerprints