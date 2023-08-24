//Advanced Mission System Developed by AIS Studios @2023


#include "Factories/AMS_Factory_JuernalObject.h"
#include "AMS_Plugin/Public/AMS_JuernalObject.h"

UAMS_Factory_JuernalObject::UAMS_Factory_JuernalObject(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UAMS_JuernalObject::StaticClass();
}

UObject* UAMS_Factory_JuernalObject::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UAMS_JuernalObject* NewJuernalObject = NewObject<UAMS_JuernalObject>(InParent, Class, Name, Flags | RF_Transactional);

	return NewJuernalObject;
}