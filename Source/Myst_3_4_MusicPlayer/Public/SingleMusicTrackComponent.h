// File: SingleMusicTrackComponent.h
// Description: A component spawned by MusicPlayerComponent in order to play single, one-shot music tracks.
// 
// Author: Andrew Sales
// Created: August 2025

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MusicTrackComponent.h"
#include "Sound/SoundBase.h"
#include "MusicDataAsset.h"
#include "Delegates/DelegateCombinations.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "SingleMusicTrackComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYST_3_4_MUSICPLAYER_API USingleMusicTrackComponent : public UMusicTrackComponent
{
	GENERATED_BODY()

public:	
	
	USingleMusicTrackComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSingleMusicTrackStarted);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSingleMusicTrackStopped);

	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnSingleMusicTrackStarted OnSingleMusicTrackStarted;

	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnSingleMusicTrackStopped OnSingleMusicTrackStopped;


	UPROPERTY()
	TSoftObjectPtr<USoundBase> SingleTrack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundComponents")
    UAudioComponent* PrimaryAudioComponent;

protected:
	
	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "TrackComponentFunction")
	void SetupMusicComponent(FSingleMusicTrack trackData);

	UFUNCTION(BlueprintCallable)
	void DestroyComponentOnTrackFinished();
		
	UFUNCTION(BlueprintCallable, Category = "TrackComponentFunction")
	void PlayTrack() override;

	UFUNCTION(BlueprintCallable)
	void FadeTrackOut() override;
		
};
