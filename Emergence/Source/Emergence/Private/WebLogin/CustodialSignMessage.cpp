// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#include "WebLogin/CustodialSignMessage.h"
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
#include <sstream>
#include <iomanip>
#include <string>

bool UCustodialSignMessage::_isServerStarted = false;
FHttpRouteHandle UCustodialSignMessage::RouteHandle = nullptr;
TDelegate<void(FString, EErrorCode)> UCustodialSignMessage::CallbackComplete;

UCustodialSignMessage* UCustodialSignMessage::CustodialSignMessage(UObject* WorldContextObject, FString FVCustodialEOA, FString Message)
{
	//Blueprint Async Action Base initialization
	UCustodialSignMessage* BlueprintNode = NewObject<UCustodialSignMessage>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->FVCustodialEOA = FVCustodialEOA;
	BlueprintNode->Message = Message;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCustodialSignMessage::BeginDestroy() //uobject is being destroyed 
{
	//cleanup the static httpserver endpoint:
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);

	if (httpRouter.IsValid() && UCustodialSignMessage::RouteHandle && !UCustodialSignMessage::_isServerStarted)
	{
		httpRouter->UnbindRoute(UCustodialSignMessage::RouteHandle);
		UCustodialSignMessage::_isServerStarted = false;
	}

	OnCustodialSignMessageComplete.Unbind(); //remove all bindings from the httpserver endpoint's delegate
	UEmergenceAsyncSingleRequestBase::BeginDestroy(); //call parent destroy
}

void UCustodialSignMessage::SetReadyToDestroy() //blueprintasyncactionbase is being removed from game instance.
{ //@TODO check if "BeginDestroy" is enough to handle all of this or if this is still required
	//cleanup the static httpserver endpoint:
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);

	if (httpRouter.IsValid() && UCustodialSignMessage::RouteHandle && !UCustodialSignMessage::_isServerStarted)
	{
		httpRouter->UnbindRoute(UCustodialSignMessage::RouteHandle);
		UCustodialSignMessage::_isServerStarted = false;
	}

	OnCustodialSignMessageComplete.Unbind(); //remove all bindings from the httpserver endpoint's delegate
	UEmergenceAsyncSingleRequestBase::SetReadyToDestroy(); //call parent destroy
}

