// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#include "WebLogin/CustodialWriteTransaction.h"
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
#include "HttpService/HttpHelperLibrary.h"
#include "TimerManager.h"
#include "EmergenceSingleton.h"
#include "FutureverseEnvironmentLibrary.h"

bool UCustodialWriteTransaction::_isServerStarted = false;
TDelegate<void(FString, FString, EErrorCode)> UCustodialWriteTransaction::CallbackComplete;

UCustodialWriteTransaction* UCustodialWriteTransaction::CustodialWriteTransaction(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FString Method, FString Value, TArray<FString> Content)
{
	//Blueprint Async Action Base initialization
	UCustodialWriteTransaction* BlueprintNode = NewObject<UCustodialWriteTransaction>();
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->Method = Method;
	BlueprintNode->Value = Value;
	BlueprintNode->Content = Content;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UCustodialWriteTransaction::Activate()
{
	//if we don't have the bare minimum to send a write transaction, just broadcast failed and return
	if (Method.IsEmpty() || !DeployedContract) {
		UE_LOG(LogEmergence, Error, TEXT("Could not do CustodialWriteTransaction, param invalid!"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceClientFailed);
		return;
	}

	//start to set up a http callback server
	//@TODO consider creating a service for doing this for this function and the other custodials that require callbacks
	FHttpServerModule& httpServerModule = FHttpServerModule::Get();
	TSharedPtr<IHttpRouter> httpRouter = httpServerModule.GetHttpRouter(3000);

	if (httpRouter.IsValid() && !UCustodialWriteTransaction::_isServerStarted) //if we don't have a server already
	{

#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5) //if we're on 5.4 or higher
		FHttpRequestHandler Handler;
		Handler.BindLambda([this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
		httpRouter->BindRoute(FHttpPath(TEXT("/write-callback")), EHttpServerRequestVerbs::VERB_GET, Handler);

#else
		httpRouter->BindRoute(FHttpPath(TEXT("/write-callback")), EHttpServerRequestVerbs::VERB_GET,
			[this](const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete) { return HandleSignatureCallback(Req, OnComplete); });
#endif

		httpServerModule.StartAllListeners();

		UCustodialWriteTransaction::_isServerStarted = true;
		UE_LOG(LogEmergence, Log, TEXT("Web server started on port = 3000"));

		FTimerDelegate TimerCallback;
		/*since we seemingly need to wait for the port to open (its async) 
		but theres no way to know when thats happened without rewriting the whole
		of the httpserver module, adding a 1 second delay to make fully sure its open. 
		1 second should be way more than is nessacery.*/
		TimerCallback.BindLambda([&]
		{
			GetEncodedData(); //go to the next function in the sequence
		});
		FTimerHandle Handle;
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(Handle, TimerCallback, 1.0f, false);
	}
	else if (UCustodialWriteTransaction::_isServerStarted) { //if the server is already started
		UE_LOG(LogEmergence, Log, TEXT("Web already started on port = 3000"));
		GetEncodedData(); //go to the next function in the sequence
	}
	else //if there isn't a server and we couldn't get httprouter
	{
		UCustodialWriteTransaction::_isServerStarted = false;
		UE_LOG(LogEmergence, Error, TEXT("Could not start web server on port = 3000"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}
}

void UCustodialWriteTransaction::GetEncodedData()
{
	//First we make a json object that has data about the transactions "data" parameter we want to have encoded
	TSharedPtr<FJsonObject> JsonToSend = MakeShared<FJsonObject>();
	JsonToSend->SetStringField("ABI", DeployedContract->Contract->ABI);
	JsonToSend->SetStringField("ContractAddress", DeployedContract->Address);
	JsonToSend->SetStringField("MethodName", Method);
	TArray<TSharedPtr<FJsonValue>> CallInputsJsonArray;
	for (FString Param : Content) {
		CallInputsJsonArray.Add(MakeShared<FJsonValueString>(Param));
	}
	JsonToSend->SetArrayField("CallInputs", CallInputsJsonArray);

	//Then, we turen the json object into a string
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(JsonToSend.ToSharedRef(), Writer);
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedData()\n%s"), *OutputString);

	//Then we send this json to our server
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::GetEncodedData_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "getEncodedData",
		"POST",
		60.0F,
		Headers,
		OutputString);
}

void UCustodialWriteTransaction::GetEncodedData_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedData_HttpRequestComplete:\n%s"), *HttpResponse->GetContentAsString());

	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedDataJson = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);

	if (StatusCode == EErrorCode::EmergenceOk) { //if the response comes back okay, as parseable JSON
		FString Data = GetEncodedDataJson.GetObjectField("message")->GetStringField("Data"); //we get the message data field
		FString ChainID = FString::FromInt(DeployedContract->Blockchain->ChainID);
		FString ContractAddress = DeployedContract->Address;
		auto Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
		//and we get the parameters together to encode the entire transaction, not just the transaction's data parameter

		if (Singleton) {
			//Encode the entire transaction
			EncodeTransaction(Singleton->GetCachedAddress(true), ChainID, ContractAddress, Value, Data, DeployedContract->Blockchain->NodeURL);
		}
	}
	else { //if its not valid json, or we had a http error
		UE_LOG(LogEmergence, Error, TEXT("GetEncodedData_HttpRequestComplete: Failed!"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), StatusCode);
		return;
	}
}

void UCustodialWriteTransaction::EncodeTransaction(FString Eoa, FString ChainId, FString ToAddress, FString InputValue, FString Data, FString InputRpcUrl)
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	//defensive programming, these should always be correct by this point but just in-case they arn't
	if (Eoa.IsEmpty() ||
		ChainId.IsEmpty() ||
		ToAddress.IsEmpty() ||
		InputValue.IsEmpty() ||
		Data.IsEmpty() ||
		InputRpcUrl.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("One of the required inputs to EncodeTransaction() was empty."));
		UE_LOG(LogEmergence, Error, TEXT("Eoa = %s"), *Eoa);
		UE_LOG(LogEmergence, Error, TEXT("ChainId = %s"), *ChainId);
		UE_LOG(LogEmergence, Error, TEXT("ToAddress = %s"), *ToAddress);
		UE_LOG(LogEmergence, Error, TEXT("InputValue = %s"), *InputValue);
		UE_LOG(LogEmergence, Error, TEXT("Data = %s"), *Data);
		UE_LOG(LogEmergence, Error, TEXT("InputRpcUrl = %s"), *InputRpcUrl);
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}

	//construct the json to be sent to the entire transcaction encoder
	TSharedPtr<FJsonObject> JsonInputs = MakeShared<FJsonObject>();
	JsonInputs->SetStringField("eoa", Eoa);
	JsonInputs->SetStringField("chainId", ChainId);
	JsonInputs->SetStringField("toAddress", ToAddress);
	JsonInputs->SetStringField("value", InputValue);
	JsonInputs->SetStringField("data", Data);
	JsonInputs->SetStringField("rpcUrl", InputRpcUrl);
	JsonInputs->SetStringField("environment", UFutureverseEnvironmentLibrary::GetFVEnvironment());

	//serialize the JSON into a string
	FString JsonInputsString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&JsonInputsString);
	FJsonSerializer::Serialize(JsonInputs.ToSharedRef(), Writer);
	UE_LOG(LogEmergence, Display, TEXT("JsonInputsString: %s"), *JsonInputsString);

	//call the encode-transaction endpoint, calls GetEncodedPayload_HttpRequestComplete when done
	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete,
		UFutureverseEnvironmentLibrary::GetFVHelperServiceURL() + "encode-transaction",
		"POST",
		300.0F,
		Headers,
		JsonInputsString);
}

