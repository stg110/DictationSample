#pragma once
#include <mfidl.h>
#include <Mfapi.h>
#include <Mferror.h>
#include <Mfreadwrite.h>
#include <WinBase.h>
#include <atlbase.h>



enum MicrophoneStatus
{
	started,
};

struct IMicrophoneCallback
{
	virtual void OnMicrophoneStatus(MicrophoneStatus status)  noexcept  = 0;
};

class AudioSampleHandler : public IMFSourceReaderCallback
{
public:
	HRESULT SetStreamReader(_In_ IMFSourceReader* reader, DWORD streamIndex);
	HRESULT SetOutputStream(IStream* pStream);
	HRESULT Start();
	HRESULT Stop();
	HRESULT OnReadSample(HRESULT, DWORD, DWORD, LONGLONG, IMFSample*);

	HRESULT QueryInterface(REFIID   riid,
		LPVOID * ppvObj);
	ULONG AddRef();
	ULONG Release();
	HRESULT OnEvent(
		__in DWORD         /*dwStreamIndex*/,
		__in IMFMediaEvent */*pEvent*/
	);
	HRESULT OnFlush(
		DWORD /*dwStreamIndex*/
	);

private:
	CComPtr<IMFSourceReader> m_spReader;
	CComPtr<IMFMediaType>      m_spMediaType;
	DWORD m_steamIndex = (DWORD) MF_SOURCE_READER_FIRST_AUDIO_STREAM;
	CComPtr<IStream> m_spStreamWriter;
	bool                            m_started = false;
	WAVEFORMATEX					m_audioFormat = { 0 };

	HRESULT UpdateMediaType(_In_ CComPtr<IMFMediaType>& mediaType);
	HRESULT ReadSample();
	ULONG m_cRef = 0;
	ULONG m_TotalAudioSize = 0;
	ULONG m_headrSize = 0;
	void FixChunkSize();
};

class MicrophoneSource
{
public:
	HRESULT StartMic(IStream* pStream) noexcept;
	HRESULT StopMic();
	
	MicrophoneSource();
	~MicrophoneSource();

private:
	CComPtr<AudioSampleHandler>            m_sampleHandler;
	CComPtr<IMFMediaSource>                m_spSource;
	HRESULT Initialize() noexcept;
};

