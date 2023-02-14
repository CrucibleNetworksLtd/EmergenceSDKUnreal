// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "UI/EmergenceCarouselItem.h"

void UEmergenceCarouselItem::SetIndex(int indexParam, float targetXParam, bool immediate)
{
	auto left = index > indexParam;
	index = indexParam;
	targetX = targetXParam;
	OnMove(index, left, immediate);
}

int UEmergenceCarouselItem::GetIndex()
{
	return index;
}

float UEmergenceCarouselItem::GetTargetX()
{
	return targetX;
}

void UEmergenceCarouselItem::Move(int indexParam, float targetXParam)
{
	SetIndex(indexParam, targetXParam, false);
}
