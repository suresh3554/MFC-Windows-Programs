
// VersionDetailsDlg.h : header file
//

#pragma once

struct _VersionDetails
{
   CString sName;
   CString sValue;
};

// CVersionDetailsDlg dialog
class CVersionDetailsDlg : public CDialogEx
{
   // Construction
   CString m_sFilepath;
   CListCtrl  m_List;
   CMFCEditBrowseCtrl m_cFilePath;
   CArray<_VersionDetails,_VersionDetails&> m_sDetails;
   void FillTable();

public:
	CVersionDetailsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VERSIONDETAILS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedVdGet();
};
