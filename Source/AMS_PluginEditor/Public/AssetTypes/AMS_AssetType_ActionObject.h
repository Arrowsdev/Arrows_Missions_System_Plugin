//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"


class AMS_PLUGINEDITOR_API UAMS_AssetType_ActionObject : public FAssetTypeActions_Base
{

public:
	UAMS_AssetType_ActionObject(EAssetTypeCategories::Type InAssetCategory);
	~UAMS_AssetType_ActionObject() {};

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
