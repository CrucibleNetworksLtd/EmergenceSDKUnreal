// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/WriteMethod.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "WalletService/LoadContractInternal.h"

UWriteMethod* UWriteMethod::WriteMethod(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FEmergenceContractMethod MethodName, FString Value, TArray<FString> Content, FString LocalAccountName, FString GasPrice, int NumberOfConfirmations, float TimeBetweenChecks)
{
	UWriteMethod* BlueprintNode = NewObject<UWriteMethod>();
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->MethodName = MethodName;
	BlueprintNode->Content = Content;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->LocalAccountName = LocalAccountName;
	BlueprintNode->GasPrice = GasPrice;
	BlueprintNode->Value = Value;
	BlueprintNode->NumberOfConfirmations = NumberOfConfirmations;
	BlueprintNode->TimeBetweenChecks = TimeBetweenChecks;
	BlueprintNode->RegisterWithGameInstance(WorldContextObject);
	return BlueprintNode;
}

void UWriteMethod::LoadContractCompleted(FString Response, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		this->Activate();
	}
	else {
		this->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), StatusCode);
	}
}

void UWriteMethod::Activate()
{
	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	//if this contract has never had its ABI loaded...
	if (!Singleton->ContractsWithLoadedABIs.Contains(DeployedContract->Blockchain->Name.ToString() + DeployedContract->Address)) {
		ULoadContractInternal* LoadContract = ULoadContractInternal::LoadContract(WorldContextObject, DeployedContract);
		LoadContract->OnLoadContractCompleted.AddDynamic(this, &UWriteMethod::LoadContractCompleted);
		LoadContract->Activate();
		return;
	}

	TArray<TPair<FString, FString>> Headers;

	if (!UEmergenceSingleton::DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", UEmergenceSingleton::DeviceID));
	}
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
		UHttpHelperLibrary::APIBase + "writeMethod?contractAddress=" + DeployedContract->Address + "&nodeUrl=" + DeployedContract->Blockchain->NodeURL + "&network=" + DeployedContract->Blockchain->Name.ToString().Replace(TEXT(" "), TEXT("")) + "&methodName=" + MethodName.MethodName + "&value=" + Value + (LocalAccountName != "" ? "&localAccountName=" + LocalAccountName : "") + GasString,
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		Headers,
		ContentString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("WriteMethod request started on method %s with value %s, calling WriteMethod_HttpRequestComplete on request completed. Json sent as part of the request: "), *MethodName.MethodName, *Value);
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *ContentString);
}

void UWriteMethod::WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	UE_LOG(LogEmergenceHttp, Display, TEXT("WriteMethod_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
	if (StatusCode == EErrorCode::EmergenceOk) {
		this->TransactionHash = JsonObject.GetObjectField("message")->GetStringField("transactionHash");
		GetTransactionStatus();
		OnTransactionSent.Broadcast();
		return;
	}
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("WriteMethod", StatusCode);
	this->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), StatusCode);
}

void UWriteMethod::GetTransactionStatus()
{
	UGetTransactionStatus* TransactionStatusCall = UGetTransactionStatus::GetTransactionStatus(this->WorldContextObject, this->TransactionHash, this->DeployedContract->Blockchain);
	TransactionStatusCall->OnGetTransactionStatusCompleted.AddDynamic(this, &UWriteMethod::TransactionStatusReturns);
	TransactionStatusCall->Activate();
}

void UWriteMethod::TransactionStatusReturns(FEmergenceTransaction Transaction, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceClientJsonParseFailed) {
		//probably hasn't been picked up by the node yet
		WaitThenGetStatus(); //wait for some seconds and then try to get the transaction status again
	}
	else if (StatusCode == EErrorCode::EmergenceOk) {
		//probably good to read
		if (Transaction.Confirmations > this->NumberOfConfirmations) { //if we have a good amount of confirmations
			//send this transaction
			this->OnTransactionConfirmed.Broadcast(Transaction, StatusCode);
			SetReadyToDestroy();
		}
		else {
			WaitThenGetStatus(); //wait for some seconds and then try to get the transaction status again
		}
	}
	else {
		//any other error
		//send the error and a blank struct
		this->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), StatusCode);
		SetReadyToDestroy();
	}
}

void UWriteMethod::WaitThenGetStatus()
{
	this->WorldContextObject->GetWorld()->GetTimerManager().SetTimer(this->TimerHandle, this, &UWriteMethod::GetTransactionStatus, 5.0F, false);
}
