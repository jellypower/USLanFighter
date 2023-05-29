// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AndroidProject : ModuleRules
{
	public AndroidProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
	        "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput", "UMG"
	        , "USSimplestLANMatcher",
        });
        
        PublicIncludePaths.AddRange(
	        new string[] { "../Plugins/USSimplestLanMatcher", "AndroidProject"});
        
    }
}
