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

void CreateModalessDialog(HWND hwnd, int DlgID);
INT_PTR CALLBACK ModalessFindDialogProc(
   _In_ HWND   hwndDlg,
   _In_ UINT   uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
   );
INT_PTR CALLBACK ModalessGoToDialogProc(
   _In_ HWND   hwndDlg,
   _In_ UINT   uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
   );

// Holds all dialog that are created using CreateDialog
// To dispatch its messages
HWND* g_modalessdlg_list = NULL;
int g_modalessdlg_count = 0;
void AddModalessDialogToList(HWND* hDlg);

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
      // IsDialogMessage() does default processing of messages 
      int i ;
      for (i = 0 ; i < g_modalessdlg_count; i++)
      {
         if(IsDialogMessage(g_modalessdlg_list[i],&Msg))
            break;
      }
      if(i == g_modalessdlg_count)
      {
         TranslateMessage(&Msg);
         DispatchMessage(&Msg);
      }
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
      // call to create modaless dialog
      CreateModalessDialog(hwnd,IDD_FIND_DIALOG);
      break;
   case ID_EDIT_MYGOTO:
      // call to create modaless dialog
      CreateModalessDialog(hwnd,IDD_GOTO_DIALOG);
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


//
void CreateModalessDialog(HWND hwnd, int DlgID)
{
   if(DlgID == IDD_FIND_DIALOG)
   {
      static HWND FindDlg = NULL;
      if(FindDlg == NULL)
      {
         FindDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FIND_DIALOG), hwnd, ModalessFindDialogProc);
         if(FindDlg == NULL)
         {
            MessageBox(hwnd, L"Failed to Create Modaless Dialog", L"Info",
               MB_OK | MB_ICONINFORMATION);
            return;
         }
         AddModalessDialogToList(&FindDlg);
      }
      ShowWindow(FindDlg,SW_SHOW);
   }
   else if(DlgID == IDD_GOTO_DIALOG)
   {
      static HWND GoToDlg = NULL;
      if(GoToDlg == NULL)
      {
         GoToDlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GOTO_DIALOG), hwnd, ModalessGoToDialogProc);
         if(GoToDlg == NULL)
         {
            MessageBox(hwnd, L"Failed to Create Modaless Dialog", L"Info",
               MB_OK | MB_ICONINFORMATION);
            return;
         }
         AddModalessDialogToList(&GoToDlg);
      }
      ShowWindow(GoToDlg,SW_SHOW);
   }
}

// Specify Dialog Procedure for each modeless Dialog

// -  No need to call DefWindowProc() for message that are not handled, as it is done automatically.
// -  Return TRUE for messages that are processed, and FALSE that are not
// -  call DestroyWindow() not EndDialog()
INT_PTR CALLBACK ModalessFindDialogProc(
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
      case IDC_FIND_BUTTON:
         TCHAR sText[1024],buff[1024];
         GetDlgItemText(hwndDlg,IDC_FIND_TEXT,sText,1024);
         wsprintf(buff,L"Find button is clicked with search string '%s'",sText);
         MessageBox(hwndDlg, buff, L"Info",
            MB_ICONINFORMATION | MB_OK);
         break;
      case IDCANCEL:
         ShowWindow(hwndDlg,SW_HIDE);
         break;
      }
      break;
   default:
      return FALSE;
   }
   return TRUE;
}

//
INT_PTR CALLBACK ModalessGoToDialogProc(
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
      {
         switch(LOWORD(wParam))
         {
         case IDC_GOTO_GO_BTN:
            {
               TCHAR buff[1024];
               BOOL  bSuccess = FALSE;
               int iValue = 0;
               iValue = GetDlgItemInt(hwndDlg,IDC_GOTO_LINE_NUM_VALUE,&bSuccess,FALSE);
               if(bSuccess)
                  wsprintf(buff,L"Go button is clicked with valid Line Number '%d'",iValue);
               else
                  wsprintf(buff,L"Go button is clicked with invalid Line Number");

               MessageBox(hwndDlg, buff, L"Info",
                  MB_ICONINFORMATION | MB_OK);
            }
            break;
         case IDCANCEL:
            ShowWindow(hwndDlg,SW_HIDE);
            break;
         }
      }
      break;
   default:
      return FALSE;
   }
   return TRUE;
}
/////////////////////////////////////////////

// collect all modaless dialog to pass messages from windows when they are currently active.

void AddModalessDialogToList(HWND* hDlg)
{
   if(g_modalessdlg_count == 0)
   {
      g_modalessdlg_count++;
      g_modalessdlg_list = (HWND*) malloc(sizeof(HWND) * g_modalessdlg_count);
   }
   else
   {
      g_modalessdlg_count++;
      g_modalessdlg_list = (HWND*) realloc(g_modalessdlg_list, sizeof(HWND) * g_modalessdlg_count);
   }
}