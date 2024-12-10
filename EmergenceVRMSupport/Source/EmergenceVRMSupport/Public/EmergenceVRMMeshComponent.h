// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "VrmAssetListObject.h"
#include "EmergenceVRMMeshComponent.generated.h"


UENUM(BlueprintType)
enum class EEmergenceVRMImportMaterialType : uint8
{
	VRMIMT_Auto			UMETA(DisplayName = "Auto(MToon Unlit)"),
	VRMIMT_MToon		UMETA(DisplayName = "MToon Lit"),
	VRMIMT_MToonUnlit	UMETA(DisplayName = "MToon Unlit"),
	VRMIMT_SSS			UMETA(DisplayName = "Subsurface"),
	VRMIMT_SSSProfile	UMETA(DisplayName = "Subsurface Profile"),
	VRMIMT_Unlit		UMETA(DisplayName = "Unlit"),
	VRMIMT_glTF			UMETA(DisplayName = "PBR(glTF2)"),
	VRMIMT_Custom		UMETA(DisplayName = "Custom"),
	VRMIMT_EmergenceMToonUnlit  UMETA(DisplayName = "MToon Unlit Material Override"),

	VRMIMT_MAX,
};

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
	 * @param MaterialOverride This is only used if you select MToon Unlit Material Override (or use VRMIMT_EmergenceMToonUnlit if you're calling from C++). It allows you to override the material VRM4U uses for this VRM.
	 */
	UFUNCTION(BlueprintCallable)
	void ActivateVRMMeshFromData(const TArray<uint8>& Data, const EEmergenceVRMImportMaterialType MaterialType = EEmergenceVRMImportMaterialType::VRMIMT_MToonUnlit, UMaterial* MaterialOverride = nullptr);
	
	UFUNCTION(BlueprintPure)
	static const EEmergenceVRMImportMaterialType MaterialTypeFromString(const FString MaterialString);
	
	UFUNCTION()
	void VRMLoadCompleted(int Linkage);

	UPROPERTY()
	bool IsLoading = false;

	UPROPERTY()
	UVrmAssetListObject* VrmAssetListObject;

	UPROPERTY()
	UMaterial* VrmMToonBaseOpaque;

	UPROPERTY()
	UVrmImportMaterialSet* VrmImportMaterialSet;

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
