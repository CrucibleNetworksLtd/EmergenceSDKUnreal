// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "SlateBasics.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "EmergenceContract.h"
#include "EmergenceDeployment.h"

class SNameComboBox;

// You can also check files in the engine to see a lot of examples here:
// [UE4 directory]\Engine\Source\Editor\GraphEditor\Private\KismetPins\SGraphPinCollisionProfile.h

class SEmergenceContractMethodGraphPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SEmergenceContractMethodGraphPin) {}
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:
	// this override is used to display slate widget used for customization.
	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	void OnNameSelected(TSharedPtr<FName> ItemSelected, ESelectInfo::Type SelectInfo);
	void OnNameComboBoxOpening();
	void UpdateOptions();

	TSharedPtr<FName> GetSelectedName() const;

	void SetPropertyWithName(const FName& Name);
	void GetPropertyAsName(FName& OutName) const;
private:
	TArray<TSharedPtr<FName>> Options = {
		MakeShared<FName>(FName("Select..."))
	};
	TArray<FEmergenceContractMethod> OptionStructs = {
		FEmergenceContractMethod("")
	};
	TSharedPtr<SNameComboBox> NameComboBox;
	UEmergenceDeployment* PreviouslyAssociatedDeployment;
};