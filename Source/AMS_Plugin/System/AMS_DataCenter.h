//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AMS_DataCenter.generated.h"


class UAMS_SaveGame;

/**
 * this class is meant to be a portal to add custom data saving along side the mission save
 * just make sure that your save class inherits from the AMS_SaveGame and put it in the ams project settings
 */
UCLASS(Blueprintable, BlueprintType, HideDropDown)
class AMS_PLUGIN_API UAMS_DataCenter : public UObject
{
	GENERATED_BODY()

public:
	/*called when the subsystem tries to save the game data so if you want to save custom data you can do it here
	* be careful when using if you have many active missions , if that is the case then use the subsystem delegate and subscirbe
	* to it in the player character and hook your custom data from there or even inside some of the missions to avoid '
	* overheads
	*/
	UFUNCTION(BlueprintNativeEvent, Category = MissionEvents)
		void OnGameSaveStarted(UAMS_SaveGame* SaveGameObject);
	virtual void OnGameSaveStarted_Implementation(UAMS_SaveGame* SaveGameObject);/*Implemented*/


	/*called when the load game is called and it helpes to give a central place to read loaded data along side with the save game object
	 * that is returned with the load game function
	 */
	UFUNCTION(BlueprintNativeEvent, Category = MissionEvents)
		void OnGameLoaded(UAMS_SaveGame* SaveGameObject);
	virtual void OnGameLoaded_Implementation(UAMS_SaveGame* SaveGameObject){/*Implemented*/ }

	//used to hook your custom save data to the default save process by hijaking the save event returned save object
	//and after setting the values you call this function to complete the saving
	UFUNCTION(BlueprintCallable, Category = "Mission System")
		void CompleteSaving(UPARAM(ref)UAMS_SaveGame* saveGameObject);


	UWorld* GetWorld() const;
	
};
