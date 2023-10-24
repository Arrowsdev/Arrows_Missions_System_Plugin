//Advanced Mission System Developed by AIS Studios @2023


#include "Helpers/AMS_ResourcesHelper.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"

AMS_ResourcesHelper::AMS_ResourcesHelper()
{
}

AMS_ResourcesHelper::~AMS_ResourcesHelper()
{
	UE_LOG(LogTemp, Warning, TEXT("Resources Helper Was Destroyed"));
	{
		Do_Texture->RemoveFromRoot();
	}
	if (Dont_Texture)
	{
		Dont_Texture->RemoveFromRoot();
	}
}

AMS_ResourcesHelper::AMS_ResourcesHelper(FString resPath)
{
	FString DoPath   = FPaths::ProjectPluginsDir() + TEXT("AMS_Plugin/Resources/AIconR.png");
	FString DontPath = FPaths::ProjectPluginsDir() + TEXT("AMS_Plugin/Resources/AIconBL.png");

	RegisterTexture(DoPath, Do_Texture);
	RegisterTexture(DontPath, Dont_Texture);
	UE_LOG(LogTemp, Warning, TEXT("Path Passed : %s"), *resPath);
}

void AMS_ResourcesHelper::RegisterTexture(FString& resPath, TObjectPtr<UTexture2D>& outResults)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	EImageFormat Format = EImageFormat::PNG;

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);

	TArray<uint8> RowFileData;

	if (!FFileHelper::LoadFileToArray(RowFileData, *resPath))
	{
		outResults = nullptr;
	}

	if (!ImageWrapper->SetCompressed(RowFileData.GetData(), RowFileData.Num()))
	{
		outResults = nullptr;
	}

	TArray64<uint8> RowImageData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RowImageData))
	{
		outResults = nullptr;
	}

	//create texture 
	UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());

	if (!Texture) return;

	Texture->UpdateResource();

	void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, RowImageData.GetData(), RowImageData.Num());
	Texture->GetPlatformData()->Mips[0].BulkData.Unlock();

	Texture->SRGB = true;
	Texture->CompressionSettings = TC_Default;
	Texture->AddressX = TA_Clamp;
	Texture->AddressY = TA_Clamp;

	outResults = Texture;
	outResults->AddToRoot();
}