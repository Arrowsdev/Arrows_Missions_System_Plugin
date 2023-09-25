//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"



class AMS_PLUGINEDITOR_API UAMS_AssetType_SaveGame : public FAssetTypeActions_Base
{
public:

	UAMS_AssetType_SaveGame(EAssetTypeCategories::Type InAssetCategory);
	~UAMS_AssetType_SaveGame() {};

	// IAssetTypeActions interface
	virtual FText GetName() const override { return FText::FromString("AMS SaveGame"); };
	virtual FColor GetTypeColor() const override { return FColor::Blue; };
	virtual UClass* GetSupportedClass() const override;
	virtual uint32 GetCategories() override { return MyAssetCategory; };
	//// End of IAssetTypeActions interface

private:
	EAssetTypeCategories::Type MyAssetCategory;
};
