#include <windows.h>

LRESULT CALLBACK WindProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void  ProcessCommands(HWND hwnd, WPARAM wParam,LPARAM lParam);

void AddButtonControls(HWND hwnd);
// Button Ids
#define IDC_GROUPBUTTON_1        1001
#define IDC_DEFBUTTON_1          1002
#define IDC_CHECKBOXBUTTON_1     1003
#define IDC_CHECKBOXBUTTON_2     1004
#define IDC_RADIOBUTTON_1        1005
#define IDC_RADIOBUTTON_2        1006
#define IDC_RADIOBUTTON_3        1007
#define IDC_RADIOBUTTON_4        1008
#define IDC_RADIOBUTTON_5        1009
#define IDC_RADIOBUTTON_6        1010
#define IDC_RADIOBUTTON_7        1011
#define IDC_RADIOBUTTON_8        1012

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
      400,                    // window width
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
         AddButtonControls(hwnd);
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
   case IDC_DEFBUTTON_1:
      {
         //check the notification code
         switch(HIWORD(wParam))
         {
         case BN_CLICKED:
            {
               GetWindowText((HWND)lParam,sText,1024);
               wsprintf(buff,L"Button is clicked with caption '%s'",sText);
               MessageBox(hwnd,buff, L"Info",
                  MB_ICONINFORMATION | MB_OK);
            }
            break;
         }
      }
      break;
   case IDC_CHECKBOXBUTTON_1:
   case IDC_CHECKBOXBUTTON_2:
      {
         WORD ret = HIWORD(wParam);
         switch(ret)
         {
         case BN_CLICKED:
            {
               BOOL check = BST_UNCHECKED;
               check = SendMessage((HWND)lParam, BM_GETCHECK, 0, 0);
               if(check == BST_CHECKED)
                  wsprintf(buff,L"Button is Checked");
               else
                  wsprintf(buff,L"Button is Un-Checked");

               MessageBox(hwnd,buff, L"Info",
                  MB_ICONINFORMATION | MB_OK);
               break;
            }
            break;
         }
      }
      break;
   case IDC_RADIOBUTTON_1:
   case IDC_RADIOBUTTON_2:
   case IDC_RADIOBUTTON_3:
   case IDC_RADIOBUTTON_4:
   case IDC_RADIOBUTTON_5:
   case IDC_RADIOBUTTON_6:
   case IDC_RADIOBUTTON_7:
   case IDC_RADIOBUTTON_8:
      {
         GetWindowText((HWND)lParam,sText,1024);
         wsprintf(buff,L"Button with caption '%s' is selected",sText);
         MessageBox(hwnd,buff, L"Info",
            MB_ICONINFORMATION | MB_OK);
         break;
      }
   default:
      break;
   }
}

void AddButtonControls(HWND hwnd)
{
   HWND hwndgroupButton = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"Group Button",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_GROUPBOX,  // Styles 
      20,         // x position 
      10,         // y position 
      340,        //  width
      230,         //  height
      hwnd,       // Parent window
      (HMENU)IDC_GROUPBUTTON_1,       //  menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndgroupButton == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndDefButton = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"Push Button",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
      40,         // x position 
      40,         // y position 
      110,        //  width
      20,         //  height
      hwnd,       // Parent window
      (HMENU)IDC_DEFBUTTON_1,       //  menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndDefButton == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndCheckButton1 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"Check Box 1",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
      40,         // x position 
      70,         // y position 
      120,        //  width
      30,         //  height
      hwnd,       // Parent window
      (HMENU)IDC_CHECKBOXBUTTON_1,   //  menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndCheckButton1 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndCheckButton2 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"Check Box 2",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
      180,         // x position 
      70,         // y position 
      120,        //  width
      30,         //  height
      hwnd,       // Parent window
      (HMENU)IDC_CHECKBOXBUTTON_2,   //  menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndCheckButton2 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton1 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_1",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,  // Styles 
      40,110,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_1,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton1 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton2 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_1",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Styles 
      110,110,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_2,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton2 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton3 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_1",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Styles 
      180,110,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_3,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton3 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton4 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_1",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Styles 
      250,110,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_4,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton4 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton5 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_2",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON | WS_GROUP,  // Styles 
      40,140,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_5,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton5 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton6 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_2",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Styles 
      110,140,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_6,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton6 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton7 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_2",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Styles 
      180,140,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_7,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton7 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }

   HWND hwndRadioButton8 = CreateWindowEx( 
      NULL,       // Extended Style
      L"button",  // Predefined class; Unicode assumed 
      L"set_2",      // text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,  // Styles 
      250,140,60,20,
      hwnd,       // Parent window
      (HMENU)IDC_RADIOBUTTON_8,       // menu.
      (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.

   if(hwndRadioButton8 == NULL)
   {
      MessageBox(NULL, L"Button Window Creation Failed!", L"Error!",
         MB_ICONEXCLAMATION | MB_OK);
   }
}