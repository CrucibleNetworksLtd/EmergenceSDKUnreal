// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "AvatarService/AvatarServiceLibrary.h"
#include "Kismet/KismetStringLibrary.h"

bool UAvatarServiceLibrary::GetEmergencePreferredNodeURL(FString Blockchain, FString &URL) {
	const TMap<FString, FString> BlockchainToURL = {
		{"RINKEBY", "https://rinkeby.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134"},
		{"POLYGON","https://polygon-mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134"},
		{"Ethereum","https://mainnet.infura.io/v3/cb3531f01dcf4321bbde11cd0dd25134"}
	};
	if (BlockchainToURL.Contains(Blockchain)) {
		URL = *BlockchainToURL.Find(Blockchain);
		return true;
	}
	else {
		return false;
	}
}

FEmergenceAvatarData UAvatarServiceLibrary::FindAvatarFromString(TArray<FEmergenceAvatarResult> Avatars, FString AvatarString, bool& FoundAvatar)
{
	if (AvatarString.IsEmpty() || Avatars.Num() == 0) { //if the data was empty
		FoundAvatar = false;
		return FEmergenceAvatarData();
	}

	TArray<FString> AvatarStringParts = UKismetStringLibrary::ParseIntoArray(AvatarString, ":", true);

	if (AvatarStringParts.Num() < 4) { //if there wasn't enough data (formatted wrong)
		FoundAvatar = false;
		return FEmergenceAvatarData();
	}

	for (int i = 0; i < Avatars.Num(); i++) {
		for (int j = 0; j < Avatars[i].Avatars.Num(); j++) {
			if (Avatars[i].chain == AvatarStringParts[0] &&
				Avatars[i].contractAddress == AvatarStringParts[1] &&
				Avatars[i].tokenId == AvatarStringParts[2] &&
				Avatars[i].Avatars[j].GUID == AvatarStringParts[3])
			{
				FEmergenceAvatarData ReturnValue;
				ReturnValue.AvatarNFT = Avatars[i];
				ReturnValue.Avatar = Avatars[i].Avatars[j];
				FoundAvatar = true;
				return ReturnValue;
			}
		}
	}
	FoundAvatar = false;
	return FEmergenceAvatarData(); //if we didn't find it
}
