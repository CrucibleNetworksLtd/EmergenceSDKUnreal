// Fill out your copyright noticetd 2022. All Rights Reserved.


#include "EmergenceVRMMeshComponent.h"
#include "UObject/UObjectGlobals.h"
#include "LoaderBPFunctionLibrary.h"
#include "VRMAnimInstanceCopy.h"
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

void UEmergenceVRMMeshComponent::ActivateVRMMeshFromData(const TArray<uint8>& Data, const EVRMImportMaterialType MaterialType) {
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

	OptionForRuntimeLoad.MaterialType = MaterialType;

	ULoaderBPFunctionLibrary::LoadVRMFromMemoryAsync(this->GetOwner(), VrmAssetListObject, OutVrmAsset, Data, OptionForRuntimeLoad, LatentInfo);
}

const EVRMImportMaterialType UEmergenceVRMMeshComponent::MaterialTypeFromString(const FString MaterialString)
{
	FString SanitizedMaterialString = MaterialString.ToLower();

	//MToonUnlit
	if (MaterialString == "mtoonunlit") {
		return EVRMImportMaterialType::VRMIMT_MToonUnlit;
	}

	//MToonLit
	if (MaterialString == "mtoonlit") {
		return EVRMImportMaterialType::VRMIMT_MToon;
	}

	//PBR
	if (MaterialString == "pbr") {
		return EVRMImportMaterialType::VRMIMT_glTF;
	}

	//Subsurface
	if (MaterialString == "subsurface") {
		return EVRMImportMaterialType::VRMIMT_SSS;
	}

	//Subsurface Profile
	if (MaterialString == "subsurfaceprofile") {
		return EVRMImportMaterialType::VRMIMT_SSSProfile;
	}

	//Unlit
	if (MaterialString == "unlit") {
		return EVRMImportMaterialType::VRMIMT_Unlit;
	}

	return EVRMImportMaterialType::VRMIMT_MToonUnlit;
}

void UEmergenceVRMMeshComponent::VRMLoadCompleted(int Linkage)
{
	USkeletalMeshComponent* ParentSkeletalMesh = Cast<USkeletalMeshComponent>(GetAttachParent());
	ParentSkeletalMesh->SetSkeletalMesh(OutVrmAsset->SkeletalMesh, false);
	ParentSkeletalMesh->SetAnimClass(UVrmAnimInstanceCopy::StaticClass());
	if (ParentSkeletalMesh->GetAnimInstance()) {
		Cast<UVrmAnimInstanceCopy>(ParentSkeletalMesh->GetAnimInstance())->SetSkeletalMeshCopyData(OutVrmAsset, this, nullptr, VRoidSimpleAssetList, nullptr);
	}
}
