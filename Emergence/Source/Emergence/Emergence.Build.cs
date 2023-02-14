// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;

public class Emergence : ModuleRules
{
	public Emergence(ReadOnlyTargetRules Target) : base(Target)
	{
		bool MarketplaceBuild = false;
		
		if(MarketplaceBuild){
			PublicDefinitions.Add("UNREAL_MARKETPLACE_BUILD=1");
		}
		else{
			PublicDefinitions.Add("UNREAL_MARKETPLACE_BUILD=0");
			if(Target.Platform == UnrealTargetPlatform.Win64)
			{
				RuntimeDependencies.Add("$(BinaryOutputDir)", "$(PluginDir)/EmergenceServer/Windows/...");
			}
			
			if(Target.Platform == UnrealTargetPlatform.Mac)
			{
				RuntimeDependencies.Add("$(BinaryOutputDir)", "$(PluginDir)/EmergenceServer/Mac/...");
			}
		}
		
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		CppStandard = CppStandardVersion.Cpp17;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"UMG",
				"Sockets",
				"Networking",
				"JsonUtilities",
				"Slate",
				"SlateCore",
				"Projects",
				"HTTP"
				// ... add other public dependencies that you statically link with here ...
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"SlateCore",
				"Json"
				// ... add private dependencies that you statically link with here ...	
			}
		);
	}
}
