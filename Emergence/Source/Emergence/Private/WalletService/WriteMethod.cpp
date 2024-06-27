// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletService/WriteMethod.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HttpService/HttpHelperLibrary.h"
#include "EmergenceSingleton.h"
#include "WalletService/LoadContractInternal.h"
#include "Templates/SharedPointer.h"

UWriteMethod* UWriteMethod::WriteMethod(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FEmergenceContractMethod MethodName, FString Value, TArray<FString> Content, FString PrivateKey, FString GasPrice, int NumberOfConfirmations, float TimeBetweenChecks)
{
	UWriteMethod* BlueprintNode = NewObject<UWriteMethod>();
	BlueprintNode->DeployedContract = DeployedContract;
	BlueprintNode->MethodName = MethodName;
	BlueprintNode->Content = Content;
	BlueprintNode->WorldContextObject = WorldContextObject;
	BlueprintNode->LocalAccountName = PrivateKey;
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
	if (!DeployedContract) {
		UE_LOG(LogEmergenceHttp, Error, TEXT("You need a Deployed Contract to use WriteMethod!"));
		return;
	}

	if (!WorldContextObject) {
		UE_LOG(LogEmergenceHttp, Error, TEXT("You need a WorldContextObject to use WriteMethod!"));
		return;
	}

	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);

	if (this->LocalAccountName.IsEmpty() && !Singleton->HasAccessToken()) {
		UE_LOG(LogEmergenceHttp, Error, TEXT("If you aren't using a local wallet you need to have connected with WalletConnect to use WriteMethod!"));
		return;
	}

	
	//if this contract has never had its ABI loaded...
	if (!Singleton->ContractsWithLoadedABIs.Contains(DeployedContract->Blockchain->Name.ToString() + DeployedContract->Address)) {
		ULoadContractInternal* LoadContract = ULoadContractInternal::LoadContract(WorldContextObject, DeployedContract);
		LoadContractRequest = LoadContract->Request;
		LoadContract->OnLoadContractCompleted.AddDynamic(this, &UWriteMethod::LoadContractCompleted);
		LoadContract->Activate();
		return;
	}

	//if we're working with a wallet connected wallet
	if (this->LocalAccountName.IsEmpty()) {
		TArray<TPair<FString, FString>> SwitchChainHeaders;

		if (!Singleton->DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
			SwitchChainHeaders.Add(TPair<FString, FString>("deviceId", Singleton->DeviceID));
		}
		SwitchChainHeaders.Add(TPair<FString, FString>{"Content-Type", "application/json"});

		TSharedPtr<FJsonObject> SwitchChainContent = MakeShareable(new FJsonObject);
		SwitchChainContent->SetNumberField("chainId", DeployedContract->Blockchain->ChainID);
		SwitchChainContent->SetStringField("chainName", DeployedContract->Blockchain->Name.ToString());
		SwitchChainContent->SetStringField("currencyName", DeployedContract->Blockchain->Symbol);
		SwitchChainContent->SetStringField("currencySymbol", DeployedContract->Blockchain->Symbol);
		SwitchChainContent->SetNumberField("currencyDecimals", 18);
		SwitchChainContent->SetArrayField("rpcUrls", { MakeShared<FJsonValueString>(DeployedContract->Blockchain->NodeURL) });
		SwitchChainContent->SetArrayField("blockExplorerUrls", {});
		FString SwitchChainContentString;
		TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&SwitchChainContentString);
		FJsonSerializer::Serialize(SwitchChainContent.ToSharedRef(), Writer);

		//call switchChain to make sure we're on the right chain
		auto Request = UHttpHelperLibrary::ExecuteHttpRequest<UWriteMethod>(
			this,
			nullptr,
			UHttpHelperLibrary::APIBase + "switchChain",
			"POST",
			300.0F, //give the user lots of time to mess around setting high gas fees
			SwitchChainHeaders,
			SwitchChainContentString, false);
		SwitchChainRequest = Request;
		Request->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr req, FHttpResponsePtr res, bool bSucceeded) {
			EErrorCode StatusCode;
			FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(res, bSucceeded, StatusCode);
			if (res.IsValid()) { //this was required here for 5.4?
				UE_LOG(LogEmergenceHttp, Display, TEXT("SwitchChain_HttpRequestComplete: %s"), *res->GetContentAsString());
				if (StatusCode == EErrorCode::EmergenceOk) {
					CallWriteMethod();
					return;
				}
				else {
					this->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), StatusCode);
					return;
				}
			}
			else {
				this->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), EErrorCode::EmergenceClientJsonParseFailed);
				return;
			}
		});
		Request->ProcessRequest();
		UE_LOG(LogEmergenceHttp, Display, TEXT("SwitchChain request started on method %s with value %s, calling lambda function on request completed. Json sent as part of the request: "), *MethodName.MethodName, *Value);
		UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *SwitchChainContentString);
	}
	//if we're working with a local wallet 
	else {
		//switching networks isn't allowed
		//CallWriteMethod();
		auto EmergenceModule = FModuleManager::GetModuleChecked<FEmergenceModule>("Emergence");

		FString TransactionResponse;
		EmergenceModule.SendTransactionViaKeystore(this, DeployedContract, MethodName.MethodName, LocalAccountName, FString(), GasPrice, Value, TransactionResponse);
		
	}
}

