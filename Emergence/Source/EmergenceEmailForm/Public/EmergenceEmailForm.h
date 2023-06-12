// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Brushes/SlateImageBrush.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"

class FToolBarBuilder;
class FMenuBuilder;

class FEmergenceEmailFormModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void OpenFormFromButtonPress();

	void ActivateFormChecked(bool Force = false);

	UFUNCTION()
	FReply OnSendButtonClicked();

	UFUNCTION()
	void OnEmailBoxTextChanged(const FText &Text);

	UFUNCTION()
	void OnCheckboxChanged(ECheckBoxState NewState);

	UFUNCTION()
	void OnMapChanged(const FString& MapName, bool MapChangeFlags);

	UPROPERTY()
	bool DefaultActivationOccuredThisSession = false;

	UFUNCTION()
	void SendEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
private:
	
	
	void RegisterMenus();

	UPROPERTY()
	TSharedPtr<FSlateImageBrush> Brush;

	TSharedRef<class SWindow> OnSpawnPluginTab();

private:
	TSharedPtr<SWindow> CurrentWindow;
	TSharedPtr<class FUICommandList> PluginCommands;
	static TSharedPtr< class FSlateStyleSet > StyleInstance;
	FString Email;
	TSharedPtr<SEditableTextBox> MyEditableTextBox;

};
