// Copyright Crucible Networks Ltd 2022. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceEditor : ModuleRules
{
	public EmergenceEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
			new string[] {
                "Core",
				"CoreUObject",
				"Engine", 
				"RHI",
				"Slate",
				"SlateCore",
				"EditorStyle"
            }
		);

		PublicDependencyModuleNames.AddRange(
		new string[] { 
			"Emergence" 
			}
		);
	}
	
}
