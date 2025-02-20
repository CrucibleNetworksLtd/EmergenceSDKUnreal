// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryService/InventoryByOwner.h"
#include "AvatarService/AvatarByOwner.h"
#include "InventoryHelperLibrary.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceCombinedInventoryItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Inventory Service|Combined Inventory Item")
	FEmergenceInventoryItem InventoryItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Inventory Service|Combined Inventory Item")
	FEmergenceAvatarResult AvatarItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Inventory Service|Combined Inventory Item")
	bool HasMatchingAvatar = false;
};

UCLASS()
class EMERGENCEINVENTORY_API UInventoryHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * Given the output of InventoryByOwner and AvatarsByOwner, merge these outputs into an array of paired data about NFTs. 
	 * So, if there is avatar data about an inventory object, these will be put into the same FEmergenceCombinedInventoryItem.
	 * @param InventoryItems Output of InventoryByOwner.
	 * @param Avatars Output of AvatarsByOwner.
	 */
	UFUNCTION(BlueprintCallable, Category="Emergence|Helpers")
	static TArray<FEmergenceCombinedInventoryItem> OrganiseInventoryItems(TArray<FEmergenceInventoryItem> InventoryItems, TArray< FEmergenceAvatarResult> Avatars);

	/**
	 * Given an EmergenceInventoryItemsMetaContent array, find the best image's URL. PNGs are best, then JPEGs, then GIFs (GIFs only on Windows). If nothing is found, returns an empty string.
	 * @param Contents This is usually retrieved from a Emergence Inventory Item's Metadata's Content array.
	 */
	UFUNCTION(BlueprintPure, Category = "Emergence|Inventory Service")
	static FString GetBestDisplayImage(TArray<FEmergenceInventoryItemsMetaContent> Contents);
};
