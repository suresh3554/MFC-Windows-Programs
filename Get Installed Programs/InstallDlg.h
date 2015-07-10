
// InstallDlg.h : header file
//

#pragma once

struct ListData
{
   CString sName;
   CString sDisplayVersion;
   CString sDisplayIcon;
   CString sPublisher;
   bool    isUpdate;
   ListData()
   {
      sName.Empty();
      sDisplayVersion.Empty();
      isUpdate = false;
   }
};

// CInstallDlg dialog
class CInstallDlg : public CDialogEx
{
   CListCtrl m_List;
   CArray<ListData,ListData&>  m_ListData;

   CImageList* m_Imagelist;
   CRect    m_WindowRect;
// Construction
public:
   CInstallDlg(CWnd* pParent = NULL);	// standard constructor
   ~CInstallDlg();	// standard destructor

// Dialog Data
	enum { IDD = IDD_INSTALL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

   void GetInstalledPrograms();
   void GetUnInstallKeyProgramData(HKEY& classKey, HKEY& UninstallKey);
   void GetSubKeyNames(HKEY hKey, CStringArray &subKeys) ;
   void GetKeyValueNames(HKEY hKey, CStringArray &sValues);

   int  GetKeyValueDataType(HKEY hKey, CString sValueName, DWORD& sDataType);
   int  GetKeyValueData(HKEY hKey, CString sValueName, CString& sData);
   int  GetKeyValueData(HKEY hKey, CString sValueName, DWORD& sData);

   void GetSubKeysData(HKEY& classKey, HKEY& UninstallKey, CStringArray &subKeyNames);
   CString GetInstallerKeyNameFromGuid(CString sGUIDName);
   bool IsWindowsUpdate(CString subKey);

   void GetUserInstallKeyProgramData(HKEY& HklmRootKey, HKEY& CuInstallerKey);
   void GetSystemAccountProgramData(HKEY& HklmRootKey);

   void FillTable(bool includeUpdates, bool showOnlyUpdates);

// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnSize(UINT nType, int cx, int cy);
   afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
   afx_msg void OnBnClickedIncludeUpdates();
   afx_msg void OnBnClickedShowOnlyUpdates();
};
