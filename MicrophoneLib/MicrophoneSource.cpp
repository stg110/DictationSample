#include "stdafx.h"
#include "MicrophoneSource.h"
#include <mutex>

MicrophoneSource::MicrophoneSource()
{
}


MicrophoneSource::~MicrophoneSource()
{
}

HRESULT MicrophoneSource::StartMic(IStream* pStream) noexcept
{
	m_sampleHandler = nullptr;

	HRESULT hr = Initialize();

	if (SUCCEEDED(hr))
	{
		hr = m_sampleHandler->SetOutputStream(pStream);

		if (SUCCEEDED(hr))
		{
			m_sampleHandler->Start();
		}
	}

	return hr;

}

std::recursive_mutex            m_lock;


HRESULT MicrophoneSource::Initialize() noexcept
{
	static bool s_fMFInitizlized = false;
	if (!s_fMFInitizlized)
	{
		s_fMFInitizlized = true;
		MFStartup(MF_VERSION);
	}
	HRESULT                         hr = S_OK;
	CComPtr<IMFAttributes>          spAttr;
	CComPtr<IMFSourceReader>   spReader;
	

	hr = MFCreateAttributes(&spAttr, 1);
	
	UINT count;
	IMFActivate **ppDevices = NULL;

	if (SUCCEEDED(hr))
	{
		hr = spAttr->SetGUID(
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE,
			MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID
		);
	}

		if (SUCCEEDED(hr))
		{
			hr = MFEnumDeviceSources(spAttr, &ppDevices, &count);
		}


		for (int i = 0; i < count; ++i)
		{
			hr = ppDevices[i]->ActivateObject(IID_PPV_ARGS(&m_spSource));
			
			if(SUCCEEDED(hr))
			{
				break;
			}
		}

		
		for (DWORD i = 0; i < count; i++)
		{
			ppDevices[i]->Release();
		}
		CoTaskMemFree(ppDevices);

		if (SUCCEEDED(hr))
		{
			m_sampleHandler = new AudioSampleHandler();
			hr = spAttr->DeleteAllItems();
			hr = spAttr->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, static_cast<IMFSourceReaderCallback*>(m_sampleHandler.p));

			if (SUCCEEDED(hr))
			{
				hr = MFCreateSourceReaderFromMediaSource(m_spSource, spAttr, &spReader);
			}

			// disable all streams by default
			if (SUCCEEDED(hr))
			{
				hr = spReader->SetStreamSelection((DWORD) MF_SOURCE_READER_ALL_STREAMS, FALSE);
			}

			// select the first audio stream
			m_sampleHandler->SetStreamReader(spReader, (DWORD) MF_SOURCE_READER_FIRST_AUDIO_STREAM);
		}

Exit:
	return hr;
}

#define BITS_PER_SECOND     16
#define CHANNELS            1
#define SAMPLES_PER_SECOND  16000
#define BLOCK_ALIGN         2


HRESULT
AudioSampleHandler::UpdateMediaType(_In_ CComPtr<IMFMediaType>& mediaType)
{
	HRESULT hr = S_OK;

	mediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, BITS_PER_SECOND);
	mediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, SAMPLES_PER_SECOND);
	mediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, CHANNELS);
	mediaType->SetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, BLOCK_ALIGN);

	return hr;
}

HRESULT AudioSampleHandler::SetStreamReader(_In_ IMFSourceReader* reader, DWORD streamIndex)
{
	HRESULT                 hr = S_OK;
	CComPtr<IMFMediaType>   spPartialType;
	m_spMediaType = nullptr;

	m_spReader = reader;

	// Create a partial media type that specifies uncompressed PCM audio.
	hr = MFCreateMediaType(&spPartialType);
	spPartialType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	spPartialType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	
	UpdateMediaType(spPartialType);

	// Set this type on the source reader. The source reader will
	// load the necessary decoder.
	m_spReader->SetCurrentMediaType(streamIndex, NULL, spPartialType.p);

	// Get the complete uncompressed format.
	m_spReader->GetCurrentMediaType(streamIndex, &m_spMediaType);

	// Ensure the stream is selected.
	m_spReader->SetStreamSelection(streamIndex, TRUE);

	m_steamIndex = streamIndex;

	return hr;
}

HRESULT AudioSampleHandler::SetOutputStream(IStream* pStream)
{
	if (NULL == pStream)
	{
		return E_FAIL;
	}

	m_spStreamWriter = pStream;

	HRESULT hr = S_OK;
	WAVEFORMATEX wfx = { 0 };
	ULONG        bytesWritten;
	UINT32 cbFormat = 0;

	WAVEFORMATEX *pWav = NULL;

	hr = MFCreateWaveFormatExFromMFMediaType(m_spMediaType, &pWav, &cbFormat);

	m_headrSize += cbFormat;

	m_audioFormat = *pWav;

	if (SUCCEEDED(hr))
	{
		DWORD header[] = {
			// RIFF header
			FCC('RIFF'),
			0,
			FCC('WAVE'),
			// Start of 'fmt ' chunk
			FCC('fmt '),
			cbFormat
		};

		DWORD dataHeader[] = { FCC('data'), 0 };

		hr = m_spStreamWriter->Write(header, sizeof(header), &bytesWritten);

		m_headrSize += sizeof(header) + sizeof(dataHeader);


		if (SUCCEEDED(hr))
		{
			hr = m_spStreamWriter->Write(pWav, cbFormat, &bytesWritten);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_spStreamWriter->Write(dataHeader, sizeof(dataHeader), &bytesWritten);
		}
	}

	return hr;
}

