// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "UI/EmergenceScreen.h"
#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5)
#include "HAL/PlatformApplicationMisc.h"
#else
#include "GenericPlatform/GenericPlatformMisc.h"
#endif

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

void UEmergenceScreen::CopyStringToClipboard(FString StringToCopy)
{
#if(ENGINE_MINOR_VERSION >= 4) && (ENGINE_MAJOR_VERSION >= 5)
	FPlatformApplicationMisc::ClipboardCopy(*StringToCopy);
#else
	FPlatformMisc::ClipboardCopy(*StringToCopy);
#endif
}
