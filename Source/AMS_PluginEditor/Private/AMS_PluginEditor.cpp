//Advanced Mission System Developed by AIS Studios @2023

#include "AMS_PluginEditor.h"
#include "ISettingsModule.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

#include "Editor.h"
#include "EditorModeRegistry.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"

#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_ActionObject.h"
#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_JuernalObject.h"
#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_MissionObject.h"
#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_SaveGame.h"

#include "AMS_PluginEditor/Public/AMS_Tools/AMS_TransformHelper.h"

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


TSharedRef<FWorkspaceItem> FAMS_PluginEditor::MenuRoot = FWorkspaceItem::NewGroup(FText::FromString("Menu Root"));

void FAMS_PluginEditor::StartupModule()
{
	//create bar option
	CreateBarExtention();

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
		Logo = new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/FadeScreen.png")), FVector2D(100));
	}

	RegisterAssetTypes();

}

void FAMS_PluginEditor::ShutdownModule()
{
	//tells tools to shutdown
	InvokeShutdown();

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

void FAMS_PluginEditor::MakePulldownMenu(FMenuBarBuilder& menuBuilder)
{
	menuBuilder.AddPullDownMenu(
		FText::FromString("AMS Tools"),
		FText::FromString("Open the AMS Tools menu"),
		FNewMenuDelegate::CreateRaw(this, &FAMS_PluginEditor::FillPulldownMenu),
		"AMS_MenuExtenderHook",
		FName(TEXT("AMS Tools"))
	);
}

void FAMS_PluginEditor::FillPulldownMenu(FMenuBuilder& menuBuilder)
{
	// frame to put tools in, planning to also make the mission tweaker as a tool so we dont need to use pawns anymore
	menuBuilder.BeginSection("Tools", FText::FromString("Section 1"));
	menuBuilder.AddMenuSeparator(FName("Section_1"));
	menuBuilder.EndSection();
}

void FAMS_PluginEditor::CreateBarExtention()
{
	if (!IsRunningCommandlet())
	{
		FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorMenuExtensibilityManager = LevelEditorModule.GetMenuExtensibilityManager();
		MenuExtender = MakeShareable(new FExtender);
		MenuExtender->AddMenuBarExtension("Window", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FAMS_PluginEditor::MakePulldownMenu));
		LevelEditorMenuExtensibilityManager->AddExtender(MenuExtender);
	}

	InvokeStartup();
}

void FAMS_PluginEditor::InvokeStartup()
{
	if (!IsRunningCommandlet())
	{
		AddModuleListeners();
		for (int32 i = 0; i < ModuleListeners.Num(); ++i)
		{
			ModuleListeners[i]->OnStartupModule();
		}
	}
}

void FAMS_PluginEditor::InvokeShutdown()
{
	for (int32 i = 0; i < ModuleListeners.Num(); ++i)
	{
		ModuleListeners[i]->OnShutdownModule();
	}
}

void FAMS_PluginEditor::AddModuleListeners()
{
	ModuleListeners.Add(MakeShareable(new AMS_TransformHelper));
	
	/*@TODO : add missions tweaker for editing all missions details in one window to make it wasy to tweak details if no need to
	* edit to logics , so users can reset tasks and values or names and descriptions in central place for all available missions
	* in the project
	*/
	
}

void FAMS_PluginEditor::AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList>& CommandList, EExtensionHook::Position position)
{
	MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAMS_PluginEditor, AMS_PluginEditor)