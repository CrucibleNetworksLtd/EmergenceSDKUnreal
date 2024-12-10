// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#include "WebLogin/CustodialLogin.h"
#include "HttpPath.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerModule.h"
#include "HttpServerResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Misc/Base64.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "WalletService/WriteMethod.h"
#include "JwtVerifier.h"
#include "SHA256Hash.h"
#include "Containers/ArrayView.h"
#include "EmergenceEVMServerSubsystem.h"
#include <random>
THIRD_PARTY_INCLUDES_START
#include <openssl/rand.h>
THIRD_PARTY_INCLUDES_END
bool UCustodialLogin::_isServerStarted = false;
FHttpRouteHandle UCustodialLogin::RouteHandle = nullptr;
const UObject* UCustodialLogin::ContextObject = nullptr;
FString UCustodialLogin::code;
FString UCustodialLogin::state;

FString UCustodialLogin::GetClientID()
{
	//This function is to get the FV client ID, based on the SDKs Futureverse environment.
	//There is one for Development and Staging, and one for Production. We provide a default client ID for each,
	//but developers can set their own in the Project Settings.
	auto Singleton = UEmergenceSingleton::GetEmergenceManager(UCustodialLogin::ContextObject);
	
	check(Singleton);

	EFutureverseEnvironment Env = Singleton->GetFutureverseEnvironment();
	if (Env == EFutureverseEnvironment::Development || Env == EFutureverseEnvironment::Staging) {
		FString DefaultFutureverseWebLoginStagingEnvClientID = "3KMMFCuY59SA4DDV8ggwc"; //default staging client id to be overriden
		FString FutureverseWebLoginStagingEnvClientID;
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseWebLoginStagingEnvClientID"), FutureverseWebLoginStagingEnvClientID, GGameIni);
		if (FutureverseWebLoginStagingEnvClientID.IsEmpty()) {
			return DefaultFutureverseWebLoginStagingEnvClientID;
		}
		else {
			return FutureverseWebLoginStagingEnvClientID;
		}
	}
	else {
		FString DefaultFutureverseWebLoginProductionEnvClientID = "G9mOSDHNklm_dCN0DHvfX"; //default production client id to be overriden
		FString FutureverseWebLoginProductionEnvClientID;
		GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("FutureverseWebLoginStagingEnvClientID"), FutureverseWebLoginProductionEnvClientID, GGameIni);
		if (FutureverseWebLoginProductionEnvClientID.IsEmpty()) {
			return DefaultFutureverseWebLoginProductionEnvClientID;
		}
		else {
			return FutureverseWebLoginProductionEnvClientID;
		}
	}
}

UCustodialLogin* UCustodialLogin::CustodialLogin(const UObject* WorldContextObject)
{
	//Blueprint Async Action Base initialization
	UCustodialLogin* BlueprintNode = NewObject<UCustodialLogin>();
	UCustodialLogin::ContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(const_cast<UObject*>(WorldContextObject));
	return BlueprintNode;
}

FString UCustodialLogin::CleanupBase64ForWeb(FString Input)
{
	//This function takes a normal base64 string (containing "+"s and "/"s, potentially ending with a number of "="s...
	//and transforms it so it can be used in a web URL.
	//To do this, any "+"s are replaced with "-", and any "/"s are replaced with "_"
	//any "="s are removed from the end.

	Input.ReplaceCharInline('+', '-');
	Input.ReplaceCharInline('/', '_');
	for (int i = 0; i < Input.Len(); i++) {
		if (Input[i] == '=') {
			Input.RemoveAt(i);
			i--;
		}
	}

	return Input;
}

void UCustodialLogin::Activate()
{	
	//start up the HTTP endpoint
	//@TODO consider moving to a service with the ones from CustodialSignMessage and CustodialWriteTransaction
	FHttpServerModule& httpServerModule = FHttpServerModule::Get(); 
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);
	auto Singleton = UEmergenceSingleton::GetEmergenceManager(UCustodialLogin::ContextObject);
	
	auto EmergenceSub = UGameplayStatics::GetGameInstance(UCustodialLogin::ContextObject)->GetSubsystem<UEmergenceEVMServerSubsystem>();

	if (httpRouter.IsValid() && !UCustodialLogin::_isServerStarted) //server isn't started
	{
#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5) //if we're 5.4 or up
		FHttpRequestHandler Handler;
		Handler.BindLambda([this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleAuthRequestCallback(Req, OnComplete); });
		UCustodialLogin::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/callback")), EHttpServerRequestVerbs::VERB_GET, Handler);

#else
		UCustodialLogin::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleAuthRequestCallback(Req, OnComplete); });
