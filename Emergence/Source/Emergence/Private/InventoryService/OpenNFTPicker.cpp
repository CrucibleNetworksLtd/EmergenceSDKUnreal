// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/OpenNFTPicker.h"
#include "EmergenceSingleton.h"
#include "InventoryService/InventoryScreen.h"
#include "Blueprint/UserWidget.h"

UOpenNFTPicker* UOpenNFTPicker::OpenNFTPicker(UObject* WorldContextObject, APlayerController* PlayerController, const FEmergenceInventoryFilterSet& Filters, const FString OverrideAddress)
{
	UOpenNFTPicker* BlueprintNode = NewObject<UOpenNFTPicker>();
	BlueprintNode->Filters = Filters;
	BlueprintNode->OpeningPlayerController = PlayerController;
	BlueprintNode->OverrideAddress = OverrideAddress;
	BlueprintNode->WorldContextObject = WorldContextObject;
	return BlueprintNode;
}

void UOpenNFTPicker::Activate()
{
	UEmergenceSingleton* Emergence = UEmergenceSingleton::GetEmergenceManager(WorldContextObject);

	if (!Emergence->HasCachedAddress()) { //if the user hasn't logged in through wallet connect yet...
		this->OnSelectionCompleted.Broadcast(FEmergenceCombinedInventoryItem(), EEmergenceNFTPickerError::UserNotConnected);
		return;
	}

	if (!OpeningPlayerController) {
		this->OnSelectionCompleted.Broadcast(FEmergenceCombinedInventoryItem(), EEmergenceNFTPickerError::NoPlayerController);
		return;
	}

	static TSubclassOf<UEmergenceUI> EmergenceUIBPClass = StaticLoadClass(UObject::StaticClass(), OpeningPlayerController, TEXT("/Emergence/EmergenceUI_BP.EmergenceUI_BP_C"));
	check(EmergenceUIBPClass);

	//Get the current state of showing the mouse so we can set it back to this later
	this->PreviousMouseShowState = OpeningPlayerController->bShowMouseCursor;

	//Get the current state of input mode so we can set it back to this later
	UGameViewportClient* GameViewportClient = WorldContextObject->GetWorld()->GetGameViewport();
	bool IgnoringInput = GameViewportClient->IgnoreInput();
	EMouseCaptureMode CaptureMouse = GameViewportClient->GetMouseCaptureMode();

	if (IgnoringInput == false && CaptureMouse == EMouseCaptureMode::CaptureDuringMouseDown) //Game And UI
	{
		this->PreviousGameInputMode = 0;
	}
	else if (IgnoringInput == true && CaptureMouse == EMouseCaptureMode::NoCapture) //UI Only
	{
		this->PreviousGameInputMode = 1;
	}
	else //Game Only
	{
		this->PreviousGameInputMode = 2;
	}

	//Open the UI
	EmergenceUI = Cast<UEmergenceUI>(Emergence->OpenEmergenceUI(OpeningPlayerController, EmergenceUIBPClass));
	EmergenceUI->OpeningFinished.AddDynamic(this, &UOpenNFTPicker::EmergenceOverlayReady);
	OpeningPlayerController->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode = FInputModeUIOnly();
	InputMode.SetWidgetToFocus(EmergenceUI->GetCachedWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	OpeningPlayerController->SetInputMode(InputMode);
}

void UOpenNFTPicker::ItemSelectionCompleted(FEmergenceCombinedInventoryItem Item)
{
	this->OnSelectionCompleted.Broadcast(Item, EEmergenceNFTPickerError::Ok);
	InventoryScreen->OnItemSelected.RemoveDynamic(this, &UOpenNFTPicker::ItemSelectionCompleted);
	EmergenceUI->OnScreenSwitched.RemoveDynamic(this, &UOpenNFTPicker::EmergenceOverlayScreenSwitched);
	EmergenceUI->Closed.RemoveDynamic(this, &UOpenNFTPicker::EmergenceOverlayClosed);
	EmergenceUI->Close(); //this will trigger EmergenceOverlayClosed.
}

void UOpenNFTPicker::EmergenceOverlayScreenSwitched(UUserWidget* NewScreen)
{
	this->OnSelectionCompleted.Broadcast(FEmergenceCombinedInventoryItem(), EEmergenceNFTPickerError::UserSwitchedScreen);
	InventoryScreen->OnItemSelected.RemoveDynamic(this, &UOpenNFTPicker::ItemSelectionCompleted);
	EmergenceUI->OnScreenSwitched.RemoveDynamic(this, &UOpenNFTPicker::EmergenceOverlayScreenSwitched);
	EmergenceUI->Closed.RemoveDynamic(this, &UOpenNFTPicker::EmergenceOverlayClosed);
}

void UOpenNFTPicker::EmergenceOverlayClosed()
{
	this->OnSelectionCompleted.Broadcast(FEmergenceCombinedInventoryItem(), EEmergenceNFTPickerError::UserClosedOverlay);
	InventoryScreen->OnItemSelected.RemoveDynamic(this, &UOpenNFTPicker::ItemSelectionCompleted);
	EmergenceUI->OnScreenSwitched.RemoveDynamic(this, &UOpenNFTPicker::EmergenceOverlayScreenSwitched);
	EmergenceUI->Closed.RemoveDynamic(this, &UOpenNFTPicker::EmergenceOverlayClosed);
}

void UOpenNFTPicker::AfterOverlayCloseCleanup()
{
	OpeningPlayerController->SetShowMouseCursor(this->PreviousMouseShowState);
	switch (this->PreviousGameInputMode) {
	case 0:
		OpeningPlayerController->SetInputMode(FInputModeGameAndUI());
		break;
	case 1:
		OpeningPlayerController->SetInputMode(FInputModeUIOnly());
		break;
	case 2:
		OpeningPlayerController->SetInputMode(FInputModeGameOnly());
		break;
	}
	EmergenceUI->Closed.RemoveDynamic(this, &UOpenNFTPicker::AfterOverlayCloseCleanup);
}

void UOpenNFTPicker::EmergenceOverlayReady()
{
	static TSubclassOf<UInventoryScreen> InventoryScreenClass = StaticLoadClass(UObject::StaticClass(), OpeningPlayerController, TEXT("/Emergence/Screens/InventoryScreenBP.InventoryScreenBP_C"));
	check(InventoryScreenClass);
	InventoryScreen = CreateWidget<UInventoryScreen>(OpeningPlayerController->GetWorld(), InventoryScreenClass);
	InventoryScreen->Filters = this->Filters;
	
	InventoryScreen->Address = this->OverrideAddress == "" ? UEmergenceSingleton::GetEmergenceManager(WorldContextObject)->GetCachedAddress() : this->OverrideAddress;
	InventoryScreen->ShowExternalInventoryMessage = this->OverrideAddress != "";
	
	InventoryScreen->OnItemSelected.AddDynamic(this, &UOpenNFTPicker::ItemSelectionCompleted);

	EmergenceUI->SwitchCurrentScreen(InventoryScreen);
	EmergenceUI->OnScreenSwitched.AddDynamic(this, &UOpenNFTPicker::EmergenceOverlayScreenSwitched);
	EmergenceUI->Closed.AddDynamic(this, &UOpenNFTPicker::EmergenceOverlayClosed);
	EmergenceUI->Closed.AddDynamic(this, &UOpenNFTPicker::AfterOverlayCloseCleanup);
}