void UWriteMethod::CallWriteMethod()
{
	TArray<TPair<FString, FString>> Headers;
	UEmergenceSingleton* Singleton = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);
	if (!Singleton->DeviceID.IsEmpty()) { //we need to send the device ID if we have one, we won't have one for local EVM servers
		Headers.Add(TPair<FString, FString>("deviceId", Singleton->DeviceID));
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

	WriteMethodRequest = UHttpHelperLibrary::ExecuteHttpRequest<UWriteMethod>(
		this,
		&UWriteMethod::WriteMethod_HttpRequestComplete,
		UHttpHelperLibrary::APIBase + "writeMethod?contractAddress=" + DeployedContract->Address + "&nodeUrl=" + DeployedContract->Blockchain->NodeURL + "&network=" + DeployedContract->Blockchain->Name.ToString().Replace(TEXT(" "), TEXT("")) + "&methodName=" + MethodName.MethodName + "&chainId=" + FString::Printf(TEXT("%lld"), DeployedContract->Blockchain->ChainID) + "&value=" + Value + (LocalAccountName != "" ? "&localAccountName=" + LocalAccountName : "") + GasString,
		"POST",
		300.0F, //give the user lots of time to mess around setting high gas fees
		Headers,
		ContentString);
	UE_LOG(LogEmergenceHttp, Display, TEXT("WriteMethod request started on method %s with value %s, calling WriteMethod_HttpRequestComplete on request completed. Json sent as part of the request: "), *MethodName.MethodName, *Value);
	UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *ContentString);
}

void UWriteMethod::Cancel()
{
	if (LoadContractRequest) {
		LoadContractRequest->OnProcessRequestComplete().Unbind();
		LoadContractRequest->CancelRequest();
	}
	if (SwitchChainRequest) {
		SwitchChainRequest->OnProcessRequestComplete().Unbind();
		SwitchChainRequest->CancelRequest();
	}
	if (WriteMethodRequest) {
		WriteMethodRequest->OnProcessRequestComplete().Unbind();
		WriteMethodRequest->CancelRequest();
	}
}

bool UWriteMethod::IsActive() const
{
	return LoadContractRequest->GetStatus() == EHttpRequestStatus::Processing ||
		SwitchChainRequest->GetStatus() == EHttpRequestStatus::Processing ||
		WriteMethodRequest->GetStatus() == EHttpRequestStatus::Processing;
}

void UWriteMethod::SendTransactionViaKeystoreComplete(FString Response)
{
	EErrorCode StatusCode = EErrorCode::EmergenceInternalError;
	if (!Response.IsEmpty())
	{
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
		TSharedRef <TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response);
		if (FJsonSerializer::Deserialize(JsonReader, JsonObject) && JsonObject.IsValid())
		{
			if (JsonObject->HasField("statusCode"))
			{
				StatusCode = UErrorCodeFunctionLibrary::Conv_IntToErrorCode(JsonObject->GetIntegerField("statusCode"));
			}
		}

		if (StatusCode == EErrorCode::EmergenceOk) {
			this->TransactionHash = JsonObject->GetObjectField("message")->GetStringField("transactionHash");
			GetTransactionStatus();
			OnTransactionSent.Broadcast();
			return;
		}
	}

	//If the code gets to this point something has failed
	UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->CallRequestError("WriteMethod", StatusCode);
	this->OnTransactionConfirmed.Broadcast(FEmergenceTransaction(), StatusCode);
}

void UWriteMethod::WriteMethod_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	EErrorCode StatusCode;
	FJsonObject JsonObject = UErrorCodeFunctionLibrary::TryParseResponseAsJson(HttpResponse, bSucceeded, StatusCode);
	if (HttpResponse) {
		UE_LOG(LogEmergenceHttp, Display, TEXT("WriteMethod_HttpRequestComplete: %s"), *HttpResponse->GetContentAsString());
		if (StatusCode == EErrorCode::EmergenceOk) {
			this->TransactionHash = JsonObject.GetObjectField("message")->GetStringField("transactionHash");
			GetTransactionStatus();
			OnTransactionSent.Broadcast();
			return;
		}
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
