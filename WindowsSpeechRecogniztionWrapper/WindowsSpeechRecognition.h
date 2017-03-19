#pragma once
#include <ppltasks.h>
#include <windows.globalization.h>
#include <windows.media.speechrecognition.h>
#include <wrl.h>
#include "SpeechRecognizition.h"


// Creates an agile callback function. This means that the completion callback can
//	be called on the async thread rather than the originating thread. It is important
//	to remember that the default behavior for the callback template is to create
//	a proxied callback which must execute on the originating thread.
template<typename TDelegateInterface, typename TCallback>
::Microsoft::WRL::ComPtr<TDelegateInterface> MakeAgileCallback(TCallback callback) throw()
{
	return ::Microsoft::WRL::Callback <
		::Microsoft::WRL::Implements <
		::Microsoft::WRL::RuntimeClassFlags<::Microsoft::WRL::ClassicCom>,
		TDelegateInterface,
		::Microsoft::WRL::FtmBase >> (callback);
}

// Creates an regular callback function. This means that the completion callback is
//	called on the originating thread. 
template<typename TDelegateInterface, typename TCallback>
::Microsoft::WRL::ComPtr<TDelegateInterface> MakeCallback(TCallback callback) throw()
{
	return ::Microsoft::WRL::Callback <
		::Microsoft::WRL::Implements <
		::Microsoft::WRL::RuntimeClassFlags<::Microsoft::WRL::ClassicCom>,
		TDelegateInterface >> (callback);
}

using namespace ABI::Windows::Media::SpeechRecognition;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
using namespace ABI::Windows::Foundation;

namespace WindowsSpeechRecogniztionWrapper {

class SpeechRecognizitionWrapper : public ISpeechRecognizitionWrapper
{

	bool StartDictation();

	void SetCallback(std::function<void(BSTR)>&fn)
	{
		m_callback = fn;
	}

private:
	bool EnsureRecognizer();
	HRESULT onSpeechResultGenerated(ISpeechContinuousRecognitionSession* session, ISpeechContinuousRecognitionResultGeneratedEventArgs* args);
	HRESULT OnSessionCompleted(ISpeechContinuousRecognitionSession* session, ISpeechContinuousRecognitionCompletedEventArgs* args);
	void StartRecognizer(_In_ IAsyncOperation<SpeechRecognitionCompilationResult*> * result, AsyncStatus status);
	ComPtr<ISpeechRecognizer> recognizer;
	concurrency::cancellation_token_source m_RecognitionCancellationTokenSource;
	concurrency::cancellation_token_source m_CompilationCancellationTokenSource;
	EventRegistrationToken m_speechRecognitionSessionResultGeneratedToken;
	EventRegistrationToken m_speechRecognitionCompleted;
	EventRegistrationToken m_speechRecognitionStatChanged;
	std::function<void(BSTR)> m_callback;
};

}
