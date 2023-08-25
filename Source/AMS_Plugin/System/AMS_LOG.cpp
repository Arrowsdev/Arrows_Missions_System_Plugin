// Fill out your copyright notice in the Description page of Project Settings.


#include "AMS_LOG.h"
#include "AMS_Plugin/System/AMS_SubSystem.h"

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

	TArray<FString> Line;
	Line[0] = "Active Missions : " + FString::FromInt(subsystem->ActiveMissions.Num());
	Line[1] = "Finished Missions : " + FString::FromInt(subsystem->FinishedMissions.Num());
	Line[2] = "Jeurnal Class : "  SPIT_CLASS(subsystem->JuernalClass, subsystem->JuernalSingelton);
	Line[3] = "Save Game Class : " SPIT_CLASS(subsystem->SaveGameClass, nullptr);
	Line[4] = "Data Venter Class : " SPIT_CLASS(subsystem->DataCenterClass, subsystem->DataCenterSinglton);

	for (FString lin : Line)
	{
		LOG_AMS(lin, duration, FColor::Magenta);
	}
}