#endif
		httpServerModule.StartAllListeners();
		
		UCustodialLogin::_isServerStarted = true;
		UE_LOG(LogEmergenceHttp, Display, TEXT("Web server started on port = 3000"));
	}
	else if (UCustodialLogin::_isServerStarted) { //server is already started
		UE_LOG(LogEmergenceHttp, Display, TEXT("Web already started on port = 3000"));
	}
	else //couldn't get httpRouter
	{
		UCustodialLogin::_isServerStarted = false;
		UE_LOG(LogEmergenceHttp, Error, TEXT("Could not start web server on port = 3000"));
		Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}

	//create a new state for this interaction
	UCustodialLogin::state = GetSecureRandomBase64();

	//Create a random string to be the "code"
	UCustodialLogin::code = GetSecureRandomBase64();

	//Create a SHA256 of code to be the "code_challange"
	FSHA256Hash SHA256Hash;
	SHA256Hash.FromString(UCustodialLogin::code);

	//convert the output of SHA256Hash.GetHash() from a hex number to a base64 number
	TArray<uint8> UtfChar;
	for (size_t Index_Chars = 0; Index_Chars < SHA256Hash.GetHash().Len(); Index_Chars += 2)
	{
		FString Part = SHA256Hash.GetHash().Mid(Index_Chars, 2);
		char Character = std::stoul(TCHAR_TO_ANSI(*Part), nullptr, 16);
		UtfChar.Add(Character);
	}
	FString CodeChallenge = CleanupBase64ForWeb(FBase64::Encode(UtfChar));

	TArray<TPair<FString, FString>> UrlParams({
		TPair<FString, FString>{"response_type", "code"},
		TPair<FString, FString>{"client_id", UCustodialLogin::GetClientID()},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"scope", "openid"},
		TPair<FString, FString>{"code_challenge", CodeChallenge},
		TPair<FString, FString>{"code_challenge_method", "S256"},
		TPair<FString, FString>{"response_mode", "query"},
		TPair<FString, FString>{"prompt", "login"}, //switch "login" to "none" to attempt to skip having a new login
		TPair<FString, FString>{"state", FString(UCustodialLogin::state)},
		TPair<FString, FString>{"nonce", GetSecureRandomBase64()},
		//TPair<FString, FString>{"login_hint", "social%3Agoogle"} //send the user directly to google, etc
		});

	//Encode the params in a GET request style
	FString URL = UHttpHelperLibrary::GetFutureverseAuthURL() + "/auth?";
	for (int i = 0; i < UrlParams.Num(); i++) {
		URL += UrlParams[i].Key;
		URL += "=";
		URL += UrlParams[i].Value;
		if (i != UrlParams.Num() - 1) {
			URL += "&";
		}
	}

	//Open the auth URL with the params
	FString Error;
	FPlatformProcess::LaunchURL(*URL, nullptr, &Error);
	if (!Error.IsEmpty()) { //if there was an error launching the browser (I've never actually seen this happen, but its good to handle the error)
		UE_LOG(LogEmergence, Display, TEXT("LaunchURL: failed, %s"), *Error);
		Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}
}

void UCustodialLogin::BeginDestroy() //@TODO this function in theory shouldn't be necessary 
{
	UEmergenceAsyncSingleRequestBase::BeginDestroy(); //call parent destroy
}

