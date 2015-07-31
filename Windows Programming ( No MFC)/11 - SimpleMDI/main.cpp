#include <windows.h>
#include <commctrl.h>
#include "resource.h"

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void  ProcessCommands(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void CreateToolBarControl(HWND hwnd);
void UpdateMenuItem(HWND hwnd, int nID, int bCheck);
void CreateStatusBarControl(HWND hwnd);

HWND g_hwndMDIClient = NULL;
void AddMDIClientControl(HWND hwnd);
HWND CreateNewMDIChild(HWND hMDIClient);

void DoFileOpen(HWND hWnd);
void DoFileSave(HWND hWnd);

LRESULT WINAPI MDIChildWndProc(
   _In_ HWND   hWnd,
   _In_ UINT   uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
   );
// Control Ids
#define IDC_STATICTEXT_1 1001

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
   wClass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);         // menu resource
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
      400,          // default horizontal position
      200,          // default vertical position 
      1000,                    // window width
      800,                    // window height
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
      if (!TranslateMDISysAccel(g_hwndMDIClient, &Msg))
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
   case WM_CREATE:
      {
         CreateToolBarControl(hwnd);
         CreateStatusBarControl(hwnd);
         AddMDIClientControl(hwnd);
      }
      break;
   case WM_COMMAND:
      {
         // 9. Process the clicked menu item
         ProcessCommands(hwnd,msg,wParam,lParam);
      }
      break;
   case WM_CLOSE:
      DestroyWindow(hwnd);
      break;
   case WM_DESTROY:
      PostQuitMessage(0);
      break;
   case WM_SIZE:
      {
         HWND hTool;
         RECT rcTool;
         int iToolHeight = 0;

         HWND hStatus;
         RECT rcStatus;
         int iStatusHeight = 0;

         HWND hEdit;
         int iEditHeight;
         RECT rcClient;

         // Size toolbar and get height

         hTool = GetDlgItem(hwnd, IDC_MAIN_TOOL);
         if(IsWindowVisible(hTool))
         {
            SendMessage(hTool, TB_AUTOSIZE, 0, 0);

            GetWindowRect(hTool, &rcTool);
            iToolHeight = rcTool.bottom - rcTool.top;
         }

         // Size status bar and get height

         hStatus = GetDlgItem(hwnd, IDC_MAIN_STATUS);
         if(IsWindowVisible(hStatus))
         {
            SendMessage(hStatus, WM_SIZE, 0, 0);

            GetWindowRect(hStatus, &rcStatus);
            iStatusHeight = rcStatus.bottom - rcStatus.top;
         }

         // Calculate remaining height and size edit

         GetClientRect(hwnd, &rcClient);

         iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;

         hEdit = GetDlgItem(hwnd, IDC_MAIN_MDI);
         SetWindowPos(hEdit, NULL, 0, iToolHeight, rcClient.right, iEditHeight, SWP_NOZORDER);

         return DefWindowProc(hwnd, msg, wParam, lParam);
      }
      // DefWindowProc is must
   default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
   }
   return 0;
}

//10. Process the clicked menu item. Messagebox is used to display
//The Notification Code is passed as the HIWORD of wParam

