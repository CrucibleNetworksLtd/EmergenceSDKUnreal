// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "WalletConnectCode.h"
#include "TimerManager.h"
#include "Engine/World.h"

TSharedRef<SWidget> UWalletConnectCode::RebuildWidget() {
	Singleton = UEmergenceSingleton::GetEmergenceManager(this->GetOwningPlayer());
	
	if (Singleton) {
		//start the ticking for the countdown timer. Keep in mind, this is visual ONLY and doesn't affact the requests themselves
		this->GetOwningPlayer()->GetWorld()->GetTimerManager().SetTimer(TimeRemainingTimerHandle, this, &UWalletConnectCode::UpdateTimeRemaining, 1.0F, true, 1.0F);
		
		if (ActivateOnConstruct) {
			this->StartAttempt();
		}
	}
	return Super::RebuildWidget();
}

void UWalletConnectCode::QRCodeCompleted(UTexture2D* _Icon, FString _WalletConnectString, EErrorCode _StatusCode) {
	Singleton->OnGetQRCodeCompleted.RemoveDynamic(this, &UWalletConnectCode::QRCodeCompleted);
	if (_StatusCode == EErrorCode::EmergenceOk) {
		this->SetBrushFromTexture(_Icon, false);
		this->WalletConnectString = _WalletConnectString;
		Singleton->GetHandshake(this->ConnectionRefreshTime);
	}
	else {
		this->OnSignInFailure.Broadcast(EEmergenceWalletConnectStepError::QRCodeFail);
		this->CancelAllAndRestartAttempt();
	}
}

void UWalletConnectCode::AccessTokenCompleted(EErrorCode StatusCode) {
	Singleton->OnLoginFinished.RemoveDynamic(this, &UWalletConnectCode::AccessTokenCompleted);
	if (StatusCode == EErrorCode::EmergenceOk) { //EVERYTHING IS DONE, CLEANUP AND SAY SUCCESS
		this->GetOwningPlayer()->GetWorld()->GetTimerManager().ClearTimer(TimeRemainingTimerHandle);
		Singleton->OnGetHandshakeCompleted.RemoveDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);
		Singleton->OnGetQRCodeCompleted.RemoveDynamic(this, &UWalletConnectCode::QRCodeCompleted);
		this->OnSignInSuccess.Broadcast(); //call sign in success
	}
	else {
		this->OnSignInFailure.Broadcast(EEmergenceWalletConnectStepError::HandshakeFail);
		this->CancelAllAndRestartAttempt();
	}
}

void UWalletConnectCode::CancelAllAndRestartAttempt() {
	Singleton->CancelSignInRequest();
	this->StartAttempt();
}

void UWalletConnectCode::GetHandshakeCompleted(FString Address, EErrorCode StatusCode)
{
	Singleton->OnGetHandshakeCompleted.RemoveDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);
	if (StatusCode != EErrorCode::EmergenceOk) {
		this->OnSignInFailure.Broadcast(EEmergenceWalletConnectStepError::HandshakeFail);
		if (StatusCode == EErrorCode::EmergenceClientFailed) { //@TODO this currently happens if the request was cancelled. We need a better way to detect intentionally cancelled statuses
			return;
		}
		this->CancelAllAndRestartAttempt();
	}
}

void UWalletConnectCode::CancelAll()
{
	Singleton->CancelSignInRequest();
	this->GetOwningPlayer()->GetWorld()->GetTimerManager().ClearTimer(TimeRemainingTimerHandle);
}

void UWalletConnectCode::StartAttempt()
{
	this->TimeRemaining = ConnectionRefreshTime;

	Singleton->OnGetQRCodeCompleted.RemoveDynamic(this, &UWalletConnectCode::QRCodeCompleted);
	Singleton->OnLoginFinished.RemoveDynamic(this, &UWalletConnectCode::AccessTokenCompleted);
	Singleton->OnGetHandshakeCompleted.RemoveDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);

	Singleton->OnGetQRCodeCompleted.AddDynamic(this, &UWalletConnectCode::QRCodeCompleted);
	
	Singleton->GetQRCode();
	this->SetBrush(FSlateBrush()); //Clear out this widget so the user doesn't try to scan an old QR code if one has been downloaded already

	Singleton->OnLoginFinished.AddDynamic(this, &UWalletConnectCode::AccessTokenCompleted);
	Singleton->OnGetHandshakeCompleted.AddDynamic(this, &UWalletConnectCode::GetHandshakeCompleted);
}

void UWalletConnectCode::UpdateTimeRemaining()
{
	if (this->TimeRemaining > 0) {
		this->TimeRemaining--;
	}
}

void UWalletConnectCode::ReinitializeWalletConnectCompleted(EErrorCode StatusCode) {
	if (StatusCode == EErrorCode::EmergenceOk) {
		this->StartAttempt();
	}
}