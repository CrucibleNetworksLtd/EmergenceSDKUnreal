// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "EmergenceInteroperableAssetElement.generated.h"

UCLASS(Abstract, HideDropdown, Category = "Emergence|Interoperable Asset System")
class EMERGENCEIAS_API UEmergenceInteroperableAssetElement : public UObject
{
    GENERATED_BODY()
public:
    FString ElementName;
};