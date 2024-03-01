// Copyright Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class ActionPluginEditor : ModuleRules
	{
		public ActionPluginEditor(ReadOnlyTargetRules Target) : base(Target)
		{
			PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"AnimationCore",
				//"AnimationWarpingRuntime",
				"ActionPluginRuntime",
                "AnimGraph",
				"AnimGraphRuntime",
				"AnimationModifiers",
				"AnimationBlueprintLibrary",
				//"AnimationModifierLibrary",
				"Core",
                "CoreUObject",
                "Engine",
            });

			PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "SlateCore",
            });

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
						"BlueprintGraph",
						"EditorFramework",
						"Kismet",
                        "UnrealEd",
                    }
                );
            }
        }
	}
}
