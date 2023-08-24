//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"
#include "C:\Program Files\Epic Games\UE_5.1\Engine\Plugins\Developer\TraceSourceFiltering\Source\SourceFilteringEditor\Private\ClassFilterObject.h"

class AMS_PLUGINEDITOR_API UAMS_AssetType_MissionObject : public FAssetTypeActions_Base
{
public:
	UAMS_AssetType_MissionObject(EAssetTypeCategories::Type InAssetCategory);
	~UAMS_AssetType_MissionObject() {};

	// IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	// End of IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;
};


class AMS_EditorUtilities
{
public:

	// Helper function which defined what classes are available to class selector

	static bool PickChildrenOfClass(const FText& TitleText, UClass*& OutChosenClass, UClass* Class);

};
