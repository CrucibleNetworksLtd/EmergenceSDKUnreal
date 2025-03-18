// Copyright Crucible Networks Ltd 2023. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class EmergenceBlockchainWallet : ModuleRules
{
	public EmergenceBlockchainWallet(ReadOnlyTargetRules Target) : base(Target)
	{
		
		if(Target.Platform == UnrealTargetPlatform.Win64){
			RuntimeDependencies.Add("$(TargetOutputDir)/../../Plugins/Emergence/EmergenceDll/Win64/...");
		}
		
		bEnforceIWYU = true;
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "UMG",
				"JsonUtilities",
				"HTTP",
                "EmergenceCore"
            }
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Json",
                "SlateCore",
                "Projects",
                "OpenSSL",
                "EmergenceCore"
            }
		);

		if (Target.Version.MajorVersion >= 5 && Target.Version.MinorVersion >= 2)
		{
			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"ApplicationCore"
				}
			);
		}

		//Stuff added for WebLogin
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"HTTPServer"
			}
		);

        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				
				"JwtVerifier",
				"HashSHA256"
			}
		);
	}
}
