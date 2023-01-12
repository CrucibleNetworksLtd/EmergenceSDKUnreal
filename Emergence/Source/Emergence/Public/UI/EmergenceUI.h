// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/NamedSlot.h"
#include "ErrorCodeFunctionLibrary.h"
#include "AvatarService/AvatarByOwner.h"
#include "EmergenceUI.generated.h"

UENUM(BlueprintType)
enum class  EmergenceConfirmationType : uint8
{
	Normal     UMETA(ToolTip = "For normal 'are you sure' type questions"),
	Error      UMETA(ToolTip = "To be implemented as bright red, for when you really need the users attention")
};

UCLASS(HideDropdown)
class EMERGENCE_API UEmergenceUI : public UUserWidget
{
	GENERATED_BODY()
private:
	TMap<FName, FText> LoadingMessages;

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScreenSwitched, UUserWidget*, NewScreen);

	//Called whenever the screen is changed
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers|Emergence UI")
	FOnScreenSwitched OnScreenSwitched;

	UFUNCTION(BlueprintCallable)
	void SetUserHasLoggedInBefore(bool HasLoggedInBefore);

	UFUNCTION(BlueprintPure)
	bool GetUserHasLoggedInBefore();

	//Switches the active screen to a new widget object
	UFUNCTION(BlueprintCallable)
	void SwitchCurrentScreen(UUserWidget* NewScreen);

	//Switches the active screen to a new widget class
	UFUNCTION(BlueprintCallable)
	void SwitchCurrentScreenByClass(TSubclassOf<UUserWidget> NewScreenClass);

	//The current screen slot sub-component. This is a C++ "BindWidget"'d widget, DO NOT RENAME IN UMG BLUEPRINTS!
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget), Category = "C++ Bind Widgets")
	UNamedSlot* CurrentScreenSlotBoundWidget;

	//Adds a message with the given Id to the LoadingMessages table
	UFUNCTION(BlueprintCallable)
	void ShowLoadingMessage(FName MessageId, FText Reason);

	//Removes the message with the given Id from the LoadingMessages table
	UFUNCTION(BlueprintCallable)
	void HideLoadingMessage(FName MessageId);

	//Gets the most recent loading message
	UFUNCTION(BlueprintPure)
	bool GetMostRecentLoadingMessage(FText& Message);

	//Shows an error code for the given error
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ShowServerErrorMessage(const EErrorCode& ErrorCode);

	//Closes the EmergenceUI.
	UFUNCTION(BlueprintCallable)
	void Close();
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClosed);

	//Called when the UI is closed.
	UPROPERTY(BlueprintAssignable)
	FOnClosed Closed;

	//Called when the UI reaches the home screen.
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnClosed OpeningFinished;

	//Gets the default avatar icon from the plugin settings
	UFUNCTION(BlueprintPure)
	static TSoftObjectPtr<UTexture2D> GetDefaultAvatarIcon();
};
