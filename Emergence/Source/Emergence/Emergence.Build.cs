// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;

public class Emergence : ModuleRules
{
	public Emergence(ReadOnlyTargetRules Target) : base(Target)
	{
		bool MarketplaceBuild = true;
		
		if(MarketplaceBuild || Target.Platform == UnrealTargetPlatform.Mac){
			PublicDefinitions.Add("UNREAL_MARKETPLACE_BUILD=1");
		}
		else{
			PublicDefinitions.Add("UNREAL_MARKETPLACE_BUILD=0");
			RuntimeDependencies.Add("$(BinaryOutputDir)", "$(PluginDir)/EmergenceServer/Windows/...");
		}
		
		RuntimeDependencies.Add(
		"$(TargetOutputDir)/../../Plugins/Emergence/EmergenceDll/Win64/...");
		
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
				"HTTP",
			}
		);

		if (Target.Version.MajorVersion >= 5 && Target.Version.MinorVersion >= 4)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"ApplicationCore"
				}
			);
		}	
		
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
