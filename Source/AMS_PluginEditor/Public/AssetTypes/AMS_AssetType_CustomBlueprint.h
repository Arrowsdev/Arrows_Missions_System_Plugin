// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AMS_Plugin/System/AMS_CustomBlueprint.h"
#include "AssetTypeActions/AssetTypeActions_Blueprint.h"


/**
 * this is the new parent of AMS asset types , which now overrides the custom blueprints type
 */
class AMS_PLUGINEDITOR_API AMS_AssetType_CustomBlueprint : public FAssetTypeActions_Blueprint
{
public:
	AMS_AssetType_CustomBlueprint(EAssetTypeCategories::Type InAssetCategory);
	~AMS_AssetType_CustomBlueprint() {};

	// IAssetTypeActions interface
	virtual FText GetName() const override { return AssetName; };
	virtual FColor GetTypeColor() const override { return AssetColor; };
	virtual UClass* GetSupportedClass() const override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override { return FText::FromString("AMS Asset"); };
	virtual uint32 GetCategories() override { return MyAssetCategory;  };
	// End of IAssetTypeActions interface

protected:
	FText AssetName;
	FText AssetDescription;
	FColor AssetColor;
	UClass* AMS_SupportedClass;
	EAssetTypeCategories::Type MyAssetCategory;

};

//mission blueprints asset style
class AMS_PLUGINEDITOR_API AMS_AssetType_MissionBlueprint : public AMS_AssetType_CustomBlueprint
{
public:
	AMS_AssetType_MissionBlueprint(EAssetTypeCategories::Type InAssetCategory);
	~AMS_AssetType_MissionBlueprint() {};

};


//Save Game blueprints asset style
class AMS_PLUGINEDITOR_API AMS_AssetType_SaveGameBlueprint : public AMS_AssetType_CustomBlueprint
{
public:
	AMS_AssetType_SaveGameBlueprint(EAssetTypeCategories::Type InAssetCategory);
	~AMS_AssetType_SaveGameBlueprint() {};

};

//action blueprint style
class AMS_PLUGINEDITOR_API AMS_AssetType_ActionBlueprint : public AMS_AssetType_CustomBlueprint
{
public:
	AMS_AssetType_ActionBlueprint(EAssetTypeCategories::Type InAssetCategory);
	~AMS_AssetType_ActionBlueprint() {};
};

class AMS_PLUGINEDITOR_API AMS_AssetType_DataCenterBlueprint : public AMS_AssetType_CustomBlueprint
{
public:
	AMS_AssetType_DataCenterBlueprint(EAssetTypeCategories::Type InAssetCategory);
	~AMS_AssetType_DataCenterBlueprint() {};

};