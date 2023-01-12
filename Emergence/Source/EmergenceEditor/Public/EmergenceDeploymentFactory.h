// Fill out your copyright notice in the Description page of Project Settings.

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
