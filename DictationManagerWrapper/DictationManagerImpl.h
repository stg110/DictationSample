#pragma once
#include "DictationManager.h"
#using <mscorlib.dll>
#using <SpeechClient.dll>
#include <gcroot.h>

using namespace System::Runtime::InteropServices;
using namespace Microsoft::CognitiveServices::SpeechRecognition;

namespace DictationManager {

class DictationManagerImpl;

public ref class DictationAdapter
{
public:
	DictationAdapter(DictationManagerImpl* pNativeManager) :
		m_pNativeManager(pNativeManager)
	{

	}

	bool EnsureMicrophoneClient();
	bool StartDictate();
	bool StopDictate();

private:
	MicrophoneRecognitionClientWithIntent^ m_micClient = nullptr;
	DictationManagerImpl* m_pNativeManager;
	const System::String^ m_subscriptionKey = "da3f3586e91e4c27b29a03784d2f441d";
	const System::String^ m_language = "en-US";
	void OnResponseReceived(System::Object ^sender, Microsoft::CognitiveServices::SpeechRecognition::SpeechResponseEventArgs ^e);
	void OnOnIntent(System::Object ^sender, Microsoft::CognitiveServices::SpeechRecognition::SpeechIntentEventArgs ^e);
	void OnOnPartialResponseReceived(System::Object ^sender, Microsoft::CognitiveServices::SpeechRecognition::PartialSpeechResponseEventArgs ^e);
};

class DictationManagerImpl : public IDictationManager
{
public:
	void RegisterCallback(std::function<void(BSTR)>& callback) noexcept;
	bool StartDictation() noexcept;
	bool StopDictation() noexcept;
		
	DictationManagerImpl()
	{
		m_adapter = gcnew DictationAdapter(this);
	}

	~DictationManagerImpl()
	{

	}
	
	std::function<void(BSTR)> m_callback;

private:
	gcroot<DictationAdapter^> m_adapter = nullptr;
	
};




}