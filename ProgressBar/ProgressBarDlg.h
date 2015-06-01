
// ProgressBarDlg.h : header file
//

#pragma once
#include "afxcmn.h"

// CProgressBarDlg dialog
class CProgressBarDlg : public CDialogEx
{
// Construction
   CProgressCtrl m_cProgressBar;
   CProgressCtrl m_cProgressBarMarquee;

public:
	CProgressBarDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROGRESSBAR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedPbarColor();
   afx_msg void OnBnClickedPbarStart();
   afx_msg void OnBnClickedPbarStop();
   afx_msg void OnTimer(UINT_PTR nIDEvent);
};
