// Copyright Maxim Dudin, 2024. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class TPSTestServerTarget : TargetRules
{
	public TPSTestServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_1;
		ExtraModuleNames.Add("TPSTest");
	}
}