HRESULT AudioSampleHandler::ReadSample()
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);
	if (m_spReader != nullptr)
	{
		return m_spReader->ReadSample(m_steamIndex, 0, nullptr, nullptr, nullptr, nullptr);
	}

	return E_FAIL;
}

void AudioSampleHandler::FixChunkSize()
{
	LARGE_INTEGER offset;
	ULARGE_INTEGER bytesMoved;
	offset.QuadPart = m_headrSize - sizeof(DWORD);
	HRESULT hr = m_spStreamWriter->Seek(offset, FILE_BEGIN, &bytesMoved);
	ULONG bytesWritten;

	if (SUCCEEDED(hr))
	{
		hr = m_spStreamWriter->Write(&m_TotalAudioSize, sizeof(DWORD), &bytesWritten);
	}

	if (SUCCEEDED(hr))
	{
		offset.QuadPart = 4;
		hr = m_spStreamWriter->Seek(offset, FILE_BEGIN, &bytesMoved);
	}

	if (SUCCEEDED(hr))
	{
		DWORD cbRiffFileSize = m_headrSize + m_TotalAudioSize - 8;
		hr = m_spStreamWriter->Write(&cbRiffFileSize, sizeof(DWORD), &bytesWritten);
	}
}

HRESULT AudioSampleHandler::Start()
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);
	if (!m_started)
	{
		m_started = true;
		return ReadSample();
	}

	return E_FAIL;
}

HRESULT AudioSampleHandler::OnReadSample(
	_In_  HRESULT           /* hrStatus */,
	_In_  DWORD             /* dwStreamIndex */,
	_In_  DWORD             /* dwStreamFlags */,
	_In_  LONGLONG          /* llTimestamp */,
	_In_opt_  IMFSample*    pSample)
{
	HRESULT hr = S_OK;
	LPBYTE  pBuffer;
	DWORD   i, bufferLen;

	if (NULL != pSample && m_started)
	{
		DWORD count = 0;
		ATL::CComPtr<IStream> outputStream;
		
		if (m_spStreamWriter)
		{
			std::lock_guard<std::recursive_mutex> lock(m_lock);
			hr = pSample->GetBufferCount(&count);
			if (SUCCEEDED(hr))
			{
				for (i = 0; i < count; i++)
				{
					CComPtr<IMFMediaBuffer> buffer;
					hr = pSample->GetBufferByIndex(i, &buffer);
					if (SUCCEEDED(hr))
					{
						hr = buffer->Lock(&pBuffer, NULL, &bufferLen);
						if (FAILED(hr))
						{
							break;
						}
					}
					
					hr = m_spStreamWriter->Write(pBuffer, bufferLen, &bufferLen);
					m_TotalAudioSize += bufferLen;

					hr = buffer->Unlock();
				}
			}
		}
	}

	hr = ReadSample();

	return hr;
}

HRESULT AudioSampleHandler::QueryInterface(REFIID   riid,
	LPVOID * ppvObj)
{
	// Always set out parameter to NULL, validating it first.
	if (!ppvObj)
		return E_INVALIDARG;
	*ppvObj = NULL;
	if (riid == IID_IUnknown || riid == IID_IMFSourceReaderCallback)
	{
		// Increment the reference count and return the pointer.
		*ppvObj = (LPVOID)this;
		AddRef();
		return NOERROR;
	}
	return E_NOINTERFACE;
}

ULONG AudioSampleHandler::AddRef()
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}
ULONG AudioSampleHandler::Release()
{
	// Decrement the object's internal counter.
	ULONG ulRefCount = InterlockedDecrement(&m_cRef);
	if (0 == m_cRef)
	{
		delete this;
	}
	return ulRefCount;
}

HRESULT AudioSampleHandler::OnEvent(
	__in DWORD         /*dwStreamIndex*/,
	__in IMFMediaEvent */*pEvent*/
)
{
	return S_OK;
}

HRESULT AudioSampleHandler::OnFlush(
	DWORD /*dwStreamIndex*/
)
{
	return S_OK;
}

HRESULT AudioSampleHandler::Stop()
{
	std::lock_guard<std::recursive_mutex> lock(m_lock);
	HRESULT hr = S_OK;

	if (m_started)
	{
		if (m_spStreamWriter != nullptr)
		{
			FixChunkSize();
			hr = m_spStreamWriter->Commit(STGC_OVERWRITE | STGC_CONSOLIDATE);
		}


		m_started = false;
	}

	return hr;
}

HRESULT MicrophoneSource::StopMic()
{
	if (m_sampleHandler != nullptr)
	{
		m_sampleHandler->Stop();
		m_sampleHandler = nullptr;
	}

	if (m_spSource != nullptr)
	{
		m_spSource->Stop();
		m_spSource = nullptr;
	}

	return S_OK;
}