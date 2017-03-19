
// TestAudioRecordingDlg.h : header file
//

#pragma once
#include "..\MicrophoneLib\MicrophoneSource.h"
#include <fstream>
using namespace std;

class OutStrem : public IStream
{
public:
	OutStrem()
	{
		file.open("audio.wav", ios::binary | ios::out);
	}
	HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ LARGE_INTEGER dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *plibNewPosition)
	{
		DWORD offset = dlibMove.QuadPart;
		file.seekp(offset, std::ios_base::beg);

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetSize(
		/* [in] */ ULARGE_INTEGER libNewSize)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE CopyTo(
		/* [annotation][unique][in] */
		_In_  IStream *pstm,
		/* [in] */ ULARGE_INTEGER cb,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *pcbRead,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *pcbWritten)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Commit(
		/* [in] */ DWORD grfCommitFlags)
	{
		file.flush();
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Revert(void)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE LockRegion(
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE UnlockRegion(
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE Stat(
		/* [out] */ __RPC__out STATSTG *pstatstg,
		/* [in] */ DWORD grfStatFlag)
	{
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Clone(
		/* [out] */ __RPC__deref_out_opt IStream **ppstm)
	{
		return E_NOTIMPL;
	}

	HRESULT Read(
		 void  *pv,
		  ULONG cb,
		 ULONG *pcbRead
	)
	{
		return E_NOTIMPL;
	}

	HRESULT Write(
		  void const *pv,
		  ULONG      cb,
		 ULONG      *pcbWritten
	)
	{
		*pcbWritten = cb;
		file.write((char*) pv, cb);
		return S_OK;
	}


	~OutStrem()
	{
		file.close();
	}

	HRESULT QueryInterface(REFIID   riid,
		LPVOID * ppvObj)
	{
		return E_NOINTERFACE;
	}

	ULONG AddRef()
	{
		InterlockedIncrement(&m_cRef);
		return m_cRef;
	}
	ULONG Release()
	{
		// Decrement the object's internal counter.
		ULONG ulRefCount = InterlockedDecrement(&m_cRef);
		if (0 == m_cRef)
		{
			delete this;
		}
		return ulRefCount;
	}
private:
	fstream file;
	ULONG m_cRef = 0;
};

// CTestAudioRecordingDlg dialog
class CTestAudioRecordingDlg : public CDialogEx
{
// Construction
public:
	CTestAudioRecordingDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTAUDIORECORDING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	MicrophoneSource m_microphone;
	CComPtr<IStream> m_spStream;


	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
};
