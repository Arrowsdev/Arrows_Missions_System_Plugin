//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "SScreenFade.generated.h"
/**
 * the idea of using slate for this is ditched , ill use umg for it , i was thinking this can be a way for me to practice slate but
 * i think this will slow down the development
 */
UCLASS()
class AMS_PLUGIN_API UScreenFade : public UUserWidget
{
public:

	GENERATED_BODY()

public:

	UScreenFade(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "MissionfadeVars")
		UBorder* Border;


	UWidgetAnimation* FadeToPlay;

	UWidgetAnimation* PlayToFade;

	//initially used for when a mission starts but it has to set the player location so the fade is used to hide the transition
	void RunFadeToPlay();

	//this is when we restart any mission we fade to dark screen so we can set locations and other cleanup for mission to restart with proper world state
	//like deleting old unkilled enemies so when it restarts it can spawn fresh new ones
	void RunPlayToFade();

	
};
