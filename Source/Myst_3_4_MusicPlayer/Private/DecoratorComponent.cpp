// Fill out your copyright notice in the Description page of Project Settings.


#include "DecoratorComponent.h"


UDecoratorComponent::UDecoratorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PrimaryAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PrimaryAudioComponent"));
	PrimaryAudioComponent->bAutoActivate = false;
	PrimaryAudioComponent->SetupAttachment(this);

}



void UDecoratorComponent::BeginPlay()
{
	Super::BeginPlay();
	
}



void UDecoratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UDecoratorComponent::SetupDecoratorComponent(USoundBase* track, bool bLooping, int minLoops, int maxLoops, USoundBase* LoopOut, float InFadeInDuration, float InFadeOutDuration, FName InDecoratorName, TArray<FName> InProhibitedDecorators)
{
	if (!track)
	{
		UE_LOG(LogTemp, Error, TEXT("Decorator track is null, destroying decorator component."));
		DecoratorFinished();
	}

	FadeInDuration = InFadeInDuration;
	FadeOutDuration = InFadeOutDuration;
	DecoratorName = InDecoratorName;
	ProhibitedDecorators = InProhibitedDecorators;

	if (LoopOut)
	{
		bHasLoopOut = true;
		LoopOutTrack = LoopOut;
	}

	PrimaryAudioComponent->SetSound(track);

	if (bLooping)
	{
		PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::PostLoopFinished);
		MaxLoopCount = FMath::RandRange(minLoops, maxLoops);
		PrimaryAudioComponent->FadeIn(FadeInDuration);
	}
	else
	{
		PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::DecoratorFinished);
		PrimaryAudioComponent->Play(0.0f);
	}
	
}

void UDecoratorComponent::FadeDecoratorOut()
{
	PrimaryAudioComponent->FadeOut(FadeOutDuration, 0);

}

void UDecoratorComponent::PostLoopFinished()
{
	
	if (bLoopOutBound)
	{
		DecoratorFinished();
	}
	else
	{
		NumTimesLooped += 1;

		//no loop out, one loop left - fading out last loop
		if (!bHasLoopOut && (NumTimesLooped == MaxLoopCount - 1))
		{
			PrimaryAudioComponent->OnAudioFinished.RemoveDynamic(this, &UDecoratorComponent::PostLoopFinished);
			PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::DecoratorFinished);
			PrimaryAudioComponent->Play(0.0f);
			float TrackDuration = PrimaryAudioComponent->Sound->GetDuration();
			PrimaryAudioComponent->FadeOut(TrackDuration, 0);
		}
		//still has remaining loops
		else
		{
			//max num loops reached
			if (NumTimesLooped >= MaxLoopCount)
			{
				OnLoopFinished.RemoveDynamic(this, &UDecoratorComponent::PostLoopFinished);

				//check if there is a loop out track
				if (bHasLoopOut)
				{
					PrimaryAudioComponent->SetSound(LoopOutTrack);
					bLoopOutBound = true;
					PrimaryAudioComponent->Play(0.0f);
				}
		
			}
			//continue to play the next track's loop
			else
			{
				PrimaryAudioComponent->Play(0.0f);
			}
		}

	}

}

void UDecoratorComponent::DecoratorFinished()
{
	OnDecoratorFinished.Broadcast(this);

}

