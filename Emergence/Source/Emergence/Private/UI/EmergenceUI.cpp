// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "UI/EmergenceUI.h"
#include "EmergenceSingleton.h"

void UEmergenceUI::SetUserHasLoggedInBefore(bool HasLoggedInBefore)
{
	if (!GConfig) return;

	GConfig->SetBool(TEXT("Emergence"), TEXT("HasLoggedInBefore"), HasLoggedInBefore, GGameIni);
	GConfig->Flush(false, GGameIni);
}

bool UEmergenceUI::GetUserHasLoggedInBefore()
{
	if (!GConfig) return false;

	bool value = false;
	GConfig->GetBool(TEXT("Emergence"), TEXT("HasLoggedInBefore"), value, GGameIni);
	return value;
}

void UEmergenceUI::SwitchCurrentScreen(UUserWidget* NewScreen) {
	if (CurrentScreenSlotBoundWidget) {
		UUserWidget* TempNewScreen = NewScreen; //keep this around in-case doing the next method deletes whats holding it
		CurrentScreenSlotBoundWidget->RemoveChildAt(0); //can only have one child, this is fine
		CurrentScreenSlotBoundWidget->AddChild(TempNewScreen);
		this->OnScreenSwitched.Broadcast(TempNewScreen);
	}
}

void UEmergenceUI::SwitchCurrentScreenByClass(TSubclassOf<UUserWidget> NewScreenClass)
{
	SwitchCurrentScreen(CreateWidget<UUserWidget>(this, NewScreenClass));
}

void UEmergenceUI::ShowLoadingMessage(FName MessageId, FText Reason)
{
	LoadingMessages.Add(MessageId, Reason);
}

void UEmergenceUI::HideLoadingMessage(FName MessageId)
{
	LoadingMessages.Remove(MessageId);
}

bool UEmergenceUI::GetMostRecentLoadingMessage(FText& Message)
{
	int32 NumberInMap = this->LoadingMessages.Num();
	if (NumberInMap == 0) {
		return false;
	}

	else {
		TArray<FText> Texts;
		this->LoadingMessages.GenerateValueArray(Texts);
		Message = Texts[NumberInMap - 1];
		return true;
	}
}

void UEmergenceUI::Close()
{
	this->RemoveFromParent();
	this->Closed.Broadcast();
}

TSoftObjectPtr<UTexture2D> UEmergenceUI::GetDefaultAvatarIcon()
{
	FString DefaultAvatarIconString;
	if (!GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("DefaultAvatarIcon"), DefaultAvatarIconString, GGameIni)) {
		DefaultAvatarIconString = "/Emergence/Components/UE.UE";
	}
	return TSoftObjectPtr<UTexture2D>(FSoftObjectPath("Texture2D'" + DefaultAvatarIconString + "'"));
}
