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

