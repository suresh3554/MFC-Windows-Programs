
// ScreenCaptureMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ScreenCapture.h"
#include "ScreenCaptureDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
   CAboutDlg();

   // Dialog Data
   enum { IDD = IDD_ABOUTBOX };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

   // Implementation
protected:
   DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
CScreenCaptureDlg::CScreenCaptureDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(CScreenCaptureDlg::IDD, pParent)
{
   m_hCaptureDC = NULL;
   m_hCaptureBitmap = NULL;
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
//////////////////////////////////////////////////////////////////////////
CScreenCaptureDlg::~CScreenCaptureDlg()
{
   if(m_hCaptureBitmap)
      DeleteObject(m_hCaptureBitmap);
   if(m_hCaptureDC)
      DeleteDC(m_hCaptureDC);
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_HORTZ_SCROLL, m_hbar);
   DDX_Control(pDX, IDC_VERT_SCROLL, m_vbar);
   DDX_Control(pDX, IDC_SCREEN_IMAGE_FRAME, m_FrameWnd);
   DDX_Control(pDX, IDC_ZOOM_SLIDER, m_cZoomCtrl);
   DDX_Control(pDX, IDC_ZOOM_VALUE, m_cZoomTextCtrl);
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CScreenCaptureDlg, CDialogEx)
   ON_BN_CLICKED(IDC_CAPTURE_BTN, OnBnClickedCaptureBtn)
   ON_BN_CLICKED(IDC_SAVE_BTN, OnBnClickedSaveBtn)
   ON_WM_HSCROLL()
   ON_WM_VSCROLL()
   ON_WM_SIZING()
   ON_WM_SIZE()
