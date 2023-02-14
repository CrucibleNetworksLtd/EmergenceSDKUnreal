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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence Internal|Avatar Service|AvatarData")
	FEmergenceAvatarResult AvatarNFT;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Emergence Internal|Avatar Service|AvatarData")
	FEmergenceAvatarMetadata Avatar;
};

UCLASS()
class EMERGENCE_API UAvatarServiceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|Avatar Service")
	static bool GetEmergencePreferredNodeURL(FString Blockchain, FString& URL);

	//Given an array of avatar results, find the one that matches the avatar string (which is in the format [BLOCKCHAIN]:[ADDRESS]:[TOKENID]:[AVATARGUID])
	UFUNCTION(BlueprintPure, Category = "Emergence|Helpers")
	static FEmergenceAvatarData FindAvatarFromString(TArray<FEmergenceAvatarResult> Avatars, FString AvatarString, bool& FoundAvatar);
};
