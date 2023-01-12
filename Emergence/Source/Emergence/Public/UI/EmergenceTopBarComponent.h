// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ErrorCodeFunctionLibrary.h"
#include "EmergenceChainObject.h"
#include "JsonObjectWrapper.h"
#include "EmergenceTopBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UEmergenceTopBarComponent : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void StartGetBalanceTextAsync();

	//returns false if the balance shouldn't be shown
	UFUNCTION(BlueprintPure)
	bool ShouldDisplayBalanceText();

	UFUNCTION()
	void GetBalanceResponseHandler(FString Balance, EErrorCode StatusCode);

	UFUNCTION()
	void BalanceOfResponseHandler(FJsonObjectWrapper Response, EErrorCode StatusCode);

	UFUNCTION()
	void SymbolResponseHandler(FJsonObjectWrapper Response, EErrorCode StatusCode);

	UFUNCTION(BlueprintPure)
	FString GetBalanceText();

	UFUNCTION(BlueprintImplementableEvent)
	void BalanceTextUpdated();

protected:
	UPROPERTY(BlueprintReadOnly)
	FString CurrencyDisplayText = "";

private:
	UPROPERTY()
	UEmergenceChain* CurrencyDisplayChainData;

	UPROPERTY()
	FString ReturnedBalance = "";

	UPROPERTY()
	FString ReturnedSymbol = "";
};