void UCustodialWriteTransaction::TransactionEnded()
{
	SetReadyToDestroy(); //Unhook this BlueprintAsyncActionBase from the game instance and then destory its UObject
}

void UCustodialWriteTransaction::GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload_HttpRequestComplete"));
	UE_LOG(LogEmergence, Display, TEXT("Transaction data: %s"), *HttpResponse->GetContentAsString());

	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedPayloadResponse = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	
	if (StatusCode != EErrorCode::EmergenceOk) { //if there were HTTP error codes and the response wasn't parsable as JSON
		UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload_HttpRequestComplete: failed"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), StatusCode);
		return;
	}

	FString SignerUrl = GetEncodedPayloadResponse.GetStringField("fullSignerUrl"); //get the full signer URL
	RawTransactionWithoutSignature = *GetEncodedPayloadResponse.GetObjectField("rawTransactionWithoutSignature").Get(); //and the raw transaction
	if (SignerUrl.IsEmpty()) //if we don't have a signer URL
	{
		UE_LOG(LogEmergence, Display, TEXT("GetEncodedPayload_HttpRequestComplete: failed, signer URL empty."));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}
		
	//bind to the callback which will be called back by the URL we're about to launch in the users browser
	CallbackComplete.BindLambda([&](FString Signature, FString EOA, EErrorCode ErrorCode) { //this is the callback triggered by the handler of the below launchURL
		if(ErrorCode == EErrorCode::EmergenceOk){ //if the callback came back alright
			SendTranscation(Signature, EOA); //send the transaction
			return;
		}
		else { //if the callback comes back with an error
			UE_LOG(LogEmergence, Warning, TEXT("GetEncodedPayload_HttpRequestComplete: failed."));
			OnCustodialWriteTransactionCompleted.Broadcast(FString(), ErrorCode);
			return;
		}
	});

	FString Error;
	FPlatformProcess::LaunchURL(*SignerUrl, nullptr, &Error); //launch the URL in the users browser
	if (!Error.IsEmpty()) { //if it fails to launch (this has never happened but its good to try to handle the error)
		UE_LOG(LogEmergence, Display, TEXT("LaunchURL: failed, %s"), *Error);
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}
	
}

