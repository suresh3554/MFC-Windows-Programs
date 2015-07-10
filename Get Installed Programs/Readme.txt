Registry Location for Installed Programs

Uninstall Registry Key:

1. HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\[ProductName-GUID]

2. For 32-bit applications installed on 64-bit Operating System
   HKEY_LOCAL_MACHINE\Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\[ProductName-GUID]
   
3. For Programs installed via Windows Installer file (.msi File) :
   a. ‘WindowsInstaller’ value is set to 1 for HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Uninstall\[ProductName-GUID].
   b. Convert Product GUID to Windows Installer ID
   c. HKLM\Software\Classes\Installer\Products\<Installer ID>

4. Programs installed for specific users only : For all users in HKEY_USERS
   a. HKEY_USERS\USER-SID-HERE\Software\Microsoft\Windows\CurrentVersion\Uninstall
   b. For windows installer programs : HKCU\Software\Microsoft\Installer\Products


Product GUID to Windows Installer ID Conversion:

1. GUIDs are stored as 128-bit values, and are displayed as 32 hexadecimal digits with groups separated by hyphens.
2. Format GUID is {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX} where X is a hex digit (0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F).
3. Converting to Installer ID.
   a. Split GUID into 2 halves. 
      i.   Ex: GUID:{0003C1E0-E0E7-49BB-A0F6-4AE6D2B09202}
      ii.  1st half - “0003C1E0-E0E7-49BB”
      iii. 2nd half - “A0F6-4AE6D2B09202”
   b. 1st half - Reverse each section  - “0E1C3000-7E0E-BB94”
   c. 2nd half - Reverse every 2  - “0A6F-A46E2D0B2902”



Installing applications in windows:
3 ways : 

1. A Windows Installer installation in an MSI file.
2. An EXE file that bootstraps an MSI file (that may be embedded inside the EXE file).
3. A custom, third-party installation system in an EXE file.

MSI file :

1. MSI files are database files, used by Windows Installer. 
2. Contains information about an application which is divided into features and components, and every component may hold files, registry data, shortcuts etc. 
3. Contains the UI that is to be used for installing, and various other data such as any prerequisites to look for, custom actions to execute, the order of the installation procedure, whether to support Administrative installations, etc. 
4. Contains the actual files to be installed themselves (this isn't always the case though, the files can also be in an external CAB file or just as plain uncompressed files in a location where MSI can find them).
5. Installations that use MSI still come with an EXE (e.g. SETUP.EXE). This EXE is a so-called 'bootstrapper'. It doesn't perform the installation, it simply checks if the correct version of Windows Installer is present on the system, if not it launched the MSI Redistributable (MsiInstA.exe or MsiInstW.exe depending on the platform) and then launches MSIEXEC.EXE on the MSI file. In certain cases (especially Internet downloads), the MSI file and MSI redistributable are packed inside that EXE file, so you don't see they're there.


Algorithm:

1. For all subkeys (product GUID’s) in HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall 
   Do the following:
    a. if the ‘SystemComponent’ value exists and is set to 1 ,then go to the next subkey.
    b. if the ‘WindowsInstaller’ value exists and is set to 1, then
        i.   Convert subkey to Windows installer ID.
        ii.  Get key ‘HKLM\Software\Classes\Installer\Products\<ID>’
        iii. if the ‘ProductName’ value exists, then add this name to our list of installed programs.
	     else:
        i.   if the ‘UninstallString’ value doesn’t exist , then go to next subkey
        ii.  if ‘ReleaseType’ value is ‘Security Update’ or ‘Hotfix’ or ‘Update Rollup’, then go to next subkey
        iii. if ‘ParentKeyName’ value exists, then go to next subkey
        iv.  if the ‘DisplayName’ value exists, then add this name to our list of installed programs.
        
2. Repeat step 1. for all subkeys in HKLM\Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall.

3. For all users in HKEY_USERS, do the following:
   Repeat step 1. for all subkeys in ‘HKU\USER-SID-HERE\Software\Microsoft\Windows\CurrentVersion\Uninstall’

4. For all users in HKEY_USERS, do the following:
   a. Get all subkeys for key ‘HKU\USER-SID-HERE\Software\Microsoft\Installer\Products’
   b. Get all subkeys for key ‘HKLM\Software\Microsoft\Windows\CurrentVersion\Installer\UserData\USER-SID-HERE\Products’
   c. If a key exists in both places, then:
      i.  if the ‘SystemComponent’ value in the ‘InstallProperties’ subkey is set to 1 ,then go to the next subkey.
      ii. if the ‘ProductName’ value exists, then add this name to our list of installed programs.

5. Do the following:
   a. Get all subkeys for key ‘HKLM\Software\Classes\Installer\Products’
   b. Get all subkeys for key ‘HKLM\Software\Microsoft\Windows\CurrentVersion\Installer\UserData\S-1-5-18\Products’
      i.  if a key exists in both places, then:
      ii. if the ‘SystemComponent’ value is set to 1 ,then go to the next subkey.
      iii.if the ‘WindowsInstaller’ value exists and is not set to 1, then go to next subkey.
      iv. if the ‘ProductName’ value exists, then add this name to our list of installed programs.


Notes:

1. If ‘SystemComponent’ is set to 1, then corresponding application is not displayed in the Programs List of the Add/Remove Programs in the Control Panel.
2. ‘ParentKeyName’ is set with name of parent program, if the program is an update of a parent program, specify the parent program subkey name. If "OperatingSystem", it's an update for Windows.
3. The subkey must not have a name that starts with KB and is followed by 6 numbers, e.g KB879032. If it has this name format then it will be classed as a Windows Update and will be added to the list of programs that only appear when you click Show Installed Updates. 
4. If ReleaseType is set to any of the following: Security Update, Update Rollup, Hotfix, it indicates that it is an update rather than a full program.

API’s Used:

1. RegOpenKeyEx - https://msdn.microsoft.com/en-us/library/windows/desktop/ms724897(v=vs.85).aspx 
2. RegCloseKey - https://msdn.microsoft.com/en-us/library/windows/desktop/ms724837(v=vs.85).aspx 
3. RegQueryInfoKey - https://msdn.microsoft.com/en-us/library/windows/desktop/ms724902(v=vs.85).aspx
4. RegEnumKeyEx - https://msdn.microsoft.com/en-us/library/windows/desktop/ms724862(v=vs.85).aspx 
5. RegQueryValueEx - https://msdn.microsoft.com/en-us/library/windows/desktop/ms724911(v=vs.85).aspx 
5. ExtractIcon - https://msdn.microsoft.com/en-us/library/windows/desktop/ms648068(v=vs.85).aspx 

References:

http://community.spiceworks.com/how_to/2238-how-add-remove-programs-works 
http://forum.sysinternals.com/finding-all-installed-programs-from-the-registry_topic21312.html 
http://www.symantec.com/connect/articles/understanding-difference-between-exe-and-msi 
