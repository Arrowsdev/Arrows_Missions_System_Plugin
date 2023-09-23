//Advanced Mission System Developed by AIS Studios @2023


#include "SScreenFade.h"
#include "Animation/WidgetAnimation.h"

UScreenFade::UScreenFade(const FObjectInitializer& ObjectInitializer) : UUserWidget(ObjectInitializer)
{
}

void UScreenFade::NativeConstruct()
{
	Super::NativeConstruct();

	//getting the fade animation
	FProperty* prop = GetClass()->PropertyLink;

	while (prop)
	{

		if (prop->GetClass() == FObjectProperty::StaticClass())
		{
			FObjectProperty* ObjProp = CastField<FObjectProperty>(prop);

			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);
				UWidgetAnimation* WidgetAnimation = Cast<UWidgetAnimation>(Obj);

				if (WidgetAnimation)
				{
					UWidgetAnimation** AnimationPtr = WidgetAnimation->GetName() == FString("FadeToPlay") ? &FadeToPlay : &PlayToFade;
					*AnimationPtr = WidgetAnimation;
				}
			}
		}

		prop = prop->PropertyLinkNext;
	}
}

void UScreenFade::RunFadeToPlay()
{
}

void UScreenFade::RunPlayToFade()
{
}