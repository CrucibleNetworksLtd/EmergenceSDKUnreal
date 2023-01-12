
#pragma once

#include "EdGraphUtilities.h"
#include "EdGraph/EdGraphNode.h"

class FEmergenceWriteMethodGraphPanelNodeFactory: public FGraphPanelNodeFactory
{
	virtual TSharedPtr<class SGraphNode> CreateNode(UEdGraphNode* Node) const override {
		if ("Write Method" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()) {
			UEdGraphPin* OnTransactionSentPin = *Node->Pins.FindByPredicate([](UEdGraphPin* Pin) {
				return "OnTransactionSent" == Pin->GetName();
			});
			Node->Pins.Remove(OnTransactionSentPin);
			Node->Pins.Add(OnTransactionSentPin);

			return nullptr;
		}

		if ("Load Contract" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()) {
			Node->SetEnabledState(ENodeEnabledState::Disabled, false);
			Node->SetForceDisplayAsDisabled(true);
			return nullptr;
		}
		
		return nullptr;
	};
};