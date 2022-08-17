// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#include "EmergenceToolbar.h"
#include "EmergenceToolbarStyle.h"
#include "EmergenceToolbarCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "LevelEditor.h"
#include "LocalEmergenceServer.h"

static const FName EmergenceToolbarTabName("EmergenceToolbar");

#define LOCTEXT_NAMESPACE "FEmergenceToolbarModule"

void FEmergenceToolbarModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FEmergenceToolbarStyle::Initialize();
	FEmergenceToolbarStyle::ReloadTextures();

	FEmergenceToolbarCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FEmergenceToolbarCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FEmergenceToolbarModule::PluginButtonClicked),
		FCanExecuteAction());
	
	/*PluginCommands->MapAction(
		FEmergenceToolbarCommands::Get().CheckServerStatusAction,
		FExecuteAction::CreateRaw(this, &FEmergenceToolbarModule::CheckStatusButtonClicked),
		FCanExecuteAction());*/

	PluginCommands->MapAction(
		FEmergenceToolbarCommands::Get().StartServerAction,
		FExecuteAction::CreateRaw(this, &FEmergenceToolbarModule::StartServerButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FEmergenceToolbarCommands::Get().StopServerAction,
		FExecuteAction::CreateRaw(this, &FEmergenceToolbarModule::StopServerButtonClicked),
		FCanExecuteAction());

	PluginCommands->MapAction(
		FEmergenceToolbarCommands::Get().RestartServerAction,
		FExecuteAction::CreateRaw(this, &FEmergenceToolbarModule::RestartServerButtonClicked),
		FCanExecuteAction());


	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("LevelEditor", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FEmergenceToolbarModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::Before, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FEmergenceToolbarModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEmergenceToolbarModule::RegisterMenus));
}

void FEmergenceToolbarModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FEmergenceToolbarStyle::Shutdown();

	FEmergenceToolbarCommands::Unregister();
}

void FEmergenceToolbarModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	FUIAction TempCompileOptionsCommand;

	Builder.AddComboButton(
		TempCompileOptionsCommand,
		FOnGetContent::CreateRaw(this, &FEmergenceToolbarModule::FillComboButton, PluginCommands),
		FText::FromString("Emergence"),
		FText::FromString("Emergence Menu"),
		FSlateIcon(FEmergenceToolbarStyle::GetStyleSetName(), "EmergenceToolbar.PluginAction"),
		false
	);

}

void FEmergenceToolbarModule::FillSubmenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().StartServerAction);
	MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().StopServerAction);
	MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().RestartServerAction);
	//MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().CheckServerStatusAction);

}

TSharedRef<SWidget> FEmergenceToolbarModule::FillComboButton(TSharedPtr<class FUICommandList> Commands)
{
	FMenuBuilder MenuBuilder(true, Commands);

	MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().StartServerAction);
	MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().StopServerAction);
	MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().RestartServerAction);
	//MenuBuilder.AddMenuEntry(FEmergenceToolbarCommands::Get().CheckServerStatusAction);

	return MenuBuilder.MakeWidget();
}

void FEmergenceToolbarModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.BeginSection("EmergenceMenu");

	Builder.AddSubMenu(FText::FromString("Emergence"),
		FText::FromString("Interact with integration server"),
		FNewMenuDelegate::CreateRaw(this, &FEmergenceToolbarModule::FillSubmenu),
		false,
		FSlateIcon(FEmergenceToolbarStyle::GetStyleSetName(), "EmergenceToolbarStyle"));

	Builder.EndSection();
}

void FEmergenceToolbarModule::PluginButtonClicked()
{
	// Put your "OnButtonClicked" stuff here
	FText DialogText = FText::Format(
							LOCTEXT("PluginButtonDialogText", "Add code to {0} in {1} to override this button's actions"),
							FText::FromString(TEXT("FEmergenceToolbarModule::PluginButtonClicked()")),
							FText::FromString(TEXT("EmergenceToolbar.cpp"))
					   );
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
}

/*void FEmergenceToolbarModule::CheckStatusButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("CheckStatusButtonClicked"));
}*/

void FEmergenceToolbarModule::StartServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("StartServerButtonClicked"));
	ULocalEmergenceServer::LaunchLocalServerProcess();
}

void FEmergenceToolbarModule::StopServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("StopServerButtonClicked"));
	
	ULocalEmergenceServer::KillLocalServerProcess();
}

void FEmergenceToolbarModule::RestartServerButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("RestartServerButtonClicked"));
	ULocalEmergenceServer::KillLocalServerProcess();
	ULocalEmergenceServer::LaunchLocalServerProcess();
}

void FEmergenceToolbarModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FEmergenceToolbarCommands::Get().PluginAction, PluginCommands);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceToolbarModule, EmergenceToolbar)