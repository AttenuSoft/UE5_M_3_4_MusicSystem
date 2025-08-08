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

void UDecoratorComponent::SetupDecoratorComponent(TSoftObjectPtr<USoundBase> track, bool bLooping, int minLoops, int maxLoops, TSoftObjectPtr<USoundBase> LoopOut, float InFadeInDuration, float InFadeOutDuration, FName InDecoratorName, TArray<FName> InProhibitedDecorators)
{
	if (!track.ToSoftObjectPath().IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Decorator track is null, destroying decorator component."));
		DecoratorFinished();
	}

	DecoratorTrack = track;
	LoopOutTrack = LoopOut;
	FadeInDuration = InFadeInDuration;
	FadeOutDuration = InFadeOutDuration;
	DecoratorName = InDecoratorName;
	ProhibitedDecorators = InProhibitedDecorators;
	bIsLooping = bLooping;
	MinLoopsCount = minLoops;
	MaxLoopsCount = maxLoops;

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	if (LoopOut)
	{
		bHasLoopOut = true;
		LoopOutTrack = LoopOut;
		Streamable.RequestAsyncLoad(LoopOutTrack.ToSoftObjectPath(), nullptr);
	}

	MaxLoopCount = FMath::RandRange(minLoops, maxLoops);

	//load track to be played
	Streamable.RequestAsyncLoad(DecoratorTrack.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &UDecoratorComponent::OnDecoratorTrackLoaded));

}

void UDecoratorComponent::FadeDecoratorOut()
{
	if (!PrimaryAudioComponent->OnAudioFinished.IsBound())
	{
		PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::DecoratorFinished);
	}
		
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
				//check if there is a loop out track
				if (bHasLoopOut && LoopOutTrack.Get())
				{
					PrimaryAudioComponent->SetSound(LoopOutTrack.Get());
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

void UDecoratorComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	PrimaryAudioComponent->OnAudioFinished.RemoveDynamic(this, &UDecoratorComponent::DecoratorFinished);

}

void UDecoratorComponent::DecoratorFinished()
{	
	OnDecoratorFinished.Broadcast(this);
	DestroyComponent();
}

void UDecoratorComponent::OnDecoratorTrackLoaded()
{

	if (DecoratorTrack.Get())
	{
		PrimaryAudioComponent->SetSound(DecoratorTrack.Get());

		if (bIsLooping)
		{
		float TimerDuration = 0.0f;

			//determine how long
			if (LoopOutTrack.Get())
			{
				TimerDuration = FMath::RandRange(MinLoopsCount, MaxLoopsCount) * DecoratorTrack->Duration;
			}
			else
			{
				TimerDuration = (FMath::RandRange(MinLoopsCount, MaxLoopsCount) * DecoratorTrack->Duration) - FadeOutDuration;
			}

			if (UWorld* World = GetWorld())
			{
				//setup timer for number of loops
				World->GetTimerManager().SetTimer(
					LoopTimerHandle,
					this,
					&UDecoratorComponent::OnLoopTimerFinished,
					TimerDuration,									//duration for the timer
					false											//does not loop, fires once
				);

				PrimaryAudioComponent->FadeIn(FadeInDuration);
			}
			
		}
		else
		{
			PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::DecoratorFinished);
			PrimaryAudioComponent->Play(0.0f);
		}
	}

}

void UDecoratorComponent::OnLoopTimerFinished()
{
	PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::DecoratorFinished);
	LoopTimerHandle.Invalidate();

	if (LoopOutTrack.Get())
	{
		PrimaryAudioComponent->SetSound(LoopOutTrack.Get());
		bLoopOutBound = true;
		PrimaryAudioComponent->Play(0.0f);
	}
	else
	{
		PrimaryAudioComponent->FadeOut(FadeOutDuration, 0);
	}

}

