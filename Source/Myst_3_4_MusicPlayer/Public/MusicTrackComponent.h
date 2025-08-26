// File: MusicTrackComponent.h
// Description: Parent class for both AmbientMusicTrackComponent and SingleMusicTrackComponent. Contains shared variables and functions to be overridden in 
// child classes.
// 
// Author: Andrew Sales
// Created: August 2025

#pragma once

#include "CoreMinimal.h"
#include "MusicDataAsset.h"
#include "Components/ActorComponent.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "MusicTrackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYST_3_4_MUSICPLAYER_API UMusicTrackComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	
	UMusicTrackComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTrackEnd, FName, ThisTrack, FName,  NextTrack);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTrackEnd OnTrackEnd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeSettings")
	float FadeInDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeSettings")
	float FadeOutDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrackInfo")
	FName TrackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrackInfo")
	FName NextTrack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TrackInfo")
	bool bTrackFadingOut = false;

	bool bProceedToNextTrack = true;


protected:
	
	virtual void BeginPlay() override;

public:	


	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void FadeTrackOut();

	virtual void PlayTrack();
		
};
