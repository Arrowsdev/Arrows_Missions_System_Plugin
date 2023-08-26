//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"

class AMS_PLUGINEDITOR_API UAMS_AssetType_MissionObject : public FAssetTypeActions_Blueprint //FAssetTypeActions_Base
{
public:
	UAMS_AssetType_MissionObject(EAssetTypeCategories::Type InAssetCategory);
	~UAMS_AssetType_MissionObject() {};

	// IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;
   // virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	// End of IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;
};


//class AMS_EditorUtilities
//{
//  the logics that was intened here is now implemented in a separate file so it can be included easily in other types
// without the need 
//};
