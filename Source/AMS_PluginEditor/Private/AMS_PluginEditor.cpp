//Advanced Mission System Developed by AIS Studios @2023

#include "AMS_PluginEditor.h"
#include "ISettingsModule.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#include "Editor.h"
#include "EditorModeRegistry.h"

#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_ActionObject.h"
#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_JuernalObject.h"
#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_MissionObject.h"
#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_SaveGame.h"

#define LOCTEXT_NAMESPACE "FAMS_PluginEditorModule"

//small macro to make the style for classes
#define MAKE_STYLE(className,ThumbName, ThumbSize, IconSize)\
\
    AMS_StyleSet->Set("ClassThumbnail."#className,\
    new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/"#ThumbName)), FVector2D(ThumbSize)));\
    AMS_StyleSet->Set("ClassIcon."#className,\
    new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/"#ThumbName)), FVector2D(IconSize)))

//macro to register asset types to asset category
#define REGISTER_ASSET_TYPE(AssetType)\
  TSharedRef<AssetType> ##AssetType##_Action = MakeShareable(new AssetType(AssetCategoryBit));\
  AssetToolsModule.RegisterAssetTypeActions(##AssetType##_Action);\
  CreatedAssetTypeActions.Add(##AssetType##_Action)


void FAMS_PluginEditor::StartupModule()
{
	//Register Editor Settings Category
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->RegisterSettings("Project", "Game", "AMSSettings", LOCTEXT("RuntimeSettingsName", "AMS Settings"),
			           LOCTEXT("RuntimeSettingsDescription", "Missions System Configurations and settings"), GetMutableDefault<UAMS_SubSystem>());
	}

	//Register Custom Assets Styles
	StyleWasSet = SetupStyle();
	if (StyleWasSet)
	{
		FSlateStyleRegistry::RegisterSlateStyle(*AMS_StyleSet);
	}

	RegisterAssetTypes();
}

void FAMS_PluginEditor::ShutdownModule()
{
	//Unregister the settings category
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Project", "Game", "AMSSettings");
	}

	//unregister custom styles
	if (StyleWasSet)
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*AMS_StyleSet);
	}

	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index]);
		}
	}

	CreatedAssetTypeActions.Empty();

}

bool FAMS_PluginEditor::SetupStyle()
{
	FString RootDir;
	auto PluginDir = IPluginManager::Get().FindPlugin("AMS_Plugin")->GetBaseDir();
	auto EngineDir = FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::EnginePluginsDir() + TEXT("Marketplace/AMS_Plugin")));

	if (FPaths::DirectoryExists(PluginDir))
	{
		RootDir = PluginDir;
	}
	else if (FPaths::DirectoryExists(EngineDir))
	{
		RootDir = EngineDir;
	}

	else return false;

	AMS_StyleSet = MakeShareable(new FSlateStyleSet("AMS"));
	AMS_StyleSet->SetContentRoot(RootDir);

	MAKE_STYLE(AMS_SaveGame, SIcon.png, 64, 32);
	MAKE_STYLE(AMS_JuernalObject, JIcon2.png, 64, 32);
	MAKE_STYLE(MissionObject, JIcon2.png, 64, 32);

	return true;
}

void FAMS_PluginEditor::RegisterAssetTypes()
{
	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetCategoryBit = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("AMS_CategoryKey")), LOCTEXT("AMS_NameKey", "Mission System"));

	REGISTER_ASSET_TYPE(UAMS_AssetType_ActionObject);
	REGISTER_ASSET_TYPE(UAMS_AssetType_JuernalObject);
	REGISTER_ASSET_TYPE(UAMS_AssetType_MissionObject);
	REGISTER_ASSET_TYPE(UAMS_AssetType_SaveGame);
}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAMS_PluginEditor, AMS_PluginEditor)