
// SpeechRecogniztionTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SpeechRecogniztionTest.h"
#include "SpeechRecogniztionTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSpeechRecogniztionTestDlg dialog



CSpeechRecogniztionTestDlg::CSpeechRecogniztionTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SPEECHRECOGNIZTIONTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSpeechRecogniztionTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAY_TEXT, m_display);
}

BEGIN_MESSAGE_MAP(CSpeechRecogniztionTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CSpeechRecogniztionTestDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// CSpeechRecogniztionTestDlg message handlers

BOOL CSpeechRecogniztionTestDlg::OnInitDialog()
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

	m_dictationInstance = LoadLibrary(L"WindowsSpeechRecogniztionWrapper.dll");

	if (m_dictationInstance == nullptr)
	{
		DWORD errorCode;
		errorCode = GetLastError();
		WCHAR error[200];
		_itow_s(errorCode, error, 10);
		MessageBox(L"Failed to Load Dll");
		MessageBox(error);
		return true;
	}

	CREATEDICTATION fpnCreate = (CREATEDICTATION) GetProcAddress(m_dictationInstance, "CreateSpeechRecognitionWrapper");
	if (fpnCreate == nullptr || fpnCreate == INVALID_HANDLE_VALUE)
	{
		MessageBox(L"Failed to get procaddress");
	}


	if (!fpnCreate(m_dictationManager) || m_dictationManager == nullptr)
	{
		MessageBox(L"Failed To create Dictation Manager");
	}

	std::function<void(BSTR)> callback = [&](BSTR text)
	{
		setText(text);
	};

	m_dictationManager->SetCallback(callback);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSpeechRecogniztionTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSpeechRecogniztionTestDlg::setText(BSTR text)
{
	int nLength = m_display.GetWindowTextLength();
	m_display.SetSel(nLength, nLength);
	m_display.ReplaceSel((WCHAR*) text);

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSpeechRecogniztionTestDlg::OnPaint()
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
HCURSOR CSpeechRecogniztionTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSpeechRecogniztionTestDlg::OnBnClickedStart()
{
	m_dictationManager->StartDictation();
}
