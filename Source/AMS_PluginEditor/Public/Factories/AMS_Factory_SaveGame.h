//Advanced Mission System Developed by AIS Studios @2023
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AMS_Factory_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AMS_PLUGINEDITOR_API UAMS_Factory_SaveGame : public UFactory
{
	GENERATED_BODY()

	UAMS_Factory_SaveGame(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
