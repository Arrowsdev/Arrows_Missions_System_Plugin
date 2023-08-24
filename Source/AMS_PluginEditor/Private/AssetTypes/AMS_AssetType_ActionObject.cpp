//Advanced Mission System Developed by AIS Studios @2023


#include "AssetTypes/AMS_AssetType_ActionObject.h"
#include "AMS_Plugin/Public/ActionObject.h"


UAMS_AssetType_ActionObject::UAMS_AssetType_ActionObject(EAssetTypeCategories::Type InAssetCategory):
     MyAssetCategory(InAssetCategory)
{
}

FText UAMS_AssetType_ActionObject::GetName() const
{
	return FText::FromString("Action Object");
}

FColor UAMS_AssetType_ActionObject::GetTypeColor() const
{
	return FColor(201, 29, 85);
}

UClass* UAMS_AssetType_ActionObject::GetSupportedClass() const
{
	return UActionObject::StaticClass();
}

void UAMS_AssetType_ActionObject::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

uint32 UAMS_AssetType_ActionObject::GetCategories()
{
	return MyAssetCategory;
}
