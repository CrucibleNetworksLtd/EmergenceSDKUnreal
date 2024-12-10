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
	void QRCodeCompleted(UTexture2D* Icon, FString WalletConnectString, EErrorCode StatusCode);

	UFUNCTION()
	void AccessTokenCompleted(EErrorCode StatusCode);

	UFUNCTION()
	void ReinitializeWalletConnectCompleted(EErrorCode StatusCode);

	UFUNCTION()
	void CancelAllAndRestartAttempt();

	UFUNCTION()
	void GetHandshakeCompleted(FString Address, EErrorCode StatusCode);

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Wallet Connect")
	void CancelAll();

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Wallet Connect")
	void StartAll();

	//Starts a new attempt at showing the user a QR code and getting a handshake. Only manually call this if you're using ActivateOnConstruct = false
	UFUNCTION(BlueprintCallable, Category = "Emergence|Wallet Connect")
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

	/*How long before the QR code / WalletConnect URI changes. 
	This can be modified if users are struggling to login in the time provided.
	It is important to allow the login attempt to reset eventually, just in case the login gets into a "stuck state" for whatever reason.
	Note: this should be changed before the login attempt starts. Changing after that will have no effect.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Time")
	int ConnectionRefreshTime = 120;

	//This is the seconds remaining until the QR code is refreshed. Use this to display to the user how long they have.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="Time")
	int TimeRemaining = -1;
	
	/*Should this widget activate as soon as its constructed? 
	If you're going to start the widget inside of a Widget Switcher, you'll need to call StartAttempt() if this is false.*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Construction")
	bool ActivateOnConstruct = true;

	//this is a string version of the contents of the QR code. Technically, it isn't identical. The first and last parameters are reordered, as this seems to make it play more nicely with the Ledger Live desktop app.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Wallet Connect")
	FString WalletConnectString = "";

	virtual TSharedRef<SWidget> RebuildWidget() override;
};