END_MESSAGE_MAP()
//////////////////////////////////////////////////////////////////////////
BOOL CScreenCaptureDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   GetWindowRect(&m_WindowRect);
   ScreenToClient(&m_WindowRect);

   m_vbar.ShowWindow(false);
   m_hbar.ShowWindow(false);

   m_FrameWnd.GetClientRect(&m_FrameRect);

   m_yScrollPageSize = 1;
   m_xScrollPageSize = 1;

   m_xCurrentScroll = 0; 
   m_yCurrentScroll = 0; 

   SCROLLINFO scrollInfo;
   scrollInfo.cbSize = sizeof(SCROLLINFO);
   scrollInfo.fMask = SIF_ALL; 
   scrollInfo.nMin = 0;
   scrollInfo.nMax = 99;
   scrollInfo.nPage = m_yScrollPageSize;
   scrollInfo.nPos = 0;
   scrollInfo.nTrackPos = 0;
   m_vbar.SetScrollInfo(&scrollInfo);
   m_hbar.SetScrollInfo(&scrollInfo);
   GetDlgItem(IDC_SAVE_BTN)->EnableWindow(FALSE);

   m_cZoomCtrl.SetRange(50, 300);
   m_cZoomCtrl.SetPos(100);
   m_cZoomCtrl.SetTicFreq(25);
   m_cZoomCtrl.SetLineSize(25);
   m_cZoomCtrl.SetPageSize(25);
   m_cZoomCtrl.SetBuddy(&m_cZoomTextCtrl, FALSE);
   m_ZoomPercent = 1.;

   return TRUE;  // return TRUE  unless you set the focus to a control
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::OnBnClickedCaptureBtn()
{
   // TODO: Add your control notification handler code here
   UpdateData(FALSE);
   CaptureScreen();
   DisplayImage();
   GetDlgItem(IDC_SAVE_BTN)->EnableWindow();
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::OnBnClickedSaveBtn()
{
   // TODO: Add your control notification handler code here
   if(m_hCaptureBitmap)
      SaveCapturedBitmap(m_hCaptureBitmap);
}
//////////////////////////////////////////////////////////////////////////
BOOL CScreenCaptureDlg::CaptureRect(CRect& rect)
{
   m_ImageRect.CopyRect(&rect);

   // create a device-context
   HDC m_hScreenDC =  CreateDC( 
      L"DISPLAY",  // pointer to string specifying driver name 
      NULL,        // pointer to string specifying device name 
      NULL,        // do not use; set to NULL 
      NULL         // pointer to optional printer data 
      );

   if(m_hCaptureDC)
      DeleteDC(m_hCaptureDC);

   // create memory device context
   m_hCaptureDC = CreateCompatibleDC(m_hScreenDC);
   // create bitmap object

   if(m_hCaptureBitmap)
      DeleteObject(m_hCaptureBitmap);

   m_hCaptureBitmap = CreateCompatibleBitmap(m_hScreenDC, m_ImageRect.Width(), m_ImageRect.Height());

   // selects an object into the specified device context (DC). 
   SelectObject(m_hCaptureDC,m_hCaptureBitmap); 

   // performs a bit-block transfer of the color data corresponding to a rectangle of pixels 
   // from the specified source device context into a destination device context.
   BitBlt(
      m_hCaptureDC,  // destination device context.
      m_ImageRect.left,             // x - upper-left corner of the destination rectangle.
      m_ImageRect.top,             // y - upper-left corner of the destination rectangle.
      m_ImageRect.Width(),        // width of source and destination rectangles.
      m_ImageRect.Height(),       // height of source and destination rectangles.
      m_hScreenDC,   // source device context.
      0,             // x - upper-left corner of the source rectangle.
      0,             // y - upper-left corner of the source rectangle.
      SRCCOPY|CAPTUREBLT // A raster-operation code.
      ); 

   if(m_ImageRect.Width() <= m_FrameRect.Width())
      m_hbar.ShowWindow(false);
   else
      m_hbar.ShowWindow(true);
   if(m_ImageRect.Height() <= m_FrameRect.Height())
      m_vbar.ShowWindow(false);
   else
      m_vbar.ShowWindow(true);

   DeleteDC(m_hScreenDC);
   m_hScreenDC = NULL;

   m_xScrollPageSize = 100 * m_FrameRect.Width() / m_ImageRect.Width();
   m_yScrollPageSize = 100 * m_FrameRect.Height() / m_ImageRect.Height();

   SCROLLINFO scrollInfo;
   m_hbar.GetScrollInfo(&scrollInfo);
   scrollInfo.cbSize = sizeof(SCROLLINFO);
   scrollInfo.fMask = SIF_PAGE; 
   scrollInfo.nPage = m_xScrollPageSize;
   m_hbar.SetScrollInfo(&scrollInfo);

   m_vbar.GetScrollInfo(&scrollInfo);
   scrollInfo.cbSize = sizeof(SCROLLINFO);
   scrollInfo.fMask = SIF_PAGE; 
   scrollInfo.nPage = m_yScrollPageSize;
   m_vbar.SetScrollInfo(&scrollInfo);

   return TRUE;
}
//////////////////////////////////////////////////////////////////////////
BOOL CScreenCaptureDlg::CaptureScreen()
{
   ShowWindow(FALSE);
   Sleep(500);
   CRect rect(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
   CaptureRect(rect);
   ShowWindow(TRUE);
   return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::DisplayImage()
{
   if(!m_hCaptureDC)
      return;

   if(!m_vbar.IsWindowVisible() || !m_hbar.IsWindowVisible())
      m_FrameWnd.GetDC()->FillSolidRect(m_FrameRect, RGB(255,255,255));

   HDC h_screendata = m_FrameWnd.GetDC()->GetSafeHdc(); // the area to copy to.
   // performs a bit-block transfer of the color data corresponding to a rectangle of pixels 
   // from the specified source device context into a destination device context.
   StretchBlt(
      h_screendata,    // destination device context.
      0,            // x - upper-left corner of the destination rectangle.
      0,             // y - upper-left corner of the destination rectangle.
      m_FrameRect.Width(),   // width of destination rectangle.
      m_FrameRect.Height(),  // height of destination rectangle.
      m_hCaptureDC,    // source device context.
      m_xCurrentScroll,              // x - upper-left corner of the source rectangle.
      m_yCurrentScroll,              // y - upper-left corner of the source rectangle.
      m_FrameRect.Width()/m_ZoomPercent, // width of source rectangle.
      m_FrameRect.Height()/m_ZoomPercent, // height of source rectangle.
      SRCCOPY|CAPTUREBLT // A raster-operation code.
      ); 

   DeleteDC(h_screendata); // release device-context
}
//////////////////////////////////////////////////////////////////////////
void  CScreenCaptureDlg::SaveCapturedBitmap(HBITMAP hBitmap)
{
   OPENFILENAME ofn;
   TCHAR pszFileName[MAX_PATH] = L"";

   ZeroMemory(&ofn, sizeof(ofn));

   ofn.lStructSize = sizeof(ofn); 
   ofn.hwndOwner = GetSafeHwnd();
   ofn.lpstrFilter = L"Bitmap Files (*.bmp)\0*.bmp\0";
   ofn.lpstrFile = pszFileName;
   ofn.nMaxFile = MAX_PATH;
   ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
   ofn.lpstrDefExt = L".bmp";

   if(GetSaveFileName(&ofn))
   {
      HDC            hdc=NULL;
      FILE*          fp=NULL;

      // 24 bit bmp file has 3 parts
      // 1. file header
      // 2. info header
      // 3. image data
      BITMAPFILEHEADER  bmpFileHeader;
      BITMAPINFO        bmpInfo;
      LPVOID            pBuf=NULL;

      hdc = ::GetDC(NULL);

      ZeroMemory(&bmpInfo,sizeof(BITMAPINFO));

      bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

      // Get Bitmap info header
      GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmpInfo, DIB_RGB_COLORS);

      if(bmpInfo.bmiHeader.biSizeImage <= 0)
         bmpInfo.bmiHeader.biSizeImage = bmpInfo.bmiHeader.biWidth * abs(bmpInfo.bmiHeader.biHeight)*(bmpInfo.bmiHeader.biBitCount+7)/8;

      if((pBuf=malloc(bmpInfo.bmiHeader.biSizeImage))==NULL)
      {
         MessageBox(L"Unable to Allocate Bitmap Memory", L"Error", MB_OK|MB_ICONERROR);
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

      if((fp=_wfopen(ofn.lpstrFile,L"wb"))==NULL)
      {
         MessageBox(L"Unable to Create Bitmap File",L"Error",MB_OK|MB_ICONERROR);
         return;
      }

      fwrite(&bmpFileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
      fwrite(&bmpInfo.bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
      fwrite(pBuf, bmpInfo.bmiHeader.biSizeImage, 1, fp);


      if(hdc)
         ::ReleaseDC(NULL,hdc);

      if(pBuf)
         free(pBuf);

      if(fp)
         fclose(fp);
   }
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::OnSizing(UINT fwSide, LPRECT pRect)
{

   CRect rect;
   rect.CopyRect(pRect);
   ScreenToClient(rect);
   switch (fwSide)
   {
   case WMSZ_TOP:
      {
         if(rect.Height() < m_WindowRect.Height())
            pRect->top = pRect->bottom - m_WindowRect.Height();
      }
      break;

   case WMSZ_LEFT:
      {
         if(rect.Width() < m_WindowRect.Width())
            pRect->left = pRect->right - m_WindowRect.Width();
      }
      break;

   case WMSZ_BOTTOM:
      {
         if(rect.Height() < m_WindowRect.Height())
            pRect->bottom = pRect->top + m_WindowRect.Height();
      }
      break;

   case WMSZ_RIGHT:
      {
         if(rect.Width() < m_WindowRect.Width())
            pRect->right = pRect->left + m_WindowRect.Width();
      }
      break;

   case WMSZ_TOPLEFT:
      {
         if(rect.Height() < m_WindowRect.Height())
            pRect->top = pRect->bottom - m_WindowRect.Height();
         if(rect.Width() < m_WindowRect.Width())
            pRect->left = pRect->right - m_WindowRect.Width();
      }
      break;
   case WMSZ_TOPRIGHT:
      {
         if(rect.Height() < m_WindowRect.Height())
            pRect->top = pRect->bottom - m_WindowRect.Height();
         if(rect.Width() < m_WindowRect.Width())
            pRect->right = pRect->left + m_WindowRect.Width();
      }
      break;
   case WMSZ_BOTTOMLEFT:
      {
         if(rect.Height() < m_WindowRect.Height())
            pRect->bottom = pRect->top + m_WindowRect.Height();
         if(rect.Width() < m_WindowRect.Width())
            pRect->left = pRect->right - m_WindowRect.Width();
      }
   case WMSZ_BOTTOMRIGHT:
      {
         if(rect.Height() < m_WindowRect.Height())
            pRect->bottom = pRect->top + m_WindowRect.Height();
         if(rect.Width() < m_WindowRect.Width())
            pRect->right = pRect->left + m_WindowRect.Width();
      }
      break;
   }

   CDialogEx::OnSizing(fwSide, pRect);
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::OnSize(UINT nType, int cx, int cy)
{
   CDialogEx::OnSize(nType, cx, cy);

   CRect win_rect;
   GetWindowRect(&win_rect);
   ScreenToClient(&win_rect);

   if(m_FrameWnd.GetSafeHwnd())
   {
      CRect horz_scroll_rect;
      GetDlgItem(IDC_HORTZ_SCROLL)->GetWindowRect(&horz_scroll_rect);
      ScreenToClient(&horz_scroll_rect);

      horz_scroll_rect.MoveToXY( horz_scroll_rect.left , win_rect.bottom - horz_scroll_rect.Height() - 20 );
      horz_scroll_rect.right = win_rect.right - 40;

      GetDlgItem(IDC_HORTZ_SCROLL)->SetWindowPos(NULL, horz_scroll_rect.left , horz_scroll_rect.top , horz_scroll_rect.Width(), 
         horz_scroll_rect.Height() , 0 ) ;

      CRect vert_scroll_rect;
      GetDlgItem(IDC_VERT_SCROLL)->GetWindowRect(&vert_scroll_rect);
      ScreenToClient(&vert_scroll_rect);

      vert_scroll_rect.MoveToXY( win_rect.right - vert_scroll_rect.Width() - 20 , vert_scroll_rect.top );
      vert_scroll_rect.bottom = win_rect.bottom - 40;

      GetDlgItem(IDC_VERT_SCROLL)->SetWindowPos(NULL, vert_scroll_rect.left , vert_scroll_rect.top , vert_scroll_rect.Width(), 
         vert_scroll_rect.Height() , 0 ) ;

      CRect frame_rect;
      m_FrameWnd.GetWindowRect(&frame_rect);
      ScreenToClient(&frame_rect);

      frame_rect.right = win_rect.right - 40;
      frame_rect.bottom = win_rect.bottom - 40;

      m_FrameWnd.SetWindowPos(NULL, frame_rect.left , frame_rect.top , frame_rect.Width(), 
         frame_rect.Height() , 0 ) ;

      m_FrameWnd.GetClientRect(&m_FrameRect);

      UpdateScrollBars();

      DisplayImage();
   }
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   if(pScrollBar->GetDlgCtrlID() == IDC_HORTZ_SCROLL)
   {
      SCROLLINFO ScrollInfo;
      pScrollBar->GetScrollInfo(&ScrollInfo);  // get information about the scroll
      switch(nSBCode)
      {
      case SB_RIGHT:         //Scrolls to the right. 
         break;
      case SB_ENDSCROLL:      //Ends scroll. 
         break;
      case SB_LINERIGHT:       //Scrolls one line right. 
         pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + 1);
         break;
      case SB_LINELEFT:         //Scrolls one line left. 
         pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - 1);
         break;
      case SB_PAGERIGHT:       //Scrolls one page right. 
         pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + ScrollInfo.nPage);
         break;
      case SB_PAGELEFT:         //Scrolls one page left. 
         pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - ScrollInfo.nPage);
         break;
      case SB_THUMBPOSITION:  //The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation. 
         break;
      case SB_THUMBTRACK:     //The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to. 
         pScrollBar->SetScrollPos(nPos);
         break;
      case SB_LEFT:            //Scrolls to the upper left. 
         break;

      default:
         return;
      }

      m_xCurrentScroll = pScrollBar->GetScrollPos() * (m_ImageRect.Width() - (m_FrameRect.Width() / m_ZoomPercent))/(100 - ScrollInfo.nPage);

      DisplayImage();
   }
   else if(pScrollBar->GetDlgCtrlID() == IDC_ZOOM_SLIDER)
   {
      switch(nSBCode)
      {
      case SB_ENDSCROLL:      //Ends scroll. 
      case SB_RIGHT:         //Scrolls to the lower right. 
      case SB_LEFT:            //Scrolls to the upper left. 
         break;

      case SB_LINERIGHT:       //Scrolls one line down. 
      case SB_LINELEFT:         //Scrolls one line up. 
      case SB_PAGERIGHT:       //Scrolls one page down. 
      case SB_PAGELEFT:         //Scrolls one page up. 
      case SB_THUMBPOSITION:  //The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation. 
      case SB_THUMBTRACK:     //The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to. 
         {
            int nSliderPos;
            CString sText;

            nSliderPos = m_cZoomCtrl.GetPos();
            sText.Format(L"%d %%", nSliderPos);
            m_cZoomTextCtrl.SetWindowText(sText);
            m_ZoomPercent = (float) nSliderPos / 100.;

            UpdateScrollBars();

            DisplayImage();
         }
         break;

      default:
         return;
      }
   }

   CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
//////////////////////////////////////////////////////////////////////////
void CScreenCaptureDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
   SCROLLINFO ScrollInfo;
   pScrollBar->GetScrollInfo(&ScrollInfo);  // get information about the scroll
   switch(nSBCode)
   {
   case SB_BOTTOM:         //Scrolls to the lower right.
      break;
   case SB_ENDSCROLL:      //Ends scroll.
      break;
   case SB_LINEDOWN:       //Scrolls one line down.
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + 1);
      break;
   case SB_LINEUP:         //Scrolls one line up.
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - 1);
      break;
   case SB_PAGEDOWN:       //Scrolls one page down.
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() + ScrollInfo.nPage);
      break;
   case SB_PAGEUP:         //Scrolls one page up.
      pScrollBar->SetScrollPos(pScrollBar->GetScrollPos() - ScrollInfo.nPage);
      break;
   case SB_THUMBPOSITION:  //The user has dragged the scroll box (thumb) and released the mouse button. The nPos parameter indicates the position of the scroll box at the end of the drag operation. 
      break;
   case SB_THUMBTRACK:     //The user is dragging the scroll box. This message is sent repeatedly until the user releases the mouse button. The nPos parameter indicates the position that the scroll box has been dragged to. 
      pScrollBar->SetScrollPos(nPos);
      break;
   case SB_TOP:            //Scrolls to the upper left. 
      break;

   default:
      return;
   }

   m_yCurrentScroll = pScrollBar->GetScrollPos() * (m_ImageRect.Height()- (m_FrameRect.Height() / m_ZoomPercent))/(100 - ScrollInfo.nPage);

   DisplayImage();

   CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}