void ProcessCommands(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   switch(LOWORD(wParam))
   {
   case ID_FILE_MYNEW:
      CreateNewMDIChild(g_hwndMDIClient);
      break;
   case ID_FILE_MYOPEN:
      {
         HWND hChild = CreateNewMDIChild(g_hwndMDIClient);
         if(hChild)
            DoFileOpen(hChild);
      }
      break;

   case ID_FILE_MYSAVE:
      {
         HWND hChild = (HWND)SendMessage(g_hwndMDIClient, WM_MDIGETACTIVE,0,0);
         if(hChild)
            DoFileSave(hChild);
      }
      break;

   case ID_FILE_MYEXIT:
      PostMessage(hwnd, WM_CLOSE, 0, 0);
      break;

   case ID_VIEW_MYTB:
      {
         static int toolbarShown = 1;
         toolbarShown = toolbarShown ? 0 : 1;
         UpdateMenuItem(hwnd,ID_VIEW_MYTB,toolbarShown);
         HWND hToolBar = GetDlgItem(hwnd, IDC_MAIN_TOOL);
         ShowWindow(hToolBar,toolbarShown);
         SendMessage(hwnd, WM_SIZE, 0, 0);
      }
      break;

   case ID_VIEW_MYSB:
      {
         static int StatusbarShown = 1;
         StatusbarShown = StatusbarShown ? 0 : 1;
         UpdateMenuItem(hwnd,ID_VIEW_MYSB,StatusbarShown);
         HWND hStatusBar = GetDlgItem(hwnd, IDC_MAIN_STATUS);
         ShowWindow(hStatusBar,StatusbarShown);
         SendMessage(hwnd, WM_SIZE, 0, 0);
      }
      break;

   case ID_WIN_MYCAS:
      SendMessage(g_hwndMDIClient, WM_MDICASCADE, 0, 0);
      break;

   case ID_WIN_MYTILE:
      SendMessage(g_hwndMDIClient, WM_MDITILE, 0, 0);
      break;

   case ID_WIN_MYCLOSE:
      {
         HWND hChild = (HWND)SendMessage(g_hwndMDIClient, WM_MDIGETACTIVE,0,0);
         while(hChild)
         {
            SendMessage(hwnd, WM_MDINEXT, (WPARAM)hChild, 0);
            SendMessage(hChild, WM_CLOSE, 0, 0);
            hChild = (HWND)SendMessage(g_hwndMDIClient, WM_MDIGETACTIVE,0,0);
         }
      }
      break;
      // Handle MDI Window commands
   default:
      {
         if(LOWORD(wParam) > ID_WIN_MYCLOSE)
         {
            DefFrameProc(hwnd, g_hwndMDIClient, msg, wParam, lParam);
         }
         else 
         {
            HWND hChild = (HWND)SendMessage(g_hwndMDIClient, WM_MDIGETACTIVE,0,0);
            if(hChild)
            {
               SendMessage(hChild, WM_COMMAND, wParam, lParam);
            }
         }
      }
      break;
   }
}

