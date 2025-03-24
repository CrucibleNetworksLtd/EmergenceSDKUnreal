// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "EmergenceInteroperableAssetElement.h"
#include "ThumbnailsElement.generated.h"

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FThumbnailsElementInnerThumbnail
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    FString Id;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    FString Url;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    int Width;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    int Height;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    int CropStyle;

    UPROPERTY(Category = "Emergence|Interoperable Asset System|Thumbnail", EditAnywhere, BlueprintReadWrite)
    FString FileFormat;

    FThumbnailsElementInnerThumbnail() {};

    FThumbnailsElementInnerThumbnail(
        FString _Id,
        FString _Url,
        int _Width,
        int _Height,
        int _CropStyle,
        FString _FileFormat) {
        Id = _Id;
        Url = _Url;
        Width = _Width;
        Height = _Height;
        CropStyle = _CropStyle;
        FileFormat = _FileFormat;
    }

};

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FThumbnailsElementInner
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FThumbnailsElementInnerThumbnail SmallThumbnail;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FThumbnailsElementInnerThumbnail LargeThumbnail;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    TArray<FThumbnailsElementInnerThumbnail> OtherThumbnails;

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString ElementName;

    FThumbnailsElementInner() {};

    FThumbnailsElementInner(FString _json_) {
        FThumbnailsElementInner _tmpEmergenceInteroperableAssetThumbnailsElementInner;

        FJsonObjectConverter::JsonObjectStringToUStruct<FThumbnailsElementInner>(
            _json_,
            &_tmpEmergenceInteroperableAssetThumbnailsElementInner,
            0, 0);

        ElementName = _tmpEmergenceInteroperableAssetThumbnailsElementInner.ElementName;
        SmallThumbnail = _tmpEmergenceInteroperableAssetThumbnailsElementInner.SmallThumbnail;
        LargeThumbnail = _tmpEmergenceInteroperableAssetThumbnailsElementInner.LargeThumbnail;
        OtherThumbnails = _tmpEmergenceInteroperableAssetThumbnailsElementInner.OtherThumbnails;
    }

};

UCLASS(Category= "Emergence|Interoperable Asset System", BlueprintType)
class EMERGENCEIAS_API UThumbnailsElement : public UEmergenceInteroperableAssetElement
{
  GENERATED_BODY()

public:
    UThumbnailsElement() {
        ElementName = "thumbnails";
    }

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FThumbnailsElementInner ThumbnailsElementData;
    
};