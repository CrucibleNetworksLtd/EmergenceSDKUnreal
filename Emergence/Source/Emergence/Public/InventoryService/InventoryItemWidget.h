// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GetTextureFromURL.h"
#include "InventoryItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category="Emergence Internal|Inventory Item Widget")
	void OnGetTextureFromURLCompleted(UTexture2D* Texture, EErrorCode StatusCode);

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Inventory Item Widget")
	void GetTextureFromURL(FString URL);

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|Inventory Item Widget")
	void StopConnections();

private:
	UGetTextureFromUrl* GetTextureFromUrlRequest;
};
