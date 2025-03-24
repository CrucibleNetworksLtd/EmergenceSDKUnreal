// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "Types/EmergenceChain.h"
#include "GetBalance.generated.h"

UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UGetBalance : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the balance of the given address.
	 * @param Address Address to get the balance of.
	 * @param Blockchain Blockchain to get the balance on.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Blockchain Interactions")
	static UGetBalance* GetBalance(UObject* WorldContextObject, FString Address, UEmergenceChain* Blockchain);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetBalanceCompleted, FString, Balance, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetBalanceCompleted OnGetBalanceCompleted;
private:
	void GetBalance_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString Address;
	UEmergenceChain* Blockchain;
};
