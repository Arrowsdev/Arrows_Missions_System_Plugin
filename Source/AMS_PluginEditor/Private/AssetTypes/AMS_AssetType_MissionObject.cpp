//Advanced Mission System Developed by AIS Studios @2023

#include "AssetTypes/AMS_AssetType_MissionObject.h"
#include "AMS_Plugin/Public/MissionObject.h"


#define LOCTEXT_NAMESPACE "AssetTypeActions"

UAMS_AssetType_MissionObject::UAMS_AssetType_MissionObject(EAssetTypeCategories::Type InAssetCategory):
MyAssetCategory(InAssetCategory)
{
}

FText UAMS_AssetType_MissionObject::GetName() const
{
    return FText::FromString("Mission Object");
}

FColor UAMS_AssetType_MissionObject::GetTypeColor() const
{
    return FColor::Emerald;
}

UClass* UAMS_AssetType_MissionObject::GetSupportedClass() const
{
    return UMissionObject::StaticClass();
}

#undef LOCTEXT_NAMESPACE 
