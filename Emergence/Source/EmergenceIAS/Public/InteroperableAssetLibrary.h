// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "EmergenceInteroperableAssetElement.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "InteroperableAssetLibrary.generated.h"

UCLASS()
class EMERGENCEIAS_API UEmergenceInteroperableAssetLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /**
     * Given an array of Emergence IA Elements, find the one that matches a given Element class.
     * @param Array An array of Emergence IA Elements.
     * @param ElementClass A class of Elements to find.
     */
    UFUNCTION(BlueprintPure, meta=(DeterminesOutputType="ElementClass"), Category = "Emergence|Interoperable Asset System")
    static UEmergenceInteroperableAssetElement* GetInteroperableAssetElement(TArray<UEmergenceInteroperableAssetElement*> Array, UPARAM(meta = (AllowAbstract = false))TSubclassOf<UEmergenceInteroperableAssetElement> ElementClass, bool& success);
};