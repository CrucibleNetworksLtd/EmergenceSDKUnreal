// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceEmailForm : ModuleRules
{
	public EmergenceEmailForm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;	
		CppStandard = CppStandardVersion.Cpp17;
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
				"UnrealEd",
				"ToolMenus",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"SharedSettingsWidgets",
				"HTTP",
				"Json",
				"JsonUtilities",
				"Emergence"
			}
			);
	}
}
