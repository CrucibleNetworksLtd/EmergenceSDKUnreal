// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceVRMSupport : ModuleRules
{
	public EmergenceVRMSupport(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;	
		bEnforceIWYU = true;
		
		PublicDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"VRM4U",
				"VRM4ULoader"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"CoreUObject",
				"Engine"
			}
		);
	}
}
