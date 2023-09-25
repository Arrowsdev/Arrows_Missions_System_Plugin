//Advanced Mission System Developed by AIS Studios @2023
#pragma once

#include "CoreMinimal.h"
#include "Factories/BlueprintFactory.h"
#include "AMS_Factory_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AMS_PLUGINEDITOR_API UAMS_Factory_SaveGame : public UBlueprintFactory
{
	GENERATED_BODY()

	UAMS_Factory_SaveGame(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
	virtual bool ConfigureProperties() override;
};
