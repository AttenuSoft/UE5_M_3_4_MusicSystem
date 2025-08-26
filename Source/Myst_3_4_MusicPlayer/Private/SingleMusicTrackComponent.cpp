// File: SingleMusicTrackComponent.cpp
// Description: A component spawned by MusicPlayerComponent in order to play single, one-shot music tracks.
// 
// Author: Andrew Sales
// Created: August 2025


#include "SingleMusicTrackComponent.h"


USingleMusicTrackComponent::USingleMusicTrackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;	

	PrimaryAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PrimaryAudioComponent"));
	PrimaryAudioComponent->bAutoActivate = false;
	PrimaryAudioComponent->SetupAttachment(this);

}


// Called when the game starts
void USingleMusicTrackComponent::BeginPlay()
{
	Super::BeginPlay();
}


void USingleMusicTrackComponent::SetupMusicComponent(FSingleMusicTrack trackData)
{
	if (trackData.Track.ToSoftObjectPath().IsValid())
	{
		//Set fade in/out settings
		FadeInDuration = trackData.FadeSettings.FadeInDuration;
		FadeOutDuration = trackData.FadeSettings.FadeOutDuration;
		NextTrack = trackData.NextTrack;
		TrackName = trackData.TrackName;
		SingleTrack = trackData.Track;

		PrimaryAudioComponent->SetVolumeMultiplier(trackData.Volume);

		//bind to on finished delegate
		PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &USingleMusicTrackComponent::DestroyComponentOnTrackFinished);

		//load track to be played
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(SingleTrack.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &USingleMusicTrackComponent::PlayTrack));

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid or empty track"));
		DestroyComponentOnTrackFinished();
	}
}


void USingleMusicTrackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USingleMusicTrackComponent::DestroyComponentOnTrackFinished()
{
	PrimaryAudioComponent->OnAudioFinished.RemoveDynamic(this, &USingleMusicTrackComponent::DestroyComponentOnTrackFinished);

	OnSingleMusicTrackStopped.Broadcast();

	if (bProceedToNextTrack)
	{
		OnTrackEnd.Broadcast(TrackName, NextTrack);
	}

	UE_LOG(LogTemp, Warning, TEXT("Track has finished and been destroyed."));
	DestroyComponent();
}


void USingleMusicTrackComponent::PlayTrack()
{
	//Make sure the sound asset was loaded successfully
	if (SingleTrack.Get())
	{
		PrimaryAudioComponent->SetSound(SingleTrack.Get());

		if (FadeInDuration > 0.0f)
		{
			PrimaryAudioComponent->FadeIn(FadeInDuration);
		}
		else
		{
			PrimaryAudioComponent->Play();
		}
		OnSingleMusicTrackStarted.Broadcast();
	}
	//Otherwise, destroy the component
	else
	{
		DestroyComponentOnTrackFinished();
	}
	
}

void USingleMusicTrackComponent::FadeTrackOut()
{
if (PrimaryAudioComponent)
	{
		OnSingleMusicTrackStopped.Broadcast();
		bTrackFadingOut = true;
		bProceedToNextTrack = false;
		PrimaryAudioComponent->FadeOut(FadeOutDuration, 0.0f);
	}
}

