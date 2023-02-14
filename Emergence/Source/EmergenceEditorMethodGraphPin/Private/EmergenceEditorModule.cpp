// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "Interfaces/ITargetPlatformModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/Package.h"
#include "UObject/WeakObjectPtr.h"
#include "EmergenceContractMethodGraphPanelPinFactory.h"
#include "EmergenceWriteMethodGraphPanelNodeFactory.h"
#define LOCTEXT_NAMESPACE "EmergenceEditorMethodGraphPinModule"

class FEmergenceEditorMethodGraphPinModule : public IModuleInterface
{
	TSharedPtr<FEmergenceContractMethodGraphPanelPinFactory> EmergenceContractMethodGraphPanelPinFactory;
	TSharedPtr<FEmergenceWriteMethodGraphPanelNodeFactory> EmergenceWriteMethodGraphPanelNodeFactory;

	virtual void StartupModule() override
	{
		EmergenceContractMethodGraphPanelPinFactory = MakeShareable(new FEmergenceContractMethodGraphPanelPinFactory());
		FEdGraphUtilities::RegisterVisualPinFactory(EmergenceContractMethodGraphPanelPinFactory);
		EmergenceWriteMethodGraphPanelNodeFactory = MakeShareable(new FEmergenceWriteMethodGraphPanelNodeFactory());
		FEdGraphUtilities::RegisterVisualNodeFactory(EmergenceWriteMethodGraphPanelNodeFactory);
	}

	virtual void ShutdownModule() override
	{
		FEdGraphUtilities::UnregisterVisualPinFactory(EmergenceContractMethodGraphPanelPinFactory);
		FEdGraphUtilities::UnregisterVisualNodeFactory(EmergenceWriteMethodGraphPanelNodeFactory);
	}

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEmergenceEditorMethodGraphPinModule, EmergenceEditorMethodGraphPin );