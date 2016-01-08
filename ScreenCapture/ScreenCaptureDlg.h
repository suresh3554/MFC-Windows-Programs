
// ScreenCaptureDlg.h : header file
//

#pragma once


// CScreenCaptureMFCDlg dialog
class CScreenCaptureDlg : public CDialogEx
{
   // Construction
public:
   CScreenCaptureDlg(CWnd* pParent = NULL);
   ~CScreenCaptureDlg();

   HDC      m_hCaptureDC;
   HBITMAP  m_hCaptureBitmap ;
   CRect    m_ImageRect;

   CStatic  m_FrameWnd;
   CRect    m_FrameRect;

   CScrollBar  m_vbar;
   CScrollBar  m_hbar;

   int m_xCurrentScroll;   // current horizontal scroll value 
   int m_yCurrentScroll;   // current vertical scroll value 

   int m_xScrollPageSize;
   int m_yScrollPageSize;

   CRect m_WindowRect;

   CSliderCtrl m_cZoomCtrl;
   CStatic m_cZoomTextCtrl;

   float m_ZoomPercent; //[.25 - 2]

   // Dialog Data
   enum { IDD = IDD_SCREENCAPTURE_DIALOG };

protected:
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

   BOOL CaptureRect(CRect& rect);
   BOOL CaptureScreen();
   void DisplayImage();
   void SaveCapturedBitmap(HBITMAP hBitmap);
   void UpdateScrollBars();
   // Implementation
protected:
   HICON m_hIcon;
   // Generated message map functions
   virtual BOOL OnInitDialog();
   DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedCaptureBtn();
   afx_msg void OnBnClickedSaveBtn();
   afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
   afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
   afx_msg void OnSize(UINT nType, int cx, int cy);
};
