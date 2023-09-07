// Fill out your copyright notice in the Description page of Project Settings.


#include "AMS_LOG.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

DEFINE_LOG_CATEGORY(MissionSystem);

#define SPIT_CLASS(TheClass,Instance) +##TheClass##->GetName() + " Instance : " + (##Instance? "Valid" : "Not Valid")
AMS_LOG::AMS_LOG()
{
}

AMS_LOG::~AMS_LOG()
{
}

void LogObjective()
{
}

void AMS_LOG::LogSubsystem(const UAMS_SubSystem* subsystem, const float duration)
{
	if (!subsystem) return;

	TArray<FString> Lines;
	Lines.EmplaceAt(0, "Active Missions : " + FString::FromInt(subsystem->ActiveMissions.Num()));
	Lines.EmplaceAt(1, "Finished Missions : " + FString::FromInt(subsystem->FinishedMissions.Num()));
	Lines.EmplaceAt(2, "Jeurnal Class : "     SPIT_CLASS(subsystem->JuernalClass, subsystem->JuernalSingelton));
	Lines.EmplaceAt(3, "Save Game Class : "   SPIT_CLASS(subsystem->SaveGameClass, nullptr));
	Lines.EmplaceAt(4, "Data Venter Class : " SPIT_CLASS(subsystem->DataCenterClass, subsystem->DataCenterSinglton));
	
	//for screen print we flip the log since the top on screen is the latest
	PrintLog("############## END SUBSYSTEM LOG #####################", 0.0f, FColor::Blue);
	for (FString& Line : Lines)
	{
		PrintLog(Line,0.0f,FColor::Yellow);
	}
	LOG_AMS("Subsystem Logging :", duration, FColor::Blue);

	//to do here : log to the console 
}

bool AMS_LOG::IsOnScreenDebugIsEnabled()
{
	if (MissionSubSystemInstance)
		return MissionSubSystemInstance->AllowScreenDebug;

	return false;
}
