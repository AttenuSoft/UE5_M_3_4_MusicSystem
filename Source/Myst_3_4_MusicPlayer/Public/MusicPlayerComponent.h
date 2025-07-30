// Fill out your copyright notice in the Description page of Project Settings.

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

private:

	bool IsActiveMusicTrack(FName InMusicTrack);


};
