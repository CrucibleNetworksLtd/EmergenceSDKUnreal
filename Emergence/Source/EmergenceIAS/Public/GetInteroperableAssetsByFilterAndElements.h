// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actions/EmergenceAsyncSingleRequestBase.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "EmergenceInteroperableAssetElement.h"
#include "Types/EmergenceErrorCode.h"
#include "Interfaces/IHttpRequest.h"
#include "Templates/SubclassOf.h"
#include "InteroperableAsset.h"
#include "GetInteroperableAssetsByFilterAndElements.generated.h"

UCLASS()
class EMERGENCEIAS_API UGetInteroperableAssetsByFilterAndElements : public UEmergenceAsyncSingleRequestBase
{
	GENERATED_BODY()
public:
	/**
	 * Gets IAs by one or many filters, and one or many IAS Elements.
	 * @param DesiredElements A set of Emergence IAS Elements that should be included.
	 * @param CollectionID A collection that the NFTs must be from.
	 * @param WalletAddress A wallet address which all the NFTs must be owned by.
	 * @param NFTIDs An array of NFT IDs to include. They must be in the format CHAIN:Address:TokenId
	 * @param Blockchains A array of blockchains to include results from. Options are "ETHEREUM", "SEPOLIA".
	 * @pamam PageNumber Which page number to get as part of pagination.
	 * @param PageSize Size of the page to get. Default is 50.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AutoCreateRefTerm = "NFTIDs, Blockchains"), Category = "Emergence|Interoperable Asset System")
	static UGetInteroperableAssetsByFilterAndElements* GetInteroperableAssetsByFilterAndElements(
		UObject* WorldContextObject, 
		const TSet<TSubclassOf<class UEmergenceInteroperableAssetElement>>& DesiredElements, 
		const FString& CollectionID, 
		const FString& WalletAddress, 
		const TArray<FString>& NFTIDs, 
		const TArray<FString>& Blockchains, 
		const int PageNumber = 0,
		const int PageSize = 50);

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGetInteroperableAssetsByFilterAndElementsCompleted, const TArray<FEmergenceInteroperableAsset>&, InteroperableAssets, EErrorCode, StatusCode);

	UPROPERTY(BlueprintAssignable)
	FOnGetInteroperableAssetsByFilterAndElementsCompleted OnGetInteroperableAssetsByFilterAndElementsCompleted;

private:
	void OnGetInteroperableAssetsByFilterAndElements_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);

	FString CollectionID, WalletAddress;
	TArray<FString> NFTIDs, Blockchains;
	int PageNumber, PageSize;
	TSet<TSubclassOf<UEmergenceInteroperableAssetElement>> DesiredElements;
};
