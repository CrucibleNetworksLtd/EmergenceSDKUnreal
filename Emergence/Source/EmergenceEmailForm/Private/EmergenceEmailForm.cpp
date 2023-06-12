// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceEmailForm.h"
#include "EmergenceEmailFormStyle.h"
#include "EmergenceEmailFormCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "UObject/ConstructorHelpers.h"
#include "Interfaces/IPluginManager.h"
#include "Templates/SharedPointer.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Input/SRichTextHyperlink.h"
#include "Widgets/Input/SHyperlink.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "SHyperlinkLaunchURL.h"
#include "Misc/CoreDelegates.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IPluginManager.h"
#include "PlatformHttp.h"
#include "Templates/SharedPointer.h"
#include "Dom/JsonObject.h"
#include "HttpService/HttpHelperLibrary.h"
#include "Serialization/JsonWriter.h"
#include "Interfaces/IHttpResponse.h"

static const FName EmergenceEmailFormTabName("Emergence Email Form");
static const TCHAR* ShowAgainConfigName = TEXT("ShowEmailBox");

#define LOCTEXT_NAMESPACE "FEmergenceEmailFormModule"

void FEmergenceEmailFormModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FEmergenceEmailFormStyle::Initialize();
	FEmergenceEmailFormStyle::ReloadTextures();

	FEmergenceEmailFormCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FEmergenceEmailFormCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FEmergenceEmailFormModule::OpenFormFromButtonPress),
		FCanExecuteAction());
	
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FEmergenceEmailFormModule::RegisterMenus));
}

void FEmergenceEmailFormModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FEmergenceEmailFormStyle::Shutdown();

	FEmergenceEmailFormCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EmergenceEmailFormTabName);


}

TSharedRef<SWindow> FEmergenceEmailFormModule::OnSpawnPluginTab()
{
	GConfig->SetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), ShowAgainConfigName, false, GEditorIni);

	auto SlateStyle = FSlateStyleRegistry::FindSlateStyle("EmergenceEmailFormStyle");
	FSlateFontInfo WindowFont = FSlateFontInfo(FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Regular.ttf"), 12);
	const FTextBlockStyle LinkStyle = FTextBlockStyle()
		.SetFont(WindowFont)
		.SetColorAndOpacity(FSlateColor(FLinearColor::Blue))
		.SetUnderlineBrush(FSlateColorBrush(FLinearColor::Blue));

	return SNew(SWindow)
		.Title(FText::FromString(TEXT("Welcome to Emergence!")))
		.ClientSize(FVector2D(600, 400))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.SizingRule(ESizingRule::FixedSize)
		[
			SNew(SBorder)
			.BorderImage(new FSlateColorBrush(FLinearColor::FromSRGBColor(FColor(62, 62, 62, 255))))
			[
			// Put your tab content here!
			SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage).Image(SlateStyle->GetBrush("EmergenceEmailForm.Logo"))
			]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SSpacer).Size(FVector2D(1, 10))
			]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Optionally, if you would like to keep up date with all the latest developments on Emergence and also the Open Meta DAO, please enter your email here:"))
				.AutoWrapText(true)
				.Font(WindowFont)
			].Padding(8.0F, 0.0F)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SSpacer).Size(FVector2D(1, 10))
			]
			+ SVerticalBox::Slot().AutoHeight()
				[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().Padding(0, 2.0f, 0, 0)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Email:"))
					.Font(WindowFont)
				]
				+ SHorizontalBox::Slot().Padding(10.0F, 0.0F)
				[
					SNew(SEditableTextBox).OnTextChanged_Raw(this, &FEmergenceEmailFormModule::OnEmailBoxTextChanged)
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SButton).Content()[
						SNew(STextBlock).Text(FText::FromString("Send"))
						.Font(WindowFont)
						
					].OnClicked_Raw(this, &FEmergenceEmailFormModule::OnSendButtonClicked)
				].Padding(8.0F,0.0F)
			]
			+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(SSpacer).Size(FVector2D(1, 10))
				]
			+ SVerticalBox::Slot().AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(STextBlock).Text(FText::FromString("Don't show this again:")).Font(WindowFont)
				]
				+ SHorizontalBox::Slot().AutoWidth()
				[
					SNew(SCheckBox)
					.IsChecked(true)
					.OnCheckStateChanged_Raw(this, &FEmergenceEmailFormModule::OnCheckboxChanged)
				].Padding(8.0F, 0.0F)
			]
			+ SVerticalBox::Slot().FillHeight(1.0f)
				[
					SNew(SSpacer).Size(FVector2D(1, 10))
				]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(EHorizontalAlignment::HAlign_Center)
			.Padding(0.0f, 8.0f, 0.0f, 0.0f)
			[
			SNew(SHyperlinkLaunchURL, TEXT("https://www.emergence.site/assets/documents/Emergence_Privacy%20Policy_FINAL%20(25%20Nov%2021).pdf"))
			.Text((FText::FromString("Privacy & Terms")))
			]
		]
		];
}

void FEmergenceEmailFormModule::OpenFormFromButtonPress()
{
	ActivateFormChecked(true);
}

