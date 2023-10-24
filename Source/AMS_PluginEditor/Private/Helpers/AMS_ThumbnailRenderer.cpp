// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/AMS_ThumbnailRenderer.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "AMS_Plugin/Public/ActionObject.h"
#include "AMS_PluginEditor/Public/AMS_PluginEditor.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

void UAMS_ThumbnailRenderer::GetThumbnailSize(UObject* Object, float Zoom, uint32& OutWidth, uint32& OutHeight) const
{
	UBlueprint* ActionBP = Cast<UBlueprint>(Object);
	if (ActionBP)
	{
		UTexture2D* Texture = GetTextureFromGeneratedClass(ActionBP->GeneratedClass);
		if (Texture)
		{
			OutWidth = FMath::TruncToInt(Zoom * (float)Texture->GetSurfaceWidth());
			OutHeight = FMath::TruncToInt(Zoom * (float)Texture->GetSurfaceHeight());
		}

		else
		{
			Super::GetThumbnailSize(Object, Zoom, OutWidth, OutHeight);
		}
	}

	else
	{
		Super::GetThumbnailSize(Object, Zoom, OutWidth, OutHeight);
	}
}

void UAMS_ThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas, bool bAdditionalViewFamily)
{
	UBlueprint* ActionBP = Cast<UBlueprint>(Object);
	if (ActionBP)
	{
		UTexture2D* Texture2D = GetTextureFromGeneratedClass(ActionBP->GeneratedClass);
		if (Texture2D)
		{
			
			const bool bUseTranslucentBlend = Texture2D && Texture2D->HasAlphaChannel() && ((Texture2D->LODGroup == TEXTUREGROUP_UI) || (Texture2D->LODGroup == TEXTUREGROUP_Pixels2D));
			TRefCountPtr<FBatchedElementParameters> BatchedElementParameters;
			if (bUseTranslucentBlend)
			{
				// If using alpha, draw a checkerboard underneath first.
				const int32 CheckerDensity = 8;
				auto& Checker = UThumbnailManager::Get().CheckerboardTexture;
				Canvas->DrawTile(
					0.0f, 0.0f, Width, Height, // Dimensions
					0.0f, 0.0f, CheckerDensity, CheckerDensity, // UVs
					FLinearColor::White, Checker->GetResource()); // Tint & Texture
			}
			// Use A canvas tile item to draw
			FCanvasTileItem CanvasTile(FVector2D(X, Y), Texture2D->GetResource(), FVector2D(Width, Height), FLinearColor::White);
			CanvasTile.BlendMode = bUseTranslucentBlend ? SE_BLEND_Translucent : SE_BLEND_Opaque;
			CanvasTile.BatchedElementParameters = BatchedElementParameters;
			CanvasTile.Draw(Canvas);
			//if (Texture2D && Texture2D->IsCurrentlyVirtualTextured())
			//{
			//	auto VTChars = TEXT("VT");
			//	int32 VTWidth = 0;
			//	int32 VTHeight = 0;
			//	StringSize(GEngine->GetLargeFont(), VTWidth, VTHeight, VTChars);
			//	float PaddingX = Width / 128.0f;
			//	float PaddingY = Height / 128.0f;
			//	float ScaleX = Width / 64.0f; //Text is 1/64'th of the size of the thumbnails
			//	float ScaleY = Height / 64.0f;
			//	// VT overlay
			//	FCanvasTextItem TextItem(FVector2D(Width - PaddingX - VTWidth * ScaleX, Height - PaddingY - VTHeight * ScaleY), FText::FromString(VTChars), GEngine->GetLargeFont(), FLinearColor::White);
			//	TextItem.EnableShadow(FLinearColor::Black);
			//	TextItem.Scale = FVector2D(ScaleX, ScaleY);
			//	TextItem.Draw(Canvas);
			//}
		}
		return;
	}
}

bool UAMS_ThumbnailRenderer::CanVisualizeAsset(UObject* Object)
{
	UBlueprint* ActionBP = Cast<UBlueprint>(Object);
	if (ActionBP && GetTextureFromGeneratedClass(ActionBP->GeneratedClass) != nullptr)
	{
		return true;
	}

	return false;
}

UTexture2D* UAMS_ThumbnailRenderer::GetTextureFromGeneratedClass(UClass* Class) const
{
	if (Class)
	{

		if (Class->IsChildOf(UActionObject::StaticClass()))
		{
			if (UActionObject* CDO = Class->GetDefaultObject<UActionObject>())
			{
				if (FAMS_PluginEditor::Get().ResourcesHelper)
				{
					return FAMS_PluginEditor::Get().ResourcesHelper->Do_Texture;
				}	
			}
		}
	}
	return UThumbnailManager::Get().CheckerboardTexture;
}
