// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/EmergenceUI.h"
#include "EmergenceScreen.generated.h"

/**
 * Should be the parent of any widget that needs to be displayed as a main "screen" shown inside the Emergence UI.
 */
UCLASS()
class UEmergenceScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	//Get the main Emergence UI instance that is holding this screen.
	UFUNCTION(BlueprintPure, Category = "Emergence Internal|UI")
	UEmergenceUI* GetEmergenceUI();
};
