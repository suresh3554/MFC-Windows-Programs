
// VersionDetailsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VersionDetails.h"
#include "VersionDetailsDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVersionDetailsDlg dialog


static CString _sDetails[] = {
   _T("CompanyName"),
   _T("FileDescription"),
   _T("FileVersion"),
   _T("InternalName"),
   _T("LegalCopyright"),
   _T("LegalTrademarks"),
   _T("OriginalFilename"),
   _T("ProductName"),
   _T("ProductVersion"),
   _T("PrivateBuild"),
   _T("SpecialBuild"),
   _T("Comments")
};

CVersionDetailsDlg::CVersionDetailsDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(CVersionDetailsDlg::IDD, pParent)
{
   m_sFilepath.Empty();
   m_sDetails.RemoveAll();
}

void CVersionDetailsDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_VD_LIST, m_List);
   DDX_Control(pDX, IDC_VD_FILE, m_cFilePath);
}

BEGIN_MESSAGE_MAP(CVersionDetailsDlg, CDialogEx)
   ON_BN_CLICKED(ID_VD_GET, OnBnClickedVdGet)
END_MESSAGE_MAP()


// CVersionDetailsDlg message handlers

BOOL CVersionDetailsDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();

   // TODO: Add extra initialization here
   CString szfilters = _T("executable files (*.exe)|*.exe|dll files (*.dll)|*.dll|");
   m_cFilePath.EnableFileBrowseButton(_T("exe"),szfilters);

   CRect rect;
   m_List.GetWindowRect(&rect);

   m_List.InsertColumn(0, _T("Name"), LVCFMT_FIXED_RATIO,rect.Width() * 0.5);
   m_List.InsertColumn(1, _T("Value"),LVCFMT_FIXED_RATIO,rect.Width() * 0.49);
   m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

   return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVersionDetailsDlg::OnBnClickedVdGet()
{
   CString sFilepath;
   m_cFilePath.GetWindowText(sFilepath);

   if(m_sFilepath == sFilepath)
      return;

   m_sFilepath = sFilepath;
   m_sDetails.RemoveAll();
   // TODO: Add your control notification handler code here

   DWORD dwHandle, dwLen;
   UINT BufLen;
   LPTSTR lpData, lpBuffer;
   _VersionDetails sTemp;
   sTemp.sName = _T("VersionInfo");
   sTemp.sValue = _T("Not Found");

   dwLen = GetFileVersionInfoSize (m_sFilepath, &dwHandle);

   if (!dwLen)
   {
      m_sDetails.Add(sTemp);
      FillTable();
      return;
   }

   lpData = (LPTSTR) malloc (dwLen);
   if (!lpData) 
   {
      m_sDetails.Add(sTemp);
      FillTable();
      return ;
   }
   if (!GetFileVersionInfo (m_sFilepath, dwHandle, dwLen, lpData)) {
      free (lpData);
      m_sDetails.Add(sTemp);
      FillTable();
      return;
   }

   struct LANGANDCODEPAGE {
      WORD wLanguage;
      WORD wCodePage;
   } *lpTranslate;

   // Get language information
   VerQueryValue(lpData,  _T("\\VarFileInfo\\Translation"), (LPVOID*)&lpTranslate, &BufLen);
   int count =(BufLen/sizeof(struct LANGANDCODEPAGE));

   int sDetails_count = sizeof(_sDetails)/sizeof(CString);

   CString SubBlock;
   for( int i = 0; i < count; i++ )
   {
      sTemp.sName = _T("Language");
      TCHAR   sLangName[1024];
      if(!VerLanguageName(lpTranslate[i].wLanguage,sLangName,1024))
         sTemp.sValue = _T("cannot be identified");
      else
         sTemp.sValue = sLangName;

      m_sDetails.Add(sTemp);
      for(int j = 0; j < sDetails_count; j++)
      {
         // Build query string
         SubBlock.Format(_T("\\StringFileInfo\\%04x%04x\\%s"),lpTranslate[i].wLanguage,lpTranslate[i].wCodePage,_sDetails[j]);

         if (!VerQueryValue (lpData, SubBlock, (LPVOID*) &lpBuffer, (PUINT) &BufLen))
         {
            /* language ID 040904E4: U.S. English, char set = Windows, Multilingual */
            sTemp.sValue = _T("Not Found");
         }
         else
            sTemp.sValue = lpBuffer;

         sTemp.sName = _sDetails[j];
         m_sDetails.Add(sTemp);
      }
   }
   free (lpData);
   FillTable();
}
void CVersionDetailsDlg::FillTable()
{
   m_List.DeleteAllItems();
   for (int i = 0; i < m_sDetails.GetCount(); i++)
   {
      m_List.InsertItem(i,_T(""));
      m_List.SetItemText(i,0,m_sDetails[i].sName);
      m_List.SetItemText(i,1,m_sDetails[i].sValue);
   }
}
