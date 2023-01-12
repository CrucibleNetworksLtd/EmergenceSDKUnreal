// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergenceChainObject.generated.h"

/**
 * Emergence Blockchain assets are used by various methods and objects as a reference to a blockchain, including data about the blockchain and the node that connects to it.
 */
UCLASS(BlueprintType, CollapseCategories)
class EMERGENCE_API UEmergenceChain : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceChain() {};

	UEmergenceChain(FString _NodeURL);

	UEmergenceChain(FText Name, FString _NodeURL, FString Symbol);

	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FString NodeURL;

	UPROPERTY(EditAnywhere)
	FString Symbol;

	UPROPERTY(EditAnywhere)
	int ChainID;


	UFUNCTION()
	static UEmergenceChain* GetEmergenceChainDataFromConfig(const UObject* Outer);

	UFUNCTION(BlueprintPure)
	static UEmergenceChain* CreateEmergenceChain(FText _Name, FString _NodeURL, FString _Symbol, int _ChainID);
};
