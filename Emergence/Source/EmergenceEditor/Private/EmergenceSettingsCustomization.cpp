// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceSettingsCustomization.h"
#include "EmergencePluginSettings.h"
#include "PropertyEditing.h"
#include "Widgets/Text/SRichTextBlock.h"
#include "Styling/SlateColor.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "MyEditorModule"

TSharedRef< IDetailCustomization > FEmergenceSettingsCustomization::MakeInstance()
{
    return MakeShareable(new FEmergenceSettingsCustomization);
}

void FEmergenceSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& OverlayCategory = DetailBuilder.EditCategory(TEXT("UI Overlay"));
	OverlayCategory.AddCustomRow(LOCTEXT("UpgradeInfo", "Upgrade Info"), false)
		.WholeRowWidget
		[
			SNew(SBorder)
			.Padding(1)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.Padding(FMargin(10, 10, 10, 10))
				.FillWidth(1.0f)
				[
					SNew(SRichTextBlock)
					.Text(LOCTEXT("ChainText", "<RichTextBlock.TextHighlight>Note on chains selected here</>: This selection only set the chain used by the UI overlay. It doesn't set the chains used by anything else.\n\n<RichTextBlock.TextHighlight>Also note</>: The preset blockchains supplied with Emergence have Node URLs provided by Crucible. These are pretty much just for testing as they only have a limited amount of requests per day, and these are shared by everyone using Emergence. You probably shouldn't be using it in shipping."))
					.TextStyle(FEditorStyle::Get(), "MessageLog")
					.DecoratorStyleSet(&FEditorStyle::Get())
					.AutoWrapText(true)
				]
			]
		];
}

#undef LOCTEXT_NAMESPACE