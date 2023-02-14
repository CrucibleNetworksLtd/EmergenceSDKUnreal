// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

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
	UFUNCTION(BlueprintCallable, Category="Emergence Internal|UI|Top Bar")
	void StartGetBalanceTextAsync();

	//returns false if the balance shouldn't be shown
	UFUNCTION(BlueprintPure, Category="Emergence Internal|UI|Top Bar")
	bool ShouldDisplayBalanceText();

	UFUNCTION()
	void GetBalanceResponseHandler(FString Balance, EErrorCode StatusCode);

	UFUNCTION()
	void BalanceOfResponseHandler(FJsonObjectWrapper Response, EErrorCode StatusCode);

	UFUNCTION()
	void SymbolResponseHandler(FJsonObjectWrapper Response, EErrorCode StatusCode);

	UFUNCTION(BlueprintPure, Category="Emergence Internal|UI|Top Bar")
	FString GetBalanceText();

	UFUNCTION(BlueprintImplementableEvent, Category="Emergence Internal|UI|Top Bar")
	void BalanceTextUpdated();

protected:
	UPROPERTY(BlueprintReadOnly, Category="Emergence Internal|UI|Top Bar")
	FString CurrencyDisplayText = "";

private:
	UPROPERTY()
	UEmergenceChain* CurrencyDisplayChainData;

	UPROPERTY()
	FString ReturnedBalance = "";

	UPROPERTY()
	FString ReturnedSymbol = "";
};
