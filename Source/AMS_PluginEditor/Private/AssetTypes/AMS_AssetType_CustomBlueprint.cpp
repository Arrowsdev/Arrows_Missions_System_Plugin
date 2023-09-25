// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTypes/AMS_AssetType_CustomBlueprint.h"
#include "AMS_PluginEditor/Public/AMS_CustomBlueprint.h"
#include "AMS_Plugin/Public/MissionObject.h"

AMS_AssetType_CustomBlueprint::AMS_AssetType_CustomBlueprint(EAssetTypeCategories::Type InAssetCategory):
	MyAssetCategory(InAssetCategory)
{
	AssetName = FText::FromString("AMS Custom Asset");
	AssetDescription = FText::FromString("AMS Custom Asset");
	AssetColor = FColor::Yellow;
}

UClass* AMS_AssetType_CustomBlueprint::GetSupportedClass() const
{
	return AMS_SupportedClass;
}


AMS_AssetType_MissionBlueprint::AMS_AssetType_MissionBlueprint(EAssetTypeCategories::Type InAssetCategory):AMS_AssetType_CustomBlueprint(InAssetCategory)
{
	AssetName = FText::FromString("Mission Blueprint");
	AssetDescription = FText::FromString("AMS Custom Asset");
	AssetColor = FColor::Emerald;
	AMS_SupportedClass = UAMS_MissionBlueprint::StaticClass();
}

AMS_AssetType_SaveGameBlueprint::AMS_AssetType_SaveGameBlueprint(EAssetTypeCategories::Type InAssetCategory):AMS_AssetType_CustomBlueprint(InAssetCategory)
{
	AssetName = FText::FromString("Save Game Blueprint");
	AssetDescription = FText::FromString("AMS Custom Asset");
	AssetColor = FColor::Blue;
	AMS_SupportedClass = UAMS_SaveGameBlueprint::StaticClass();
}

AMS_AssetType_ActionBlueprint::AMS_AssetType_ActionBlueprint(EAssetTypeCategories::Type InAssetCategory):AMS_AssetType_CustomBlueprint(InAssetCategory)
{
	AssetName = FText::FromString("Action Blueprint");
	AssetDescription = FText::FromString("AMS Custom Asset");
	AssetColor = FColor(201, 29, 85);
	AMS_SupportedClass = UAMS_ActionBlueprint::StaticClass();
}


AMS_AssetType_DataCenterBlueprint::AMS_AssetType_DataCenterBlueprint(EAssetTypeCategories::Type InAssetCategory):AMS_AssetType_CustomBlueprint(InAssetCategory)
{
	AssetName = FText::FromString("Data Center Blueprint");
	AssetDescription = FText::FromString("AMS Custom Asset");
	AssetColor = FColor::Magenta;
	AMS_SupportedClass = UAMS_DataCenterBlueprint::StaticClass();
}
