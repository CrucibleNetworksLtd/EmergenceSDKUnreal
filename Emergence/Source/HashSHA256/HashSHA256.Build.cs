// Copyright 2023 Blue Mountains GmbH. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class HashSHA256 : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}

	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "../ThirdParty/")); }
	}
	
	public HashSHA256(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "CoreUObject",
                "Engine"
            }
		);
		
		PrivateIncludePaths.Add(Path.Combine(ThirdPartyPath, "PicoSha2"));
	}
}
