
// SpeechRecogniztionTestDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "..\WindowsSpeechRecogniztionWrapper\SpeechRecognizition.h"

using namespace WindowsSpeechRecogniztionWrapper;
typedef bool (CALLBACK * CREATEDICTATION)(std::unique_ptr<ISpeechRecognizitionWrapper>&);

// CSpeechRecogniztionTestDlg dialog
class CSpeechRecogniztionTestDlg : public CDialogEx
{
// Construction
public:
	CSpeechRecogniztionTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SPEECHRECOGNIZTIONTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HINSTANCE  m_dictationInstance = nullptr;
	std::unique_ptr<ISpeechRecognizitionWrapper> m_dictationManager;
	void setText(BSTR text);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_display;
	afx_msg void OnBnClickedStart();
};
