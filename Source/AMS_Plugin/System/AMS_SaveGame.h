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
	UPROPERTY(EditAnywhere, Category = "AMS")
	TArray<FRecordEntry> SG_FinishedMissions;

	
	//used to save the not finished missions so it resembles a checkpoints 
	//next time when loading we activate those missions from this record list
	UPROPERTY(EditAnywhere, Category = "AMS")
	TArray<FRecordEntry> SG_ActiveMissionsWhenSaved;

};
