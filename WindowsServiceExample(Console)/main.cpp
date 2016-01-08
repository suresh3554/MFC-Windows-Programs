#include <windows.h>
#include <stdio.h>
#include <wtsapi32.h>
#include <winbase.h>

#define SLEEP_TIME 5000

char  sLogFilePath[1024];
SERVICE_STATUS ServiceStatus; 
SERVICE_STATUS_HANDLE hStatus; 

void  ServiceMain(int argc, char** argv); 
void  ControlHandler(DWORD request); 
int InitService();
DWORD WINAPI ServiceWorkerThread (LPVOID lpParam);

// Divide to get MB
#define DIV  1048576 //1024*1024

// Specify the width of the field in which to print the numbers. 
// The asterisk in the format specifier "%*I64d" takes an integer 
// argument and uses it to pad and right justify the number.
#define WIDTH 7

void GetLogFile(char* exePath)
{
   strcpy(sLogFilePath, "C:\\MemoryStatus.txt");
   char *pos = strrchr(exePath, '\\');
   if(pos)
   {
      pos++;
      pos[0] = '\0';
      strcpy(sLogFilePath,exePath);
      strcat(sLogFilePath,"MemoryStatus.txt");
   }
}

int WriteToLogFile(char* str)
{
   FILE* log;
   log = fopen(sLogFilePath, "a+");
   if (log == NULL)
      return -1;
   fprintf(log, "%s\n", str);
   fclose(log);
   return 0;
}

int main(int argc, char** argv)
{ 
   SERVICE_TABLE_ENTRY ServiceTable[2];

   GetLogFile(argv[0]);

   ServiceTable[0].lpServiceName = L"MemoryStatus";
   ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;

   ServiceTable[1].lpServiceName = NULL;
   ServiceTable[1].lpServiceProc = NULL;
   // Start the control dispatcher thread for our service
   StartServiceCtrlDispatcher(ServiceTable); 
   return 1;
}


void ServiceMain(int argc, char** argv) 
{ 
   int error; 

   ServiceStatus.dwServiceType        = SERVICE_WIN32; 
   ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
   ServiceStatus.dwControlsAccepted   = 0;
   ServiceStatus.dwWin32ExitCode      = 0; 
   ServiceStatus.dwServiceSpecificExitCode = 0; 
   ServiceStatus.dwCheckPoint         = 0; 
   ServiceStatus.dwWaitHint           = 0; 

   hStatus = RegisterServiceCtrlHandler(L"MemoryStatus", (LPHANDLER_FUNCTION)ControlHandler); 
   if (hStatus == (SERVICE_STATUS_HANDLE)0) 
   { 
      // Registering Control Handler failed
      return; 
   }  
   // Initialize Service 
   error = InitService(); 
   if (error) 
   {
      // Initialization failed
      ServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
      ServiceStatus.dwWin32ExitCode      = -1; 
      SetServiceStatus(hStatus, &ServiceStatus); 
      return; 
   } 
   // We report the running status to SCM. 
   ServiceStatus.dwCurrentState = SERVICE_RUNNING;
   ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
   SetServiceStatus (hStatus, &ServiceStatus);

   // Start a thread that will perform the main task of the service
   HANDLE hThread = CreateThread (NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

   // Wait until our worker thread exits signaling that the service needs to stop
   WaitForSingleObject (hThread, INFINITE);

   return; 
}

// Service initialization
int InitService() 
{ 
   int result;
   result = WriteToLogFile("Monitoring started.........");
   return(result); 
} 

// Control handler function
void ControlHandler(DWORD request) 
{ 
   switch(request) 
   { 
   case SERVICE_CONTROL_STOP: 
      WriteToLogFile("Monitoring stopped..............");

      ServiceStatus.dwWin32ExitCode = 0; 
      ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
      SetServiceStatus (hStatus, &ServiceStatus);
      return; 

   case SERVICE_CONTROL_SHUTDOWN: 
      WriteToLogFile("Monitoring stopped.");

      ServiceStatus.dwWin32ExitCode = 0; 
      ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
      SetServiceStatus (hStatus, &ServiceStatus);
      return; 

   default:
      break;
   } 

   // Report current status
   SetServiceStatus (hStatus,  &ServiceStatus);

   return; 
} 

DWORD WINAPI ServiceWorkerThread (LPVOID lpParam)
{
   MEMORYSTATUSEX memory;
   // The worker loop of a service
   while (ServiceStatus.dwCurrentState == SERVICE_RUNNING)
   {
      memory.dwLength = sizeof (memory);

      GlobalMemoryStatusEx (&memory);

      char buffer[1024];

      //      sprintf (buffer, "There is  %*ld percent of memory in use.\n", WIDTH, memory.dwMemoryLoad);
      //      sprintf (buffer, "There are %*I64d total KB of physical memory.\n", WIDTH, memory.ullTotalPhys/DIV);
      sprintf (buffer, "There are %*I64d free MB of physical memory.\n", WIDTH, memory.ullAvailPhys/DIV);
      //       sprintf (buffer, "There are %*I64d total KB of paging file.\n", WIDTH, memory.ullTotalPageFile/DIV);
      //       sprintf (buffer, "There are %*I64d free  KB of paging file.\n", WIDTH, memory.ullAvailPageFile/DIV);
      //       sprintf (buffer, "There are %*I64d total KB of virtual memory.\n", WIDTH, memory.ullTotalVirtual/DIV);
      //       sprintf (buffer, "There are %*I64d free  KB of virtual memory.\n", WIDTH, memory.ullAvailVirtual/DIV);
      int result = WriteToLogFile(buffer);
      if(memory.ullAvailPhys/DIV < 6000)
      {
         typedef BOOL (*TypeSendMessage) ( HANDLE,DWORD,LPWSTR,DWORD, LPWSTR, DWORD, DWORD,DWORD, DWORD*, BOOL);

         TypeSendMessage SendMessage;
         HMODULE hModule = LoadLibrary(L"Wtsapi32.dll");

         if(hModule)
         {
            ////////////////// WTSSendMessage ////////////////
            SendMessage = (TypeSendMessage) GetProcAddress(hModule,
               "WTSSendMessageW");
            if(!SendMessage) 
               return FALSE;

            DWORD res;
            WCHAR title[256];
            wcscpy(title, L"Low memory alert");
            DWORD  titleLength = (wcslen(title) + 1) * sizeof(wchar_t);

            WCHAR msg[1024];
            wcscpy(msg, L"Available physical memory is less");
            DWORD  msgLength = (wcslen(msg) + 1) * sizeof(wchar_t);

            SendMessage( WTS_CURRENT_SERVER_HANDLE,
               WTSGetActiveConsoleSessionId(), title, titleLength,
               msg, msgLength, MB_ICONERROR| MB_TOPMOST|MB_SETFOREGROUND,
               FALSE,    &res,  0);
         }
      }
      if (result)
      {
         ServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
         ServiceStatus.dwWin32ExitCode      = -1; 
         SetServiceStatus(hStatus, &ServiceStatus);
         return ERROR_SUCCESS;
      }

      Sleep(SLEEP_TIME);
   }
   return ERROR_SUCCESS;
}
