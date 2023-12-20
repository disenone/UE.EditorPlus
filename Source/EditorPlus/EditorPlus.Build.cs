// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditorPlus: ModuleRules
{
	public EditorPlus(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PrivateDefinitions.AddRange(
			new string[] {
			}
			);
		
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
