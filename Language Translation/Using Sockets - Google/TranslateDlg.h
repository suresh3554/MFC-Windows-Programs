
// TranslateDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Translator.h"
#include "AboutDlg.h"

// CTranslateDlg dialog
class CTranslateDlg : public CDialogEx
{
// Construction
   CTranslator    m_cTranslator;

   CFont m_font;
   CFont m_Errorfont;
   CEdit m_cSourceText;
   CEdit m_cTranslatedText;
   CComboBox m_cToLang;

public:
	CTranslateDlg(CWnd* pParent = NULL);	// standard constructor
   ~CTranslateDlg();
// Dialog Data
	enum { IDD = IDD_TRANSLATE_DIALOG };
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
   afx_msg void OnBnClickedTranslateBtn();
   afx_msg void OnCbnSelchangeToCombo();
   CStatic m_cErrorText;
   afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
