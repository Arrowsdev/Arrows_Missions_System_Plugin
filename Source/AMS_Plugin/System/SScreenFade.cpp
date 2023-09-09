// Fill out your copyright notice in the Description page of Project Settings.


#include "SScreenFade.h"

void SScreenFade::Construct(const FArguments& Args)
{
	ChildSlot
		[
		   SNew(SBorder)
		  .HAlign(HAlign_Fill)
		  .VAlign(VAlign_Fill)
		  .ColorAndOpacity(FColor::Black)
		];

	Opacity = 1.0f;
}

void SScreenFade::StartFadeIn()
{

}

void SScreenFade::StartFadeOut()
{
}

void SScreenFade::UpdateOpacity(float NewOpacity)
{
}
