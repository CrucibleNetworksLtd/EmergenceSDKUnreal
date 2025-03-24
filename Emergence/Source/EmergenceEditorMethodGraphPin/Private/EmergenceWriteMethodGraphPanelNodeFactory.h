// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphNode.h"

class FEmergenceWriteMethodGraphPanelNodeFactory: public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override {

		//This code is to make sure the OnTransactionSent pin comes after the data pins that relate to OnTransactionConfirmed
		//This is to better imply to the user that these data pins only relate to OnTransactionConfirmed, 
		//and should only be read when OnTransactionConfirmed has been called
		if ("Write Method" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()) {
			UEdGraphPin* OnTransactionSentPin = *Node->Pins.FindByPredicate([](UEdGraphPin* Pin) {
				return "OnTransactionSent" == Pin->GetName();
			});
			Node->Pins.Remove(OnTransactionSentPin);
			Node->Pins.Add(OnTransactionSentPin);

			return nullptr;
		}
		
		return nullptr;
	};
};