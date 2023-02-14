// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Interfaces/IHttpRequest.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "AvatarService/AvatarByOwner.h"
#include "AvatarService/AvatarServiceLibrary.h"
#include "AvatarById.generated.h"


UCLASS()
class EMERGENCE_API UAvatarById : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the avatar of a given ID from the Avatar System.
	 * @param AvatarIdString AvatarIdString of the avatar we want to get the details of.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Avatar Service")
	static UAvatarById* AvatarById(UObject* WorldContextObject, const FString& AvatarIdString);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAvatarByIdCompleted, const FEmergenceAvatarData&, Avatar, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnAvatarByIdCompleted OnAvatarByIdCompleted;
private:
	void AvatarById_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString AvatarIdString;
	UObject* WorldContextObject;
};
