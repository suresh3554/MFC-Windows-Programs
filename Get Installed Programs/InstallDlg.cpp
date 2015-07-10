
// InstallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Install.h"
#include "InstallDlg.h"
#include "afxdialogex.h"
#include "Userenv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
// CInstallDlg dialog

CInstallDlg::CInstallDlg(CWnd* pParent /*=NULL*/)
   : CDialogEx(CInstallDlg::IDD, pParent)
{
   m_ListData.RemoveAll();
}

CInstallDlg::~CInstallDlg()
{
   delete m_Imagelist;
   m_ListData.RemoveAll();
}

void CInstallDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialogEx::DoDataExchange(pDX);
   DDX_Control(pDX,IDC_INSTALL_LIST,m_List);
}

BEGIN_MESSAGE_MAP(CInstallDlg, CDialogEx)
   ON_WM_SIZE()
   ON_WM_SIZING()
   ON_BN_CLICKED(IDC_INCLUDE_UPDATES, OnBnClickedIncludeUpdates)
   ON_BN_CLICKED(IDC_SHOW_ONLY_UPDATES, &CInstallDlg::OnBnClickedShowOnlyUpdates)
END_MESSAGE_MAP()


// CInstallDlg message handlers

BOOL CInstallDlg::OnInitDialog()
{
   CDialogEx::OnInitDialog();
   
   GetWindowRect(&m_WindowRect);
   ScreenToClient(&m_WindowRect);
   m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
   m_List.SetExtendedStyle(LVS_EX_GRIDLINES);
   //m_List.SetExtendedStyle(LVS_EX_AUTOSIZECOLUMNS);

   m_Imagelist = new CImageList();
   m_Imagelist->Create(20,20,ILC_COLOR32,0,0);

   CRect rect;
   m_List.GetWindowRect(&rect);

   m_List.InsertColumn(0, _T("Name"), LVCFMT_FIXED_WIDTH,rect.Width() * 0.5);
   m_List.InsertColumn(1, _T("Publisher"),LVCFMT_FIXED_WIDTH,rect.Width() * 0.3);
   m_List.InsertColumn(2, _T("Version"),LVCFMT_FIXED_WIDTH,rect.Width() * 0.2);
    //m_List.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);

   GetInstalledPrograms();

   FillTable(false,false);

   return TRUE;  // return TRUE  unless you set the focus to a control
}
//////////////////////////////////////////////////////////////////////////
static  int  _SortbyName(ListData& list1, ListData& list2 )
{
   return list1.sName.CompareNoCase(list2.sName);
}
//////////////////////////////////////////////////////////////////////////
void CInstallDlg::GetInstalledPrograms()
{
   HKEY     machineRootKey = HKEY_LOCAL_MACHINE, userRootKey = HKEY_USERS;

   HKEY sClassKey, Wow64UninstallKey, StdUninstallKey;

   CString sMachine_Path = _T("Software\\Classes\\Installer\\Products");
   int retval = RegOpenKeyEx(machineRootKey, sMachine_Path, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &sClassKey);
   if(ERROR_SUCCESS != retval)
      return;

   // ---Wow64 Uninstall key
   CString Wow64UninstallKeyPath = _T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
   retval = RegOpenKeyEx(machineRootKey, Wow64UninstallKeyPath, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &Wow64UninstallKey);
   if(ERROR_SUCCESS != retval)
   {
      RegCloseKey(sClassKey);
      return;
   }

   GetUnInstallKeyProgramData(sClassKey, Wow64UninstallKey);
   int count = m_ListData.GetSize();

   RegCloseKey(Wow64UninstallKey);

      //---Standard Uninstall key
   CString StdUninstallKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall");
   retval = RegOpenKeyEx(machineRootKey, StdUninstallKeyPath, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &StdUninstallKey);

   if(ERROR_SUCCESS != retval)
   {
      RegCloseKey(sClassKey);
      return;
   }

   GetUnInstallKeyProgramData(sClassKey, StdUninstallKey);
   count = m_ListData.GetSize();
   RegCloseKey(StdUninstallKey);

   // Users 

   CStringArray  users;
   GetSubKeyNames(userRootKey, users);

   for (int i = 0 ; i < users.GetCount(); i++)
   {
      //---HKU Uninstall key
       CString user = users.GetAt(i);
       user += "\\";
       user += StdUninstallKeyPath;
       retval = RegOpenKeyEx(userRootKey, user, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &StdUninstallKey);
       if(ERROR_SUCCESS == retval)
       {
          GetUnInstallKeyProgramData(sClassKey, StdUninstallKey);
          RegCloseKey(StdUninstallKey);
       }

       CString installKeyPath = _T("\\Software\\Microsoft\\Installer\\Products");

       user = users.GetAt(i);
       user += installKeyPath;

       retval = RegOpenKeyEx(userRootKey, user, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &StdUninstallKey);
       if(ERROR_SUCCESS == retval)
       {
          GetUserInstallKeyProgramData(machineRootKey, StdUninstallKey);
          RegCloseKey(StdUninstallKey);
       }
   }

   // system account " S-1-5-18 "
   GetSystemAccountProgramData(machineRootKey);

   RegCloseKey(sClassKey);
}

