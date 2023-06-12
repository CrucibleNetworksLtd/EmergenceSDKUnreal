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
	/**
	 * Activates the given VRM model (as raw data) on this VRM Mesh Component.
	 * @param Data Bytes of the VRM model.
	 * @param MaterialType Defaults to MToonUnlit. Usually should be used with "MaterialTypeFromString" method to convert the "Material Type" information from AvatarByID or AvatarByOwner.
	 */
	UFUNCTION(BlueprintCallable)
	void ActivateVRMMeshFromData(const TArray<uint8>& Data, const EVRMImportMaterialType MaterialType = EVRMImportMaterialType::VRMIMT_MToonUnlit);
	
	UFUNCTION(BlueprintPure)
	static const EVRMImportMaterialType MaterialTypeFromString(const FString MaterialString);
	
	UFUNCTION()
	void VRMLoadCompleted(int Linkage);

	UPROPERTY()
	UVrmAssetListObject* VrmAssetListObject;

	UPROPERTY()
	UVrmAssetListObject* OutVrmAsset;

	UPROPERTY()
	TSubclassOf<UVrmAssetListObject> VrmAssetListObjectBPClass;

	UPROPERTY()
	UVrmAssetListObject* VRoidSimpleAssetList;

	UPROPERTY()
	FLatentActionInfo LatentInfo;

	UPROPERTY()
	FImportOptionData OptionForRuntimeLoad;
private:
	
	
	
};
