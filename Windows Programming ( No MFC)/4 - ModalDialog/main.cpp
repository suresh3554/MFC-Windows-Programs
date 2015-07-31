#include <windows.h>
#include "resource.h"

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void  ProcessCommands(HWND hwnd, WPARAM wParam);
void  CreateModalDialog(HWND hwnd);
INT_PTR CALLBACK ModalAboutDialogProc(
   _In_ HWND   hwndDlg,
   _In_ UINT   uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
   );


// 1. WinMain - entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
   WNDCLASSEX wClass;
   HWND hwnd;

   // 2: Specify what window is required To OS
   ZeroMemory(&wClass,sizeof(WNDCLASSEX));
   wClass.cbSize        = sizeof(WNDCLASSEX);               // size of structure
   wClass.style         = 0;                                // class styles
   wClass.lpfnWndProc   = WindProc;                         // points to window procedure
   wClass.cbClsExtra    = 0;                                // no extra class memory
   wClass.cbWndExtra    = 0;                                // no extra window memory 
   wClass.hInstance     = hInstance;                        // handle to instance 
   wClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);  // predefined app. icon 
   wClass.hCursor       = LoadCursor(NULL, IDC_ARROW);      // predefined arrow 
   wClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);// white background brush 
   wClass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);       // menu resource
   wClass.lpszClassName = L"FirstWindowClass";              // name of the class
   wClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);  // small class icon 

   // 3. Register window class - register the class template with Windows
   if(!RegisterClassEx(&wClass))
   {
      MessageBox(NULL, L"Window Registration Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   // 4: Create the actual window using CreateWindowEx(). Pass the correct class name .
   hwnd = CreateWindowEx(
      NULL,                   // Extended Style
      L"FirstWindowClass",    // name of window class
      L"Title of my window",  // title-bar string 
      WS_OVERLAPPEDWINDOW ,    // top-level window
      CW_USEDEFAULT,          // default horizontal position
      CW_USEDEFAULT,          // default vertical position 
      500,                    // window width
      700,                    // window height
      NULL,                   // no owner window 
      NULL,                   // use class menu
      hInstance,              // handle to application instance
      NULL                    // no window-creation data
      );

   if(hwnd == NULL)
   {
      MessageBox(NULL, L"Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
      return 0;
   }

   // 5. Show the window 
   ShowWindow(hwnd, nCmdShow);

   // 6. Update the client area by sending a WM_PAINT message 
   //to the window procedure bypassing message queue
   UpdateWindow(hwnd);

   // 7. Listen to messages sent by OS continuously
   MSG Msg;
   ZeroMemory(&Msg,sizeof(MSG));
   while(GetMessage(&Msg, NULL, 0, 0) > 0)
   {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
   return Msg.wParam;
}

// 8: The Window Procedure
LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(msg)
   {
   case WM_COMMAND:
      {
         // 9. Process the clicked menu item
         ProcessCommands(hwnd,wParam);
      }
      break;
   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;

      // DefWindowProc is must
   default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return 0;
}

//10. Process the clicked menu item. Messagebox is used to display
void ProcessCommands(HWND hwnd, WPARAM wParam)
{
   switch(LOWORD(wParam))
   {

   case ID_FILE_MYNEW:
      MessageBox(hwnd, L"File -> New is Clicked ", L"Info",
         MB_ICONINFORMATION | MB_OK);
      break;
   case ID_FILE_MYOPEN:
      MessageBox(hwnd, L"File -> Open is Clicked ", L"Info",
         MB_ICONINFORMATION | MB_OK);
      break;
   case ID_FILE_MYEXIT:
      MessageBox(hwnd, L"File -> Exit is Clicked ", L"Info",
         MB_ICONINFORMATION | MB_OK);
      break;
   case ID_EDIT_MYFIND:
      MessageBox(hwnd, L"Edit -> Find is Clicked ", L"Info",
         MB_ICONINFORMATION | MB_OK);
      break;
   case ID_EDIT_MYGOTO:
      MessageBox(hwnd, L"Edit -> GoTo is Clicked ", L"Info",
         MB_ICONINFORMATION | MB_OK);
      break;
   case ID_HELP_MYABOUT:
      // call to create modal dialog
      CreateModalDialog(hwnd);
      break;
   default:
      break;
   }
}

// Modal Dialog is created 
void CreateModalDialog(HWND hwnd)
{
   int ret = DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUT_DIALOG), hwnd, ModalAboutDialogProc);
   if(ret == IDOK)
   {
      MessageBox(hwnd, L"Dialog exited with IDOK.", L"Info",
         MB_OK | MB_ICONINFORMATION);
   }
   else if(ret == IDCANCEL)
   {
      MessageBox(hwnd, L"Dialog exited with IDCANCEL.", L"Info",
         MB_OK | MB_ICONINFORMATION);
   }
   else if(ret == -1){
      MessageBox(hwnd, L"Dialog failed!", L"Error",
         MB_OK | MB_ICONINFORMATION);
   }
}

// Specify Dialog Procedure. 
// -  No need to call DefWindowProc() for message that are not handled, as it is done automatically.
// -  Return TRUE for messages that are processed, and FALSE that are not
// -  EndDialog() called to close the dialog, not DestroyWindow().
INT_PTR CALLBACK ModalAboutDialogProc(
   _In_ HWND   hwndDlg,
   _In_ UINT   uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
   )
{
   switch(uMsg)
   {
   case WM_INITDIALOG:
      return TRUE;
   case WM_COMMAND:
      switch(LOWORD(wParam))
      {
      case IDOK:
         EndDialog(hwndDlg, IDOK);
         break;
      case IDCANCEL:
         EndDialog(hwndDlg, IDCANCEL);
         break;
      }
      break;
   default:
      return FALSE;
   }
   return TRUE;
}