// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnrealEd/Classes/ThumbnailRendering/ThumbnailManager.h"
#include "UnrealEd/Classes/ThumbnailRendering/BlueprintThumbnailRenderer.h"
#include "AMS_ThumbnailRenderer.generated.h"

/**
 * class to override the default renderer that using the asset type icon and thumbnails
 */
UCLASS()
class AMS_PLUGINEDITOR_API UAMS_ThumbnailRenderer : public UBlueprintThumbnailRenderer
{
	GENERATED_BODY()	
protected:

	// UThumbnailRenderer implementation
	virtual void GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const override;
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget*, FCanvas* Canvas, bool bAdditionalViewFamily) override;
	virtual bool CanVisualizeAsset(UObject* Object) override;
	virtual bool AllowsRealtimeThumbnails(UObject* Object) const { return true; }

	UTexture2D* GetTextureFromGeneratedClass(UClass* Class) const;
};
