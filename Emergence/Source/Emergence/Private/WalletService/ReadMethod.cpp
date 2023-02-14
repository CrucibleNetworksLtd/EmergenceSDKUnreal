// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/ReadMethod.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "EmergenceChainObject.h"
#include "WalletService/LoadContractInternal.h"
#include "EmergenceSingleton.h"

UReadMethod* UReadMethod::ReadMethod(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FEmergenceContractMethod MethodName, TArray<FString> Content)
{
	UReadMethod* BlueprintNode = NewObject<UReadMethod>();
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->MethodName = MethodName;
	BlueprintNode->Content = Content;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UReadMethod::LoadContractCompleted(FString Response, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		this->Activate();
	}
	else {
		OnReadMethodCompleted.Broadcast(FJsonObjectWrapper(), StatusCode);
	}
}

void UReadMethod::Activate()
{
	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	//if this contract has never had its ABI loaded...
	if (!Singleton->ContractsWithLoadedABIs.Contains(DeployedContract->Blockchain->Name.ToString() + DeployedContract->Address)) {
		ULoadContractInternal* LoadContract = ULoadContractInternal::LoadContract(WorldContextObject, DeployedContract);
		LoadContract->OnLoadContractCompleted.AddDynamic(this, &UReadMethod::LoadContractCompleted);
		LoadContract->Activate();
		return;
	}

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

	UHttpHelperLibrary::ExecuteHttpRequest<UReadMethod>(
		this, 
		&UReadMethod::ReadMethod_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "readMethod?contractAddress=" + DeployedContract->Address + "&network=" + DeployedContract->Blockchain->Name.ToString().Replace(TEXT(" "), TEXT("")) + "&methodName=" + MethodName.MethodName + "&nodeUrl=" + DeployedContract->Blockchain->NodeURL,
		"POST",
		60.0F,
		Headers,
		ContentString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("ReadMethod request started with JSON, calling ReadMethod_HttpRequestComplete on request completed. Json sent as part of the request: "));
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *ContentString);
}

void UReadMethod::ReadMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("ReadMethod_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {	
		FJsonObjectWrapper OutJsonObject;
		OutJsonObject.JsonObject = JsonObject.GetObjectField("message");
		OnReadMethodCompleted.Broadcast(OutJsonObject, EErrorCode::EmergenceOk);
	}
	else {
		OnReadMethodCompleted.Broadcast(FJsonObjectWrapper(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("ReadMethod", StatusCode);
	}
	SetReadyToDestroy();
}