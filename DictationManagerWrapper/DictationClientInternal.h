#pragma once
#include "DictationManager.h"
#include <gcroot.h>
#include "..\MicrophoneLib\MicrophoneSource.h"

using namespace Microsoft::Bing::Speech;
using namespace System::Threading::Tasks;

namespace DictationManager {

class DictationClientInternal;

public ref class ManagedDictationClientInternal
{
public:
	ManagedDictationClientInternal(DictationClientInternal* pNativClient, MicrophoneSource* microphone) : 
		m_nativeClient(pNativClient),
		m_microphone(microphone)
	{

	}
	bool StartDictation();
	Task^ OnRecognitionResult(RecognitionResult^ args);
	bool StopDictation();
private:
	DictationClientInternal* m_nativeClient;
	SpeechClient^ m_speechClient = nullptr;
	bool m_started = false;
	Preferences^ CreatePreferences();
	MicrophoneSource* m_microphone;
	System::Threading::CancellationTokenSource^ m_cts;
	
};


class DictationClientInternal : public IDictationManager
{
public:
	DictationClientInternal();
	~DictationClientInternal();

	virtual void RegisterCallback(std::function<void(BSTR) >& callback) noexcept override;
	virtual bool StopDictation() noexcept override;
	virtual bool StartDictation() noexcept override;


	std::function<void(BSTR) > m_callback;

private:
	gcroot<ManagedDictationClientInternal^> m_managedClient;
	MicrophoneSource m_microphone;
};
}

