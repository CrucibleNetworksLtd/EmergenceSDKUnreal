// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

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
	UFUNCTION(BlueprintCallable)
	void SetIndex(int indexParam, float targetXParam, bool immediate);

	UFUNCTION(BlueprintPure)
	int GetIndex();

	UFUNCTION(BlueprintPure)
	float GetTargetX();

	UFUNCTION(BlueprintCallable)
	void Move(int indexParam, float targetXParam);

	UFUNCTION(BlueprintImplementableEvent)
	void OnMove(int indexParam, bool left, bool immediate);

private:
	int index;
	float targetX;
};
