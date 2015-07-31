This Project is created in VisualStudio 2010.

Notes:

1. WinMain - entry point.

2. Initialize the window class.
      - Fill WNDCLASSEX structure.
      - Give a unique class name.
      - Assign a Windows procedure function.

3. Register window class - register the class template with Windows.

4. Create the actual window using CreateWindowEx(). Pass the correct class name. 

5. Show the window using ShowWindow().

6. Update the client area of the window using UpdateWindow().

7. OS Windows sends messages to created window:
      - Get from message queue. GetMessage().
      - Decode the message using TranslateMessage().
      - Dispatch the message to Windows procedure function using DispatchMessage().

8. Windows procedure is a must.
      - Handle message from Windows.
      - If handling is not done, then always call DefWndProc() ,where handling is done.

9. Create MDI client Window which is a child of main window.

10.Use Class "MdiClient" in CreateWindowEx() to create a control.

11.Make MDI client window to occupy entire client area.

12.Create MDI child Windows which are bound to MDI client window.

13.For each type of MDI child Window. (for Ex: display images, text editor etc)

   - Initialize the window class.
      - Fill WNDCLASSEX structure.
      - Give a unique class name.
      - Assign a Windows procedure function.

   - Register window class - register the class template with Windows.
     
   - Windows procedure is a must.
      - Handle message from Windows.
      - If handling is not done, then always call DefMDIChildProc() ,where handling is done.

14.To create ToolBar and StatusBar include 
   - #include <commctrl.h>
   - add comctl32.lib to your linker setting

15.Use 'TOOLBARCLASSNAME' in CreateWindowEx() to create a ToolBar.

16.Use 'STATUSCLASSNAME' in CreateWindowEx() to create a Statusbar.
