// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class EmergenceInventory : ModuleRules
{
	public EmergenceInventory(ReadOnlyTargetRules Target) : base(Target)
	{
		
		bEnforceIWYU = true;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EmergenceCore"
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Json",
				"JsonUtilities",
				"Engine",
				"HTTP"
			}
		);

	}
}
