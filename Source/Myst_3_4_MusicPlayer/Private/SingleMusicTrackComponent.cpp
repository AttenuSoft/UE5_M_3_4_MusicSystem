// Fill out your copyright notice in the Description page of Project Settings.


#include "SingleMusicTrackComponent.h"


USingleMusicTrackComponent::USingleMusicTrackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;	

}


// Called when the game starts
void USingleMusicTrackComponent::BeginPlay()
{
	Super::BeginPlay();

}


void USingleMusicTrackComponent::SetupMusicComponent(FSingleMusicTrack trackData)
{
	if (trackData.Track)
	{
		//Set fade in/out settings
		bShouldFadeIn = trackData.FadeSettings.bShouldFadeIn;
		FadeInDuration = trackData.FadeSettings.FadeInDuration;
		bShouldFadeOut = trackData.FadeSettings.bShouldFadeOut;
		FadeOutDuration = trackData.FadeSettings.FadeOutDuration;
		NextTrack = trackData.NextTrack;
		TrackName = trackData.TrackName;

		//load track to be played and set it as the audio component's sound
		PrimaryAudioComponent->SetSound(trackData.Track);

		//bind to on finished delegate
		PrimaryAudioComponent->OnAudioFinished.AddDynamic(this, &USingleMusicTrackComponent::DestroyComponentOnTrackFinished);

		//play or fade in the track when ready
		PlayTrack();
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

	if (bProceedToNextTrack)
	{
		OnTrackEnd.Broadcast(TrackName, NextTrack);
	}

	UE_LOG(LogTemp, Warning, TEXT("Track has finished and been destroyed."));
	DestroyComponent();
}


void USingleMusicTrackComponent::PlayTrack()
{
	if (bShouldFadeIn)
	{
		PrimaryAudioComponent->FadeIn(FadeInDuration);
	}
	else
	{
		PrimaryAudioComponent->Play();
	}
}

void USingleMusicTrackComponent::FadeTrackOut()
{
if (PrimaryAudioComponent)
	{
		bProceedToNextTrack = false;
		PrimaryAudioComponent->FadeOut(FadeOutDuration, 0.0f);
	}
}

