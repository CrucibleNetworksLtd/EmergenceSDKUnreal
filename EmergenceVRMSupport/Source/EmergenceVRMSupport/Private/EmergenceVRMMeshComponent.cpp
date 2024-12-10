// Fill out your copyright noticetd 2022. All Rights Reserved.


#include "EmergenceVRMMeshComponent.h"
#include "UObject/UObjectGlobals.h"
#include "LoaderBPFunctionLibrary.h"
#include "VrmAnimInstanceCopy.h"
#include "UObject/ConstructorHelpers.h"
#include "LoaderBPFunctionLibrary.h"
#include "Engine/LatentActionManager.h"
#include "Animation/Skeleton.h"

UEmergenceVRMMeshComponent::UEmergenceVRMMeshComponent()
{
	this->SetVisibility(false, false);
	static ConstructorHelpers::FClassFinder<UVrmAssetListObject> VrmAssetListObjectBP(TEXT("/VRM4U/VrmAssetListObjectBP"));
	static ConstructorHelpers::FObjectFinder<UVrmAssetListObject> VRoidSimpleAssetListBP(TEXT("VrmAssetListObject'/VRM4U/Util/BaseCharacter/Mesh/VRoidSimple_VrmAssetList.VRoidSimple_VrmAssetList'"));
	VrmAssetListObjectBPClass = VrmAssetListObjectBP.Class;
	VRoidSimpleAssetList = VRoidSimpleAssetListBP.Object;
}

void UEmergenceVRMMeshComponent::ActivateVRMMeshFromData(const TArray<uint8>& Data, const EEmergenceVRMImportMaterialType MaterialType, UMaterial* MaterialOverride) {
	if (!VrmAssetListObjectBPClass) {
		UE_LOG(LogTemp, Error, TEXT("Couldn't find /VRM4U/VrmAssetListObjectBP.VrmAssetListObjectBP"));
		return;
	}

	if (!VRoidSimpleAssetList) {
		UE_LOG(LogTemp, Error, TEXT("Couldn't find /VRM4U/Util/BaseCharacter/Mesh/VRoidSimple_VrmAssetList.VRoidSimple_VrmAssetList"));
		return;
	}

	if (Data.Num() < 1) {
		UE_LOG(LogTemp, Error, TEXT("No data was provided to ActivateVRMMeshFromData"));
		return;
	}

	VrmAssetListObject = VrmAssetListObjectBPClass.GetDefaultObject();
	
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName(TEXT("VRMLoadCompleted"));
	LatentInfo.UUID = FGuid::NewGuid().A;
	LatentInfo.Linkage = 1;

	if ((int)MaterialType < (int)EVRMImportMaterialType::VRMIMT_MAX) {
		OptionForRuntimeLoad.MaterialType = (EVRMImportMaterialType)MaterialType;
	}
	else {
		if (MaterialType == EEmergenceVRMImportMaterialType::VRMIMT_EmergenceMToonUnlit && MaterialOverride) {
			VrmImportMaterialSet = NewObject<UVrmImportMaterialSet>();
			VrmImportMaterialSet->Opaque = MaterialOverride;
			VrmImportMaterialSet->OpaqueTwoSided = MaterialOverride;
			VrmImportMaterialSet->Translucent = MaterialOverride;
			VrmImportMaterialSet->TranslucentTwoSided = MaterialOverride;
			VrmAssetListObject->MtoonUnlitSet = VrmImportMaterialSet;
			OptionForRuntimeLoad.MaterialType = EVRMImportMaterialType::VRMIMT_MToonUnlit;
		}
	}

	if (!IsLoading) {
		IsLoading = true;
		ULoaderBPFunctionLibrary::LoadVRMFromMemoryAsync(this->GetOwner(), VrmAssetListObject, OutVrmAsset, Data, OptionForRuntimeLoad, LatentInfo);
	}
	else {
		UE_LOG(LogAnimation, Error, TEXT("Tried to start loading a VRM while there is a VRM load in progress."));
	}
}

const EEmergenceVRMImportMaterialType UEmergenceVRMMeshComponent::MaterialTypeFromString(const FString MaterialString)
{
	FString SanitizedMaterialString = MaterialString.ToLower();

	//MToonUnlit
	if (MaterialString == "mtoonunlit") {
		return EEmergenceVRMImportMaterialType::VRMIMT_MToonUnlit;
	}

	//MToonLit
	if (MaterialString == "mtoonlit") {
		return EEmergenceVRMImportMaterialType::VRMIMT_MToon;
	}

	//PBR
	if (MaterialString == "pbr") {
		return EEmergenceVRMImportMaterialType::VRMIMT_glTF;
	}

	//Subsurface
	if (MaterialString == "subsurface") {
		return EEmergenceVRMImportMaterialType::VRMIMT_SSS;
	}

	//Subsurface Profile
	if (MaterialString == "subsurfaceprofile") {
		return EEmergenceVRMImportMaterialType::VRMIMT_SSSProfile;
	}

	//Unlit
	if (MaterialString == "unlit") {
		return EEmergenceVRMImportMaterialType::VRMIMT_Unlit;
	}

	if (MaterialString == "emergencemtoonunlit") {
		return EEmergenceVRMImportMaterialType::VRMIMT_EmergenceMToonUnlit;
	}

	return EEmergenceVRMImportMaterialType::VRMIMT_MToonUnlit;
}

void UEmergenceVRMMeshComponent::VRMLoadCompleted(int Linkage)
{
	USkeletalMeshComponent* ParentSkeletalMesh = Cast<USkeletalMeshComponent>(GetAttachParent());
	ParentSkeletalMesh->SetSkeletalMesh(OutVrmAsset->SkeletalMesh, false);
	ParentSkeletalMesh->SetAnimClass(UVrmAnimInstanceCopy::StaticClass());
	if (ParentSkeletalMesh->GetAnimInstance()) {
		Cast<UVrmAnimInstanceCopy>(ParentSkeletalMesh->GetAnimInstance())->SetSkeletalMeshCopyData(OutVrmAsset, this, nullptr, VRoidSimpleAssetList, nullptr);
	}
	IsLoading = false;
}
