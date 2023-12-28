//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetTools\Public\AssetTypeCategories.h"
#include "AMS_Tools/ModuleListenerInterface.h"
#include "Helpers/AMS_ResourcesHelper.h"


class FSlateStyleSet;
class FAssetTypeActions_Base;
class IAssetTypeActions;

class FAMS_PluginEditor : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**Register Styles**/
	TSharedPtr<FSlateStyleSet> AMS_StyleSet;

	AMS_ResourcesHelper* ResourcesHelper;

	bool StyleWasSet;
	bool SetupStyle();
	FString ResourcesRoot;
	/**Tools**/

	/*tell listeners about the module state*/
	void InvokeStartup();
	void InvokeShutdown();

	void AddModuleListeners();
	void AddMenuExtension(const FMenuExtensionDelegate& extensionDelegate, FName extensionHook, const TSharedPtr<FUICommandList>& CommandList = NULL, EExtensionHook::Position position = EExtensionHook::Before);
	TSharedRef<FWorkspaceItem> GetMenuRoot() { return MenuRoot; };

	static inline FAMS_PluginEditor& Get()
	{
		return FModuleManager::LoadModuleChecked< FAMS_PluginEditor >("AMS_PluginEditor");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("AMS_PluginEditor");
	}

	void OnActionAssetCreated(TSubclassOf<UObject> newActionAsset);


private:
	EAssetTypeCategories::Type AssetCategoryBit;

	TArray< TSharedRef<IAssetTypeActions> > CreatedAssetTypeActions;
	void RegisterAssetTypes();

	
	TArray<TSharedRef<IExampleModuleListenerInterface>> ModuleListeners;

	TSharedPtr<FExtensibilityManager> LevelEditorMenuExtensibilityManager;
	TSharedPtr<FExtender> MenuExtender;

	static TSharedRef<FWorkspaceItem> MenuRoot;

	void MakePulldownMenu(FMenuBarBuilder& menuBuilder);
	void FillPulldownMenu(FMenuBuilder& menuBuilder);
	void CreateBarExtention();

};
