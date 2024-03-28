// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditorPlusTools : ModuleRules
{
	public EditorPlusTools(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PrivateDefinitions.AddRange(
			new string[] {
				"EP_ENABLE_CLASS_BROWSER",
				"EP_ENABLE_MENU_COLLECTION",
				"EP_ENABLE_RESOURCE_BROWSER",
				"EP_ENABLE_RESOURCE_BROWSER_ALL_IN_ONE",
			}
			);

		if (Target.Configuration != UnrealTargetConfiguration.Development &&
		    Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PrivateDefinitions.Add("EP_ENABLE_MENU_TEST");
		}

		if (Target.Platform == UnrealTargetPlatform.Mac)
		{
			PrivateDefinitions.Add("EP_USE_TOOLBAR");
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
				"EditorPlus",
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
				"DeveloperSettings",
				"DeveloperToolSettings",
				"EditorSubsystem",
				"EditorFramework",
				"Projects",
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