bool UCustodialLogin::HandleAuthRequestCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogEmergence, Display, TEXT("HandleRequestCallback"));
	TUniquePtr<FHttpServerResponse> response = GetHttpPage();
	UHttpHelperLibrary::RequestPrint(Req); //debug logging for the request sent to our local server

	auto Singleton = UEmergenceSingleton::GetEmergenceManager(UCustodialLogin::ContextObject);

	if (!Req.QueryParams.Contains("code")) { //if we don't have a "code" in the query params
		UE_LOG(LogEmergence, Error, TEXT("HandleRequestCallback: No \"code\""));
		Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return true;
	}

	if (Req.QueryParams.FindRef("state") != UCustodialLogin::state) { //if we don't have a "state" in the query params
		UE_LOG(LogEmergence, Error, TEXT("HandleRequestCallback: \"state\" doesn't match. Returned state was \"%s\", we think it should be \"%s\"."), *Req.QueryParams.FindRef("state"), *UCustodialLogin::state);
		Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return true;
	}

	TArray<TPair<FString, FString>> UrlParams({ //generate up some new params to send to the token endpoint
		TPair<FString, FString>{"grant_type", "authorization_code"},
		TPair<FString, FString>{"code",* Req.QueryParams.Find("code")},
		TPair<FString, FString>{"redirect_uri", "http%3A%2F%2Flocalhost%3A3000%2Fcallback"},
		TPair<FString, FString>{"client_id", UCustodialLogin::GetClientID()},
		TPair<FString, FString>{"code_verifier", UCustodialLogin::code},
	});

	FString URL = UHttpHelperLibrary::GetFutureverseAuthURL() + "/token?"; //URL changes depending on FV environment

	//for some reason, the parameters on this request are encoded like a GET url's parameters, but then sent in a POST as part of the content, don't ask me why lol
	FString Params;
	for (int i = 0; i < UrlParams.Num(); i++) {
		Params += UrlParams[i].Key;
		Params += "=";
		Params += UrlParams[i].Value;
		if (i != UrlParams.Num() - 1) {
			Params += "&";
		}
	}
	
	TArray<TPair<FString, FString>> Headers({
		TPair<FString, FString>{"Content-Type", "application/x-www-form-urlencoded"}
	});
	const UObject* Context = UCustodialLogin::ContextObject;
	
	//send the HttpRequest
	auto HttpRequest = UHttpHelperLibrary::ExecuteHttpRequest<UCustodialLogin>(nullptr, nullptr, URL, TEXT("POST"), 60.0F, Headers, Params);
	
	//Bind Lambda to when the request has got a response
	HttpRequest->OnProcessRequestComplete().BindLambda([Context](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
		//Due to the architecture of the singleton, we're going to call some functions in the singleton when this token request is completed
		auto Singleton = UEmergenceSingleton::GetEmergenceManager(Context);
		
		UE_LOG(LogEmergence, Display, TEXT("GetTokensRequest_HttpRequestComplete"));
		UE_LOG(LogEmergence, Display, TEXT("Tokens Data: %s"), *HttpResponse->GetContentAsString());
		TSharedPtr<FJsonObject> JsonParsed;
		TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(*HttpResponse->GetContentAsString());

		if (!FJsonSerializer::Deserialize(JsonReader, JsonParsed)) //if we fail to deserialize
		{
			UE_LOG(LogEmergence, Error, TEXT("GetTokensRequest_HttpRequestComplete: Deserialize failed!"));
			//tell the singleton this custodial login has failed
			Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientJsonParseFailed);
			return;
		}

		FString IdToken;
		if (!JsonParsed->TryGetStringField("id_token", IdToken)) { //if we didn't get an ID token
			UE_LOG(LogEmergence, Error, TEXT("GetTokensRequest_HttpRequestComplete: Could not get id_token!"));
			UE_LOG(LogEmergence, Error, TEXT("code was: %s"), *UCustodialLogin::code);
			//tell the singleton this custodial login has failed
			Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientJsonParseFailed);
			return;
		}

		//try to decode the id token as a JWT
		UE_LOG(LogEmergence, Display, TEXT("id_token: %s"), *IdToken);
		TMap<FString, FString> IdTokenDecoded;
		FJwtVerifierModule JwtVerifier = FJwtVerifierModule::Get();
		IdTokenDecoded = JwtVerifier.GetClaims(IdToken);

		//if the JWT has 0 claims, meaning that it wasn't the JWT we're looking for / it failed
		if (IdTokenDecoded.Num() == 0) {

			UE_LOG(LogEmergence, Error, TEXT("IdTokenDecoded length was 0"));
			//tell the singleton this custodial login has failed
			Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(), EErrorCode::EmergenceClientInvalidResponse);
			return;
		}

		//it succeeded to get a JWT. Tell the singleton
		Singleton->CompleteLoginViaWebLoginFlow(FEmergenceCustodialLoginOutput(IdTokenDecoded), EErrorCode::EmergenceOk);
		return;
	});
	UE_LOG(LogEmergence, Display, TEXT("Sent Params Data: %s"), *Params);
	OnComplete(MoveTemp(response)); //send the HTML to the user
	return true;
}

TUniquePtr<FHttpServerResponse> UCustodialLogin::GetHttpPage()
{
	//generate a HTML page to show
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	return response;
}

FString UCustodialLogin::GetSecureRandomBase64(int Length)
{	
	//Generate some random bytes using OpenSSL's secure random bytes function
	unsigned char randomBytes[1024];
	if (RAND_bytes(randomBytes, sizeof(randomBytes)) != 1) {
		std::cerr << "Error generating random bytes." << std::endl;
	}

	TArray<uint8> Data;
	for (int i = 0; i < (Length / 2); i++) { //each these will come out to two characters, so half length
		Data.Add(randomBytes[i]);
	}

	//Make a hex string, base64 encode that, then run it through the base64 cleanup function
	return CleanupBase64ForWeb(FBase64::Encode(FString::FromHexBlob(Data.GetData(), Data.Num()))).Left(Length);
}


