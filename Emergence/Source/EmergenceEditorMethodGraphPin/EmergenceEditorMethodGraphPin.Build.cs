// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;

public class EmergenceEditorMethodGraphPin : ModuleRules
{
	public EmergenceEditorMethodGraphPin(ReadOnlyTargetRules Target) : base(Target)
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
				"EditorStyle",
				"UnrealEd",
				"Projects",
				"BlueprintGraph",
				"GraphEditor"
			}
		);

		PublicDependencyModuleNames.AddRange(
		new string[] { 
			"Emergence"
			}
		);

		PublicDefinitions.Add("UNREAL_MARKETPLACE_BUILD=1");
	}
}
