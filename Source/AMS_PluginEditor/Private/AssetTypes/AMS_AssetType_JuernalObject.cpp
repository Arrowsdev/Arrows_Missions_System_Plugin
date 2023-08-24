//Advanced Mission System Developed by AIS Studios @2023

#include "AssetTypes/AMS_AssetType_JuernalObject.h"
#include "AMS_Plugin/Public/AMS_JuernalObject.h"

UAMS_AssetType_JuernalObject::UAMS_AssetType_JuernalObject(EAssetTypeCategories::Type InAssetCategory):
   MyAssetCategory(InAssetCategory)
{
}

FText UAMS_AssetType_JuernalObject::GetName() const
{
	return FText::FromString("Game Juernal");
}

FColor UAMS_AssetType_JuernalObject::GetTypeColor() const
{
	return FColor::Yellow;
}

UClass* UAMS_AssetType_JuernalObject::GetSupportedClass() const
{
	return UAMS_JuernalObject::StaticClass();
}

void UAMS_AssetType_JuernalObject::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
	FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

uint32 UAMS_AssetType_JuernalObject::GetCategories()
{
	return MyAssetCategory;
}
