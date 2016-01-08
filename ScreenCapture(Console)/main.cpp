#include <stdio.h>
#include <windows.h>
#include <commdlg.h>

void  SaveCapturedBitmap(HBITMAP hBitmap);
void  CaptureScreen();

int main() 
{
   char ch;
   while(1)
   {
      printf("Do you want capture current screen? (y / n) :");
      scanf("%c", &ch);

      if(ch == 'Y' || ch == 'y')
         CaptureScreen();
      else if(ch == 'N' || ch == 'n')
         break;
      else
         printf("Invalid character!\n");
   }
   return 1;
}

void CaptureScreen()
{
   HWND consoleWnd = GetForegroundWindow();
   ShowWindow(consoleWnd, FALSE);
   Sleep(1000);

   char fileName[1024];
   strcpy(fileName, "D:\\Suresh\\And\\Projects\\ScreenCapture\\ScreenImage.bmp");

   SetCursor(LoadCursor(NULL,IDC_WAIT));

   int nScreenWidth  = GetSystemMetrics(SM_CXSCREEN);
   int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

   HWND hDesktopWnd = GetDesktopWindow();
   HDC hDesktopDC = GetDC(hDesktopWnd);

   // create memory device context
   HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
   // create bitmap object
   HBITMAP hCaptureBitmap =CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);

   // selects an object into the specified device context (DC). 
   SelectObject(hCaptureDC,hCaptureBitmap); 

   // performs a bit-block transfer of the color data corresponding to a rectangle of pixels 
   // from the specified source device context into a destination device context.
   BitBlt(
      hCaptureDC,    // destination device context.
      0,             // x - upper-left corner of the destination rectangle.
      0,             // y - upper-left corner of the destination rectangle.
      nScreenWidth,  // width of source and destination rectangles.
      nScreenHeight, // height of source and destination rectangles.
      hDesktopDC,    // source device context.
      0,             // x - upper-left corner of the source rectangle.
      0,             // y - upper-left corner of the source rectangle.
      SRCCOPY|CAPTUREBLT // A raster-operation code.
      ); 

   SaveCapturedBitmap(hCaptureBitmap);

   ReleaseDC(hDesktopWnd,hDesktopDC);

   DeleteDC(hCaptureDC);
   DeleteObject(hCaptureBitmap);

   SetCursor(LoadCursor(NULL,IDC_ARROW));

   ShowWindow(consoleWnd, TRUE);
}
void  SaveCapturedBitmap(HBITMAP hBitmap)
{
   char szFilename[MAX_PATH]; 
   strcpy(szFilename, "C:\\ScreenCapture.bmp");

   // Will contain exe path
   HMODULE hModule = GetModuleHandle(NULL);
   if (hModule != NULL)
   {
      // When passing NULL to GetModuleHandle, it returns handle of exe itself
      GetModuleFileNameA(hModule, szFilename, (sizeof(szFilename)));
      char* pos = strstr(szFilename, ".exe");
      if(pos)
         strcpy(pos,".bmp");
      else
         strcpy(szFilename, "C:\\ScreenCapture.bmp");
   }

   HDC            hdc=NULL;
   FILE*          fp=NULL;
   // 24 bit bmp file has 3 parts
   // 1. file header
   // 2. info header
   // 3. image data
   BITMAPFILEHEADER  bmpFileHeader;
   BITMAPINFO        bmpInfo;
   LPVOID            pBuf=NULL;

   hdc = GetDC(NULL);

   ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));

   bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

   // Get Bitmap info header
   GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

   if(bmpInfo.bmiHeader.biSizeImage <= 0)
      bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth * abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

   if((pBuf=malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
   {
      printf("Unable to Allocate Bitmap Memory\n");
      return;
   }

   bmpInfo.bmiHeader.biCompression = BI_RGB;
   GetDIBits(hdc, hBitmap, 0, bmpInfo.bmiHeader.biHeight, pBuf, &bmpInfo, DIB_RGB_COLORS);	

   // File header
   bmpFileHeader.bfReserved1=0;
   bmpFileHeader.bfReserved2=0;
   bmpFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)+ bmpInfo.bmiHeader.biSizeImage;
   bmpFileHeader.bfType='MB';
   bmpFileHeader.bfOffBits=sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

   if((fp=fopen(szFilename,"wb"))==NULL)
   {
      printf("Unable to Create Bitmap File\n");
      return;
   }

   fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
   fwrite(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
   fwrite(pBuf, bmpInfo.bmiHeader.biSizeImage, 1, fp);


   if(hdc)
      ReleaseDC(NULL,hdc);

   if(pBuf)
      free(pBuf);

   if(fp)
      fclose(fp);

   printf("Bitmap is saved at %s\n", szFilename);
   fflush(stdin);
}
