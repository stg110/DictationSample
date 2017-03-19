#include "stdafx.h"
#include "DictationManagerImpl.h"


namespace DictationManager {


extern "C" __declspec(dllexport) bool CALLBACK  CreateDictationManager(std::unique_ptr<IDictationManager>& spDictationManager) noexcept
{
	spDictationManager = std::unique_ptr<IDictationManager>(new (std::nothrow) DictationManagerImpl());
	if (spDictationManager != nullptr)
	{
		return true;
	}

	return false;
}

extern "C" __declspec(dllexport) bool CALLBACK  Test() noexcept
{
	return false;
}

void DictationManagerImpl::RegisterCallback(std::function<void(BSTR)>& callback) noexcept
{
	m_callback = callback;
}

bool DictationManagerImpl::StartDictation() noexcept
{
	return m_adapter->StartDictate();
}

bool DictationManagerImpl::StopDictation() noexcept
{
	return m_adapter->StopDictate();
}

bool DictationAdapter::EnsureMicrophoneClient()
{
	if (this->m_micClient != nullptr)
		return true;
	
	m_micClient = SpeechRecognitionServiceFactory::CreateMicrophoneClientWithIntent(
		"en-US", "da3f3586e91e4c27b29a03784d2f441d", "36d44b6a-261b-4685-b55e-e5659a90ead9", "b2016659706d4e03bb8dab2ea91aed24");

	if (m_micClient == nullptr)
		return false;

	m_micClient->AuthenticationUri = "";
	m_micClient->OnResponseReceived += gcnew System::EventHandler<Microsoft::CognitiveServices::SpeechRecognition::SpeechResponseEventArgs ^>(this, &DictationManager::DictationAdapter::OnResponseReceived);
	m_micClient->OnIntent += gcnew System::EventHandler<Microsoft::CognitiveServices::SpeechRecognition::SpeechIntentEventArgs ^>(this, &DictationManager::DictationAdapter::OnOnIntent);
	m_micClient->OnPartialResponseReceived += gcnew System::EventHandler<Microsoft::CognitiveServices::SpeechRecognition::PartialSpeechResponseEventArgs ^>(this, &DictationManager::DictationAdapter::OnOnPartialResponseReceived);
	return true;
}

void DictationAdapter::OnResponseReceived(System::Object ^sender, Microsoft::CognitiveServices::SpeechRecognition::SpeechResponseEventArgs ^e)
{
	if (e->PhraseResponse->RecognitionStatus == RecognitionStatus::RecognitionSuccess)
	{
		System::IntPtr recString = Marshal::StringToBSTR(e->PhraseResponse->Results[0]->DisplayText);
		m_pNativeManager->m_callback((BSTR)recString.ToPointer());
	}
}

bool DictationAdapter::StartDictate()
{
	if (!EnsureMicrophoneClient())
		return false;

	m_micClient->StartMicAndRecognition();
	return true;
}

bool DictationAdapter::StopDictate()
{
	if (m_micClient != nullptr)
	{
		m_micClient->EndMicAndRecognition();
	}

	return true;
}

}


void DictationManager::DictationAdapter::OnOnIntent(System::Object ^sender, Microsoft::CognitiveServices::SpeechRecognition::SpeechIntentEventArgs ^e)
{
	System::String^ intent = e->Intent->Body;
}


void DictationManager::DictationAdapter::OnOnPartialResponseReceived(System::Object ^sender, Microsoft::CognitiveServices::SpeechRecognition::PartialSpeechResponseEventArgs ^e)
{

}
