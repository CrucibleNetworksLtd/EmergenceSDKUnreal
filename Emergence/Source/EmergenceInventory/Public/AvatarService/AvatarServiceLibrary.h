// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AvatarService/AvatarByOwner.h"
#include "AvatarServiceLibrary.generated.h"

USTRUCT(BlueprintType)
struct FEmergenceAvatarData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Avatar Service|AvatarData")
	FEmergenceAvatarResult AvatarNFT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence|Avatar Service|AvatarData")
	FEmergenceAvatarMetadata Avatar;
};