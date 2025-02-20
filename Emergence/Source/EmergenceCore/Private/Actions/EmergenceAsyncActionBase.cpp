// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "Actions/EmergenceAsyncActionBase.h"

UWorld* UEmergenceAsyncActionBase::GetWorld() const
{
	if (WorldContextObject) {
		return WorldContextObject->GetWorld();
	}

	return nullptr;
}