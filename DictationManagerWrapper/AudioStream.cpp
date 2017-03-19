#include "stdafx.h"
#include "AudioStream.h"

using namespace System::Runtime::InteropServices;

namespace DictationManager {
ManagedAudioStream::ManagedAudioStream(IStream* pStrem)
{
	m_spNativeStream = pStrem;
}

int ManagedAudioStream::Read(array<unsigned char>^ buffer, int offset, int count)
{
	if (!m_endStream)
	{
		std::vector<char> nativeBuffer;
		nativeBuffer.resize(count);
		ULONG numberofBytes = 0;
		HRESULT hr = m_spNativeStream->Read((void*) nativeBuffer.data(), count, &numberofBytes);

		if (SUCCEEDED(hr))
		{
			System::IntPtr source((void*) nativeBuffer.data());
			Marshal::Copy(source, buffer, 0, numberofBytes);
			return numberofBytes;
		}
	}

	return 0;
}

void ManagedAudioStream::Write(array<unsigned char>^ buffer, int offset, int count)
{
	throw gcnew System::NotImplementedException();
}

void ManagedAudioStream::Flush()
{
	throw gcnew System::NotImplementedException();
}

long long ManagedAudioStream::Seek(long long offset, System::IO::SeekOrigin origin)
{
	throw gcnew System::NotImplementedException();
}

void ManagedAudioStream::SetLength(long long value)
{
	throw gcnew System::NotImplementedException();
}

HRESULT AudioStream::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * plibNewPosition)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::SetSize(ULARGE_INTEGER libNewSize)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::CopyTo(IStream * pstm, ULARGE_INTEGER cb, ULARGE_INTEGER * pcbRead, ULARGE_INTEGER * pcbWritten)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::Commit(DWORD grfCommitFlags)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::Revert(void)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::Stat(tagSTATSTG * pstatstg, DWORD grfStatFlag)
{
	return E_NOTIMPL;
}

HRESULT AudioStream::Clone(IStream ** ppstm)
{
	return E_NOTIMPL;
}

bool AudioStream::UpdateReadBuffer()
{
	System::Threading::Monitor::Enter(m_managedStream);

	bool ret = true;

	m_readBufferSize = m_writeBufferSize;

	if (m_readBufferSize == 0)
	{
		ret = false;
	}

	if (ret)
	{
		if (m_readBufferSize > m_readBuffer.size())
		{
			m_readBuffer.resize(m_writeBufferSize);
		}

		m_currentReadOffset = 0;
		memcpy_s(m_readBuffer.data(), m_readBuffer.size(), m_writeBuffer.data(), m_writeBufferSize);
		m_writeBufferSize = 0;
	}
	System::Threading::Monitor::Exit(m_managedStream);
	return ret;
}

HRESULT STDMETHODCALLTYPE AudioStream::QueryInterface(REFIID riid, void **ppvObject)
{
	return E_NOTIMPL;
}

ULONG STDMETHODCALLTYPE AudioStream::AddRef(void)
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG STDMETHODCALLTYPE AudioStream::Release(void)
{
	ULONG ulRefCount = InterlockedDecrement(&m_cRef);
	if (0 == m_cRef)
	{
		delete this;
	}
	return ulRefCount;
}

HRESULT STDMETHODCALLTYPE AudioStream::Read(_Out_writes_bytes_to_(cb, *pcbRead) void *pv, _In_ ULONG cb, _Out_opt_ ULONG *pcbRead)
{
	if (pv == nullptr || pcbRead == nullptr)
	{
		return E_INVALIDARG;
	}

	if (m_readBufferSize == 0 || m_currentReadOffset >= m_readBufferSize)
	{
		UINT retryCount = 10;
		do 
		{
			if (UpdateReadBuffer())
			{
				break;
			}

			Sleep(200);
		} while (retryCount--);
		
	}

	if (m_readBufferSize > 0)
	{
		size_t bytesToRead = min(cb, m_readBufferSize - m_currentReadOffset);
		memcpy_s(pv, cb, m_readBuffer.data()+m_currentReadOffset, bytesToRead);
		*pcbRead = static_cast<ULONG>(bytesToRead);
		m_currentReadOffset += bytesToRead;
		m_totalBytesRead += bytesToRead;
		return S_OK;

	}

	return E_FAIL;

}

HRESULT STDMETHODCALLTYPE AudioStream::Write(_In_reads_bytes_(cb) const void *pv, _In_ ULONG cb, _Out_opt_ ULONG *pcbWritten)
{
	System::Threading::Monitor::Enter(m_managedStream);

	if (pv == nullptr || pcbWritten == nullptr)
	{
		return E_INVALIDARG;
	}

	if (cb + m_writeBufferSize > m_writeBuffer.size())
	{
		m_writeBuffer.resize(cb + m_writeBufferSize+c_InitialBufferSize);
	}

	memcpy_s((void*) (m_writeBuffer.data() + m_writeBufferSize), m_writeBuffer.size(), pv, cb);
	m_writeBufferSize += cb;
	*pcbWritten = cb;

	m_totalBytesWritten += cb;
	
	System::Threading::Monitor::Exit(m_managedStream);
	return S_OK;
}

}
