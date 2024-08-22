// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/InventoryItemWidget.h"

void UInventoryItemWidget::GetTextureFromURL(FString URL) {
	if (!URL.IsEmpty()) {
		GetTextureFromUrlRequest = UGetTextureFromUrl::TextureFromUrl(URL, this, true);
		GetTextureFromUrlRequest->OnGetTextureFromUrlCompleted.AddDynamic(this, &UInventoryItemWidget::OnGetTextureFromURLCompleted);
		GetTextureFromUrlRequest->Activate();
		return;
	}
	this->OnGetTextureFromURLCompleted(nullptr, EErrorCode::EmergenceClientInvalidResponse); //this has been added to prevent the warnings in the log flooding in when you have a lot of empty NFT images
}

void UInventoryItemWidget::StopConnections()
{
	if (GetTextureFromUrlRequest) {
		GetTextureFromUrlRequest->Cancel();
	}
}