void FEmergenceEmailFormModule::ActivateFormChecked(bool Force)
{
	if (CurrentWindow && CurrentWindow->IsVisible()) {
		return;
	}

	if (Force) {
		CurrentWindow = FSlateApplication::Get().AddWindowAsNativeChild(FEmergenceEmailFormModule::OnSpawnPluginTab(), FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
		CurrentWindow->GetOnWindowClosedEvent().AddLambda([&](const TSharedRef<SWindow>& Window) {
			CurrentWindow = nullptr;
		});
		return;
	}

	bool ShowEmailBox = true;
	if (!GConfig->GetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), ShowAgainConfigName, ShowEmailBox, GEditorIni)) {
		ShowEmailBox = true;
	}

	if (!DefaultActivationOccuredThisSession && ShowEmailBox) {
		CurrentWindow = FSlateApplication::Get().AddWindowAsNativeChild(FEmergenceEmailFormModule::OnSpawnPluginTab(), FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
		CurrentWindow->GetOnWindowClosedEvent().AddLambda([&](const TSharedRef<SWindow>& Window) {
			CurrentWindow = nullptr;
		});
		DefaultActivationOccuredThisSession = true;
	}
}

FReply FEmergenceEmailFormModule::OnSendButtonClicked()
{
	TArray<TPair<FString, FString>> Headers;

	TSharedPtr<FJsonObject> TemplateParams = MakeShareable(new FJsonObject);
	TemplateParams->SetStringField("from_email", *this->Email);
	TemplateParams->SetStringField("from_engine", FGenericPlatformHttp::EscapeUserAgentString(FApp::GetBuildVersion()));
	TemplateParams->SetStringField("from_emergenceversion", FGenericPlatformHttp::EscapeUserAgentString(UHttpHelperLibrary::GetEmergenceVersionNumber()));
#if UNREAL_MARKETPLACE_BUILD
	TemplateParams->SetStringField("from_emergenceevmtype", "EVMOnline");
#else
	TemplateParams->SetStringField("from_emergenceevmtype", "LocalEVM");
#endif
	TemplateParams->SetStringField("from_os", FGenericPlatformHttp::EscapeUserAgentString(FString(FPlatformProperties::IniPlatformName()) + " " + FPlatformMisc::GetOSVersion()));

	TSharedPtr<FJsonObject> SendEmailContent = MakeShareable(new FJsonObject);
	SendEmailContent->SetStringField("service_id", "service_txbxvyw");
	SendEmailContent->SetStringField("template_id", "template_775t29f");
	SendEmailContent->SetStringField("user_id", "XZJBhUf8kkPvSWNuG");
	SendEmailContent->SetStringField("accessToken", "6b8PQs8_Ior6LQsbMUbWD");
	SendEmailContent->SetObjectField("template_params", TemplateParams);
	FString SendEmailContentString;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&SendEmailContentString);
	FJsonSerializer::Serialize(SendEmailContent.ToSharedRef(), Writer);

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL("https://api.emailjs.com/api/v1.0/email/send");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetTimeout(20.0F);
	HttpRequest->SetHeader("origin", "http://localhost");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetContentAsString(SendEmailContentString);
	HttpRequest->OnProcessRequestComplete().BindLambda([&](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded) {
		if (bSucceeded && HttpResponse->GetContentAsString() == "OK") {
			UE_LOG(LogTemp, Display, TEXT("Request complete, returned \"%s\""), *HttpResponse->GetContentAsString());
			if (CurrentWindow && CurrentWindow->IsVisible()) {
				CurrentWindow->RequestDestroyWindow();
			}
		}
	});
	HttpRequest->ProcessRequest();
	UE_LOG(LogTemp, Display, TEXT("SendEmail request started with email %s, calling SendEmail_HttpRequestComplete on request completed"), *this->Email);
	
	return FReply::Handled();
}

void FEmergenceEmailFormModule::OnEmailBoxTextChanged(const FText& Text)
{
	this->Email = Text.ToString();
}

void FEmergenceEmailFormModule::OnCheckboxChanged(ECheckBoxState NewState)
{
	if (NewState == ECheckBoxState::Checked) {
		GConfig->SetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), ShowAgainConfigName, false, GEditorIni);
	}
	else {
		GConfig->SetBool(TEXT("/Script/EmergenceEditor.EmergencePluginSettings"), ShowAgainConfigName, true, GEditorIni);
	}
}

void FEmergenceEmailFormModule::OnMapChanged(const FString& MapName, bool MapChangeFlags)
{
	ActivateFormChecked(false);
	UE_LOG(LogTemp, Warning, TEXT("---------------OnMapChanged: %d"), MapChangeFlags);
}


void FEmergenceEmailFormModule::SendEmail_HttpRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{

}

void FEmergenceEmailFormModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Help");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("Emergence");
			Section.AddMenuEntryWithCommandList(FEmergenceEmailFormCommands::Get().OpenPluginWindow, PluginCommands, FText::FromString("Emergence Email Form"));
		}
	}

	{
		/*UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FEmergenceEmailFormCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}*/
	}
	FModuleManager::GetModuleChecked<FLevelEditorModule>(FName("LevelEditor")).OnRegisterTabs().AddLambda([&](TSharedPtr<FTabManager> TabManager)
		{
			ActivateFormChecked(false);
		});
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEmergenceEmailFormModule, EmergenceEmailForm)