// Fill out your copyright notice in the Description page of Project Settings.

/*
	Class: MusicDataAsset
	Author: Andrew Sales
	Description: Defines all music for a specific age. 
				 SingleMusicTracks are complete, individual songs, such as "Full" or "Inst" tracks (i.e. Spire Theme).
				 Ambient tracks loop continually, generally with a "pad" track (that is either a single or short, looping track),
				 with "decorator" tracks playing simultaneously and randomly based on the pad track's BMP, either in a one-shot
				 fashion or with a random number of loops and possibly with and "out" track.
*/


#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "MusicDataAsset.generated.h"


USTRUCT(BlueprintType)
struct MYST_3_4_MUSICPLAYER_API FFadeSettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeSettings")
	bool bShouldFadeIn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeSettings")
	float FadeInDuration = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeSettings")
	bool bShouldFadeOut = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeSettings")
	float FadeOutDuration = 1.0f;
};


USTRUCT(BlueprintType)
struct MYST_3_4_MUSICPLAYER_API FAmbientLoopInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LoopInfo")
	bool bShouldLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LoopInfo")
	int MinNumLoops = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LoopInfo")
	int MaxNumLoops = 1;

};


USTRUCT(BlueprintType)
struct MYST_3_4_MUSICPLAYER_API FAmbientDecoratorWrapper
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorWrapper")
	FName DecoratorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorWrapper")
	TArray<TSoftObjectPtr<USoundBase>> Decorator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorWrapper")
	TArray<FName> ProhibitedDecorators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorWrapper")
	FAmbientLoopInfo LoopSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorWrapper")
	TSoftObjectPtr<USoundBase> DecoratorOut;

	FORCEINLINE bool operator==(const FAmbientDecoratorWrapper& Other) const
	{
		return DecoratorName == Other.DecoratorName;
	}

};



USTRUCT(BlueprintType)
struct MYST_3_4_MUSICPLAYER_API FSingleMusicTrack
{
	GENERATED_USTRUCT_BODY()

	//Single-specific music track data

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingleMusicTtrack")
	FName TrackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingleMusicTtrack")
	TSoftObjectPtr<USoundBase> Track;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingleMusicTtrack")
	FFadeSettings FadeSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SingleMusicTtrack")
	FName NextTrack;

};


USTRUCT(BlueprintType)
struct MYST_3_4_MUSICPLAYER_API FAmbientPad
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientPad")
	FName PadName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientPad")
	TSoftObjectPtr<USoundBase> Track;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientPad")
	FAmbientLoopInfo LoopSettings;

	FORCEINLINE bool operator==(const FAmbientPad& Other) const
	{
		return PadName == Other.PadName; 
	}

};



USTRUCT(BlueprintType)
struct MYST_3_4_MUSICPLAYER_API FAmbientMusicTrack
{
	GENERATED_USTRUCT_BODY()

	//Ambient-specific music track data

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientMusicTrack")
	FName TrackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientMusicTrack")
	TArray<FAmbientPad> Pads;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientMusicTrack")
	float BeatsPerMinute = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientMusicTrack")
	TArray<FAmbientDecoratorWrapper> Decorators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientMusicTrack")
	FFadeSettings FadeSettings;

};


UCLASS(BlueprintType)
class MYST_3_4_MUSICPLAYER_API UMusicDataAsset : public UDataAsset
{
	GENERATED_BODY()

	public: 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TMap<FName, FSingleMusicTrack> SingleMusicTracks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	TMap<FName, FAmbientMusicTrack> AmbientMusicTracks;

};
