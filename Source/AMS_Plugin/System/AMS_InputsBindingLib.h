//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/InputDelegateBinding.h"
#include "AMS_InputsBindingLib.generated.h"

/**
 * this is used to override the binding logics for regualr actors 
 */
UCLASS()
class AMS_PLUGIN_API UAMS_InputsBindingLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	//to bind our graph dynamic delegates to the player controller so they get fired too when the player presses an input
	static void BindInputDelegatesToObject(const UClass* InClass, UInputComponent* InputComponent, UObject* GraphObject);
};
