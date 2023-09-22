// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class InteractiveMusic2 : ModuleRules
{
	public InteractiveMusic2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		string libDir = Path.GetFullPath(Path.Combine(ModuleDirectory, "lib/Win64"));
		PublicAdditionalLibraries.Add(Path.Combine(libDir, "Ole32.Lib"));
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore",
            "EnhancedInput", "AkAudioMixer", "AudiokineticTools", 
            "AkAudio", "WwiseResourceCooker", "WwiseProjectDatabase", 
            "WwiseResourceLoader", "WwiseFileHandler", "WwiseSoundEngine" });
	}
}
