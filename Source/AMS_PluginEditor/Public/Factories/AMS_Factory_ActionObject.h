//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AMS_Factory_ActionObject.generated.h"

/**
 * 
 */
UCLASS()
class AMS_PLUGINEDITOR_API UAMS_Factory_ActionObject : public UFactory
{
	GENERATED_BODY()

	UAMS_Factory_ActionObject(const FObjectInitializer& ObjectInitializer);

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
