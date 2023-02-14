// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

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
#if UNREAL_MARKETPLACE_BUILD
		if ("Load Contract" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString() ||
		"Create Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString() ||
		"Create Wallet and Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString() ||
		"Load Account from Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()
		) {
#else
		if ("Load Contract" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()) {
#endif
			Node->SetEnabledState(ENodeEnabledState::Disabled, false);
			Node->SetForceDisplayAsDisabled(true);
			return nullptr;
		}
		
		return nullptr;
	};
};