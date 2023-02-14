// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

#include "EmergenceContractMethodGraphPin.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "SNameComboBox.h"
#include "ScopedTransaction.h"
#include "SlateCore/Public/Widgets/SBoxPanel.h"
#include "SGraphPin.h"
#include "Templates/SharedPointer.h"

#define LOCTEXT_NAMESPACE "NestedNamesFromConfigPin"

void SEmergenceContractMethodGraphPin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
	UpdateOptions();
}

TSharedRef<SWidget> SEmergenceContractMethodGraphPin::GetDefaultValueWidget()
{
	UpdateOptions();
	
	FName InitialSelectedName;
	GetPropertyAsName(InitialSelectedName);
	SetPropertyWithName(InitialSelectedName);
	
	if (!GraphPinObj->HasAnyConnections()) {

		return SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SAssignNew(NameComboBox, SNameComboBox)
				.ContentPadding(FMargin(6.0f, 2.0f))
			.OptionsSource(&Options)
			.InitiallySelectedItem(MakeShared<FName>(InitialSelectedName))
			.OnComboBoxOpening(this, &SEmergenceContractMethodGraphPin::OnNameComboBoxOpening)
			.OnSelectionChanged(this, &SEmergenceContractMethodGraphPin::OnNameSelected)
			];
	}
	else {
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot().AutoHeight()
			[
				SAssignNew(NameComboBox, SNameComboBox)
				.ContentPadding(FMargin(6.0f, 2.0f))
				.IsEnabled(false)
			];
	}
}

void SEmergenceContractMethodGraphPin::OnNameSelected(TSharedPtr<FName> ItemSelected, ESelectInfo::Type SelectInfo)
{
	if (ItemSelected.IsValid())
	{
		//UE_LOG(LogEmergenceHttp, Warning, TEXT("Name selected: %s"), *ItemSelected->ToString());
		SetPropertyWithName(*ItemSelected.Get());
	}
}

void SEmergenceContractMethodGraphPin::OnNameComboBoxOpening()
{
	UpdateOptions();
	TSharedPtr<FName> SelectedName = GetSelectedName();
	if (SelectedName.IsValid())
	{
		check(NameComboBox.IsValid());
		NameComboBox->SetSelectedItem(SelectedName);
	}
}

void SEmergenceContractMethodGraphPin::UpdateOptions()
{
	auto OwnerNode = OwnerNodePtr.Pin().Get();
	//if the node is valid
	if (OwnerNode && OwnerNode->GetNodeObj()) {
		//is this node a ReadMethod node or a WriteMethod node?
		bool ReadMethod = OwnerNode->GetNodeObj()->GetNodeTitle(ENodeTitleType::ListView).ToString().Contains("Read");
		bool WriteMethod = OwnerNode->GetNodeObj()->GetNodeTitle(ENodeTitleType::ListView).ToString().Contains("Write");
		TArray<UEdGraphPin*> Pins = OwnerNode->GetNodeObj()->GetAllPins();
		UEmergenceDeployment* FoundDeployment = nullptr;
		//for each pin
		for (int i = 0; i < Pins.Num(); i++) {
			//if pin is valid, and is of class deplyoment, and if it actually has something set or plugged into it
			if (Pins[i]->PinType.PinSubCategoryObject.IsValid() && Pins[i]->PinType.PinSubCategoryObject.Get() == UEmergenceDeployment::StaticClass() && Pins[i]->DefaultObject) {
				FoundDeployment = Cast<UEmergenceDeployment>(Pins[i]->DefaultObject); //cast to a deployment object
				//UE_LOG(LogEmergenceHttp, Display, TEXT("pin %d (%s): %s (default object)"), i, *Pins[i]->PinType.PinSubCategoryObject.Get()->GetName(), *FoundDeployment->Address);
				if (FoundDeployment == PreviouslyAssociatedDeployment)
				{
					return;
				}
			}
		}

		if(FoundDeployment){
			this->Options.Empty();
			this->Options.Add(MakeShared<FName>(FName("Select...")));
			this->OptionStructs.Empty();
			this->OptionStructs.Add(FEmergenceContractMethod(""));
			PreviouslyAssociatedDeployment = FoundDeployment;
			
			if (FoundDeployment->Contract) { //if the contract is valid
				TArray<FEmergenceContractMethod> MethodArray;
				//if it is a ReadMethod or a WriteMethod
				if (ReadMethod || WriteMethod) {
					//only include the relevent methods
					MethodArray = ReadMethod ? FoundDeployment->Contract->ReadMethods : FoundDeployment->Contract->WriteMethods;
				}
				else {
					//if this is being used outside of a read method or write method
					MethodArray.Append(FoundDeployment->Contract->ReadMethods);
					MethodArray.Append(FoundDeployment->Contract->WriteMethods);
					//let us use either
				}
				for (int j = 0; j < MethodArray.Num(); j++) { //for each method in the contract
					//UE_LOG(LogEmergenceHttp, Display, TEXT("%s"), *MethodArray[j].MethodName);
					this->Options.Add(MakeShared<FName>(FName(MethodArray[j].MethodName)));
					this->OptionStructs.Add(MethodArray[j]);
				}
			}
		}
	}
}

