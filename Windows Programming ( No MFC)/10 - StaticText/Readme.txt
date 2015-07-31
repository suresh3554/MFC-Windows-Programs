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

9. Create controls in WM_CREATE msg. Here "static" control is added.

10.There is predefined standard class "static" which is registered by the system.

11.Use CreateWindowEx() with "static" as class. specify its location and other parameters.

12.All static controls have unique id assigned to them.

13.Notification message from standard control to user is WM_COMMAND.

14.wParam 
      - Lo word is ID of the control
      - Hi word is notification code

15.lparam - contains the handle to the control
