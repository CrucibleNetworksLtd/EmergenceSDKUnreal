// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EmergenceChainObject.h"
#include "EmergenceContract.h"
#include "EmergenceDeployment.generated.h"

/**
 * Emergence Deployed Contract assets are used by various methods and objects as a reference to a specific contract deployed on-chain. It is made from a Blockchain asset, a Contract asset and an address.
 */
UCLASS(BlueprintType, CollapseCategories, meta=(DisplayName="Emergence Deployed Contract"))
class EMERGENCE_API UEmergenceDeployment : public UObject
{

  GENERATED_BODY()

public:
	UEmergenceDeployment() {};

	UEmergenceDeployment(FString Address, UEmergenceChain* Blockchain, UEmergenceContract* Contract);

	/**
	 * Creates a Emergence Deployed Contract object, which represents a contract on chain.
	 * @param Address The Address of the contract on the blockchain.
	 * @param Blockchain A blockchain object representing the blockchain that this contract resides on.
	 * @param Contract A contract object which contains the ABI of this contract.
	 */
	UFUNCTION(BlueprintPure, Category="Emergence|Deployed Contract")
	static UEmergenceDeployment* CreateEmergenceDeployment(FString Address, UEmergenceChain* Blockchain, UEmergenceContract* Contract);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deployed Contract")
	UEmergenceChain* Blockchain;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deployed Contract")
	UEmergenceContract* Contract;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Deployed Contract")
	FString Address;
  
};