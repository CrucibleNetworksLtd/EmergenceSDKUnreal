// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "PersonaStructs.h"
#include "Transaction.h"
#include "GetTransactionStatus.generated.h"

UCLASS()
class EMERGENCE_API UGetTransactionStatus : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the status of the transaction.
	 * @param TransactionHash Hash of the transaction.
	 * @param NodeURL NodeURL of the blockchain we're checking the transaction on. Leave blank for nodeURL from Emergence project settings.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetTransactionStatus* GetTransactionStatus(const UObject* WorldContextObject, FString TransactionHash, FString NodeURL);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetTransactionStatusCompleted, FEmergenceTransaction, Transaction, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetTransactionStatusCompleted OnGetTransactionStatusCompleted;
private:
	void GetTransactionStatus_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	const UObject* WorldContextObject;
	FString TransactionHash;
	FString NodeURL;
};
