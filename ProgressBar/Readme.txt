This project is created in Visual Studio 2010.

A progress bar is a window that an application can use to indicate the progress of a lengthy operation.
It consists of a rectangle that is animated as an operation progresses.
(source: https://msdn.microsoft.com/en-us/library/windows/desktop/bb760816(v=vs.85).aspx)

Two kinds of Progress:
1 - the progress of the task can be determined in some way that can be expressed in percentages.
2 - the progress of the task cannot be determined -  indeterminate progress bars

To create Indeterminate progress bar:
 - PBS_MARQUEE style need to be added to Progrss Control (CProgressCtrl)
 - call SetMarquee function.
 - To use this feature, specify Comctl32.dll version 6 in a manifest.
    add the following lines may be in stdafx.h
    
        #ifdef _UNICODE
        #if defined _M_IX86
        #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
        #elif defined _M_X64
        #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
        #else
        #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
        #endif
        #endif
        
To change progress bar color, we need to disable visual styles or else it doesn't reflect the color.
  -   Load library (may be dynamically) - UxTheme.dll
  -   Call SetWindowTheme (refer: https://msdn.microsoft.com/en-us/library/windows/desktop/bb759827(v=vs.85).aspx)
  -   Free Library (if loaded dynamically)
