// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmergenceCarouselItem.generated.h"

/**
 * 
 */
UCLASS()
class EMERGENCE_API UEmergenceCarouselItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|UI|CarouselItem")
	void SetIndex(int indexParam, float targetXParam, bool immediate);

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|UI|CarouselItem")
	int GetIndex();

	UFUNCTION(BlueprintPure, Category = "Emergence Internal|UI|CarouselItem")
	float GetTargetX();

	UFUNCTION(BlueprintCallable, Category = "Emergence Internal|UI|CarouselItem")
	void Move(int indexParam, float targetXParam);

	UFUNCTION(BlueprintImplementableEvent, Category = "Emergence Internal|UI|CarouselItem")
	void OnMove(int indexParam, bool left, bool immediate);

private:
	int index;
	float targetX;
};
