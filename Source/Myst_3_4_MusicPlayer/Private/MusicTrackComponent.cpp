// File: MusicTrackComponent.cpp
// Description: Parent class for both AmbientMusicTrackComponent and SingleMusicTrackComponent. Contains shared variables and functions to be overridden in 
// child classes.
// 
// Author: Andrew Sales
// Created: August 2025


#include "MusicTrackComponent.h"

UMusicTrackComponent::UMusicTrackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
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

