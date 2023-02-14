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
#include "EmergencePluginSettings.h"
#include "EmergenceSettingsCustomization.h"
#include "EmergenceChainAssetTypeActions.h"
#include "Styling/SlateStyle.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#include "EmergenceContractAssetTypeActions.h"
#include "EmergenceDeploymentAssetTypeActions.h"
#define LOCTEXT_NAMESPACE "EmergenceEditorModule"

class FEmergenceEditorModule : public IModuleInterface
{
	
	virtual void StartupModule() override
	{

		//Create class icons
		StyleSet = MakeShareable(new FSlateStyleSet("EmergenceAssetStyle"));
		FString ContentDir = IPluginManager::Get().FindPlugin("Emergence")->GetBaseDir();
		StyleSet->SetContentRoot(ContentDir);
		FSlateImageBrush* BlockchainThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Resources/Blockchain128"), TEXT(".png")), FVector2D(128.0f, 128.0f));
		FSlateImageBrush* ContractThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Resources/Contract128"), TEXT(".png")), FVector2D(128.0f, 128.0f));
		FSlateImageBrush* DeploymentThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("Resources/Deployment128"), TEXT(".png")), FVector2D(128.0f, 128.0f));
		if (BlockchainThumbnailBrush && ContractThumbnailBrush && DeploymentThumbnailBrush) {
			StyleSet->Set("ClassThumbnail.EmergenceChain", BlockchainThumbnailBrush);
			StyleSet->Set("ClassThumbnail.EmergenceContract", ContractThumbnailBrush);
			StyleSet->Set("ClassThumbnail.EmergenceDeployment", DeploymentThumbnailBrush);
			FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
		}

		//Create web3 category
		EAssetTypeCategories::Type EmergenceCategory = FAssetToolsModule::GetModule().Get().RegisterAdvancedAssetCategory(FName(TEXT("Web3")), FText::FromString("Web3"));
		
		//Register asset types
		EmergenceChainAssetTypeActions = MakeShareable(new FEmergenceChainAssetTypeActions(EmergenceCategory));
		FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(EmergenceChainAssetTypeActions.ToSharedRef());

		EmergenceContractAssetTypeActions = MakeShareable(new FEmergenceContractAssetTypeActions(EmergenceCategory));
		FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(EmergenceContractAssetTypeActions.ToSharedRef());
		
		EmergenceDeploymentAssetTypeActions = MakeShareable(new FEmergenceDeploymentAssetTypeActions(EmergenceCategory));
		FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(EmergenceDeploymentAssetTypeActions.ToSharedRef());

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
		FSlateStyleRegistry::UnRegisterSlateStyle(StyleSet->GetStyleSetName());

		if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(EmergenceChainAssetTypeActions.ToSharedRef());
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(EmergenceContractAssetTypeActions.ToSharedRef());
		FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(EmergenceDeploymentAssetTypeActions.ToSharedRef());

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
	TSharedPtr<FSlateStyleSet> StyleSet;
	TSharedPtr<FEmergenceChainAssetTypeActions> EmergenceChainAssetTypeActions;
	TSharedPtr<FEmergenceContractAssetTypeActions> EmergenceContractAssetTypeActions;
	TSharedPtr<FEmergenceDeploymentAssetTypeActions> EmergenceDeploymentAssetTypeActions;
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