// File: MusicPlayerComponent.h
// Description: A single instance of this component should be attached to the player character. It will in turn create either SingleMusicTrackComponent
// AmbientMusicTrackComponent instances to play any specified type of music track.
// 
// Author: Andrew Sales
// Created: August 2025

#pragma once

#include "CoreMinimal.h"
#include "MusicDataAsset.h"
#include "Components/ActorComponent.h"
#include "MusicTrackComponent.h"
#include "SingleMusicTrackComponent.h"
#include "AmbientMusicTrackComponent.h"
#include "MusicPlayerComponent.generated.h"


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYST_3_4_MUSICPLAYER_API UMusicPlayerComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	
	UMusicPlayerComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAmbientComponentCreated, UAmbientMusicTrackComponent*, AmbientTrack);

	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnAmbientComponentCreated OnAmbientComponentCreated;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicData")
	UMusicDataAsset* CurrentMusicData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicData")
	TMap<FName, UMusicTrackComponent*> ActiveMusicTracks;


protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void CleanUpActiveMusicTracks();

	UFUNCTION(BlueprintCallable)
	void SetupMusicData(UMusicDataAsset* newMusicData);

	UFUNCTION(BlueprintCallable)
	void ClearMusicData();

	UFUNCTION(BlueprintCallable)
	void CreateNewMusicTrack(FName InTrackName);

	UFUNCTION(BlueprintCallable)
	void OnMusicComponentTrackEnded(FName CurrentTrack, FName NextTrack);

	UFUNCTION(BlueprintCallable)
	void StopSpecificMusicTrack(FName TrackToStop);

	UFUNCTION(BlueprintCallable)
	void StopAllMusicTracks();

	UFUNCTION(BlueprintCallable)
	void SetNewMusicFrequency(int InFrequency);

	UFUNCTION(BlueprintCallable)
	int GetCurrentMusicFrequency();


private:

	bool IsActiveMusicTrack(FName InMusicTrack);


};
