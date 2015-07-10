Version Info of an .exe and .dll

The version number is stored in a VERSIONINFO resource (a compiled binary resource) inside the executable.

Version block in say … example.rc file :

VS_VERSION_INFO VERSIONINFO
 FILEVERSION 1,0,0,1				//four comma-separated numbers
 PRODUCTVERSION 1,0,0,1			//four comma-separated numbers
 FILEFLAGSMASK 0x3fL
#ifdef _DEBUG
 FILEFLAGS 0x1L
#else
 FILEFLAGS 0x0L
#endif
 FILEOS 0x4L
 FILETYPE 0x2L
 FILESUBTYPE 0x0L
BEGIN
    BLOCK "StringFileInfo"
    BEGIN
        BLOCK "040504b0"
        BEGIN
            VALUE "CompanyName", "TODO: <Company name>"
            VALUE "FileDescription", "TODO: <File description>"
            VALUE "FileVersion", "1.0.0.1"
            VALUE "InternalName", "example.exe"
            VALUE "LegalCopyright", "Copyright (C) 2015"
            VALUE "OriginalFilename", "example.exe"
            VALUE "ProductName", "TODO: <Product name>"
            VALUE "ProductVersion", "1.0.0.1"
        END
    END
    BLOCK "VarFileInfo"
    BEGIN
        VALUE "Translation", 0x405, 1200
    END
END


API’s Used:
1. GetFileVersionInfoSize - https://msdn.microsoft.com/en-us/library/windows/desktop/ms647005(v=vs.85).aspx 
2. GetFileVersionInfo - https://msdn.microsoft.com/en-us/library/windows/desktop/ms647003(v=vs.85).aspx 
3. VerQueryValue - https://msdn.microsoft.com/en-us/library/windows/desktop/ms647464(v=vs.85).aspx 
4. VerLanguageName - https://msdn.microsoft.com/en-us/library/windows/desktop/ms647463(v=vs.85).aspx 

References:
https://msdn.microsoft.com/en-us/library/aa381058 
