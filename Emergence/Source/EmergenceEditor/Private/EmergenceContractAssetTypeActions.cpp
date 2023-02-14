// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceContractAssetTypeActions.h"
#include "EmergenceContract.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

FEmergenceContractAssetTypeActions::FEmergenceContractAssetTypeActions(EAssetTypeCategories::Type Category)
    : MyAssetCategory(Category)
{
}

UClass* FEmergenceContractAssetTypeActions::GetSupportedClass() const
{
    return UEmergenceContract::StaticClass();
}

FText FEmergenceContractAssetTypeActions::GetName() const
{
    return INVTEXT("Contract");
}

FColor FEmergenceContractAssetTypeActions::GetTypeColor() const
{
    return FColor::Cyan;
}

uint32 FEmergenceContractAssetTypeActions::GetCategories()
{
    return MyAssetCategory;
}

bool FEmergenceContractAssetTypeActions::CanLocalize() const
{
    return false;
}
