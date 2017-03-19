
// DictationTestDlg.h : header file
//

#pragma once
#include "..\DictationManagerWrapper\DictationManager.h"
#include "afxwin.h"

typedef bool (CALLBACK * CREATEDICTATION)(std::unique_ptr<DictationManager::IDictationManager>&);
typedef bool (CALLBACK * TESTCALLL)();

// CDictationTestDlg dialog
class CDictationTestDlg : public CDialogEx
{
// Construction
public:
	CDictationTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DICTATIONTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HINSTANCE  m_dictationInstance = nullptr;
	std::unique_ptr<DictationManager::IDictationManager> m_dictationManager;
	std::unique_ptr<DictationManager::IDictationManager> m_dictationManagerInternal;
	void setText(BSTR text);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_DisplayTextControl;
	afx_msg void OnBnClickedStartDictate();
	afx_msg void OnBnClickedStopDictation();
	CButton m_UseInternal;
};
