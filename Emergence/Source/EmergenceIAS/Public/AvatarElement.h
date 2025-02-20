// Copyright Crucible Networks Ltd 2024. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "JsonObjectConverter.h"
#include "EmergenceInteroperableAssetElement.h"
#include "AvatarElement.generated.h"

USTRUCT(Category = "Emergence|Interoperable Asset System", BlueprintType)
struct FAvatarElementInner
{

    GENERATED_BODY()

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite)
    FString ElementName;

    FAvatarElementInner() {};

    FAvatarElementInner(FString _json_) {
        FAvatarElementInner _tmpEmergenceInteroperableAssetAvatarElementInner;

        FJsonObjectConverter::JsonObjectStringToUStruct<FAvatarElementInner>(
            _json_,
            &_tmpEmergenceInteroperableAssetAvatarElementInner,
            0, 0);

        ElementName = _tmpEmergenceInteroperableAssetAvatarElementInner.ElementName;
    }

};

UCLASS(Category= "Emergence|Interoperable Asset System", BlueprintType)
class EMERGENCEIAS_API UAvatarElement : public UEmergenceInteroperableAssetElement
{
  GENERATED_BODY()

public:
    UAvatarElement() {
        ElementName = "avatar";
    }

    UPROPERTY(Category = "Emergence|Interoperable Asset System", EditAnywhere, BlueprintReadWrite, meta = (ShowOnlyInnerProperties))
    FAvatarElementInner AvatarElementData;
    
};