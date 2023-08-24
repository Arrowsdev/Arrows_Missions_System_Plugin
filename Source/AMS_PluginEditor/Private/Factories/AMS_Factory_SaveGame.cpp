//Advanced Mission System Developed by AIS Studios @2023


#include "Factories/AMS_Factory_SaveGame.h"
#include "AMS_Plugin/System/AMS_SaveGame.h"

UAMS_Factory_SaveGame::UAMS_Factory_SaveGame(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UAMS_SaveGame::StaticClass();
}

UObject* UAMS_Factory_SaveGame::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UAMS_SaveGame* NewSaveGameObject = NewObject<UAMS_SaveGame>(InParent, Class, Name, Flags | RF_Transactional);

	return NewSaveGameObject;
}