// Copyright Crucible Networks Ltd 2025. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class EmergenceHelperFunctions : ModuleRules
{
	public EmergenceHelperFunctions(ReadOnlyTargetRules Target) : base(Target)
	{
		
		bEnforceIWYU = true;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"JsonUtilities",
				"EmergenceCore",
				"HTTP",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Json"	
			}
		);

	}
}
