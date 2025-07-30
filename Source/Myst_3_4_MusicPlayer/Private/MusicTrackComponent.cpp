// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicTrackComponent.h"

UMusicTrackComponent::UMusicTrackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	PrimaryAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PrimaryAudioComponent"));
	PrimaryAudioComponent->bAutoActivate = false;
	PrimaryAudioComponent->SetupAttachment(this);
	
}



void UMusicTrackComponent::BeginPlay()
{
	Super::BeginPlay();

}



void UMusicTrackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UMusicTrackComponent::FadeTrackOut()
{
	//Empty parent function
}

void UMusicTrackComponent::PlayTrack()
{
	//Empty parent function
}

