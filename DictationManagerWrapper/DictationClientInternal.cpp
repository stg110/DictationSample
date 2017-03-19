#include "stdafx.h"
#include "DictationClientInternal.h"
#include "AuthenticationProvider.h"
#include "AudioStream.h"


using namespace System::Runtime::InteropServices;
using namespace  System::Threading;

namespace DictationManager {

extern "C" __declspec(dllexport) bool CALLBACK  CreateDictationManagerInternal(std::unique_ptr<IDictationManager>& spDictationManager) noexcept
{
	spDictationManager = std::unique_ptr<IDictationManager>(new (std::nothrow) DictationClientInternal());
	if (spDictationManager != nullptr)
	{
		return true;
	}

	return false;
}

DictationClientInternal::DictationClientInternal()
{
	m_managedClient = gcnew ManagedDictationClientInternal(this, &m_microphone);
}


DictationClientInternal::~DictationClientInternal()
{
}

void DictationClientInternal::RegisterCallback(std::function<void(BSTR) >& callback) noexcept
{
	m_callback = callback;
}

bool DictationClientInternal::StartDictation() noexcept
{
	return m_managedClient->StartDictation();
}

bool DictationClientInternal::StopDictation() noexcept
{
	return m_managedClient->StopDictation();
}

bool ManagedDictationClientInternal::StartDictation()
{
	if (!m_started)
	{
		m_speechClient = gcnew SpeechClient(CreatePreferences());
		AudioStream* pAudioStream = new AudioStream();
		RequestMetadata^ metadata = gcnew RequestMetadata(System::Guid::NewGuid(),
			gcnew DeviceMetadata(DeviceType::Near, DeviceFamily::Desktop, NetworkType::Unknown, OsName::Windows, "1607", "Dell", "T3600"),
			gcnew ApplicationMetadata("SampleApp", "1.0.0"),"SampleAppService");
		SpeechInput^ input = gcnew SpeechInput(pAudioStream->GetManagedStream(), metadata);
		System::Func<RecognitionResult^, Task^>^ func = gcnew System::Func<RecognitionResult ^, Task ^>(this, &ManagedDictationClientInternal::OnRecognitionResult);
		m_speechClient->SubscribeToRecognitionResult(func);
		m_microphone->StartMic(pAudioStream);
		m_cts = gcnew System::Threading::CancellationTokenSource();
		m_speechClient->RecognizeAsync(input, m_cts->Token);
		m_started = true;
	}

	return true;
}

Task^ ManagedDictationClientInternal::OnRecognitionResult(RecognitionResult^ args)
{
	if (args->RecognitionStatus == RecognitionStatus::Cancelled || args->RecognitionStatus == RecognitionStatus::InitialSilenceTimeout
		|| args->RecognitionStatus == RecognitionStatus::PhraseSilenceTimeout)
	{
		StopDictation();
	}

	if (args->RecognitionStatus == RecognitionStatus::Success)
	{
		System::IntPtr recString = Marshal::StringToBSTR(args->Phrases[0]->DisplayText);
		m_nativeClient->m_callback((BSTR) recString.ToPointer());
	}

	return Task::FromResult(true);
}

bool ManagedDictationClientInternal::StopDictation()
{
	if (m_started)
	{
		m_microphone->StopMic();
		m_speechClient = nullptr;
		m_started = false;
	}
	return false;
}

Preferences^ ManagedDictationClientInternal::CreatePreferences()
{
	Preferences^ pref = gcnew Preferences("en-us", gcnew System::Uri("wss://speech.platform.bing.com/api/service/recognition/continuous"), gcnew AuthenticationProvider(), true);
	return pref;
}

}
