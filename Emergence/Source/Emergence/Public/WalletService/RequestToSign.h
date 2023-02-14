// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "RequestToSign.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API URequestToSign : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Sends a request to sign a message to the user's WalletConnect'd wallet.
	 * @param MessageToSign The message that they will be presented to sign.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|WalletConnect Requests")
	static URequestToSign* RequestToSign(UObject* WorldContextObject, const FString& MessageToSign);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestToSignCompleted, FString, SignedMessage, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnRequestToSignCompleted OnRequestToSignCompleted;
private:
	void RequestToSign_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
	FString MessageToSign;
};
