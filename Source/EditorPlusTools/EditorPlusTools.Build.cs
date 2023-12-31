﻿// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditorPlusTools : ModuleRules
{
	public EditorPlusTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PrivateDefinitions.AddRange(
			new string[] {
				"EP_ENABLE_CLASS_BROWSER",
			}
			);

		if (Target.Configuration != UnrealTargetConfiguration.Development &&
		    Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PrivateDefinitions.Add("EP_ENABLE_MENU_TEST");
		}
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"EditorPlusTools",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"Engine",
				"CoreUObject",
				"InputCore",
				"LevelEditor",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"AssetTools",
				"EditorWidgets",
				"UnrealEd",
				"BlueprintGraph",
				"AnimGraph",
				"ComponentVisualizers",
				"EditorPlus",
				"ToolMenus",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
