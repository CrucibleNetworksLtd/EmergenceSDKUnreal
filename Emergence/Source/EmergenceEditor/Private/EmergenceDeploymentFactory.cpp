// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "EmergenceDeploymentFactory.h"
#include "EmergenceDeployment.h"

UEmergenceDeploymentFactory::UEmergenceDeploymentFactory() {
    SupportedClass = UEmergenceDeployment::StaticClass();
    bCreateNew = true;
}

UObject* UEmergenceDeploymentFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return NewObject<UEmergenceDeployment>(InParent, Class, Name, Flags, Context);
}
