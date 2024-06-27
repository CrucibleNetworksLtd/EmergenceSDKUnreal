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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blockchain")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blockchain")
	FString NodeURL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blockchain")
	FString Symbol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Blockchain")
	int64 ChainID;


	UFUNCTION()
	static UEmergenceChain* GetEmergenceChainDataFromConfig(const UObject* Outer);

	/**
	 * Creates a Emergence Blockchain object, which represents a single blockchain.
	 * @param _Name Name of the blockchain.
	 * @param _NodeURL RPC URL of the blockchain.
	 * @param _Symbol Symbol of the blockchain (for example ETH, or MATIC).
	 * @param _ChainID Chain ID of the blockchain.
	 */
	UFUNCTION(BlueprintPure, Category="Emergence|Blockchain")
	static UEmergenceChain* CreateEmergenceChain(FText _Name, FString _NodeURL, FString _Symbol, int64 _ChainID);
};
