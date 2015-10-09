
// TranslateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Translate.h"
#include "TranslateDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTranslateDlg::CTranslateDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(CTranslateDlg::IDD, pParent)
{
   m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTranslateDlg::~CTranslateDlg()
{
   m_font.DeleteObject();
   m_Errorfont.DeleteObject();
}

void CTranslateDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_SOURCE_TEXT, m_cSourceText);
   DDX_Control(pDX, IDC_TRANSLATED_TEXT, m_cTranslatedText);
   DDX_Control(pDX, IDC_TO_COMBO, m_cToLang);
   DDX_Control(pDX, IDC_ERROR_MSG_TXT, m_cErrorText);
}

BEGIN_MESSAGE_MAP(CTranslateDlg, CDialogEx)
   ON_WM_SYSCOMMAND()
   ON_WM_PAINT()
   ON_WM_QUERYDRAGICON()
   ON_BN_CLICKED(IDC_TRANSLATE_BTN, OnBnClickedTranslateBtn)
   ON_CBN_SELCHANGE(IDC_TO_COMBO, OnCbnSelchangeToCombo)
END_MESSAGE_MAP()


// CTranslateDlg message handlers

BOOL CTranslateDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   // Add "About..." menu item to system menu.

   // IDM_ABOUTBOX must be in the system command range.
   ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
   ASSERT(IDM_ABOUTBOX < 0xF000);

   CMenu* pSysMenu = GetSystemMenu(FALSE);
   if (pSysMenu != NULL)
   {
      BOOL bNameValid;
      CString strAboutMenu;
      bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
      ASSERT(bNameValid);
      if (!strAboutMenu.IsEmpty())
      {
         pSysMenu->AppendMenu(MF_SEPARATOR);
         pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
   }

   // Set the icon for this dialog.  The framework does this automatically
   //  when the application's main window is not a dialog
   SetIcon(m_hIcon, TRUE);			// Set big icon
   SetIcon(m_hIcon, FALSE);		// Set small icon

   // TODO: Add extra initialization here

   CStringArray sLanguages;
   m_cTranslator.GetAllLanguages(sLanguages);
   int count = sLanguages.GetCount();
   for (int i = 0 ; i < count; i++)
      m_cToLang.AddString(sLanguages.GetAt(i));

   if(count)
      m_cToLang.SelectString(0, _T("Kannada"));

   m_font.CreateFont(30, 0, 0, 0, FW_DEMIBOLD, FALSE, FALSE, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
   m_cTranslatedText.SetFont(&m_font,TRUE);

   m_Errorfont.CreateFont(20, 0, 0, 0, FW_NORMAL, TRUE, FALSE, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
   m_cErrorText.SetFont(&m_Errorfont,TRUE);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTranslateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
   if ((nID & 0xFFF0) == IDM_ABOUTBOX)
   {
      CAboutDlg dlgAbout;
      dlgAbout.DoModal();
   }
   else
   {
      CDialogEx::OnSysCommand(nID, lParam);
   }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTranslateDlg::OnPaint()
{
   if (IsIconic())
   {
      CPaintDC dc(this); // device context for painting

      SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

      // Center icon in client rectangle
      int cxIcon = GetSystemMetrics(SM_CXICON);
      int cyIcon = GetSystemMetrics(SM_CYICON);
      CRect rect;
      GetClientRect(&rect);
      int x = (rect.Width() - cxIcon + 1) / 2;
      int y = (rect.Height() - cyIcon + 1) / 2;

      // Draw the icon
      dc.DrawIcon(x, y, m_hIcon);
   }
   else
   {
      CDialogEx::OnPaint();
   }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTranslateDlg::OnQueryDragIcon()
{
   return static_cast<HCURSOR>(m_hIcon);
}

void CTranslateDlg::OnBnClickedTranslateBtn()
{
   // TODO: Add your control notification handler code here
   CStringArray sLines, sFinalLines;
   int i = 0, ret = 0;
   int nLineLength, nLineIndex, nLineCount = m_cSourceText.GetLineCount();
   CString strText, strLine;

   for (i = 0; i < nLineCount; i++)
   {
      nLineIndex = m_cSourceText.LineIndex(i);
      nLineLength = m_cSourceText.LineLength(nLineIndex);
      if(nLineLength)
      {
         m_cSourceText.GetLine(i, strText.GetBufferSetLength(nLineLength + 1),
            nLineLength);

         strText.SetAt(nLineLength, _T('\0')); // null terminate
         strText.ReleaseBuffer(nLineLength + 1);
         if(strText.Trim().IsEmpty())
            continue;
         sLines.Add(strText);
      }
   }

   nLineCount = sLines.GetCount();

   if(nLineCount == 0)
      return;

   for (i = 0; i < nLineCount; i++)
   {
      strText = sLines.GetAt(i);
      ret = strText.Find('.');
      if(ret != -1 && strText.GetAt(ret+1) != ' ')
         strText.Insert(ret+1,' ');
      sFinalLines.Add(strText);
   }

   CString  sTranslatedText, sErrorMsg;
   for (int i = 0; i < sFinalLines.GetCount(); i++)
   {
      CString      orig_str, trans_str;
      orig_str =   sFinalLines[i];
      m_cTranslator.GetTranslation(orig_str, trans_str, sErrorMsg);
      sTranslatedText += trans_str;
      sTranslatedText += _T("\r\n");
   }
   m_cTranslatedText.SetWindowText(sTranslatedText);
   m_cErrorText.SetWindowText(sErrorMsg);
}


void CTranslateDlg::OnCbnSelchangeToCombo()
{
   // TODO: Add your control notification handler code here
   int index = m_cToLang.GetCurSel();
   if(index == CB_ERR)
      return;

   CString stemp;
   m_cToLang.GetLBText(index,stemp);
   m_cTranslator.SetToLanguage(stemp);
}
