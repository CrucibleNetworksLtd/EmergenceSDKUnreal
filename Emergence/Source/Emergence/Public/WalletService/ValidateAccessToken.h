// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "ValidateAccessToken.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UValidateAccessToken : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Validates an access token. Usually used server-side to authenticate players.
	 * @param AccessToken The access token to authenticate.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Signed Message Validation")
	static UValidateAccessToken* ValidateAccessToken(UObject* WorldContextObject, const FString& AccessToken);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValidateAccessTokenCompleted, bool, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnValidateAccessTokenCompleted OnValidateAccessTokenCompleted;
private:
	void ValidateAccessToken_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	UObject* WorldContextObject;
	FString AccessToken;
};
