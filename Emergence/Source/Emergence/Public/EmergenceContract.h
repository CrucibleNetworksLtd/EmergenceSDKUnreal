// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EmergenceContract.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceContractMethod
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Emergence|Contract|Method")
	FString MethodName;

	FEmergenceContractMethod() {};

	FEmergenceContractMethod(FString _MethodName) {
		MethodName = _MethodName;
	};
};

/**
 * Emergence Contract assets are used by various methods and objects as a descriptor of a contract's programming interface. Much of its data comes from the contract's ABI.
 */
UCLASS(BlueprintType, CollapseCategories)
class EMERGENCE_API UEmergenceContract : public UObject
{
	GENERATED_BODY()
	
public:
	UEmergenceContract() {};

	UEmergenceContract(FString _ABI);

	UFUNCTION(BlueprintPure, Category="Emergence|Contract")
	static UEmergenceContract* CreateEmergenceContract(FString _ABI);
	
	UPROPERTY(EditAnywhere, Category="Contract")
	FString ABI;

	UPROPERTY(VisibleAnywhere, Category="Contract Methods")
	TArray<FEmergenceContractMethod> ReadMethods;

	UPROPERTY(VisibleAnywhere, Category="Contract Methods")
	TArray<FEmergenceContractMethod> WriteMethods;

	UFUNCTION(CallInEditor, Category="Contract Methods")
	void FindMethods();
};
