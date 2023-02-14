// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EmergenceDeploymentFactory.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCEEDITOR_API UEmergenceDeploymentFactory : public UFactory
{
	GENERATED_BODY()
public:
    UEmergenceDeploymentFactory();
    UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);
};
