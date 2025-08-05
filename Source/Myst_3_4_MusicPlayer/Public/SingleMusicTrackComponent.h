// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MusicTrackComponent.h"
#include "Sound/SoundBase.h"
#include "MusicDataAsset.h"
#include "Delegates/DelegateCombinations.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "SingleMusicTrackComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYST_3_4_MUSICPLAYER_API USingleMusicTrackComponent : public UMusicTrackComponent
{
	GENERATED_BODY()

public:	
	
	USingleMusicTrackComponent();

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
