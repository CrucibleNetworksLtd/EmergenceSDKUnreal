// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "EmergenceContractFactory.h"
#include "EmergenceContract.h"

UEmergenceContractFactory::UEmergenceContractFactory() {
    SupportedClass = UEmergenceContract::StaticClass();
    bCreateNew = true;
}

UObject* UEmergenceContractFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return NewObject<UEmergenceContract>(InParent, Class, Name, Flags, Context);
}
