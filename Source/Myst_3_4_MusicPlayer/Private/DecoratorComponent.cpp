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

void UDecoratorComponent::SetupDecoratorComponent(TSoftObjectPtr<USoundBase> track, bool bLooping, int minLoops, int maxLoops, 
	TSoftObjectPtr<USoundBase> LoopOut, float InFadeInDuration, float InFadeOutDuration, FName InDecoratorName, TArray<FName> InProhibitedDecorators, 
	float InVolume, FQuartzQuantizationBoundary InQuantizationBoundary,UQuartzClockHandle* InClockHandle, UQuartzSubsystem* InQuartzSubsystem)
{
	if (!track.ToSoftObjectPath().IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Decorator track is null, destroying decorator component."));
		DecoratorFinished();
	}

	DecoratorTrack = track;
	FadeInDuration = InFadeInDuration;
	FadeOutDuration = InFadeOutDuration;
	DecoratorName = InDecoratorName;
	ProhibitedDecorators = InProhibitedDecorators;
	bIsLooping = bLooping;
	MinLoopsCount = minLoops;
	MaxLoopsCount = maxLoops;

	ClockHandle = InClockHandle;
	QuantizationBoundary = InQuantizationBoundary;
	QuartzSubsystem = InQuartzSubsystem;

	PrimaryAudioComponent->SetVolumeMultiplier(InVolume);

	MaxLoopCount = FMath::RandRange(minLoops, maxLoops);

	//create an array of paths to load
	TArray<FSoftObjectPath> AssetsToLoad;

	//if there is an out track, add it to array
	if (!LoopOut.ToSoftObjectPath().IsNull())
	{
		bHasLoopOut = true;
		LoopOutTrack = LoopOut;
		AssetsToLoad.Add(LoopOutTrack.ToSoftObjectPath());
	}

	//add decorator track to array
	AssetsToLoad.Add(DecoratorTrack.ToSoftObjectPath());

	//load track to be played
	DecoratorHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(AssetsToLoad,
		FStreamableDelegate::CreateUObject(this, &UDecoratorComponent::OnDecoratorTrackLoaded)
	);

}

void UDecoratorComponent::FadeDecoratorOut()
{
	if (!PrimaryAudioComponent->OnAudioFinished.IsBound())
	{
		PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::DecoratorFinished);
	}
		
	PrimaryAudioComponent->FadeOut(FadeOutDuration, 0);

}


void UDecoratorComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	PrimaryAudioComponent->OnAudioFinished.RemoveDynamic(this, &UDecoratorComponent::DecoratorFinished);

}

void UDecoratorComponent::DecoratorFinished()
{	
	DecoratorHandle->ReleaseHandle();
	DecoratorHandle.Reset();
	LoadedDecoratorTrack = nullptr;
	LoadedOutTrack = nullptr;
	OnDecoratorFinished.Broadcast(this);
	DestroyComponent();
}

void UDecoratorComponent::OnDecoratorTrackLoaded()
{

	if (DecoratorTrack.Get())
	{
		LoadedDecoratorTrack = DecoratorTrack.Get();

		if (bHasLoopOut)
		{
			LoadedOutTrack = LoopOutTrack.Get();
		}

		PrimaryAudioComponent->SetSound(LoadedDecoratorTrack);

		if (bIsLooping)
		{

			//determine how long
			if (LoopOutTrack.Get())
			{
				TimerDuration = FMath::RandRange(MinLoopsCount, MaxLoopsCount) * DecoratorTrack->Duration;
			}
			else
			{
				TimerDuration = (FMath::RandRange(MinLoopsCount, MaxLoopsCount) * DecoratorTrack->Duration) - FadeOutDuration;
			}

			//FireLoopTimer(EventType, DecoratorName);

			PlayQuanitizedDecorator();

			/*if (UWorld* World = GetWorld())
			{
				//setup timer for number of loops
				World->GetTimerManager().SetTimer(
					LoopTimerHandle,
					this,
					&UDecoratorComponent::OnLoopTimerFinished,
					TimerDuration,									//duration for the timer
					false											//does not loop, fires once
				);

				
				//PrimaryAudioComponent->FadeIn(FadeInDuration);
			}*/
			
		}
		else
		{
			PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &UDecoratorComponent::DecoratorFinished);
			
			PlayQuanitizedDecorator();

			//PrimaryAudioComponent->Play(0.0f);
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

void UDecoratorComponent::PlayQuanitizedDecorator()
{
	if (bIsLooping)
	{
		DecoratorStartedPlaying.BindDynamic(this, &UDecoratorComponent::FireLoopTimer);
	}
	
	PrimaryAudioComponent->PlayQuantized(
		GetWorld(),				// WorldContextObject
		ClockHandle,			// Clock handle
		QuantizationBoundary,	// Quantization settings
		DecoratorStartedPlaying,// On-start delegate
		0.0f,					// Start time (offset)
		FadeInDuration,			// Fade-in duration
		1.0f,					// Fade volume level
		EAudioFaderCurve::Linear
	);
}

void UDecoratorComponent::FireLoopTimer(EQuartzCommandDelegateSubType InEventType, FName InDecoratorName)
{
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
	}
}

