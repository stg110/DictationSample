#include "stdafx.h"
#include "AuthenticationProvider.h"

using namespace System::Net::Http;

using namespace System::Threading::Tasks;

namespace DictationManager {



System::Threading::Tasks::Task<System::String^>^ AuthenticationProvider::GetAuthorizationTokenAsync()
{
	HttpClient^ client = gcnew HttpClient();

	client->DefaultRequestHeaders->Add("Ocp-Apim-Subscription-Key", subscriptionKey);
	System::UriBuilder^ uriBuilder = gcnew System::UriBuilder(FetchTokenUri);
	uriBuilder->Path += "/issueToken";

	Task<System::Net::Http::HttpResponseMessage^>^ result = client->PostAsync(uriBuilder->Uri->AbsoluteUri, nullptr);
	result->Wait();

	Task<System::String^>^ token = result->GetAwaiter().GetResult()->Content->ReadAsStringAsync();
	token->Wait();

	return token;
}

}