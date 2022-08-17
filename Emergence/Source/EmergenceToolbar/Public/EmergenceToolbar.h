// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;

class FEmergenceToolbarModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command. */
	void PluginButtonClicked();

	// Toolbar buttons OnClick events

	void StartServerButtonClicked();
	void StopServerButtonClicked();
	void RestartServerButtonClicked();
	void CheckStatusButtonClicked();
	
private:

	// Add toolbar and menu buttons to the editor
	void RegisterMenus();
	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);
	void FillSubmenu(FMenuBuilder& MenuBuilder);
	TSharedRef<SWidget> FillComboButton(TSharedPtr<class FUICommandList> Commands);


private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
