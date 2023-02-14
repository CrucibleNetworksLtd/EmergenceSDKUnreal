// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

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

	UPROPERTY(EditAnywhere, Category="Blockchain")
	FText Name;

	UPROPERTY(EditAnywhere, Category="Blockchain")
	FString NodeURL;

	UPROPERTY(EditAnywhere, Category="Blockchain")
	FString Symbol;

	UPROPERTY(EditAnywhere, Category="Blockchain")
	int ChainID;


	UFUNCTION()
	static UEmergenceChain* GetEmergenceChainDataFromConfig(const UObject* Outer);

	UFUNCTION(BlueprintPure, Category="Emergence|Blockchain")
	static UEmergenceChain* CreateEmergenceChain(FText _Name, FString _NodeURL, FString _Symbol, int _ChainID);
};
