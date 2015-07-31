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

Use of .rc and resources.h

9. Menu resource are written usually in .rc file and compiled and linked to .exe.
   Menu command identifiers are written out in resource.h.
   Then menu is attached to the window by assigning it to 'lpszMenuName' in WNDCLASSEX structure.   

10.When a menu item is clicked, WM_COMMAND is sent. 

11. Dialogs are just windows with following difference:
    - System does some additional default processing for dialogs, such as creating and initialising controls, and handling tab order.

12. Create a Dialog resource.

13. Click on File -> About menu item. Modal is posted which is created using DialogBox() function.
     - DialogBox() has its own messageloop.
     - Doesn't return until Dialog is closed.

14. Specify Dialog Procedure. 
     -  No need to call DefWindowProc() for message that are not handled, as it is done automatically.
     -  Return TRUE for messages that are processed, and FALSE that are not
     -  EndDialog() called to close the dialog, not DestroyWindow().
     -  WM_INITDIALOG message is sent after all controls are created
