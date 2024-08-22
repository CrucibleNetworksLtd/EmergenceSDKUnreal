// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "UI/EmergenceScreen.h"
#include "Misc/EngineVersionComparison.h"
#if UE_VERSION_NEWER_THAN(5, 2, 0)
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
#if UE_VERSION_NEWER_THAN(5, 2, 0)
	FPlatformApplicationMisc::ClipboardCopy(*StringToCopy);
#else
	FPlatformMisc::ClipboardCopy(*StringToCopy);
#endif
}
