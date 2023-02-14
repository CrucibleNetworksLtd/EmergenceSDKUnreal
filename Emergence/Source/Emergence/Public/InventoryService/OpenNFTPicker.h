// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "InventoryService/EmergenceInventoryServiceStructs.h"
#include "InventoryService/InventoryScreen.h"
#include "OpenNFTPicker.generated.h"

UENUM(BlueprintType)
enum class EEmergenceNFTPickerError : uint8 {
	Ok = 0,
	UserNotConnected = 1,
	UserSwitchedScreen = 2,
	UserClosedOverlay = 3,
	NoPlayerController = 4
};

UCLASS()
class EMERGENCE_API UOpenNFTPicker : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Opens the Emergence overlay to the users inventory and applies the given filters. The player must be signed into the Emergence overlay already to make use of this.
	 * @param PlayerController The player controller to show the UI to. Should usually be the local player.
	 * @param Filters The filters to apply when the inventory opens.
	 * @param OverrideAddress This allows for the inventory to be opened to show the inventory of another user. Leave blank to show the currently logged-in user's inventory.
	 */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", AdvancedDisplay = "OverrideAddress"), Category = "Emergence|Inventory Service")
	static UOpenNFTPicker* OpenNFTPicker(UObject* WorldContextObject, APlayerController* PlayerController, const FEmergenceInventoryFilterSet& Filters, const FString OverrideAddress = "");

	virtual void Activate() override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSelected, FEmergenceCombinedInventoryItem, SelectedItem, EEmergenceNFTPickerError, SelectionError);

	UPROPERTY(BlueprintAssignable)
    FOnItemSelected OnSelectionCompleted;

	UFUNCTION()
	void ItemSelectionCompleted(FEmergenceCombinedInventoryItem Item);
	
	UFUNCTION()
	void EmergenceOverlayScreenSwitched(UUserWidget* NewScreen);

	UFUNCTION()
	void EmergenceOverlayClosed();

	UFUNCTION()
	void AfterOverlayCloseCleanup();

	UFUNCTION()
	void EmergenceOverlayReady();

private:
    FEmergenceInventoryFilterSet Filters;
    APlayerController* OpeningPlayerController;
	bool PreviousMouseShowState;
	int PreviousGameInputMode;
	UInventoryScreen* InventoryScreen;
	UEmergenceUI* EmergenceUI;
	FString OverrideAddress;
	UObject* WorldContextObject;
};
