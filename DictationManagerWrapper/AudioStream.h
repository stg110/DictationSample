#pragma once
#include <atlbase.h>
#include <gcroot.h>
#include <vector>

using namespace System::IO;
using namespace ATL;

namespace DictationManager {

ref class ManagedAudioStream : public Stream
{
public:
	ManagedAudioStream(IStream* pStrem);
	int Read(array<unsigned char>^ buffer, int offset, int count) override;
	void Write(array<unsigned char>^ buffer, int offset, int count) override;
	void Flush() override;
	long long Seek(long long offset, System::IO::SeekOrigin origin) override;
	void SetLength(long long value) override;
	property bool CanRead
	{
		bool get() override
		{
			return true;
		}
	}

	property bool CanWrite
	{
		bool get() override
		{
			return false;
		}
	}

	property bool CanSeek
	{
		bool get() override
		{
			return false;
		}
	}

	property long long Length {
		long long get() override
		{
			return 0;
		}
	}

	property long long Position {
		long long get() override
		{
			return 0;
		}

		void set(long long) override
		{

		}
	}

	void Stop()
	{
		m_endStream = true;
	}


private:
	IStream* m_spNativeStream;
	bool m_endStream = false;
};

class Lock
{
public:
	Lock(void* syncObject)
	{
		m_syncObject = System::IntPtr(syncObject);
		System::Threading::Monitor::Enter(m_syncObject);
	}

	~Lock()
	{
		System::Threading::Monitor::Exit(m_syncObject);
	}

	System::IntPtr m_syncObject;
};

class AudioStream : public IStream
{
public:
	AudioStream()
	{
		m_managedStream = gcnew ManagedAudioStream(this);
		m_writeBuffer.resize(4000);
	}

	~AudioStream()
	{
		m_managedStream->Stop();
	}

	HRESULT STDMETHODCALLTYPE Seek(
		/* [in] */ LARGE_INTEGER dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *plibNewPosition);

	HRESULT STDMETHODCALLTYPE SetSize(
		/* [in] */ ULARGE_INTEGER libNewSize);

	HRESULT STDMETHODCALLTYPE CopyTo(
		/* [annotation][unique][in] */
		_In_  IStream *pstm,
		/* [in] */ ULARGE_INTEGER cb,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *pcbRead,
		/* [annotation] */
		_Out_opt_  ULARGE_INTEGER *pcbWritten);

	HRESULT STDMETHODCALLTYPE Commit(
		/* [in] */ DWORD grfCommitFlags);

	HRESULT STDMETHODCALLTYPE Revert(void);

	HRESULT STDMETHODCALLTYPE LockRegion(
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType);

	HRESULT STDMETHODCALLTYPE UnlockRegion(
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType);

	HRESULT STDMETHODCALLTYPE Stat(
		/* [out] */ __RPC__out tagSTATSTG *pstatstg,
		/* [in] */ DWORD grfStatFlag);

	HRESULT STDMETHODCALLTYPE Clone(
		/* [out] */ __RPC__deref_out_opt IStream **ppstm);

	gcroot<ManagedAudioStream^> GetManagedStream()
	{
		return m_managedStream;
	}

private:
	std::vector<char> m_writeBuffer;
	std::vector<char> m_readBuffer;
	size_t m_writeBufferSize = 0;
	size_t m_readBufferSize = 0;
	size_t m_totalBytesWritten = 0;
	size_t m_totalBytesRead = 0;
	size_t m_currentReadOffset = 0;
	size_t m_currentTotalReadOffset = 0;

	gcroot<ManagedAudioStream^> m_managedStream;
	ULONG m_cRef = 0;
	const size_t c_InitialBufferSize = 4000;

	bool UpdateReadBuffer();
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);


	virtual ULONG STDMETHODCALLTYPE AddRef(void);


	virtual ULONG STDMETHODCALLTYPE Release(void);


	virtual HRESULT STDMETHODCALLTYPE Read(_Out_writes_bytes_to_(cb, *pcbRead) void *pv, _In_ ULONG cb, _Out_opt_ ULONG *pcbRead);


	virtual HRESULT STDMETHODCALLTYPE Write(_In_reads_bytes_(cb) const void *pv, _In_ ULONG cb, _Out_opt_ ULONG *pcbWritten);

};

};