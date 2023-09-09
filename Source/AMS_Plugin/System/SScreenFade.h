//Advanced Mission System Developed by AIS Studios @2023

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"


/**
 * 
 */
class AMS_PLUGIN_API SScreenFade : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SScreenFade) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& Args);

private:

	float Opacity;
	FTimerHandle FadeTimerHandle;

	void StartFadeIn();
	void StartFadeOut();
	void UpdateOpacity(float NewOpacity);
};
