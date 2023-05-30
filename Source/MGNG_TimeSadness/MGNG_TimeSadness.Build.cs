// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MGNG_TimeSadness : ModuleRules
{
	public MGNG_TimeSadness(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput" });
	}
}
