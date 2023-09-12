// Copyright Crucible Networks Ltd 2023. All Rights Reserved.


#include "InventoryService/InventoryItemWidget.h"

void UInventoryItemWidget::GetTextureFromURL(FString URL) {
	GetTextureFromUrlRequest = UGetTextureFromUrl::TextureFromUrl(URL, this, true);
	GetTextureFromUrlRequest->OnGetTextureFromUrlCompleted.AddDynamic(this, &UInventoryItemWidget::OnGetTextureFromURLCompleted);
	GetTextureFromUrlRequest->Activate();
}

void UInventoryItemWidget::StopConnections()
{
	if (GetTextureFromUrlRequest) {
		GetTextureFromUrlRequest->Cancel();
	}
}
