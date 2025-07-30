// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IMusicTrackComponentInterface.generated.h"

UINTERFACE(MinimalAPI)
class UIMusicTrackComponentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYST_3_4_MUSICPLAYER_API IIMusicTrackComponentInterface
{
	GENERATED_BODY()


public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MusicTrackInterface")
	void PlayTrack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MusicTrackInterface")
	void StopTrack();

};
