// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "DecoratorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYST_3_4_MUSICPLAYER_API UDecoratorComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	
	UDecoratorComponent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecoratorFinished, UDecoratorComponent*, FinishedTrack);


	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrackLoopFinished);
	
	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnDecoratorFinished OnDecoratorFinished;

	UPROPERTY(BlueprintAssignable, Category = "Callbacks")
	FOnTrackLoopFinished OnLoopFinished;

	UPROPERTY(BlueprintReadOnly)
	FName DecoratorName;

	UPROPERTY(BlueprintReadOnly)
	TArray<FName> ProhibitedDecorators;

	UAudioComponent* PrimaryAudioComponent;
	TSoftObjectPtr<USoundBase> LoopOutTrack;
	TSoftObjectPtr<USoundBase> DecoratorTrack;

	bool bIsLooping = false;
	bool bHasLoopOut = false;
	int MaxLoopCount = 0;
	int NumTimesLooped = 0;
	bool bLoopOutBound = false;
	float FadeInDuration = 0;
	float FadeOutDuration = 0;


protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void SetupDecoratorComponent(TSoftObjectPtr<USoundBase> track, bool bLooping, int minLoops, int maxLoops, TSoftObjectPtr<USoundBase> LoopOut, float InFadeInDuration, float InFadeOutDuration, FName InDecoratorName, TArray<FName> InProhibitedDecorators);

	UFUNCTION()
	void FadeDecoratorOut();

	UFUNCTION()
	void PostLoopFinished();

	UFUNCTION()
	void DecoratorFinished();

	void OnDecoratorTrackLoaded();
		
};
