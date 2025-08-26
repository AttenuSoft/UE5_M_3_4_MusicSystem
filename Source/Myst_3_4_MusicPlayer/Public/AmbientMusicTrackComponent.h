// File: AmbientMusicTrackComponent.h
// Description: A component that a spawned by MusicPlayerComponent in order to facilitate playing ambient music track. This component
// will spawn up to three DecoratorComponents at a time that are attached to it. 
// 
// Author: Andrew Sales
// Created: August 2025

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

	UPROPERTY(BlueprintReadOnly)
	TArray<int> BeatsSinceLastTrack = { 0, 0, 0 };

	//Quartz clock timer variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	UQuartzSubsystem* QuartzSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	UQuartzClockHandle* DecoratorClockHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	FQuartzClockSettings DecoratorClockSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	FName DecoratorClock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quartz")
	FQuartzQuantizationBoundary QuantizationBoundary;


	//Active spawned track components
	UPROPERTY(BlueprintReadOnly)
	UDecoratorComponent* PadComponent;

	UPROPERTY(BlueprintReadOnly)
	UDecoratorComponent* PrimaryDecoratorComponent;

	UPROPERTY(BlueprintReadOnly)
	UDecoratorComponent* SecondaryDecoratorComponent;

	FAmbientMusicTrack ThisTrack;

	UPROPERTY(BlueprintReadOnly)
	TArray<FAmbientPad> ValidPads;

	UPROPERTY(BlueprintReadOnly)
	TArray<FAmbientDecorator> ValidDecorators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MusicFrequency")
	int CurrentMusicFrequency = 9;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName>  ProhibitiedDecorators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SoundMix")
	TMap<FName, USoundMix*> AppliedSoundMixes;

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
	void StartNewDecorator(TArray<FAmbientDecorator> ambients, bool PrimaryDecorator);

	UFUNCTION()
	TSoftObjectPtr<USoundBase> SelectTrack(TArray<TSoftObjectPtr<USoundBase>> tracks, bool bInPlayInOrder);

	template <typename T>
	T SelectRandomFromArray(TArray<T> tracksArray)
	{
		return tracksArray[FMath::RandRange(0, tracksArray.Num() - 1)];
	}

	FAmbientPad SelectRandomAmbientPad(TArray<FAmbientPad> pads);

	FAmbientDecorator SelectRandomAmbientDecorator(TArray<FAmbientDecorator> ambients);

	bool IsTrackCurrentlyPlaying(FName track);

	UFUNCTION()
	void OnAllTracksFadedOut();

	UFUNCTION()
	void OnDecoratorFinished(UDecoratorComponent* FinishedTrack);

	//integer constraints for how often music is allowed to play
	const int MusicFreqMin = 0;
	const int MusicFreqMax = 9;

	void SetBeatsBeforeNextTrack_FrequencyChange(int Index);


private:

	void PopulateValidPadArray();
	void PopulateValidDecoratorArray();
	void RemoveIndividualItemFromDecorator(TSoftObjectPtr<USoundBase> DecoratorToRemove);
	void SetTrackFrequency(int InNewFreq);
	void SetBeatsBeforeNextTrack(int Index);
	bool IsDecoratorCurrentlyProhibited(FName decorator);
	void AddProhibitedDecorators(TArray<FName> decorators);
	void RemoveProhibitedDecorators(TArray<FName> decorators);
	void PushSoundMixes(FName InDecoratorName, TArray<USoundMix*> InSoundMixes);
	void PopSoundMixes(FName InDecoratorName);
	void PopAllSoundMixes();

};
