#include "InventoryService/InventoryScreen.h"
#include "InventoryService/InventoryByOwner.h"
#include "InventoryService/InventoryHelperLibrary.h"
#include "EmergenceSingleton.h"

TArray<FString> UInventoryScreen::GetCollectionWhitelist()
{
	TArray<FString> CollectionWhitelist;
	if (!GConfig->GetArray(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("InventoryScreenCollectionWhitelist"), CollectionWhitelist, GGameIni)) {
		CollectionWhitelist = {};
	}
	return CollectionWhitelist;
}

void UInventoryScreen::OnGetInventoryAndAvatarsComplete_Internal(FEmergenceInventory Inventory, EErrorCode StatusCode)
{
	if (StatusCode == EErrorCode::EmergenceOk) {
		InventoryByOwnerRequest->OnInventoryByOwnerCompleted.RemoveAll(this);
		TArray<FEmergenceCombinedInventoryItem> OrganisedItems = UInventoryHelperLibrary::OrganiseInventoryItems(Inventory.items, UEmergenceSingleton::GetEmergenceManager(this)->OwnedAvatarNFTCache);
		OnGetInventoryAndAvatarsComplete(OrganisedItems, StatusCode);
		return;
	}
	else {
		OnGetInventoryAndAvatarsComplete(TArray<FEmergenceCombinedInventoryItem>(), StatusCode);
		return;
	}
}

void UInventoryScreen::GetInventoryAndAvatars()
{
	InventoryByOwnerRequest = UInventoryByOwner::InventoryByOwner(this, this->Address);
	InventoryByOwnerRequest->OnInventoryByOwnerCompleted.AddDynamic(this, &UInventoryScreen::OnGetInventoryAndAvatarsComplete_Internal);
	InventoryByOwnerRequest->Activate();
}

void UInventoryScreen::RemoveFromParent()
{
	Super::RemoveFromParent();
	if (InventoryByOwnerRequest) {
		InventoryByOwnerRequest->Cancel();
	}
}
