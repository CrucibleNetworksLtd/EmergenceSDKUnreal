// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "EmergenceChainObject.h"

UEmergenceChain::UEmergenceChain(FString _NodeURL)
{
	this->NodeURL = _NodeURL;
}

UEmergenceChain* UEmergenceChain::GetEmergenceChainDataFromConfig(const UObject* Outer)
{
	FString FEmergenceChainStructText;
	if (GConfig->GetString(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), TEXT("Chain"), FEmergenceChainStructText, GGameIni))
	{
		auto FoundObject = Cast<UEmergenceChain>(StaticLoadObject(UEmergenceChain::StaticClass(), const_cast<UObject*>(Outer), *FEmergenceChainStructText));
		if (FoundObject) {
			return FoundObject;
		}
	}
    return Cast<UEmergenceChain>(StaticLoadObject(UEmergenceChain::StaticClass(), const_cast<UObject*>(Outer), TEXT("/Emergence/Chains/Polygon.Polygon")));
}

UEmergenceChain* UEmergenceChain::CreateEmergenceChain(FText _Name, FString _NodeURL, FString _Symbol, int _ChainID)
{
	UEmergenceChain* EmergenceChain = NewObject<UEmergenceChain>(UEmergenceChain::StaticClass());
	EmergenceChain->Name = _Name;
	EmergenceChain->NodeURL = _NodeURL;
	EmergenceChain->Symbol = _Symbol;
	EmergenceChain->ChainID = _ChainID;
	return EmergenceChain;
}
