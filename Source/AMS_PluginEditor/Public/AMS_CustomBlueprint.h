//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Engine\Blueprint.h"
#include "Engine\BlueprintGeneratedClass.h"
#include "AMS_CustomBlueprint.generated.h"
/**
 * these classes meant to help opening the blueprint editor like normal but also have a specilized blueprint type 
 * so we can make a custom type for it
 */
UCLASS(config = Engine)
class AMS_PLUGINEDITOR_API UAMS_MissionBlueprint : public UBlueprint
{
public:
	GENERATED_BODY()

	UAMS_MissionBlueprint(const FObjectInitializer& ObjectInitializer);
	~UAMS_MissionBlueprint() {};
};

UCLASS(NeedsDeferredDependencyLoading)
class AMS_PLUGINEDITOR_API UAMS_MissionGeneratedBlueprint : public UBlueprintGeneratedClass
{
public:
	GENERATED_BODY()

	UAMS_MissionGeneratedBlueprint() {};
	~UAMS_MissionGeneratedBlueprint() {};
};


UCLASS(config = Engine)
class AMS_PLUGINEDITOR_API UAMS_ActionBlueprint : public UBlueprint
{
public:
	GENERATED_BODY()

	UAMS_ActionBlueprint(const FObjectInitializer& ObjectInitializer);
	~UAMS_ActionBlueprint() {};
};

UCLASS(NeedsDeferredDependencyLoading)
class AMS_PLUGINEDITOR_API UAMS_ActionGeneratedBlueprint : public UBlueprintGeneratedClass
{
public:
	GENERATED_BODY()

	UAMS_ActionGeneratedBlueprint() {};
	~UAMS_ActionGeneratedBlueprint() {};
};


UCLASS(config = Engine)
class AMS_PLUGINEDITOR_API UAMS_SaveGameBlueprint : public UBlueprint
{
public:
	GENERATED_BODY()

	UAMS_SaveGameBlueprint(const FObjectInitializer& ObjectInitializer);
	~UAMS_SaveGameBlueprint() {};
};

UCLASS(NeedsDeferredDependencyLoading)
class AMS_PLUGINEDITOR_API UAMS_SaveGameGeneratedBlueprint : public UBlueprintGeneratedClass
{
public:
	GENERATED_BODY()

	 UAMS_SaveGameGeneratedBlueprint() {};
	~UAMS_SaveGameGeneratedBlueprint() {};
};


UCLASS(config = Engine)
class AMS_PLUGINEDITOR_API UAMS_DataCenterBlueprint : public UBlueprint
{
public:
	GENERATED_BODY()

	UAMS_DataCenterBlueprint(const FObjectInitializer& ObjectInitializer);
	~UAMS_DataCenterBlueprint() {};
};

UCLASS(NeedsDeferredDependencyLoading)
class AMS_PLUGINEDITOR_API UAMS_DataCenterGeneratedBlueprint : public UBlueprintGeneratedClass
{
public:
	GENERATED_BODY()

	 UAMS_DataCenterGeneratedBlueprint() {};
	~UAMS_DataCenterGeneratedBlueprint() {};
};