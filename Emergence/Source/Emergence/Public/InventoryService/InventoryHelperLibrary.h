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
class UInventoryHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category="Emergence Internal|Interface Helpers")
	static TArray<FEmergenceCombinedInventoryItem> OrganiseInventoryItems(TArray<FEmergenceInventoryItem> InventoryItems, TArray< FEmergenceAvatarResult> Avatars);

	//Given an EmergenceInventoryItemsMetaContent array, find the best image's URL. PNGs are best, then JPEGs, then GIFs. If nothing is found, returns an empty string.
	UFUNCTION(BlueprintPure, Category = "Emergence|Inventory Service")
	static FString GetBestDisplayImage(TArray<FEmergenceInventoryItemsMetaContent> Contents);

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Interface Helpers")
	static FString GetBestModel(TArray<FEmergenceInventoryItemsMetaContent> Contents);

	//Gets a set of categories, given a correctly formatted DynamicMetadata.
	UFUNCTION(BlueprintPure, Category = "Emergence|Inventory Service")
	static TSet<FString> GetDynamicMetadataCategories(FString DynamicMetadata);
};
