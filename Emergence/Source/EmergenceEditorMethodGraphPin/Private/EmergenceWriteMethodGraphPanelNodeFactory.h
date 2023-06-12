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
		FString NodeNotMarketplaceMessage = "This function is not available in the marketplace build. Please download the build from our github!";
		//if we're in a marketplace build, these functions are non-fuctional
#if UNREAL_MARKETPLACE_BUILD
		if ("Create Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString() ||
		"Create Wallet and Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString() ||
		"Load Account from Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()
		) {
			Node->SetEnabledState(ENodeEnabledState::Disabled, false);
			Node->SetForceDisplayAsDisabled(true);
			Node->NodeComment = NodeNotMarketplaceMessage;
			Node->bCommentBubbleVisible = true;
			return nullptr;
		}
#else
		//make sure to set them to look functional if we change build
		if ("Create Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString() ||
			"Create Wallet and Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString() ||
			"Load Account from Key Store File" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()
			) {
			Node->SetEnabledState(ENodeEnabledState::Enabled, false);
			Node->SetForceDisplayAsDisabled(false);
			if (Node->NodeComment == NodeNotMarketplaceMessage) {
				Node->NodeComment = "";
				Node->bCommentBubbleVisible = false;
			}
			return nullptr;
		}
#endif

		//This version of load contract is deprecated
		if ("Load Contract" == Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()) {
			Node->SetEnabledState(ENodeEnabledState::Disabled, false);
			Node->SetForceDisplayAsDisabled(true);
			return nullptr;
		}
		
		return nullptr;
	};
};