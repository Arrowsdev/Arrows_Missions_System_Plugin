// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "logging/LogCategory.h"

DECLARE_LOG_CATEGORY_EXTERN(MissionSystem, Log, All);


/**
 * class dedicated for logging data 
 */

class AMS_Types;
class UAMS_SubSystem;

class AMS_PLUGIN_API AMS_LOG
{
public:
	AMS_LOG();
	~AMS_LOG();

   void LogSubsystem(const UAMS_SubSystem* subsystem, const float duration);

   static bool IsOnScreenDebugIsEnabled();
};

static void LogObjective();

