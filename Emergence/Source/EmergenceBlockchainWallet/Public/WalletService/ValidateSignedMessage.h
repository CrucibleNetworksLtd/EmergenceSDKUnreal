// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Types/EmergenceErrorCode.h"
#include "ValidateSignedMessage.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCEBLOCKCHAINWALLET_API UValidateSignedMessage : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Validates a signed message. Usually used server-side to authenticate player's messages.
	 * @param Message The original message.
	 * @param SignedMessage The signed message that we want to check.
	 * @param Address The address that signed the message.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Signed Message Validation")
	static UValidateSignedMessage* ValidateSignedMessage(UObject* WorldContextObject, const FString& Message, const FString& SignedMessage, const FString& Address);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnValidateSignedMessageCompleted, bool, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence Requests")
	FOnValidateSignedMessageCompleted OnValidateSignedMessageCompleted;
private:
	void ValidateSignedMessage_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	
	FString Address, Message, SignedMessage;
};
