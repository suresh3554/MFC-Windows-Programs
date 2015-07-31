#include <windows.h>

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void  ProcessCommands(HWND hwnd, WPARAM wParam,LPARAM lParam);

void AddStaticTextControls(HWND hwnd);
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
      300,                    // window width
      300,                    // window height
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
         //AddMenu(hwnd);
         AddStaticTextControls(hwnd);
      }
      break;
   case WM_COMMAND:
      {
         // 9. Process the clicked menu item
         ProcessCommands(hwnd,wParam,lParam);
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
//The Notification Code is passed as the HIWORD of wParam

void ProcessCommands(HWND hwnd, WPARAM wParam,LPARAM lParam)
{
   TCHAR sText[1024],buff[1024];
   switch(LOWORD(wParam))
   {
   default:
      break;
   }
}

void AddStaticTextControls(HWND hwnd)
{
   HWND g_hwndStaticText1 = CreateWindowEx( 
      NULL,          // Extended Style
      L"static",   // Predefined class; Unicode assumed 
      L"This is Static Text",         // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,  // Styles 
      20,            // x position 
      20,           // y position 
      240,           // width
      200,            // height
      hwnd,          // Parent window
      (HMENU)IDC_STATICTEXT_1,       // No menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(g_hwndStaticText1 == NULL)
   {
      MessageBox(NULL, L"List Box Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }
}