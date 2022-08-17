// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "VRMSupportLibrary.h"
#include "LoaderBPFunctionLibrary.h"

void UVRMSupportLibrary::LoadVRMFileFromMemory(const UVrmAssetListObject* InVrmAsset, UVrmAssetListObject*& OutVrmAsset, TArray<uint8> Data, const FImportOptionData& OptionForRuntimeLoad) {
	VRMConverter::Options::Get().SetVrmOption(&OptionForRuntimeLoad);
	ULoaderBPFunctionLibrary::LoadVRMFileFromMemory(InVrmAsset, OutVrmAsset, TEXT("C:\\thing.vrm"), Data.GetData(), Data.Num());
}