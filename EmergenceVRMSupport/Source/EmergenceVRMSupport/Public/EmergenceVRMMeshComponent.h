// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "VrmAssetListObject.h"
#include "EmergenceVRMMeshComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, ClassGroup = (Rendering, Common), meta = (BlueprintSpawnableComponent), HideCategories=("Clothing Simulation", Collision, Rendering, "Skin Weights", Navigation, "Virtual Texture", Events, "Material Parameters", Sockets, Transform))
class EMERGENCEVRMSUPPORT_API UEmergenceVRMMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

	UEmergenceVRMMeshComponent();

public:
	UFUNCTION(BlueprintCallable)
	void ActivateVRMMeshFromData(const TArray<uint8>& Data);
private:
	TSubclassOf<UVrmAssetListObject> VrmAssetListObjectBPClass;
	UVrmAssetListObject* VRoidSimpleAssetList;
};
