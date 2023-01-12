// Fill out your copyright notice in the Description page of Project Settings.

#include "EmergenceChainAssetTypeActions.h"
#include "EmergenceChainObject.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

FEmergenceChainAssetTypeActions::FEmergenceChainAssetTypeActions(EAssetTypeCategories::Type Category)
    : MyAssetCategory(Category)
{
}

UClass* FEmergenceChainAssetTypeActions::GetSupportedClass() const
{
    return UEmergenceChain::StaticClass();
}

FText FEmergenceChainAssetTypeActions::GetName() const
{
    return INVTEXT("Blockchain");
}

FColor FEmergenceChainAssetTypeActions::GetTypeColor() const
{
    return FColor::Cyan;
}

uint32 FEmergenceChainAssetTypeActions::GetCategories()
{
    return MyAssetCategory;
}

bool FEmergenceChainAssetTypeActions::CanLocalize() const
{
    return false;
}
