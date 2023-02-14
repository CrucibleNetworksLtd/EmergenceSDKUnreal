// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "EdGraphUtilities.h"
#include "EmergenceContract.h"
#include "EmergenceContractMethodGraphPin.h"

class FEmergenceContractMethodGraphPanelPinFactory: public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		if (InPin->PinType.PinCategory == K2Schema->PC_Struct && InPin->PinType.PinSubCategoryObject == FEmergenceContractMethod::StaticStruct())
		{
			return SNew(SEmergenceContractMethodGraphPin, InPin);
			//return nullptr;
		}
		/*if (InPin->PinType.PinCategory == K2Schema->PC_Struct && InPin->PinType.PinSubCategoryObject == FGameplayTagContainer::StaticStruct())
		{
			return SNew(SGameplayTagContainerGraphPin, InPin);
		}
		if (InPin->PinType.PinCategory == K2Schema->PC_String && InPin->PinType.PinSubCategory == TEXT("LiteralGameplayTagContainer"))
		{
			return SNew(SGameplayTagContainerGraphPin, InPin);
		}
		if (InPin->PinType.PinCategory == K2Schema->PC_Struct && InPin->PinType.PinSubCategoryObject == FGameplayTagQuery::StaticStruct())
		{
			return SNew(SGameplayTagQueryGraphPin, InPin);
		}*/

		return nullptr;
	}
};