// Fill out your copyright notice in the Description page of Project Settings.

#include "EmergenceDeploymentAssetTypeActions.h"
#include "EmergenceDeployment.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

FEmergenceDeploymentAssetTypeActions::FEmergenceDeploymentAssetTypeActions(EAssetTypeCategories::Type Category)
    : MyAssetCategory(Category)
{
}

UClass* FEmergenceDeploymentAssetTypeActions::GetSupportedClass() const
{
    return UEmergenceDeployment::StaticClass();
}

FText FEmergenceDeploymentAssetTypeActions::GetName() const
{
    return INVTEXT("Deployed Contract");
}

FColor FEmergenceDeploymentAssetTypeActions::GetTypeColor() const
{
    return FColor::Cyan;
}

uint32 FEmergenceDeploymentAssetTypeActions::GetCategories()
{
    return MyAssetCategory;
}

bool FEmergenceDeploymentAssetTypeActions::CanLocalize() const
{
    return false;
}
