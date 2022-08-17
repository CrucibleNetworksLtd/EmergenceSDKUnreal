// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ISettingsContainer.h"
#include "CoreMinimal.h"
#include "ISettingsModule.h"
#include "Interfaces/ITargetPlatformModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/Package.h"
#include "UObject/WeakObjectPtr.h"
#include "EmergencePluginSettings.h"
#include "EmergenceSettingsCustomization.h"
#define LOCTEXT_NAMESPACE "EmergenceEditorModule"

class FEmergenceEditorModule : public IModuleInterface
{
	
	virtual void StartupModule() override
	{
		auto& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");

		// Register our customization to be used by a class 'UMyClass' or 'AMyClass'. Note the prefix must be dropped.
		PropertyModule.RegisterCustomClassLayout(
        UEmergencePluginSettings::StaticClass()->GetFName(),
        FOnGetDetailCustomizationInstance::CreateStatic(&FEmergenceSettingsCustomization::MakeInstance)
        );

		PropertyModule.NotifyCustomizationModuleChanged();
		
		RegisterSettings();
	}

	virtual void ShutdownModule() override
	{
		if (UObjectInitialized())
		{
			UnregisterSettings();
		}
	}

	virtual bool SupportsDynamicReloading() override
	{
		return true;
	}

private:

	bool HandleSettingsSaved()
	{
		UEmergencePluginSettings* Settings = GetMutableDefault<UEmergencePluginSettings>();
		bool ResaveSettings = false;

		if (ResaveSettings)
		{
			Settings->SaveConfig();
		}

		return true;
	}

	void RegisterSettings()
	{

		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer("Project");

			/*SettingsContainer->DescribeCategory("Emergence",
				LOCTEXT("RuntimeWDCategoryName", "Emergence"),
				LOCTEXT("RuntimeWDCategoryDescription", "Configuration for the Emergence plugin."));*/

			// Register the settings
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "Emergence",
				LOCTEXT("RuntimeEditorSettingsName", "Emergence"),
				LOCTEXT("RuntimeEditorSettingsDescription", "Settings for the Emergence plugin."),
				GetMutableDefault<UEmergencePluginSettings>()
				);


			if (SettingsSection.IsValid())
			{
				SettingsSection->OnModified().BindRaw(this, &FEmergenceEditorModule::HandleSettingsSaved);
			}
		}
	}

	void UnregisterSettings()
	{

		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->UnregisterSettings("Project", "EmergenceEditor", "Emergence");
		}
	}
};

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEmergenceEditorModule, EmergenceEditor );