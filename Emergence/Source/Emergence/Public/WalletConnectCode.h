// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Image.h"
#include "EmergenceSingleton.h"
#include "WalletConnectCode.generated.h"

UENUM(BlueprintType)
enum class EEmergenceWalletConnectStepError : uint8
{
	QRCodeFail,
	HandshakeFail,
	AccessTokenFail
};

UCLASS(HideCategories=(Appearance, Localization))
class EMERGENCE_API UWalletConnectCode : public UImage
{
	GENERATED_BODY()
private:
	UFUNCTION()
	void QRCodeCompleted(UTexture2D* Icon, EErrorCode StatusCode);

	UFUNCTION()
	void AccessTokenCompleted(EErrorCode StatusCode);

	UFUNCTION()
	void ReinitializeWalletConnectCompleted(EErrorCode StatusCode);

	UFUNCTION()
	void CancelAllAndRestartAttempt();

	UFUNCTION()
	void GetHandshakeCompleted(FString Address, EErrorCode StatusCode);

	UFUNCTION()
	void StartAttempt();

	UPROPERTY()
	UEmergenceSingleton* Singleton;

	UPROPERTY()
	FTimerHandle TimeRemainingTimerHandle;

	UFUNCTION()
	void UpdateTimeRemaining();
public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSignInSuccess);

	//Called when a sign in was successful
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnSignInSuccess OnSignInSuccess;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSignInFailure, EEmergenceWalletConnectStepError, WalletConnectionStepError);

	//Called when a sign in failed
	UPROPERTY(BlueprintAssignable, Category = "EventDispatchers")
	FOnSignInFailure OnSignInFailure;

	//This is the seconds remaining until the QR code is refreshed. Use this to display to the user how long they have. In future version, the amount of time the user gets might be customisable.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Time")
	int TimeRemaining = -1;

	virtual TSharedRef<SWidget> RebuildWidget() override;
};
