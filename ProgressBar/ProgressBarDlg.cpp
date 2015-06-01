
// ProgressBarDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProgressBar.h"
#include "ProgressBarDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(CProgressBarDlg::IDD, pParent)
{
}

void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_PBAR_HORZ, m_cProgressBar);
   DDX_Control(pDX, IDC_PBAR_MARQUEE, m_cProgressBarMarquee);
}

BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialogEx)
   ON_BN_CLICKED(IDC_PBAR_COLOR, OnBnClickedPbarColor)
   ON_BN_CLICKED(IDC_PBAR_START, OnBnClickedPbarStart)
   ON_BN_CLICKED(IDC_PBAR_STOP, OnBnClickedPbarStop)
   ON_WM_TIMER()
END_MESSAGE_MAP()


// CProgressBarDlg message handlers
BOOL CProgressBarDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   // TODO: Add extra initialization here
   m_cProgressBar.SetRange(0,100);

   m_cProgressBarMarquee.ModifyStyle(0,PBS_MARQUEE);

   GetDlgItem(IDC_PBAR_STOP)->EnableWindow(FALSE);

   UpdateData(FALSE);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

typedef HRESULT (__stdcall *SetWindowThemePtr)(
   HWND hwnd,
   LPCWSTR pszSubAppName,
   LPCWSTR pszSubIdList
   );

void CProgressBarDlg::OnBnClickedPbarColor()
{
   // The SetBarColor method sets the progress bar color only if a Windows Vista theme is not in effect
   // check documentation: https://msdn.microsoft.com/en-us/library/bb398776.aspx
   static int flag = 0;
   if(!flag)
   {
      HMODULE hMod = LoadLibrary(_T("UxTheme.dll"));
      if(hMod)
      {
         SetWindowThemePtr fn = (SetWindowThemePtr)GetProcAddress(hMod,"SetWindowTheme");
         if(fn)
         {
            fn(m_cProgressBar.GetSafeHwnd(), L"Explorer", L"");
            fn(m_cProgressBarMarquee.GetSafeHwnd(), L"", L"");
         }
         FreeLibrary(hMod);
      }
      flag = 1;
   }

   // TODO: Add your control notification handler code here
   // get the desired color
   CColorDialog coldlg(m_cProgressBar.GetBarColor(), CC_FULLOPEN | CC_RGBINIT, this);
   int nResponse = (int)coldlg.DoModal();

   // save the color if one selected
   if (nResponse == IDOK)
   {
      COLORREF color = coldlg.GetColor();
      m_cProgressBar.SetBarColor(color);
      m_cProgressBarMarquee.SetBarColor(color);
   }
}

void CProgressBarDlg::OnBnClickedPbarStart()
{
   SetTimer(IDT_TIMER, 1000, NULL);

   m_cProgressBarMarquee.SetMarquee(TRUE,10);

   GetDlgItem(IDC_PBAR_START)->EnableWindow(FALSE);
   GetDlgItem(IDC_PBAR_STOP)->EnableWindow();
}


void CProgressBarDlg::OnBnClickedPbarStop()
{
   KillTimer(IDT_TIMER);

   m_cProgressBarMarquee.SetMarquee(FALSE,10);

   GetDlgItem(IDC_PBAR_STOP)->EnableWindow(FALSE);
   GetDlgItem(IDC_PBAR_START)->EnableWindow();
}


void CProgressBarDlg::OnTimer(UINT_PTR nIDEvent)
{
   m_cProgressBar.SetPos(0);
   Sleep(10);

   m_cProgressBar.SetStep(5);
   for (int i = 0 ; i < 20; i++)
   {
      m_cProgressBar.StepIt();
      Sleep(20);
   }
}