// Copyright (c) 2021-2023 MrShaaban, Mohamad Shaaban, https://github.com/sha3sha3/UE-EasyJWT.

using UnrealBuildTool;
using System.IO;

public class JwtVerifier : ModuleRules
{
	public JwtVerifier(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicSystemIncludePaths.Add(Path.Combine(ModuleDirectory, "Public"));
        PublicIncludePaths.AddRange(
            new string[] {
				// ... add public include paths required here ...
				Path.Combine(Path.Combine(ModuleDirectory, "Public"))
            }
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"JwtCpp"
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		bEnableExceptions = true;
	}
}