void UCustodialSignMessage::Activate()
{
	//if we don't have the basic inputs, don't continue
	if (FVCustodialEOA.IsEmpty() || Message.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("Could not do CustodialSignMessage, param invalid! EOA was \"%s\", message was \"%s\""), *FVCustodialEOA, *Message);
		OnCustodialSignMessageComplete.ExecuteIfBound(FString(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}

	//start up the HTTP endpoint
	//@TODO consider moving to a service with the ones from CustodialLogin and CustodialWriteTransaction
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);

	if (httpRouter.IsValid() && !UCustodialSignMessage::_isServerStarted) //server isn't started
	{

#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5) //if we're 5.4 or up
		FHttpRequestHandler Handler;
		Handler.BindLambda([this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
		UCustodialSignMessage::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/signature-callback")), EHttpServerRequestVerbs::VERB_GET, Handler);

#else
		UCustodialSignMessage::RouteHandle = httpRouter->BindRoute(FHttpPath(TEXT("/signature-callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
#endif

		httpServerModule.StartAllListeners();
		
		LaunchSignMessageURL(); //Move to the next step, opening the users browser

		UCustodialSignMessage::_isServerStarted = true;
		UE_LOG(LogEmergence, Display, TEXT("Web server started on port = 3000"));

	}
	else if (UCustodialSignMessage::_isServerStarted) { //server was already started
		UE_LOG(LogEmergence, Display, TEXT("Web already started on port = 3000"));
		LaunchSignMessageURL(); //Move to the next step, opening the users browser
	}
	else //httpRouter.IsValid() wasn't valid
	{
		UCustodialSignMessage::_isServerStarted = false;
		UE_LOG(LogEmergence, Error, TEXT("Could not start web server on port = 3000"));
		OnCustodialSignMessageComplete.ExecuteIfBound(FString(), EErrorCode::EmergenceClientFailed);
		SetReadyToDestroy();
		return;
	}
}

void UCustodialSignMessage::LaunchSignMessageURL()
{
	//this segment is to do the same thing as ""0x" + Encoding.UTF8.GetBytes(value).ToHex()" in C#. Make sure if you implement this that it matches that output exactly.
	#ifdef __llvm__
	#pragma GCC diagnostic ignored "-Wdangling"
	#endif
	const char* UTF8Message = TCHAR_TO_UTF8(*Message);
	std::ostringstream oss;
	for (size_t i = 0; i < Message.Len(); ++i) {
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(UTF8Message[i]);
	}
	std::string UTF8MessageHex = "0x" + oss.str();

	//Make the Json to send to the signer
	TSharedPtr<FJsonObject> SignTransactionPayloadJsonObject = MakeShareable(new FJsonObject);
	SignTransactionPayloadJsonObject->SetStringField("account", *FVCustodialEOA);
	SignTransactionPayloadJsonObject->SetStringField("message", UTF8MessageHex.c_str());
	SignTransactionPayloadJsonObject->SetStringField("callbackUrl", "http://localhost:3000/signature-callback");
	SignTransactionPayloadJsonObject->SetStringField("idpUrl", UHttpHelperLibrary::GetFutureverseAuthURL());

	TSharedPtr<FJsonObject> EncodedPayloadJsonObject = MakeShareable(new FJsonObject);
	EncodedPayloadJsonObject->SetStringField("id", "client:2"); //must be formatted as `client:${ an identifier number }`
	EncodedPayloadJsonObject->SetStringField("tag", "fv/sign-msg"); //do not change this
	EncodedPayloadJsonObject->SetObjectField("payload", SignTransactionPayloadJsonObject);
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(EncodedPayloadJsonObject.ToSharedRef(), Writer);
	UE_LOG(LogEmergence, Display, TEXT("Message to sign is: %s"), *Message);
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload OutputString: %s"), *OutputString);
	FString Base64Encode = FBase64::Encode(OutputString);
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload Base64Encode: %s"), *Base64Encode);

	//Construct the URL
	FString URL = UHttpHelperLibrary::GetFutureverseSignerURL() + "?request=" + Base64Encode;
	UCustodialSignMessage::CallbackComplete.BindLambda([&](FString SignedMessage, EErrorCode Error) { //bind something for when we get a callback from the users browser
		OnCustodialSignMessageComplete.ExecuteIfBound(SignedMessage, Error);
	});

	FString Error;
	FPlatformProcess::LaunchURL(*URL, nullptr, &Error);
	if (!Error.IsEmpty()) { //if there was an error launching the browser (I've never actually seen this happen, but its good to handle the error)
		UE_LOG(LogEmergence, Display, TEXT("LaunchURL: failed, %s"), *Error);
		OnCustodialSignMessageComplete.ExecuteIfBound(FString(), EErrorCode::EmergenceInternalError);
		return;
	}
}

bool UCustodialSignMessage::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback"));
	UHttpHelperLibrary::RequestPrint(Req); //debug logging for the request sent to our local server
	TUniquePtr<FHttpServerResponse> response = GetHttpPage();
	OnComplete(MoveTemp(response)); //Send the user's browser the page we wanna show. We don't need to keep it waiting.


	FString ResponseBase64 = *Req.QueryParams.Find("response"); //get the response from the query params
	FString ResponseJsonString;
	FBase64::Decode(ResponseBase64, ResponseJsonString); //Decode it from base64 to JSON

	UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback ResponseJsonString: %s"), *ResponseJsonString);
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseJsonString);
	if (FJsonSerializer::Deserialize(JsonReader, JsonParsed)) //if its actually JSON
	{
		TSharedPtr<FJsonObject> ResultObject;
		if (JsonParsed->GetObjectField("result")->GetStringField("status") != "error") { //if this was a normal result, not an error
			FString Signature = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("signature");
			UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback ResponseJsonString: OnCustodialSignMessageComplete"));
			CallbackComplete.ExecuteIfBound(Signature, EErrorCode::EmergenceOk); //call the delegate
		}
		else { //if this was an error
			FString ErrorString = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("error");
			UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback error: %s"), *ErrorString);
			if (ErrorString == "USER_REJECTED") { //special handling for if the error is that the user rejected it, not any other sort of error
				//this is handled specially as the developer will want to know about this specific situation to handle differently
				CallbackComplete.ExecuteIfBound(FString(), EErrorCode::EmergenceClientUserRejected); //call the delegate
			}
			else {
				CallbackComplete.ExecuteIfBound(FString(), EErrorCode::ServerError); //call the delegate
			}
		}
	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback: Deserialize failed!"));
		CallbackComplete.ExecuteIfBound(FString(), EErrorCode::EmergenceClientJsonParseFailed); //call the delegate
	}

	return true;
}

TUniquePtr<FHttpServerResponse> UCustodialSignMessage::GetHttpPage()
{
	//generate a HTML page to show
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	return response;
}