//////////////////////////////////////////////////////////////////////////
void CInstallDlg::FillTable(bool includeUpdates, bool showOnlyUpdates)
{
   m_List.DeleteAllItems();
   bool showPrograms = true;
   if(showOnlyUpdates)
   {
      showPrograms = false;
      includeUpdates = true;
   }

   int count = m_ListData.GetCount();

   qsort(m_ListData.GetData(),count, sizeof(ListData),(int (*)(const void *,const void *))_SortbyName);

   for(int i = 0; i < count; i++)
   {
      HICON icon = ExtractIcon(AfxGetApp()->m_hInstance, m_ListData[i].sDisplayIcon,0);
      if(icon == NULL)
         icon = LoadIcon(NULL, IDI_APPLICATION);
      m_Imagelist->Add(icon);
      DestroyIcon(icon);
   }

   m_List.SetImageList(m_Imagelist,LVSIL_SMALL);

   ListData sdata;
   int iRow = 0;
   for (int i = 0; i < count; i++)
   {
      if(sdata.sName == m_ListData[i].sName && sdata.sDisplayVersion == m_ListData[i].sDisplayVersion)
         continue;

      if(m_ListData[i].isUpdate)
      {
         if(includeUpdates)
         {
            m_List.InsertItem(iRow,_T(""),i);
            m_List.SetItemText(iRow,0,m_ListData[i].sName);
            m_List.SetItemText(iRow,1,m_ListData[i].sPublisher);
            m_List.SetItemText(iRow,2,m_ListData[i].sDisplayVersion);
            sdata = m_ListData[i];
            iRow++;
         }
      }
      else if(showPrograms)
      {
         m_List.InsertItem(iRow,_T(""),i);
         m_List.SetItemText(iRow,0,m_ListData[i].sName);
         m_List.SetItemText(iRow,1,m_ListData[i].sPublisher);
         m_List.SetItemText(iRow,2,m_ListData[i].sDisplayVersion);
         sdata = m_ListData[i];
         iRow++;
      }
   }
   CString stext;
   stext.Format(_T("Number of Programs installed: %d"),iRow);
   GetDlgItem(IDC_INSTALL_TEXT)->SetWindowText(stext);
}
//////////////////////////////////////////////////////////////////////////
int  CInstallDlg::GetKeyValueDataType(HKEY hKey, CString sValueName, DWORD& sDataType)
{
   return RegQueryValueEx(hKey, sValueName, 0, &sDataType, NULL, NULL);
}
//////////////////////////////////////////////////////////////////////////
int  CInstallDlg::GetKeyValueData(HKEY hKey, CString sValueName, CString& sData)
{
   TCHAR buffer[1024];
   DWORD bufsize = sizeof(buffer);

   buffer[0] = '\0';

   int retval = RegQueryValueEx(hKey, sValueName, 0, NULL, (LPBYTE)&buffer, &bufsize);
   //CString str(buffer);
   sData = buffer;

   return retval;
}
//////////////////////////////////////////////////////////////////////////
int  CInstallDlg::GetKeyValueData(HKEY hKey, CString sValueName, DWORD& sData)
{
   DWORD dValue = 0 ;
   DWORD valSize = sizeof(dValue);

   valSize = sizeof(dValue);
   int retval = RegQueryValueEx(hKey, sValueName, 0, NULL, (LPBYTE)&dValue, &valSize);
   sData = dValue;

   return retval;
}
//////////////////////////////////////////////////////////////////////////
void CInstallDlg::GetUserInstallKeyProgramData(HKEY& HklmRootKey, HKEY& CuInstallerKey)
{
   HKEY           dataKey;
   CStringArray   sUserProductGuids, UserDataKeyNames, sLMProductGuids;
   CString        sCUProductId, sLMProductId, sTempPath;
   int i = 0, j = 0, k =0, retval, ProductFound;

   DWORD dValue = 0 ;
   TCHAR buffer[2048];

   ListData sData;

   GetSubKeyNames(CuInstallerKey, sUserProductGuids);

   retval = RegOpenKeyEx(HklmRootKey, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData"), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &dataKey);
   if(ERROR_SUCCESS == retval)
   {
      GetSubKeyNames(dataKey, UserDataKeyNames);
      RegCloseKey(dataKey);
   }

   for (i = 0; i < sUserProductGuids.GetCount(); i++)
   {
      sCUProductId = sUserProductGuids.GetAt(i);
      ProductFound = FALSE;
      for (j = 0; j < UserDataKeyNames.GetCount(); j++)
      {
         CString sName = UserDataKeyNames.GetAt(j);
         if(sName == _T("S-1-5-18")) //Ignore the LocalSystem account
            continue;

         sTempPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\");
         sTempPath += sName;
         sTempPath += _T("\\Products");

         retval = RegOpenKeyEx(HklmRootKey, sTempPath, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &dataKey);
         if(ERROR_SUCCESS == retval)
         {
            GetSubKeyNames(dataKey, sLMProductGuids);
            RegCloseKey(dataKey);
         }
         for (k = 0 ; k < sLMProductGuids.GetCount(); k++)
         {
            sLMProductId = sLMProductGuids.GetAt(k);
            if(sCUProductId == sLMProductId)
            {
               sTempPath += _T("\\");
               sTempPath += sLMProductId;
               sTempPath += _T("\\InstallProperties");

               retval = RegOpenKeyEx(HklmRootKey, sTempPath, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &dataKey);
               if(ERROR_SUCCESS == retval)
               {
                  GetKeyValueData(dataKey,_T("SystemComponent"),dValue);

                  if(dValue == 0)
                  {
                     HKEY  tempkey;
                     retval = RegOpenKeyEx(CuInstallerKey, sCUProductId, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &tempkey);
                     if(ERROR_SUCCESS == retval)
                     {
                        GetKeyValueData(tempkey,_T("ProductName"),sData.sName);
 
                        if(sData.sName.IsEmpty() == FALSE)
                        {
                           GetKeyValueData(dataKey,_T("DisplayVersion"),sData.sDisplayVersion);
                           GetKeyValueData(dataKey,_T("Publisher"),sData.sPublisher);
                           GetKeyValueData(tempkey,_T("ProductIcon"),sData.sDisplayIcon);
                           m_ListData.Add(sData);
                           ProductFound = TRUE;
                        }
                        RegCloseKey(tempkey);
                     }
                  }
                  RegCloseKey(dataKey);
                  break;
               }
            }
         }
         if(ProductFound)
            break;
      }
   }
}
//////////////////////////////////////////////////////////////////////////
void CInstallDlg::GetSystemAccountProgramData(HKEY& HklmRootKey)
{
   HKEY           dataKey;
   CStringArray   sUserProductGuids, sLMProductGuids;
   CString        sCUProductId, sLMProductId, sTempPath;
   int i = 0, j = 0, k =0, retval, ProductFound;

   DWORD dValue = 0 ;
   TCHAR buffer[2048];

   ListData sData;

   HKEY CuInstallerKey;

   retval = RegOpenKeyEx(HklmRootKey, _T("Software\\Classes\\Installer\\Products"), 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &CuInstallerKey);
   if(ERROR_SUCCESS == retval)
      GetSubKeyNames(CuInstallerKey, sUserProductGuids);

   for (i = 0; i < sUserProductGuids.GetCount(); i++)
   {
      sCUProductId = sUserProductGuids.GetAt(i);
      ProductFound = FALSE;

      sTempPath = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Installer\\UserData\\S-1-5-18\\Products");

      retval = RegOpenKeyEx(HklmRootKey, sTempPath, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &dataKey);
      if(ERROR_SUCCESS == retval)
      {
         GetSubKeyNames(dataKey, sLMProductGuids);
         RegCloseKey(dataKey);
      }
      for (k = 0 ; k < sLMProductGuids.GetCount(); k++)
      {
         sLMProductId = sLMProductGuids.GetAt(k);
         if(sCUProductId == sLMProductId)
         {
            retval = RegOpenKeyEx(CuInstallerKey, sCUProductId, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &dataKey);
            if(ERROR_SUCCESS == retval)
               GetKeyValueData(dataKey,_T("SystemComponent"),dValue);

            if(dValue)
               continue;

            GetKeyValueData(dataKey,_T("WindowsInstaller"),dValue);

            if(dValue != 1)
               continue;

            GetKeyValueData(dataKey,_T("ProductName"),sData.sName);

            if(sData.sName.IsEmpty() == FALSE)
            {
               GetKeyValueData(dataKey,_T("DisplayVersion"),sData.sDisplayVersion);
               GetKeyValueData(dataKey,_T("Publisher"),sData.sPublisher);
               GetKeyValueData(dataKey,_T("ProductIcon"),sData.sDisplayIcon);
               m_ListData.Add(sData);
               ProductFound = TRUE;
            }
            RegCloseKey(dataKey);
         }
      }
   }
   if(sUserProductGuids.GetCount())
      RegCloseKey(CuInstallerKey);
}

//////////////////////////////////////////////////////////////////////////
void CInstallDlg::GetUnInstallKeyProgramData(HKEY& classKey, HKEY& UninstallKey)
{
   CStringArray subKeyNames;
   subKeyNames.RemoveAll();

   GetSubKeyNames(UninstallKey, subKeyNames);

   GetSubKeysData(classKey, UninstallKey, subKeyNames);
}

//////////////////////////////////////////////////////////////////////////
void CInstallDlg::GetKeyValueNames(HKEY hKey, CStringArray &sValues)
{
   int index = 0, retCode;
   TCHAR buffer[1024];
   DWORD bufsize;
   while (1)
   {
      buffer[0] = '\0';
      bufsize = sizeof(buffer);
      retCode = RegEnumValue(hKey, index, buffer, &bufsize, NULL, NULL, NULL, NULL);
      if (retCode == ERROR_SUCCESS) 
      {
         CString str(buffer);
         sValues.Add(buffer);
      }
      else if(ERROR_NO_MORE_ITEMS)
         break;
      index++;
   }
}

void CInstallDlg::GetSubKeyNames(HKEY hKey, CStringArray &subKeys) 
{ 
   TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
   DWORD    cbName;                   // size of name string 
   TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
   DWORD    cchClassName = MAX_PATH;  // size of class string 
   DWORD    cSubKeys=0;               // number of sub keys 
   DWORD    cbMaxSubKey;              // longest subkey size 
   DWORD    cchMaxClass;              // longest class string 
   DWORD    cValues;              // number of values for key 
   DWORD    cchMaxValue;          // longest value name 
   DWORD    cbMaxValueData;       // longest value data 
   DWORD    cbSecurityDescriptor; // size of security descriptor 
   FILETIME ftLastWriteTime;      // last write time 

   DWORD i, retCode; 

   TCHAR  achValue[MAX_VALUE_NAME]; 
   DWORD  cchValue = MAX_VALUE_NAME; 

   // Get the class name and the value count. 
   retCode = RegQueryInfoKey(
      hKey,                    // key handle 
      achClass,                // buffer for class name 
      &cchClassName,           // size of class string 
      NULL,                    // reserved 
      &cSubKeys,               // number of sub keys 
      &cbMaxSubKey,            // longest subkey size 
      &cchMaxClass,            // longest class string 
      &cValues,                // number of values for this key 
      &cchMaxValue,            // longest value name 
      &cbMaxValueData,         // longest value data 
      &cbSecurityDescriptor,   // security descriptor 
      &ftLastWriteTime);       // last write time 

   // Enumerate the sub keys, until RegEnumKeyEx fails.

   if (cSubKeys)
   {
      for (i=0; i<cSubKeys; i++) 
      { 
         cbName = MAX_KEY_LENGTH;
         retCode = RegEnumKeyEx(hKey, i,
            achKey, 
            &cbName, 
            NULL, 
            NULL, 
            NULL, 
            &ftLastWriteTime); 
         if (retCode == ERROR_SUCCESS) 
         {
            subKeys.Add(achKey);
         }
         else if(ERROR_NO_MORE_ITEMS)
            break;
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void CInstallDlg::GetSubKeysData(HKEY& classKey, HKEY& UninstallKey, CStringArray &subKeyNames)
{
   int i,j;
   HKEY hNewkey;

   TCHAR buffer[1024];

   DWORD dValue = 0 ;

   ZeroMemory(buffer, 1024);

   for (i = 0; i < subKeyNames.GetCount(); i++)
   {
      ListData sData;
      CString  subKey = subKeyNames.GetAt(i);
      LONG retval = RegOpenKeyEx(UninstallKey, subKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &hNewkey);
      if (retval == ERROR_SUCCESS)
      {
         GetKeyValueData(hNewkey,_T("SystemComponent"),dValue);

         if(dValue)
            continue;

         GetKeyValueData(hNewkey,_T("WindowsInstaller"),dValue);

         if(dValue == 0)
         {
            CString strParentKeyName, strProgramReleaseType;
            GetKeyValueData(hNewkey,_T("ParentKeyName"),strParentKeyName);
            GetKeyValueData(hNewkey,_T("ReleaseType"),strProgramReleaseType);
            if(!strParentKeyName.IsEmpty() || strParentKeyName == _T("Security Update")
               || strParentKeyName == _T("Update Rollup") || strParentKeyName == _T("Hotfix")
               || IsWindowsUpdate(subKey))
            {
               GetKeyValueData(hNewkey,_T("DisplayName"),sData.sName);
               if(sData.sName.IsEmpty() == FALSE)
               {
                  GetKeyValueData(hNewkey,_T("DisplayVersion"),sData.sDisplayVersion);
                  GetKeyValueData(hNewkey,_T("Publisher"),sData.sPublisher);
                  GetKeyValueData(hNewkey,_T("DisplayIcon"),sData.sDisplayIcon);
                  sData.isUpdate = true;
                  m_ListData.Add(sData);
               }
            }
            else
            {
               CStringArray sValueNames;
               GetKeyValueNames(hNewkey,sValueNames);
               for (j = 0; j < sValueNames.GetCount(); j++)
               {
                  if(sValueNames.GetAt(j).CompareNoCase(_T("uninstallstring")) == 0)
                  {
                     GetKeyValueData(hNewkey,_T("DisplayName"),sData.sName);

                     if(sData.sName.IsEmpty() == FALSE)
                     {
                        GetKeyValueData(hNewkey,_T("DisplayVersion"),sData.sDisplayVersion);
                        GetKeyValueData(hNewkey,_T("Publisher"),sData.sPublisher);
                        GetKeyValueData(hNewkey,_T("DisplayIcon"),sData.sDisplayIcon);
                        m_ListData.Add(sData);
                     }
                  }
               }
            }
         }
         else
         {
            HKEY oneKey;
            CString msiName = GetInstallerKeyNameFromGuid(subKey);

            LONG retval = RegOpenKeyEx(classKey, msiName, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY, &oneKey);
            if (retval == ERROR_SUCCESS)
            {
               GetKeyValueData(oneKey,_T("ProductName"),sData.sName);
               if(sData.sName.IsEmpty() == FALSE)
               {
                  GetKeyValueData(hNewkey,_T("DisplayVersion"),sData.sDisplayVersion);
                  GetKeyValueData(hNewkey,_T("Publisher"),sData.sPublisher);
                  GetKeyValueData(oneKey,_T("ProductIcon"),sData.sDisplayIcon);
                  m_ListData.Add(sData);
               }
               RegCloseKey(oneKey);
            }
         }
         RegCloseKey(hNewkey);
      }
   }
}
//////////////////////////////////////////////////////////////////////////
CString CInstallDlg::GetInstallerKeyNameFromGuid(CString sGUIDName)
{
   CString sMsiName;
   CStringArray parts;
   int i, j, pos = 0;
   sGUIDName.Replace(_T("{"),_T(""));
   sGUIDName.Replace(_T("}"),_T(""));
   CString temp = sGUIDName.Tokenize(_T("-"),pos);
   while(temp.IsEmpty() == FALSE)
   {
      parts.Add(temp);
      temp = sGUIDName.Tokenize(_T("-"),pos);
   }
   //Just reverse the first 3 parts
   for (i = 0;i < 3; i++)
      sMsiName.Append(parts[i].MakeReverse());

   //For the last 2 parts, reverse each character pair
   for (i = 3; i < 5; i++)
   {
      for ( j = 0 ; j < parts[i].GetLength() - 1; j++)
      {
         sMsiName.AppendChar(parts[i][j+1]);
         sMsiName.AppendChar(parts[i][j]);
         j++;
      }
   }
   return sMsiName;
}
//////////////////////////////////////////////////////////////////////////
bool CInstallDlg::IsWindowsUpdate(CString subKey)
{
   // if name begins with 'KB' followed by 6 numbers - then it is windows update
   if(subKey.GetLength() == 8)
   {
      if(subKey.GetAt(0) != 'K' || subKey.GetAt(0) != 'B')
         return false;

      for (int i = 2 ; i < 8 ; i++)
      {
         if(isdigit(subKey.GetAt(i)) == false )
            return false;
      }
      return true;
   }
   return false;
}
//////////////////////////////////////////////////////////////////////////
void CInstallDlg::OnBnClickedIncludeUpdates()
{
   int check = 0;
   check = ((CButton *)GetDlgItem(IDC_INCLUDE_UPDATES))->GetCheck();
   if(check)
      FillTable(true,false);
   else
      FillTable(false,false);
   // TODO: Add your control notification handler code here
}
//////////////////////////////////////////////////////////////////////////
void CInstallDlg::OnBnClickedShowOnlyUpdates()
{
   // TODO: Add your control notification handler code here
   int check = 0;
   check = ((CButton *)GetDlgItem(IDC_SHOW_ONLY_UPDATES))->GetCheck();
   if(check)
   {
      FillTable(true,true);
      GetDlgItem(IDC_INCLUDE_UPDATES)->EnableWindow(FALSE);
   }
   else
   {
      FillTable(false,false);
      GetDlgItem(IDC_INCLUDE_UPDATES)->EnableWindow(TRUE);
      CheckDlgButton(IDC_INCLUDE_UPDATES,FALSE);
   }
}
//////////////////////////////////////////////////////////////////////////
void CInstallDlg::OnSize(UINT nType, int cx, int cy)
{
   CDialogEx::OnSize(nType, cx, cy);

   CRect win_rect;
   GetWindowRect(&win_rect);
   ScreenToClient(&win_rect);

   if(m_List.GetSafeHwnd())
   {
      CRect text_rect;
      GetDlgItem(IDC_INSTALL_TEXT)->GetWindowRect(&text_rect);
      ScreenToClient(&text_rect);

      text_rect.MoveToXY( text_rect.left , win_rect.bottom - text_rect.Height() - 30 );

      GetDlgItem(IDC_INSTALL_TEXT)->SetWindowPos(NULL, text_rect.left , text_rect.top , text_rect.Width(), 
         text_rect.Height() , 0 ) ;


      CRect Close_rect;
      GetDlgItem(IDCANCEL)->GetWindowRect(&Close_rect);
      ScreenToClient(&Close_rect);

      Close_rect.MoveToXY( win_rect.right - Close_rect.Width() - 25 , win_rect.bottom - Close_rect.Height() - 20);

      GetDlgItem(IDCANCEL)->SetWindowPos(NULL, Close_rect.left , Close_rect.top , Close_rect.Width(), 
         Close_rect.Height() , 0 ) ;


      CRect Ok_rect;
      GetDlgItem(IDOK)->GetWindowRect(&Ok_rect);
      ScreenToClient(&Ok_rect);

      Ok_rect.MoveToXY( win_rect.right - Ok_rect.Width() - 110 , win_rect.bottom - Ok_rect.Height() - 20);

      GetDlgItem(IDOK)->SetWindowPos(NULL, Ok_rect.left , Ok_rect.top , Ok_rect.Width(), 
         Ok_rect.Height() , 0 ) ;

      CRect list_rect;
      m_List.GetWindowRect(&list_rect);
      ScreenToClient(&list_rect);

      list_rect.right = win_rect.right - 25;
      list_rect.bottom = win_rect.bottom - 60;

      m_List.SetWindowPos(NULL, list_rect.left , list_rect.top , list_rect.Width(), 
         list_rect.Height() , 0 ) ;

      m_List.SetColumnWidth(0, list_rect.Width() * 0.5);
      m_List.SetColumnWidth(1, list_rect.Width() * 0.3);
      m_List.SetColumnWidth(2, list_rect.Width() * 0.2);
   }
}
//////////////////////////////////////////////////////////////////////////
void CInstallDlg::OnSizing(UINT fwSide, LPRECT pRect)
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
