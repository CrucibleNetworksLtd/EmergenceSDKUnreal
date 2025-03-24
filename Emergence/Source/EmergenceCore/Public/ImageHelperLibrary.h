// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/Texture2D.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "TextureResource.h"
#include "ImageHelperLibrary.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCECORE_API UImageHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//Takes a JPEG or PNG image and turns it into a UTexture2D
	static bool RawDataToBrush(FName ResourceName, const TArray<uint8>& InRawData, UTexture2D*& LoadedT2D)
	{
		int32 Width;
		int32 Height;

		TArray<uint8> DecodedImage;
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		TSharedPtr<IImageWrapper> ImageWrapper;

		if (InRawData.Num() == 0) { //if there is no raw data, fail out
			return false;
		}

		if (InRawData[0] == 0x89) {
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
		}
		else if (InRawData[0] == 0xFF && InRawData[1] == 0xD8 && InRawData[2] == 0xFF) {
			ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
		}
		else {
			return false;
		}

		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(InRawData.GetData(), InRawData.Num()))
		{
			TArray<uint8> UncompressedBGRA;
			if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
			{
				LoadedT2D = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);

				if (!LoadedT2D) return false;

				Width = ImageWrapper->GetWidth();
				Height = ImageWrapper->GetHeight();
#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5)
				void* TextureData = LoadedT2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
				LoadedT2D->GetPlatformData()->Mips[0].BulkData.Unlock();
#else
				void* TextureData = LoadedT2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
				LoadedT2D->PlatformData->Mips[0].BulkData.Unlock();
#endif
				LoadedT2D->UpdateResource();
				return true;
			}
		}
		return false;
	}
};