void AddMDIClientControl(HWND hwnd)
{
   // Retrieve the handle to the window menu and assign the 
   // first child window identifier. 
   CLIENTCREATESTRUCT ccs; 
   ccs.hWindowMenu = GetSubMenu(GetMenu(hwnd), 2);
   ccs.idFirstChild = ID_WIN_MYCLOSE + 1; 

   // Create MDI Client Window
   g_hwndMDIClient = CreateWindowEx( 
      WS_EX_CLIENTEDGE,          // Extended Style
      L"MDIClient",   // Predefined class; Unicode assumed 
      L"MDI Client Window",          // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_CLIPCHILDREN | WS_MAXIMIZE ,  // Styles 
      CW_USEDEFAULT,            // x position 
      CW_USEDEFAULT,           // y position 
      500,           // width
      500,            // height
      hwnd,          // Parent window
      (HMENU)IDC_MAIN_MDI,       // No menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      (LPSTR) &ccs);      

   if(g_hwndMDIClient == NULL)
   {
      MessageBox(NULL, L"MDI Client  Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   // Registering one class for one Type of Document
   // MDI client window can have number of child windows.
   // Icon , No Menu and Extra space- cbWndExtra
   WNDCLASSEX wClass;
   ZeroMemory(&wClass,sizeof(WNDCLASSEX));
   wClass.cbSize        = sizeof(WNDCLASSEX);               // size of structure
   wClass.style         = 0;                                // class styles
   wClass.lpfnWndProc   = MDIChildWndProc;                         // points to window procedure
   wClass.cbClsExtra    = 0;                                // no extra class memory
   wClass.cbWndExtra    = 0;                                // no extra window memory 
   wClass.hInstance     = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);                        // handle to instance 
   wClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);  // predefined app. icon 
   wClass.hCursor       = LoadCursor(NULL, IDC_ARROW);      // predefined arrow 
   wClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);// white background brush 
   wClass.lpszMenuName  = NULL;       // menu resource
   wClass.lpszClassName = L"MDIChildWndClass";              // name of the class
   wClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);  // small class icon 

   //   Registering the Window Class
   if(!RegisterClassEx(&wClass))
   {
      MessageBox(NULL, L"Window Registration Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }
}

// Creates New MDI child Window
HWND CreateNewMDIChild(HWND hMDIClient)
{ 
   MDICREATESTRUCT mcs;
   HWND hChild;
   static int count = 1;

   TCHAR  sTitle[256];
   wsprintf(sTitle,L"MDI child window - %d",count);
   count++;
   mcs.szTitle = sTitle;
   mcs.szClass = L"MDIChildWndClass";
   mcs.hOwner  = GetModuleHandle(NULL);
   mcs.x = mcs.cx = CW_USEDEFAULT;
   mcs.y = mcs.cy = CW_USEDEFAULT;
   mcs.style = MDIS_ALLCHILDSTYLES;

   hChild = (HWND)SendMessage(hMDIClient, WM_MDICREATE, 0, (LONG)&mcs);
   if(!hChild)
   {
      MessageBox(hMDIClient, L"MDI child window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }
   return hChild;
}

// child window Procedure
LRESULT WINAPI MDIChildWndProc(
   _In_ HWND   hWnd,
   _In_ UINT   uMsg,
   _In_ WPARAM wParam,
   _In_ LPARAM lParam
   )
{
   switch(uMsg)
   {
   case WM_CREATE:
      {
         // Create Edit Control

         HWND g_hwndEdit = CreateWindowEx( 
            NULL,       // Extended Style
            L"EDIT",    // Predefined class; Unicode assumed 
            L"Its a edit box",      // text 
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_MAXIMIZE | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,  // Styles 
            0,         // x position 
            0,         // y position 
            100,        //  width
            100,         //  height
            hWnd,       // Parent window
            (HMENU)IDC_EDIT_1,       // No menu.
            (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), 
            NULL);      // Pointer not needed.

         if(g_hwndEdit == NULL)
         {
            MessageBox(NULL, L"Edit Window Creation Failed!", L"Error!",
               MB_ICONEXCLAMATION | MB_OK);
         }
      }
      break;
   case WM_COMMAND:
      {
         switch(LOWORD(wParam))
         {
         case IDC_EDIT_1:
            {
               switch(HIWORD(wParam))
               {
               default:
                  break;
               }
            }
            break;
         default:
            break;
         }
      }
      break;
   case WM_MDIDESTROY :
      break;
   case  WM_SYSCOMMAND :
      {
         switch(wParam & 0xFFF0)
         {
//          case SC_CLOSE:
//             return 0;
         default:
            return DefMDIChildProc(hWnd,uMsg,wParam,lParam);
         }
      }
   case WM_SIZE:
      {
         HWND hEdit;
         RECT rcClient;
         // Calculate remaining height and size edit
         GetClientRect(hWnd, &rcClient);
         hEdit = GetDlgItem(hWnd, IDC_EDIT_1);
         SetWindowPos(hEdit, NULL, 0, 0, rcClient.right, rcClient.bottom, SWP_NOZORDER);
         return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
      }

   default:
      return DefMDIChildProc(hWnd,uMsg,wParam,lParam);
   }
   return 0;
}

// Open file
void DoFileOpen(HWND hWnd)
{
   OPENFILENAME ofn;
   TCHAR szFileName[MAX_PATH] = L"";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
   ofn.hwndOwner = hWnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile = szFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   ofn.lpstrDefExt = L"txt";

   if(GetOpenFileName(&ofn))
   {
      HANDLE hFile;
      HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_1);

      hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
         OPEN_EXISTING, 0, NULL);
      if(hFile != INVALID_HANDLE_VALUE)
      {
         DWORD dwFileSize;

         dwFileSize = GetFileSize(hFile, NULL);
         if(dwFileSize != 0xFFFFFFFF)
         {
            char *pszFileText = NULL;
            pszFileText = new char[dwFileSize+1];
            if(pszFileText != NULL)
            {
               DWORD dwRead;

               if(ReadFile(hFile, pszFileText, dwFileSize, &dwRead, NULL))
               {
                  pszFileText[dwFileSize] = 0; // Add null terminator

                  // required size
                  int nChars = MultiByteToWideChar(CP_ACP, 0, pszFileText, -1, NULL, 0);
                  // allocate it
                  WCHAR *pwcsName;
                  pwcsName = new WCHAR[nChars];
                  MultiByteToWideChar(CP_ACP, 0, pszFileText, -1, (LPWSTR)pwcsName, nChars);
                  SetWindowText(hEdit, pwcsName);
                  SetWindowText(hWnd, szFileName);
                  delete [] pwcsName;
               }
               delete [] pszFileText;
            }
         }
         CloseHandle(hFile);
      }
   }
}

// Save file
void DoFileSave(HWND hWnd)
{
   OPENFILENAME ofn;
   TCHAR pszFileName[MAX_PATH] = L"";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
   ofn.hwndOwner = hWnd;
   ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
   ofn.lpstrFile = pszFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
   ofn.lpstrDefExt = L"txt";

   if(GetSaveFileName(&ofn))
   {
      HANDLE hFile;
      HWND hEdit = GetDlgItem(hWnd, IDC_EDIT_1);

      hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
         CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
      if(hFile != INVALID_HANDLE_VALUE)
      {
         DWORD dwTextLength;

         dwTextLength = GetWindowTextLength(hEdit);
         // No need to bother if there's no text.
         if(dwTextLength > 0)
         {
            DWORD dwBufferSize = dwTextLength + 1;

            WCHAR *pszText = NULL;
            pszText = new WCHAR[dwBufferSize];

            if(pszText != NULL)
            {
               if(GetWindowText(hEdit, pszText, dwBufferSize))
               {
                  DWORD dwWritten;

                  // required size
                  int nChars = WideCharToMultiByte(CP_ACP, 0, pszText, -1, NULL, 0, NULL, NULL);
                  // allocate it
                  char *pName;
                  pName = new char[nChars];
                  WideCharToMultiByte(CP_ACP, 0, pszText, -1, (LPSTR)pName, nChars, NULL, NULL);
                  WriteFile(hFile, pName, dwTextLength, &dwWritten, NULL);
                  delete [] pName;
               }
               delete [] pszText;
            }
         }
         CloseHandle(hFile);
      }
   }
}

// Create Toolbar
void CreateToolBarControl(HWND hwnd)
{
   HWND hTool = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,
      hwnd, (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);

   // Send the TB_BUTTONSTRUCTSIZE message, which is required for
   // backward compatibility.
   SendMessage(hTool, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

   TBBUTTON tbb[3];
   TBADDBITMAP tbab;

   tbab.hInst = HINST_COMMCTRL;
   tbab.nID = IDB_STD_SMALL_COLOR;
   SendMessage(hTool, TB_ADDBITMAP, 0, (LPARAM)&tbab);

   ZeroMemory(tbb, sizeof(tbb));
   tbb[0].iBitmap = STD_FILENEW;
   tbb[0].fsState = TBSTATE_ENABLED;
   tbb[0].fsStyle = TBSTYLE_BUTTON;
   tbb[0].idCommand = ID_FILE_MYNEW;

   tbb[1].iBitmap = STD_FILEOPEN;
   tbb[1].fsState = TBSTATE_ENABLED;
   tbb[1].fsStyle = TBSTYLE_BUTTON;
   tbb[1].idCommand = ID_FILE_MYOPEN;

   tbb[2].iBitmap = STD_FILESAVE;
   tbb[2].fsState = TBSTATE_ENABLED;
   tbb[2].fsStyle = TBSTYLE_BUTTON;
   tbb[2].idCommand = ID_FILE_MYSAVE;

   SendMessage(hTool, TB_ADDBUTTONS, sizeof(tbb)/sizeof(TBBUTTON), (LPARAM)&tbb);

   UpdateMenuItem(hwnd,ID_VIEW_MYTB,1);
}

// Create Status Bar
void CreateStatusBarControl(HWND hwnd)
{
   HWND hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,
      WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,
      hwnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

   int statwidths[] = {100, -1};

   SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
   SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)L"Hi there :)");

   UpdateMenuItem(hwnd,ID_VIEW_MYSB,1);
}

void UpdateMenuItem(HWND hwnd, int nID, int bCheck)
{
   HMENU menu = GetSubMenu(GetMenu(hwnd), 1);
   if(bCheck)
      CheckMenuItem(menu,nID,MF_BYCOMMAND|MF_CHECKED);
   else
      CheckMenuItem(menu,nID,MF_BYCOMMAND|MF_UNCHECKED);
}