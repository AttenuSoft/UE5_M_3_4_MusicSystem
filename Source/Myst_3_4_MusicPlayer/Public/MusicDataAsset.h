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
	float FadeInDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FadeSettings")
	float FadeOutDuration = 0.0f;
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
struct MYST_3_4_MUSICPLAYER_API FAmbientDecorator
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	FName DecoratorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	TArray<TSoftObjectPtr<USoundBase>> Decorator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "4.0"))
	float Volume = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	bool bShouldPlayInOrder = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	TArray<FName> ProhibitedDecorators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	FAmbientLoopInfo LoopSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	TSoftObjectPtr<USoundBase> DecoratorOut;

	//fade settings for this specific decorator, not the ambient track as a whole
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	FFadeSettings FadeSettings;

	FORCEINLINE bool operator==(const FAmbientDecorator& Other) const
	{
		return (DecoratorName == Other.DecoratorName);
	}

};


USTRUCT(BlueprintType)
struct MYST_3_4_MUSICPLAYER_API FDelaySettings
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DelaySettings")
	int MinDelayBeforeNext_Pad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DelaySettings")
	int MaxDelayBeforeNext_Pad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DelaySettings")
	int MinDelayBeforeNext_Decorator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DelaySettings")
	int MaxDelayBeforeNext_Decorator;

	

	static constexpr int32 StockFrequencySettings[10][4] = {{25, 70, 50, 70}, {25, 65, 45, 65}, {20, 60, 40, 60},
													{20, 55, 35, 55}, {15, 50, 30, 50}, {15, 45, 25, 45},
													{10, 40, 20, 40}, {10, 35, 15, 35}, {5, 30, 10, 30},
													{5, 25, 5, 25} };

	FDelaySettings()
		: MinDelayBeforeNext_Pad(0), MaxDelayBeforeNext_Pad(0),
		MinDelayBeforeNext_Decorator(0), MaxDelayBeforeNext_Decorator(0)
	{}

	explicit FDelaySettings(int32 Index)
	{

		if (Index >= 0 && Index < 10)
		{
			MinDelayBeforeNext_Pad = StockFrequencySettings[Index][0];
			MaxDelayBeforeNext_Pad = StockFrequencySettings[Index][1];
			MinDelayBeforeNext_Decorator = StockFrequencySettings[Index][2];
			MaxDelayBeforeNext_Decorator = StockFrequencySettings[Index][3];
		}
		else
		{
			// Fallback values
			MinDelayBeforeNext_Pad = MaxDelayBeforeNext_Pad =
				MinDelayBeforeNext_Decorator = MaxDelayBeforeNext_Decorator = 0;
		}
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "4.0"))
	float Volume = 1;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "4.0"))
	float Volume = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DecoratorTrack")
	bool bShouoldPlayInOrder = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientPad")
	FAmbientLoopInfo LoopSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientPad")
	FFadeSettings FadeSettings;

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
	TArray<FAmbientDecorator> Decorators;

	//fade settings for when the ambient track fades as a whole
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AmbientMusicTrack")
	FFadeSettings FadeSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DelaySettings")
	int StartingDelay_Pad = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DelaySettings")
	int StartingDelay_PrimaryDecorator = 12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DelaySettings")
	int StartingDelay_SecondaryDecorator = 24;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditFixedSize), Category = "AmbientMusicTrack")
	TArray<FDelaySettings> FrequencySettings;

	FAmbientMusicTrack()
	{
		FrequencySettings.SetNum(10);
		for (int32 i = 0; i < FrequencySettings.Num(); ++i)
		{
			FrequencySettings[i] = FDelaySettings(i);
		}
	}

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
