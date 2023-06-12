// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "EmergenceAsyncActionBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class EMERGENCE_API UEmergenceAsyncActionBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	virtual class UWorld* GetWorld() const override;

protected:
	UObject* WorldContextObject;
};
