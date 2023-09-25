//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"


class AMS_PLUGINEDITOR_API UAMS_AssetType_MissionObject : public FAssetTypeActions_Base
{
public:
	UAMS_AssetType_MissionObject(EAssetTypeCategories::Type InAssetCategory);
	~UAMS_AssetType_MissionObject() {};

	// IAssetTypeActions interface
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FText GetDisplayNameFromAssetData(const FAssetData& AssetData) const  override { return FText::FromString("Mission Object Class"); }
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("Mission Object Class"); }
	virtual uint32 GetCategories() override { return MyAssetCategory; };
	// End of IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;
};