bool UCustodialWriteTransaction::HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete)
{
	UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback"));

	TUniquePtr<FHttpServerResponse> response = GetHttpPage();
	OnComplete(MoveTemp(response)); //for this one we don't need to keep the user's browser waiting for a page, so send the page now

	FString ResponseBase64 = *Req.QueryParams.Find("response"); //find the "response" query param
	FString ResponseJsonString;
	FBase64::Decode(ResponseBase64, ResponseJsonString); //decode it to Base64


	UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback ResponseJsonString: %s"), *ResponseJsonString);
	TSharedPtr<FJsonObject> JsonParsed;
	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(ResponseJsonString);
	if (!FJsonSerializer::Deserialize(JsonReader, JsonParsed)) //if we're NOT able to deserialize the JSON sent to the callback
	{
		UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback: Deserialize failed!"));
		CallbackComplete.Execute(FString(), FString(), EErrorCode::EmergenceClientJsonParseFailed);
		return true;
	}

	if (JsonParsed->GetObjectField("result")->GetStringField("status") != "error") { //if this wasn't an error
		//Get the signature:
		FString Signature = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("signature");
		//and the EOA:
		FString EOA = JsonParsed->GetObjectField("payload")->GetStringField("account");

		UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback ResponseJsonString: OnCustodialSignMessageComplete"));
		//and call our delegate with EmergenceOk
		CallbackComplete.Execute(Signature, EOA, EErrorCode::EmergenceOk);
	}
	else { //if this was an error
		//Get the error string:
		FString ErrorString = JsonParsed->GetObjectField("result")->GetObjectField("data")->GetStringField("error");
		UE_LOG(LogEmergence, Display, TEXT("HandleSignatureCallback error: %s"), *ErrorString);
		if (ErrorString == "USER_REJECTED") {
			//if specifically the user rejected this request, give it a special error code so that devs can handle this specifially
			CallbackComplete.Execute(FString(), FString(), EErrorCode::EmergenceClientUserRejected);
		}
		else {
			//otherwise give it a generic error code
			CallbackComplete.Execute(FString(), FString(), EErrorCode::ServerError);
		}
	}
	return true;
}

