// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "EmergenceChainFactory.h"
#include "EmergenceChainObject.h"

UEmergenceChainFactory::UEmergenceChainFactory() {
    SupportedClass = UEmergenceChain::StaticClass();
    bCreateNew = true;
}

UObject* UEmergenceChainFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return NewObject<UEmergenceChain>(InParent, Class, Name, Flags, Context);
}
