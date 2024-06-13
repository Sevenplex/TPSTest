// Copyright Maxim Dudin, 2024. All Rights Reserved.

using UnrealBuildTool;

public class TPSTest : ModuleRules
{
	public TPSTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
