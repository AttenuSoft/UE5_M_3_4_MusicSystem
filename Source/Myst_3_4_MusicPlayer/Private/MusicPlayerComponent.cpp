// Fill out your copyright notice in the Description page of Project Settings.


#include "MusicPlayerComponent.h"


UMusicPlayerComponent::UMusicPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}



void UMusicPlayerComponent::BeginPlay()
{
	Super::BeginPlay();


	
}



void UMusicPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}



UFUNCTION(BlueprintCallable)
void UMusicPlayerComponent::CleanUpActiveMusicTracks() 
{
	TArray<FName> KeysToRemove;

	for (const auto& track : ActiveMusicTracks)
	{
		if (track.Value == nullptr)
		{
			KeysToRemove.Add(track.Key);
		}
	}

	for (const FName& Key : KeysToRemove)
	{
		ActiveMusicTracks.Remove(Key);
	}
}

void UMusicPlayerComponent::SetupMusicData(UMusicDataAsset* newMusicData)
{
	if (CurrentMusicData == nullptr && newMusicData != CurrentMusicData)
	{
		CurrentMusicData = newMusicData;
	}
}

void UMusicPlayerComponent::ClearMusicData()
{
	CurrentMusicData = nullptr;
}

void UMusicPlayerComponent::CreateNewMusicTrack(FName InTrackName)
{
	//ensure the music data is valid
	if (CurrentMusicData && !IsActiveMusicTrack(InTrackName))
	{
		//check to see if the single music track map contains the given track name
		if (CurrentMusicData->SingleMusicTracks.Contains(InTrackName))
		{
		
			UClass* BPClass = StaticLoadClass(
				USingleMusicTrackComponent::StaticClass(),
				nullptr,
				TEXT("/Game/Blueprints/Audio/BP_SingleMusicTrackComponent.BP_SingleMusicTrackComponent_C")
			);

			USingleMusicTrackComponent* NewTrackComponent = NewObject<USingleMusicTrackComponent>(this, BPClass);
			
			//register component
			NewTrackComponent->RegisterComponent();

			//attach new component to the music player component
			//NewTrackComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

			//add new component to the array of active track components
			//ActiveMusicTracks.AddUnique(NewTrackComponent);
			ActiveMusicTracks.Add(InTrackName, NewTrackComponent);

			//store new track data locally for new music track component
			FSingleMusicTrack* NewTrackData = CurrentMusicData->SingleMusicTracks.Find(InTrackName);

			//setup new music track component
			NewTrackComponent->SetupMusicComponent(*NewTrackData);

			//bind to on track ended event
			NewTrackComponent->OnTrackEnd.AddDynamic(this, &UMusicPlayerComponent::OnMusicComponentTrackEnded);

		}
		//otherwise, check if the ambient music track map contains the given track name
		else if (CurrentMusicData->AmbientMusicTracks.Contains(InTrackName))
		{
			UClass* BPClass = StaticLoadClass(
				UAmbientMusicTrackComponent::StaticClass(),
				nullptr,
				TEXT("/Game/Blueprints/Audio/BP_AmbientMusicTrackComponent.BP_AmbientMusicTrackComponent_C")
			);

			UAmbientMusicTrackComponent* NewTrackComponent = NewObject<UAmbientMusicTrackComponent>(this, BPClass);

			NewTrackComponent->RegisterComponent();
			//NewTrackComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
			//ActiveMusicTracks.AddUnique(NewTrackComponent);
			ActiveMusicTracks.Add(InTrackName, NewTrackComponent);

			FAmbientMusicTrack* NewTrackData = CurrentMusicData->AmbientMusicTracks.Find(InTrackName);
			NewTrackComponent->SetupMusicComponent(*NewTrackData);
			NewTrackComponent->OnTrackEnd.AddDynamic(this, &UMusicPlayerComponent::OnMusicComponentTrackEnded);
		}
		//track name not found, log error message
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No track by name %s was found in either TMap."), *InTrackName.ToString());
		}
		
	}
	
}

void UMusicPlayerComponent::OnMusicComponentTrackEnded(FName CurrentTrack, FName NextTrack)
{
	

	ActiveMusicTracks.Remove(CurrentTrack);	

	if (NextTrack != "None" && NextTrack != "")
	{
		CreateNewMusicTrack(NextTrack);
	}

}

void UMusicPlayerComponent::StopSpecificMusicTrack(FName TrackToStop)
{
	if (TrackToStop != "" && TrackToStop != "None" && ActiveMusicTracks.Contains(TrackToStop))
	{
		UMusicTrackComponent* track = ActiveMusicTracks.FindChecked(TrackToStop);
		track->bProceedToNextTrack = false;
		ActiveMusicTracks.Remove(TrackToStop);
		track->FadeTrackOut();
	}
}

void UMusicPlayerComponent::StopAllMusicTracks()
{
	for (auto trackIterator = ActiveMusicTracks.CreateConstIterator(); trackIterator; ++trackIterator)
	{
		const FName key = trackIterator.Key();
		UMusicTrackComponent* track = trackIterator.Value();
		track->bProceedToNextTrack = false;
		track->FadeTrackOut();
		ActiveMusicTracks.Remove(key);
	}
}

bool UMusicPlayerComponent::IsActiveMusicTrack(FName InMusicTrack)
{
	return ActiveMusicTracks.Contains(InMusicTrack);
}

