#pragma once
#include <functional>
#include <memory>
#include <WTypes.h>

namespace DictationManager {
struct IDictationManager
{
	virtual void RegisterCallback(std::function<void(BSTR)>& callback) noexcept = 0;
	virtual bool StartDictation() noexcept = 0;
	virtual bool StopDictation() noexcept = 0;
};


extern "C" __declspec(dllexport) bool CALLBACK  CreateDictationManager(std::unique_ptr<IDictationManager>& spDictationManager) noexcept;
extern "C" __declspec(dllexport) bool CALLBACK  CreateDictationManagerInternal(std::unique_ptr<IDictationManager>& spDictationManager) noexcept;
extern "C" __declspec(dllexport) bool CALLBACK  Test() noexcept;
}
