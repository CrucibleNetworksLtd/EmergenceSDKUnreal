// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "ErrorCodeFunctionLibrary.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "EmergenceInventoryServiceStructs.h"
#include "WriteDynamicMetadata.generated.h"


UCLASS()
class EMERGENCE_API UWriteDynamicMetadata : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Writes dynamic metadata to the Emergence inventory service database.
	 * @param Network Network of the contract we're writing the metadata to.
	 * @param AuthorizationHeader Your game's Authorization Header.
	 * @param TokenID The Token ID contract we're writing metadata to.
	 * @param Metadata The metadata to write.
	 * @param Contract The contact that we're writing to.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AuthorizationHeader = "0iKoO1V2ZG98fPETreioOyEireDTYwby"), Category = "Emergence|Inventory Service")
	static UWriteDynamicMetadata* WriteDynamicMetadata(UObject* WorldContextObject, const FString& Network, const FString& AuthorizationHeader, const FString& Contract, const FString& TokenID, const FString& Metadata);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWriteDynamicMetadataCompleted, FString, Response, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnWriteDynamicMetadataCompleted OnWriteDynamicMetadataCompleted;
private:
	void WriteDynamicMetadata_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString AuthorizationHeader, Network, Contract, TokenID, Metadata;
	bool OnlyUpdate;
	
};
