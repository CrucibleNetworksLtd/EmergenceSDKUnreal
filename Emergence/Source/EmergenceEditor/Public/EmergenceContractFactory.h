// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EmergenceContractFactory.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCEEDITOR_API UEmergenceContractFactory : public UFactory
{
	GENERATED_BODY()
public:
    UEmergenceContractFactory();
    UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);
};
