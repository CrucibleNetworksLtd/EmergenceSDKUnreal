// Fill out your copyright noticetd 2022. All Rights Reserved.


#include "EmergenceVRMMeshComponent.h"
#include "UObject/UObjectGlobals.h"
#include "LoaderBPFunctionLibrary.h"
#include "VRMSupportLibrary.h"
#include "VRMAnimInstanceCopy.h"
#include "UObject/ConstructorHelpers.h"

UEmergenceVRMMeshComponent::UEmergenceVRMMeshComponent()
{
	this->SetVisibility(false, false);
	static ConstructorHelpers::FClassFinder<UVrmAssetListObject> VrmAssetListObjectBP(TEXT("/VRM4U/VrmAssetListObjectBP"));
	static ConstructorHelpers::FObjectFinder<UVrmAssetListObject> VRoidSimpleAssetListBP(TEXT("VrmAssetListObject'/VRM4U/Util/BaseCharacter/Mesh/VRoidSimple_VrmAssetList.VRoidSimple_VrmAssetList'"));
	VrmAssetListObjectBPClass = VrmAssetListObjectBP.Class;
	VRoidSimpleAssetList = VRoidSimpleAssetListBP.Object;
}

void UEmergenceVRMMeshComponent::ActivateVRMMeshFromData(const TArray<uint8>& Data) {
	USkeletalMeshComponent* ParentSkeletalMesh = Cast<USkeletalMeshComponent>(GetAttachParent());
	if (!ParentSkeletalMesh) {
		UE_LOG(LogTemp, Error, TEXT("Attach Parent of a EmergenceVRMMeshComponent must be a SkeletalMesh"));
		return;
	}
	
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

	UVrmAssetListObject* VrmAssetListObject = NewObject<UVrmAssetListObject>(this, VrmAssetListObjectBPClass);

	FImportOptionData OptionForRuntimeLoad;
	UVrmAssetListObject* OutVrmAsset = NewObject<UVrmAssetListObject>(this, VrmAssetListObjectBPClass);
	UVRMSupportLibrary::LoadVRMFileFromMemory(VrmAssetListObject, OutVrmAsset, Data, OptionForRuntimeLoad);

	ParentSkeletalMesh->SetSkeletalMesh(OutVrmAsset->SkeletalMesh, true);
	ParentSkeletalMesh->SetAnimClass(UVrmAnimInstanceCopy::StaticClass());
	if (ParentSkeletalMesh->GetAnimInstance()) {
		Cast<UVrmAnimInstanceCopy>(ParentSkeletalMesh->GetAnimInstance())->SetSkeletalMeshCopyData(OutVrmAsset, this, nullptr, VRoidSimpleAssetList, nullptr);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Couldn't GetAnimInstance, the VRM probably failed to load"));
		return;
	}
}