//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetTools\Public\AssetTypeCategories.h"

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

	bool StyleWasSet;
	bool SetupStyle();

private:
	EAssetTypeCategories::Type AssetCategoryBit;

	TArray< TSharedRef<IAssetTypeActions> > CreatedAssetTypeActions;
	void RegisterAssetTypes();

};
