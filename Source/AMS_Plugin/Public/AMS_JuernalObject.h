// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AMS_Plugin/System/AMS_Types.h"
#include "AMS_JuernalObject.generated.h"

/**
 * this class holds the history of missions and the progression
 * should hook any new logics about that in this class, like full game progression percentage and other stuff
 */
UCLASS(Blueprintable, BlueprintType, HideDropDown)
class AMS_PLUGIN_API UAMS_JuernalObject : public UObject
{
	GENERATED_BODY()

	TArray<FRecordEntry> Juernal_FinishedMissions;

public:

	//to add finished
	void AddJuernalRecord(FRecordEntry newReecord);

	//to load the saved list 
	inline void AssignRecordList(TArray<FRecordEntry>& list)
	{
		Juernal_FinishedMissions = list;
	}

	/*get a reference to the juernal recorded progress and finished missions*/
	UFUNCTION(BlueprintPure, Category="Juernal")
		FORCEINLINE TArray<FRecordEntry>& GetMissionsRecord()
	{
		return Juernal_FinishedMissions;
	}
};