void SEmergenceContractMethodGraphPin::SetPropertyWithName(const FName& Name)
{
	
	//UE_LOG(LogEmergenceHttp, Display, TEXT("SetPropertyWithName: %s"), *Name.ToString());
	check(GraphPinObj);
	int ListIndex = Options.IndexOfByPredicate([Name](TSharedPtr<FName> option){
		return option.Get()->ToString() == Name.ToString();
	});
	//UE_LOG(LogEmergenceHttp, Display, TEXT("ListIndex: %d"), ListIndex);
	if (ListIndex > 0) {
		auto ReleventStruct = OptionStructs[ListIndex];
		UScriptStruct* Struct = ReleventStruct.StaticStruct();
		FString Output = TEXT("");
		Struct->ExportText(Output, &ReleventStruct, nullptr, Struct, (PPF_ExportsNotFullyQualified | PPF_Copy | PPF_Delimited | PPF_IncludeTransient), nullptr);
		//UE_LOG(LogEmergenceHttp, Display, TEXT("OUTPUT TEXT: %s"), *Output);


		FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();

		if (CurrentDefaultValue != Output)
		{
			const FScopedTransaction Transaction(
				NSLOCTEXT("GraphEditor", "ChangeNestedNamesFromConfigPinValue", "Change Nested Names From Config Value"));
			GraphPinObj->Modify();

			//UE_LOG(LogEmergenceHttp, Warning, TEXT("Verify values old: \"%s\" chosen: \"%s\""), *CurrentDefaultValue, *Output);

			if (Output != GraphPinObj->GetDefaultAsString())
			{
				GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, Output);
			}
		}
	}
}

TSharedPtr<FName> SEmergenceContractMethodGraphPin::GetSelectedName() const
{
	if (Options.Num() <= 0)
	{
		return NULL;
	}

	FName Name;
	GetPropertyAsName(Name);
	//UE_LOG(LogEmergenceHttp, Display, TEXT("GetPropertyAsName: %s"), *Name.ToString());

	for (int32 NameIndex = 0; NameIndex < Options.Num(); ++NameIndex)
	{
		if (Name == *Options[NameIndex].Get())
		{
			return Options[NameIndex];
		}
	}
	// no value has been found, return a default value
	return Options[0];
}

void SEmergenceContractMethodGraphPin::GetPropertyAsName(FName& OutName) const
{
	check(GraphPinObj);
	//check(GraphPinObj->PinType.PinSubCategoryObject == FNestedNameAttribute::StaticStruct());

	FString PinString = GraphPinObj->GetDefaultAsString();
	//UE_LOG(LogEmergenceHttp, Display, TEXT("Get default as string: %s"), *GraphPinObj->GetDefaultAsString());
	if (PinString.StartsWith(TEXT("(")) && PinString.EndsWith(TEXT(")")))
	{
		PinString = PinString.LeftChop(1);
		PinString = PinString.RightChop(1);
		PinString.Split(TEXT("="), NULL, &PinString);
		PinString = PinString.TrimQuotes();
	}

	if (!PinString.IsEmpty())
	{
		OutName = *PinString;
	}
	else {
		OutName = FName("Select...");
	}
}
#undef LOCTEXT_NAMESPACE