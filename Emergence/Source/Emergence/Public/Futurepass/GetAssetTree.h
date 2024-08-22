// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceAsyncSingleRequestBase.h"
#include "GetAssetTree.generated.h"


USTRUCT(BlueprintType)
struct FFutureverseAssetTreeObject
{

	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futureverse|Predicate Data")
	FString Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futureverse|Predicate Data")
	TMap<FString, FString> AdditionalData;

};

USTRUCT(BlueprintType)
struct FFutureverseAssetTreePath
{

    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futureverse|Asset Tree Part")
    FString Id;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futureverse|Asset Tree Part")
	FString RDFType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Emergence|Futureverse|Asset Tree Part")
	TMap<FString, FFutureverseAssetTreeObject> Objects;

};

UCLASS()
class EMERGENCE_API UGetFutureverseAssetTree : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
	
	/**
	 * Gets the Asset Tree associated with a specific asset.
	 * @param TokenId The interger ID of the token you want to get the Asset Tree of. For example: "473" (without quotes).
	 * @param CollectionId The collection ID of the token you want to get the asset tree of. For example: "7672:root:303204" (without quotes).
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"), Category = "Emergence|Futureverse")
	static UGetFutureverseAssetTree* GetFutureverseAssetTree(UObject* WorldContextObject, FString TokenId, FString CollectionId);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetAssetTreeCompleted, const TArray<FFutureverseAssetTreePath>&, AssetTree, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetAssetTreeCompleted OnGetAssetTreeCompleted;
private:
	void GetAssetTree_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	FString TokenId, CollectionId;
};
