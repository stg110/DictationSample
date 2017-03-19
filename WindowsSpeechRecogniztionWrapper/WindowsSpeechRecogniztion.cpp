#include "stdafx.h"
#include "WindowsSpeechRecognition.h"
#include <roapi.h>

namespace WindowsSpeechRecogniztionWrapper {

using namespace ABI::Windows::Globalization;

bool SpeechRecognizitionWrapper::EnsureRecognizer()
{
	if (recognizer == nullptr)
	{
		ComPtr<IInspectable> pInst;

		HRESULT hr = RoActivateInstance(HStringReference(L"Windows.Media.SpeechRecognition.SpeechRecognizer").Get(), &pInst);
		if (FAILED(hr))
			return false;

		if (FAILED(pInst.As(&recognizer)))
			return false;
	}

	return true;
}

bool SpeechRecognizitionWrapper::StartDictation()
{
	if (!EnsureRecognizer())
		return false;

	ComPtr<IAsyncOperation<SpeechRecognitionCompilationResult*> > asyncOpCompile;
	
	if(FAILED(recognizer->CompileConstraintsAsync(&asyncOpCompile)))
	{
		return false;
	}

	auto callback = MakeAgileCallback<IAsyncOperationCompletedHandler<SpeechRecognitionCompilationResult*>>([this](_In_ IAsyncOperation<SpeechRecognitionCompilationResult*> * result, AsyncStatus status)
	{
		this->StartRecognizer(result, status);
		return S_OK;
	});

	
	asyncOpCompile->put_Completed(callback.Get());
	
	return true;
}

void SpeechRecognizitionWrapper::StartRecognizer(_In_ IAsyncOperation<SpeechRecognitionCompilationResult*> * result, AsyncStatus status)
{
	if (status == AsyncStatus::Error || status == AsyncStatus::Canceled)
		return;

	if (status == AsyncStatus::Completed)
	{
		ISpeechRecognitionCompilationResult* compileResult;
		result->GetResults(&compileResult);

		SpeechRecognitionResultStatus resultStatus;
		compileResult->get_Status(&resultStatus);
		
		if (resultStatus == SpeechRecognitionResultStatus::SpeechRecognitionResultStatus_Success)
		{


			ComPtr<ISpeechContinuousRecognitionSession> session;
			ComPtr<ISpeechRecognizer2> recognizer2;
			HRESULT hr = recognizer.As(&recognizer2);

			ComPtr<ILanguage> language;
			recognizer->get_CurrentLanguage(&language);

			recognizer->add_StateChanged(Microsoft::WRL::Callback<ITypedEventHandler
				<SpeechRecognizer*, SpeechRecognizerStateChangedEventArgs*>>([this](ISpeechRecognizer*, ISpeechRecognizerStateChangedEventArgs* args) -> HRESULT
			{
				SpeechRecognizerState state;
				args->get_State(&state);
				return S_OK;
			}).Get(), &m_speechRecognitionStatChanged);

			hr = recognizer2->get_ContinuousRecognitionSession(&session);

			session->add_ResultGenerated(
				Microsoft::WRL::Callback<ITypedEventHandler<SpeechContinuousRecognitionSession*, SpeechContinuousRecognitionResultGeneratedEventArgs*> >(
					[this](ISpeechContinuousRecognitionSession* session, ISpeechContinuousRecognitionResultGeneratedEventArgs* args) -> HRESULT
			{
				return onSpeechResultGenerated(session, args);
			}).Get(),
				&m_speechRecognitionSessionResultGeneratedToken);

			session->add_Completed(Microsoft::WRL::Callback<ITypedEventHandler<SpeechContinuousRecognitionSession*,
				SpeechContinuousRecognitionCompletedEventArgs*>>([this](ISpeechContinuousRecognitionSession* sender, 
					ISpeechContinuousRecognitionCompletedEventArgs* args) -> HRESULT
			{
				return OnSessionCompleted(sender, args);

			}).Get(), &m_speechRecognitionCompleted);

			SpeechRecognizerState state;
			recognizer2->get_State(&state);

			if (state == SpeechRecognizerState::SpeechRecognizerState_Idle)
			{

				ComPtr<IAsyncAction> asyncAction;
				hr = session->StartAsync(&asyncAction);

			}

			if (FAILED(hr))
			{
				 
			}
		}
	}

}

HRESULT SpeechRecognizitionWrapper::onSpeechResultGenerated(ISpeechContinuousRecognitionSession* session, ISpeechContinuousRecognitionResultGeneratedEventArgs* args)
{
	ComPtr<ISpeechRecognitionResult> result;
	args->get_Result(&result);

	HSTRING text;
	result->get_Text(&text);

	unsigned int cchStr = 0;
	LPCWSTR lpwzStr = WindowsGetStringRawBuffer(text, &cchStr);
	BSTR ret = SysAllocStringLen(lpwzStr, cchStr);
	m_callback(ret);

	return S_OK;
}

HRESULT SpeechRecognizitionWrapper::OnSessionCompleted(ISpeechContinuousRecognitionSession* session, ISpeechContinuousRecognitionCompletedEventArgs* args)
{
	SpeechRecognitionResultStatus status;
	args->get_Status(&status);

	if (status == SpeechRecognitionResultStatus::SpeechRecognitionResultStatus_TimeoutExceeded)
	{
		return S_FALSE;
	}
}

extern "C" __declspec(dllexport) bool CALLBACK  CreateSpeechRecognitionWrapper(std::unique_ptr<ISpeechRecognizitionWrapper>& spSpeechREcognition) noexcept
{
	spSpeechREcognition = std::unique_ptr<ISpeechRecognizitionWrapper>(new SpeechRecognizitionWrapper());
	return true;
}

}