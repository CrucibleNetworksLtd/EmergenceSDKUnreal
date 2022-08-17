// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VrmAssetListObject.h"
#include "VRMSupportLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCEVRMSUPPORT_API UVRMSupportLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void LoadVRMFileFromMemory(const UVrmAssetListObject* InVrmAsset, UVrmAssetListObject*& OutVrmAsset, TArray<uint8> Data, const FImportOptionData& OptionForRuntimeLoad);
};
