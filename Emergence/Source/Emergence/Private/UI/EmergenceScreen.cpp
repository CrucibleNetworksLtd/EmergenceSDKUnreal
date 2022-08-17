// Copyright Crucible Networks Ltd 2022. All Rights Reserved.


#include "UI/EmergenceScreen.h"

UEmergenceUI* UEmergenceScreen::GetEmergenceUI() {
	if (this != nullptr 
		&& GetParent() 
		&& GetParent()->GetOuter() 
		&& GetParent()->GetOuter()->GetOuter()) {
		return Cast<UEmergenceUI>(GetParent()->GetOuter()->GetOuter());
	}
	else {
		return nullptr;
	}
}