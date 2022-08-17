// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "WalletService/WriteMethod.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

UWriteMethod* UWriteMethod::WriteMethod(const UObject* WorldContextObject, FString ContractAddress, FString MethodName, FString Value, TArray<FString> Content, FString LocalAccountName, FString GasPrice)
{
	UWriteMethod* BlueprintNode = NewObject<UWriteMethod>();
	BlueprintNode->ContractAddress = ContractAddress;
	BlueprintNode->MethodName = MethodName;
	BlueprintNode->Content = Content;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->LocalAccountName = LocalAccountName;
	BlueprintNode->GasPrice = GasPrice;
	BlueprintNode->Value = Value;
	return BlueprintNode;
}

void UWriteMethod::Activate()
{
	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	FString ContentString;
	ContentString.Append("[");
	for (int i = 0; i < Content.Num(); i++) {
		ContentString.Append("\"" + Content[i] + "\"");
		if (i != Content.Num() - 1) {
			ContentString.Append(",");
		}
	}
	ContentString.Append("]");

	FString GasString;
	if (GasPrice != "") {
		GasString = "&gasPrice=" + GasPrice;
	}

	UHttpHelperLibrary::ExecuteHttpRequest<UWriteMethod>(
		this, 
		&UWriteMethod::WriteMethod_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "writeMethod?contractAddress=" + ContractAddress + "&methodName=" + MethodName + "&value=" + Value + ( LocalAccountName != "" ? "&localAccountName=" + LocalAccountName : "" ) + GasString,
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		Headers,
		ContentString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("WriteMethod request started on method %s with value %s, calling WriteMethod_HttpRequestComplete on request completed. Json sent as part of the request: "), *MethodName, *Value);
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *ContentString);
}

void UWriteMethod::WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("WriteMethod_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		OnWriteMethodCompleted.Broadcast(JsonObject.GetObjectField("message")->GetStringField("transactionHash"), EErrorCode::EmergenceOk);
		return;
	}
	OnWriteMethodCompleted.Broadcast(FString(), StatusCode);
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("WriteMethod", StatusCode);
}
