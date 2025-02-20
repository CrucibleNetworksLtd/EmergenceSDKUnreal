// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ARTMBuilderLibrary.generated.h"

UENUM(BlueprintType)
enum class EFutureverseARTMOperationType : uint8 {
	CREATELINK       UMETA(DisplayName = "Create Asset Link"),
	DELETELINK        UMETA(DisplayName = "Delete Asset Link"),
	NONE
};

USTRUCT(BlueprintType)
struct FFutureverseARTMOperation
{

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Emergence|Futureverse|Asset Registry|ATRM Operation")
	EFutureverseARTMOperationType OperationType = EFutureverseARTMOperationType::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse|Asset Registry|ATRM Operation")
	FString Slot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse|Asset Registry|ATRM Operation")
	FString LinkA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Futureverse|Asset Registry|ATRM Operation")
	FString LinkB;

	FFutureverseARTMOperation() {};

	FFutureverseARTMOperation(EFutureverseARTMOperationType _OperationType, FString _Slot, FString _LinkA, FString _LinkB) : OperationType(_OperationType), Slot(_Slot), LinkA(_LinkA), LinkB(_LinkB) {};
	
};

UCLASS()
class EMERGENCEFUTUREVERSEASSETREGISTRY_API UARTMBuilderLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//ADDRESS IS EOA ADDRESS / MUST BE ETH CHECKSUMMED
	UFUNCTION()
	static FString GenerateARTM(FString Message, TArray<FFutureverseARTMOperation> ARTMOperations, FString Address, FString Nonce);

};
