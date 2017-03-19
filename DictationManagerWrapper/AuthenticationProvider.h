#pragma once
using namespace Microsoft::Bing::Speech;

namespace DictationManager {

ref class AuthenticationProvider : IAuthorizationProvider
{
public:
	AuthenticationProvider()
	{

	}

	virtual System::Threading::Tasks::Task<System::String^>^ GetAuthorizationTokenAsync() override;
private:
	System::String^ FetchTokenUri = "https://api.cognitive.microsoft.com/sts/v1.0";

	/// <summary>
	/// The subscription key
	/// </summary>
	System::String^ subscriptionKey = "da3f3586e91e4c27b29a03784d2f441d";

};

}

