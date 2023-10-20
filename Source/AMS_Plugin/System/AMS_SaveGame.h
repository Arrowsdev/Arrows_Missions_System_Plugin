//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AMS_SubSystem.h"
#include "AMS_SaveGame.generated.h"

/**
 * this class for saving logics , you need to inhert your custom save game form this one so the mission system save will blend 
 * seamlessly with your save system , just use the provided (create save object) and (load game from slot) and (save game to slot)
 * and not the kismet version of those functions , because the subsystem saves missions progression under the hood and we dont want 
 * to override it's behaviours 
 */
UCLASS(Blueprintable, BlueprintType, HideDropDown)
class AMS_PLUGIN_API UAMS_SaveGame : public USaveGame
{
	GENERATED_BODY()


public:

	//record list for finished missions
	UPROPERTY()
	TArray<FRecordEntry> SG_FinishedMissions;

	
	//used to save the not finished missions so it resembles a checkpoints 
	//next time when loading we activate those missions from this record list
	UPROPERTY()
	TArray<FRecordEntry> SG_ActiveMissionsWhenSaved;

	//save also checkpoints so if the game was closed and then opened we can restore the saved checkpoint
	UPROPERTY()
		TArray<FRecordEntry> SG_CheckPointMissionsRecords;

	UPROPERTY()
		FTransform SG_StartTransform;

	UFUNCTION(BlueprintCallable, Category = "SaveGameHook")
	FORCEINLINE void HookSubSystemData(FAMS_SavePackage SubsystemPackage)
	{
		SG_FinishedMissions = SubsystemPackage.SG_FinishedMissions;
		SG_ActiveMissionsWhenSaved = SubsystemPackage.SG_ActiveMissionsWhenSaved;
		SG_CheckPointMissionsRecords = SubsystemPackage.SG_CheckPointMissionsRecords;
		SG_StartTransform = SubsystemPackage.SG_StartTransform;
	}

};
