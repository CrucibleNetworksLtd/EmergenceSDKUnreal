// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/LoadContractInternal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"

ULoadContractInternal* ULoadContractInternal::LoadContract(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract)
{
	ULoadContractInternal* BlueprintNode = NewObject<ULoadContractInternal>();
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void ULoadContractInternal::Activate()
{
	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	Json->SetStringField("contractAddress", this->DeployedContract->Address);
	Json->SetStringField("ABI", this->DeployedContract->Contract->ABI);
	
	Json->SetStringField("network", this->DeployedContract->Blockchain->Name.ToString().Replace(TEXT(" "), TEXT("")));
	FString OutputString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(Json.ToSharedRef(), Writer);


	TArray<TPair<FString, FString>> Headers;
	Headers.Add(TPair<FString, FString>{"Content-Type", "application/json"});
	UHttpHelperLibrary::ExecuteHttpRequest<ULoadContractInternal>(
		this, 
		&ULoadContractInternal::LoadContract_HttpRequestComplete, 
		UHttpHelperLibrary::APIBase + "loadContract",
		"POST",
		60.0F,
		Headers,
		OutputString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadContract request started with JSON, calling LoadContract_HttpRequestComplete on request completed. Json sent as part of the request: "));
}

void ULoadContractInternal::LoadContract_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("LoadContract_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
		Singleton->ContractsWithLoadedABIs.AddUnique(DeployedContract->Blockchain->Name.ToString() + DeployedContract->Address);
		UE_LOG(LogEmergenceHttp, Display, TEXT("Added %s to ContractsWithLoadedABIs array"), *(DeployedContract->Blockchain->Name.ToString() + DeployedContract->Address));
		OnLoadContractCompleted.Broadcast(HttpResponse->GetContentAsString(), EErrorCode::EmergenceOk);
	}
	else {
		OnLoadContractCompleted.Broadcast(FString(), StatusCode);
		UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("LoadContract", StatusCode);
	}
}
