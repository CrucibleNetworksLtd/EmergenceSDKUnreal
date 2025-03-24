// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "WebLogin/CustodialSignMessage.h"
#include "RequestToSign.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API URequestToSign : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Sends a request to sign a message to the user's WalletConnect'd wallet / Futureverse custodial wallet.
	 * @param MessageToSign The message that they will be presented to sign.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Wallet Requests")
	static URequestToSign* RequestToSign(UObject* WorldContextObject, const FString& MessageToSign);

	virtual void Activate() override;
	void BeginDestroy() override;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestToSignCompleted, FString, SignedMessage, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnRequestToSignCompleted OnRequestToSignCompleted;
private:
	void RequestToSign_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	UFUNCTION()
	void OnInternalCustodialSignMessageComplete(const FString SignedMessage, EErrorCode StatusCode);

	UPROPERTY()
	UCustodialSignMessage* CustodialSignMessage;

	UPROPERTY()
	FString MessageToSign;
};
