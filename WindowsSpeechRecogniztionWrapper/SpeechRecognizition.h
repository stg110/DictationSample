#pragma once
#include <functional>
#include <wtypes.h>
#include <memory>

namespace WindowsSpeechRecogniztionWrapper {
	struct ISpeechRecognizitionWrapper
	{
		virtual bool StartDictation() = 0;

		virtual void SetCallback(std::function<void(BSTR)>&fn) = 0;
	};

	extern "C" __declspec(dllexport) bool CALLBACK  CreateSpeechRecognitionWrapper(std::unique_ptr<ISpeechRecognizitionWrapper>& spSpeechREcognition) noexcept;
}


