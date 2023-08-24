//Advanced Mission System Developed by AIS Studios @2023


#include "Factories/AMS_Factory_ActionObject.h"
#include "AMS_Plugin/Public/ActionObject.h"

UAMS_Factory_ActionObject::UAMS_Factory_ActionObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UActionObject::StaticClass();
}

UObject* UAMS_Factory_ActionObject::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UActionObject* NewActionObject = NewObject<UActionObject>(InParent, Class, Name, Flags | RF_Transactional);

	return NewActionObject;
}