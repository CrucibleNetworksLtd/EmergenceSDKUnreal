// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpServerRequest.h"
#include "HttpResultCallback.h"
#include "ErrorCodeFunctionLibrary.h"
#include "CustodialWriteTransaction.generated.h"

//This class should only be called by WriteMethod
UCLASS(meta = (HasDedicatedAsyncNode))
class EMERGENCE_API UCustodialWriteTransaction : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "Content"), Category = "Emergence|Custodial Login")
	static UCustodialWriteTransaction* CustodialWriteTransaction(UObject* WorldContextObject, UEmergenceDeployment* DeployedContract, FString Method, FString Value, TArray<FString> Content);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCustodialWriteTransactionCompleted, const FString, TransactionHash, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnCustodialWriteTransactionCompleted OnCustodialWriteTransactionCompleted;

	void Activate() override;
private:
	void GetEncodedPayload_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	void GetEncodedData_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	static bool HandleSignatureCallback(const FHttpServerRequest& Req, const FHttpResultCallback& OnComplete);

	void SendTransaction_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	static TUniquePtr<FHttpServerResponse> GetHttpPage();

	UPROPERTY()
	UEmergenceDeployment* DeployedContract;

	UPROPERTY()
	FString Method;
	
	UPROPERTY()
	FString Value;
	
	UPROPERTY()
	TArray<FString> Content;
		
	UPROPERTY()
	FString EncodedSig;

	UPROPERTY()
	FString TransactionNonce;

	UPROPERTY()
	FString UnsignedTransaction;
	
	FJsonObject RawTransactionWithoutSignature;
	
	UPROPERTY()
	FString RpcUrl;

	static TDelegate<void(FString /*Signature*/, FString /*EOA*/, EErrorCode)> CallbackComplete;
	static bool _isServerStarted;

	void EncodeTransaction(FString Eoa, FString ChainId, FString ToAddress, FString Value, FString Data, FString RpcUrl);

	UFUNCTION()
	void GetEncodedData();

	UFUNCTION()
	void TransactionEnded();

	UFUNCTION()
	void SendTranscation(FString Signature, FString EOA);
	
};