//////////////////////////////////////////////////////////////////////////

void CScreenCaptureDlg::UpdateScrollBars()
{
   if(m_ImageRect.Width() <= (m_FrameRect.Width()/m_ZoomPercent))
      m_hbar.ShowWindow(false);
   else
      m_hbar.ShowWindow(true);

   if(m_ImageRect.Height() <= (m_FrameRect.Height()/m_ZoomPercent))
      m_vbar.ShowWindow(false);
   else
      m_vbar.ShowWindow(true);

   m_xScrollPageSize = 100 * m_FrameRect.Width() / (m_ImageRect.Width() * m_ZoomPercent);
   m_yScrollPageSize = 100 * m_FrameRect.Height() / (m_ImageRect.Height() * m_ZoomPercent);

   SCROLLINFO scrollInfo;
   m_hbar.GetScrollInfo(&scrollInfo);
   scrollInfo.cbSize = sizeof(SCROLLINFO);
   scrollInfo.fMask = SIF_PAGE; 
   scrollInfo.nPage = m_xScrollPageSize;
   m_hbar.SetScrollInfo(&scrollInfo);

   m_vbar.GetScrollInfo(&scrollInfo);
   scrollInfo.cbSize = sizeof(SCROLLINFO);
   scrollInfo.fMask = SIF_PAGE; 
   scrollInfo.nPage = m_yScrollPageSize;
   m_vbar.SetScrollInfo(&scrollInfo);

   m_xCurrentScroll = (m_hbar.GetScrollPos() * (m_ImageRect.Width() - (m_FrameRect.Width() / m_ZoomPercent)))/(100 - m_xScrollPageSize);
   m_yCurrentScroll = (m_vbar.GetScrollPos() * (m_ImageRect.Height()- (m_FrameRect.Height() / m_ZoomPercent)))/(100 - m_yScrollPageSize);
}