void UCustodialWriteTransaction::SendTranscation(FString Signature, FString EOA)
{
	//if the RawTransactionWithoutSignature is blank, stop early and error
	if (RawTransactionWithoutSignature.Values.Num() == 0) {
		UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback: RawTransactionWithoutSignature was null."));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}

	FString RpcUrl = DeployedContract->Blockchain->NodeURL;

	//if other required values are blank, stop early and error
	if (Signature.IsEmpty() || EOA.IsEmpty() || RpcUrl.IsEmpty()) {
		UE_LOG(LogEmergence, Error, TEXT("HandleSignatureCallback: one of Signature, EOA or RpcUrl was empty."));
		UE_LOG(LogEmergence, Error, TEXT("Signature = %s"), *Signature);
		UE_LOG(LogEmergence, Error, TEXT("EOA = %s"), *EOA);
		UE_LOG(LogEmergence, Error, TEXT("RpcUrl = %s"), *RpcUrl);
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError);
		return;
	}

	//start building the transaction json, including some that we saved earlier in RawTransactionWithoutSignature
	TSharedPtr<FJsonObject> RawTransactionObject = MakeShared<FJsonObject>();
	RawTransactionObject->SetObjectField("rawTransactionWithoutSignature", MakeShared<FJsonObject>(RawTransactionWithoutSignature));
	RawTransactionObject->SetStringField("transactionSignature", Signature);
	RawTransactionObject->SetStringField("fromEoa", EOA);
	RawTransactionObject->SetStringField("rpcUrl", RpcUrl);

	//turn the JSON object into a json string
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(RawTransactionObject.ToSharedRef(), Writer);
	UE_LOG(LogEmergence, Display, TEXT("RawTransactionObject: %s"), *OutputString);

	//send the transaction to the blockchain via send-transaction
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	UHttpHelperLibrary::ExecuteHttpRequest<UCustodialWriteTransaction>(
		this,
		&UCustodialWriteTransaction::SendTransaction_HttpRequestComplete,
		UFutureverseEnvironmentLibrary::GetFVHelperServiceURL() + "send-transaction",
		"POST",
		300.0F, //Give the server a bit longer to send it, as we're waiting for the blockchain here
		Headers,
		OutputString);
}

void UCustodialWriteTransaction::SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	UE_LOG(LogEmergence, Display, TEXT("SendTransaction_HttpRequestComplete"));
	UE_LOG(LogEmergence, Display, TEXT("SendTransaction_HttpRequestComplete data: %s"), *HttpResponse->GetContentAsString());
	EErrorCode StatusCode = EErrorCode::EmergenceClientFailed;
	FJsonObject GetEncodedPayloadResponse = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);

	if (StatusCode == EErrorCode::EmergenceOk) { //if the response is valid json
		FString Hash;
		if (GetEncodedPayloadResponse.TryGetStringField("hash", Hash)){ //hash exists and its a string field
			UE_LOG(LogEmergence, Display, TEXT("SendTransaction_HttpRequestComplete hash: %s"), *Hash);
			OnCustodialWriteTransactionCompleted.Broadcast(Hash, EErrorCode::EmergenceOk); //broadcast a success
			TransactionEnded(); //call transaction ended to close up this UObject
			return;
		}
		else { //we caught an error
			TSharedPtr<FJsonValue> ErrorField = GetEncodedPayloadResponse.TryGetField("error");
			FString Reason = ErrorField->AsObject()->GetObjectField("error")->GetStringField("reason"); //get the reason as a string
			UE_LOG(LogEmergence, Display, TEXT("SendTransaction_HttpRequestComplete failed: %s"), *Reason); //display the reason
			OnCustodialWriteTransactionCompleted.Broadcast(FString(), EErrorCode::EmergenceInternalError); //send an error
			return;
		}
	}
	else {
		UE_LOG(LogEmergence, Error, TEXT("SendTransaction_HttpRequestComplete: failed!"));
		OnCustodialWriteTransactionCompleted.Broadcast(FString(), StatusCode);
		return;
	}
}

TUniquePtr<FHttpServerResponse> UCustodialWriteTransaction::GetHttpPage()
{
	//generate a HTML page to show
	TUniquePtr<FHttpServerResponse> response = FHttpServerResponse::Create(TEXT("You may now close this window..."), TEXT("text/html"));
	return response;
}