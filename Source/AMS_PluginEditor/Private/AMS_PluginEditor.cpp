//Advanced Mission System Developed by AIS Studios @2023

#include "AMS_PluginEditor.h"
#include "ISettingsModule.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"
#include "AMS_Plugin/Public/MissionObject.h"
#include "AMS_Plugin/Public/ActionObject.h"

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
#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_DataCenter.h"

#include "AMS_PluginEditor/Public/AssetTypes/AMS_AssetType_CustomBlueprint.h"
#include "AMS_DetailCustomizations.h"
#include "AMS_PluginEditor/Public/Helpers/AMS_ThumbnailRenderer.h"

#include "AMS_PluginEditor/Public/AMS_Tools/AMS_TransformHelper.h"
#include "AMS_PluginEditor/Public/AMS_Tools/MissionsTweaker.h"

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
	}

	RegisterAssetTypes();

	// register custom layouts
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RegisterCustomClassLayout(UActionObject::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&Action_DetailCustomizations::MakeInstance));
		PropertyModule.RegisterCustomClassLayout(UMissionObject::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&Missions_DetailCustomizations::MakeInstance));
	}

	//ResourcesHelper = new AMS_ResourcesHelper(ResourcesRoot); //deprecated , better just devide all assets into groups 
	//register custom Renderer for actions blueprints
	//UThumbnailManager::Get().UnregisterCustomRenderer(UAMS_ActionBlueprint::StaticClass());
	//UThumbnailManager::Get().RegisterCustomRenderer(UAMS_ActionBlueprint::StaticClass(), UAMS_ThumbnailRenderer::StaticClass());
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

	//unregister the custom layouts
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomClassLayout(UActionObject::StaticClass()->GetFName());
		PropertyModule.UnregisterCustomClassLayout(UMissionObject::StaticClass()->GetFName());
	}

	if (ResourcesHelper)
	{
		delete ResourcesHelper;
	}
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
	ResourcesRoot = RootDir;

	MAKE_STYLE(AMS_SaveGame, SIcon.png, 64, 32);
	MAKE_STYLE(AMS_JuernalObject, JIcon2.png, 64, 32);
	MAKE_STYLE(MissionObject, JIcon2.png, 64, 32);
	MAKE_STYLE(LOGO, AMSLogo.png, 400, 200);
	MAKE_STYLE(ActionObject, AIconR.png, 64, 32);
	MAKE_STYLE(BlackListedAction, AIconBL.png, 64, 32);

	//registering brushes for differenet tools ui
	AMS_StyleSet->Set("AboutWindow", new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/AMSLogo.png")), FVector2D(400,200)));
	AMS_StyleSet->Set("ToolsIcon", new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/ToolsIcon.png")), FVector2D(64)));
	AMS_StyleSet->Set("SaveButton", new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/SIcon.png")), FVector2D(64)));
	AMS_StyleSet->Set("BuildList", new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/BIcon.png")), FVector2D(64)));
	AMS_StyleSet->Set("SearchIcon", new FSlateImageBrush(AMS_StyleSet->RootToContentDir(TEXT("Resources/SearchIcon.png")), FVector2D(16)));

	return true;
}


void FAMS_PluginEditor::OnActionAssetCreated(TSubclassOf<UObject> newActionAsset)
{
	UE_LOG(LogTemp, Warning, TEXT("[AMS Tools] : Action recently created!"));
}


void FAMS_PluginEditor::RegisterAssetTypes()
{
	IAssetTools& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetCategoryBit = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("AMS_CategoryKey")), LOCTEXT("AMS_NameKey", "Mission System"));

	REGISTER_ASSET_TYPE(UAMS_AssetType_ActionObject);
	REGISTER_ASSET_TYPE(UAMS_AssetType_JuernalObject);
	REGISTER_ASSET_TYPE(UAMS_AssetType_MissionObject);
	REGISTER_ASSET_TYPE(UAMS_AssetType_SaveGame);
	REGISTER_ASSET_TYPE(UAMS_AssetType_DataCenter);

	REGISTER_ASSET_TYPE(AMS_AssetType_MissionBlueprint);
	REGISTER_ASSET_TYPE(AMS_AssetType_SaveGameBlueprint);
	REGISTER_ASSET_TYPE(AMS_AssetType_ActionBlueprint);
	REGISTER_ASSET_TYPE(AMS_AssetType_DataCenterBlueprint);
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

	//documentation section
	menuBuilder.BeginSection("Docs", FText::FromString("documentation"));
	menuBuilder.AddMenuSeparator(FName("documentation"));
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
	ModuleListeners.Add(MakeShareable(new AMS_Subsystem_API));
	ModuleListeners.Add(MakeShareable(new AMS_About));
	ModuleListeners.Add(MakeShareable(new MissionsTweaker));
}

void FAMS_PluginEditor::AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList>& CommandList, EExtensionHook::Position position)
{
	MenuExtender->AddMenuExtension(extensionHook, position, CommandList, extensionDelegate);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAMS_PluginEditor, AMS_PluginEditor)
