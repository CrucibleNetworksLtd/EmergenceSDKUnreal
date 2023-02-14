// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "EmergenceInventoryServiceStructs.h"
#include "GetDynamicMetadata.generated.h"


UCLASS()
class EMERGENCE_API UGetDynamicMetadata : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets the dynamic metadata from the Emergence inventory service database.
	 * @param Network Network of the contract we're writing the metadata to.
	 * @param TokenID The Token ID contract we're writing metadata to.
	 * @param Contract The contact that we're writing to.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Inventory Service")
	static UGetDynamicMetadata* GetDynamicMetadata(UObject* WorldContextObject, const FString& Network, const FString& Contract, const FString& TokenID);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetDynamicMetadataCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetDynamicMetadataCompleted OnGetDynamicMetadataCompleted;
private:
	void GetDynamicMetadata_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString Network, Contract, TokenID;
	UObject* WorldContextObject;
};
