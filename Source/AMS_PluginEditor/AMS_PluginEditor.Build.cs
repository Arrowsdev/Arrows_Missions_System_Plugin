//Advanced Mission System Developed by AIS Studios @2023

using UnrealBuildTool;

public class AMS_PluginEditor : ModuleRules
{
	public AMS_PluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
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
                "Engine",
                "CoreUObject",
                "AMS_Plugin",
                "EditorWidgets",
                "LevelEditor",
                "Slate", 
                "EditorStyle",
                "AssetTools",
                "EditorWidgets",
                "UnrealEd",
                "ComponentVisualizers",
                "InputCore",
                
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"Projects",
				"AssetRegistry",
				"AssetTools",
				"EditorStyle",
				"UnrealEd",
                "AppFramework",
                "KismetWidgets",
                "MainFrame",
                "PropertyEditor",
                "ComponentVisualizers",
                "TypedElementRuntime",
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
