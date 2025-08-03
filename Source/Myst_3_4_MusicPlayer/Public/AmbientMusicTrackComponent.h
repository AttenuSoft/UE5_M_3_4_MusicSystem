// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MusicTrackComponent.h"
#include "MusicDataAsset.h"
#include "Sound/SoundWave.h"
#include "Engine/World.h"
#include "DecoratorComponent.h"
#include "Quartz/QuartzSubsystem.h"
#include "Quartz/AudioMixerClockHandle.h"
#include "Sound/SoundBase.h"
#include "AmbientMusicTrackComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYST_3_4_MUSICPLAYER_API UAmbientMusicTrackComponent : public UMusicTrackComponent
{
	GENERATED_BODY()

public:	
	
	UAmbientMusicTrackComponent();

	UPROPERTY(BlueprintReadOnly)
	TArray<int> BeatsBeforeNextTrack = {0, 12, 24};	//Pad, primary decorator, secondary decorator


	//Quartz clock timer variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	UQuartzSubsystem* QuartzSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	UQuartzClockHandle* DecoratorClockHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	FQuartzClockSettings DecoratorClockSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	FName DecoratorClock;


	//Active spawned track components
	UPROPERTY(BlueprintReadOnly)
	UDecoratorComponent* PadComponent;

	UPROPERTY(BlueprintReadOnly)
	UDecoratorComponent* PrimaryDecoratorComponent;

	UPROPERTY(BlueprintReadOnly)
	UDecoratorComponent* SecondaryDecoratorComponent;

	FAmbientMusicTrack ThisTrack;

	const int MaxSpawnedComponentCount = 3;

	UPROPERTY(BlueprintReadOnly)
	TArray<FAmbientPad> ValidPads;

	UPROPERTY(BlueprintReadOnly)
	TArray<FAmbientDecoratorWrapper> ValidDecorators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicFrequency")
	int CurrentMusicFrequency = 5;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName>  ProhibitiedDecorators;


protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "TrackComponentFunction")
	virtual void PlayTrack() override;

	UFUNCTION(BlueprintCallable, Category = "TrackComponentFunction")
	virtual void FadeTrackOut() override;

	UFUNCTION(BlueprintCallable, Category = "TrackComponentFunction")
	void SetupMusicComponent(FAmbientMusicTrack TrackData);

	UFUNCTION(BlueprintNativeEvent, Category = "TrackComponentFunction")
	void SetupQuartzClock(float BeatsPerMinute);
	virtual void SetupQuartzClock_Implementation(float BeatsPerMinute);

	UFUNCTION()
	void StartNewPad(TArray<FAmbientPad> pads);

	UFUNCTION()
	void StartNewDecorator(TArray<FAmbientDecoratorWrapper> ambients, bool PrimaryDecorator);

	UFUNCTION()
	TSoftObjectPtr<USoundBase> SelectRandomTrack(TArray<TSoftObjectPtr<USoundBase>> tracks);

	template <typename T>
	T SelectRandomFromArray(TArray<T> tracksArray)
	{
		return tracksArray[FMath::RandRange(0, tracksArray.Num() - 1)];
	}

	FAmbientPad SelectRandomAmbientPad(TArray<FAmbientPad> pads);

	FAmbientDecoratorWrapper SelectRandomAmbientDecorator(TArray<FAmbientDecoratorWrapper> ambients);

	bool IsTrackCurrentlyPlaying(FName track);

	UFUNCTION()
	void OnDecoratorFinished(UDecoratorComponent* FinishedTrack);

	UFUNCTION()
	void OnAllTracksFadedOut(UDecoratorComponent* decorator);


private:

	const TArray<TArray<int>> MusicFrequencySettings = { {25, 70, 50, 70}, {25, 65, 45, 65}, {20, 60, 40, 60},
															{20, 55, 35, 55}, {15, 50, 30, 50}, {15, 45, 25, 45},
															{10, 40, 20, 40}, {10, 35, 15, 35}, {5, 30, 10, 30}, 
															{5, 25, 5, 25} };

	const int MusicFreqMin = 0;
	const int MusicFreqMax = 9;

	void PopulateValidPadArray();
	void PopulateValidDecoratorArray();
	void RemoveIndividualItemFromDecorator(TSoftObjectPtr<USoundBase> DecoratorToRemove);
	void SetTrackFrequency(int InNewFreq);
	void SetBeatsBeforeNextTrack(int Index);
	bool IsDecoratorCurrentlyProhibited(FName decorator);
	void AddProhibitedDecorators(TArray<FName> decorators);
	void RemoveProhibitedDecorators(TArray<FName> decorators);

};
