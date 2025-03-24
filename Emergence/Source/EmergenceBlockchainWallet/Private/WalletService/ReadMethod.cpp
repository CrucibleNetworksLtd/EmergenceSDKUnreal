// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/ReadMethod.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Types/EmergenceChain.h"
#include "WalletService/LoadContractInternal.h"
#include "EmergenceSingleton.h"

UReadMethod* UReadMethod::ReadMethod(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FEmergenceContractMethod MethodName, TArray<FString> Parameters)
{
	UReadMethod* BlueprintNode = NewObject<UReadMethod>();
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->MethodName = MethodName;
	BlueprintNode->Parameters = Parameters;
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

void UReadMethod::Cancel()
{
	if (LoadContractRequest) {
		LoadContractRequest->OnProcessRequestComplete().Unbind();
		LoadContractRequest->CancelRequest();
	}
	if (ReadMethodRequest) {
		ReadMethodRequest->OnProcessRequestComplete().Unbind();
		ReadMethodRequest->CancelRequest();
	}
}

bool UReadMethod::IsActive() const
{
	return LoadContractRequest->GetStatus() == EHttpRequestStatus::Processing || 
		ReadMethodRequest->GetStatus() == EHttpRequestStatus::Processing;
}

void UReadMethod::Activate()
{
	if (!DeployedContract) {
		UE_LOG(LogEmergenceHttp, Error, TEXT("You need a Deployed Contract to use ReadMethod!"));
		return;
	}

	if (!WorldContextObject) {
		UE_LOG(LogEmergenceHttp, Error, TEXT("You need a WorldContextObject to use ReadMethod!"));
		return;
	}

	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	//if this contract has never had its ABI loaded...
	if (!Singleton->ContractsWithLoadedABIs.Contains(DeployedContract->Blockchain->Name.ToString() + DeployedContract->Address)) {
		ULoadContractInternal* LoadContract = ULoadContractInternal::LoadContract(WorldContextObject, DeployedContract);
		LoadContractRequest = LoadContract->Request;
		LoadContract->OnLoadContractCompleted.AddDynamic(this, &UReadMethod::LoadContractCompleted);
		LoadContract->Activate();
		return;
	}

	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});

	FString ContentString;
	ContentString.Append("[");
	for (int i = 0; i < Parameters.Num(); i++) {
		ContentString.Append("\"" + Parameters[i] + "\"");
		if (i != Parameters.Num() - 1) {
			ContentString.Append(",");
		}
	}
	ContentString.Append("]");

	ReadMethodRequest = UHttpHelperLibrary::ExecuteHttpRequest<UReadMethod>(
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
	FJsonObject JsonObject = UHttpHelperLibrary::TryParseResponseAsJson(HttpRequest, HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("ReadMethod_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {	
		FJsonObjectWrapper OutJsonObject;
		OutJsonObject.JsonObject = JsonObject.GetObjectField("message");
		OnReadMethodCompleted.Broadcast(OutJsonObject, EErrorCode::EmergenceOk);
	}
	else {
		OnReadMethodCompleted.Broadcast(FJsonObjectWrapper(), StatusCode);
	}
	SetReadyToDestroy();
}