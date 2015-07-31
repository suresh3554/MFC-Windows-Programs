#include <windows.h>

#define ID_FILE_MYNEW   100
#define ID_FILE_MYOPEN  101
#define ID_FILE_MYEXIT  102
#define ID_EDIT_MYFIND  103
#define ID_EDIT_MYGOTO  104
#define ID_HELP_MYABOUT 105

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void  AddMenu(HWND hwnd);
void  ProcessCommands(HWND hwnd, WPARAM wParam);

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
   wClass.lpszMenuName  = NULL;                             // menu resource
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
   case WM_CREATE:
      {
         // 9. Adding menus during window creation
         AddMenu(hwnd);
      }
      break;
   case WM_COMMAND:
      {
         // 10. Process the clicked menu item
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

//11. Create menus
void AddMenu(HWND hwnd)
{
   HMENU hMenu, hSubMenu;

   hMenu = CreateMenu();

   hSubMenu = CreatePopupMenu();
   AppendMenu(hSubMenu, MF_STRING, ID_FILE_MYNEW, L"&New");
   AppendMenu(hSubMenu, MF_STRING, ID_FILE_MYOPEN, L"&Open");
   AppendMenu(hSubMenu, MF_STRING, ID_FILE_MYEXIT, L"&Exit");
   AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&File");

   hSubMenu = CreatePopupMenu();
   AppendMenu(hSubMenu, MF_STRING, ID_EDIT_MYFIND, L"F&ind...");
   AppendMenu(hSubMenu, MF_STRING, ID_EDIT_MYGOTO, L"&Go to...");
   AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"E&dit");

   hSubMenu = CreatePopupMenu();
   AppendMenu(hSubMenu, MF_STRING, ID_HELP_MYABOUT, L"&About...");
   AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&Help");

   SetMenu(hwnd, hMenu);
}

//12. Process the clicked menu item. Messagebox is used to display
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
      MessageBox(hwnd, L"Help -> About is Clicked ", L"Info",
         MB_ICONINFORMATION | MB_OK);
      break;
   default:
      break;
   }
}