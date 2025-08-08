// Fill out your copyright notice in the Description page of Project Settings.


#include "AmbientMusicTrackComponent.h"


UAmbientMusicTrackComponent::UAmbientMusicTrackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}



void UAmbientMusicTrackComponent::BeginPlay()
{
	Super::BeginPlay();



}



void UAmbientMusicTrackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UAmbientMusicTrackComponent::FadeTrackOut()
{	
	bTrackFadingOut = true;

	//if any of the decorator components are valid, instruct them to fade out
	if (PadComponent)
	{
		PadComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
		PadComponent->FadeDecoratorOut();
	}
	if (PrimaryDecoratorComponent)
	{
		PrimaryDecoratorComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
		PrimaryDecoratorComponent->FadeDecoratorOut();
	}
	if (SecondaryDecoratorComponent)
	{
		SecondaryDecoratorComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
		SecondaryDecoratorComponent->FadeDecoratorOut();
	}

	//set all decorator pointers to nullptr
	PadComponent = nullptr;
	PrimaryDecoratorComponent = nullptr;
	SecondaryDecoratorComponent = nullptr;

	//destroy this component after handling all spawned components
	OnAllTracksFadedOut();

}

void UAmbientMusicTrackComponent::PlayTrack()
{
	//check if can play new pad
	if (BeatsBeforeNextTrack[0] <= 0 && PadComponent == nullptr)
	{
		StartNewPad(ValidPads);
	}
	if(PadComponent == nullptr)
	{
		BeatsBeforeNextTrack[0]--;
	}

	//check if can play new primary decorator
	if (BeatsBeforeNextTrack[1] <= 0 && PrimaryDecoratorComponent == nullptr)
	{
		StartNewDecorator(ValidDecorators, true);
	}
	if(PrimaryDecoratorComponent == nullptr)
	{
		BeatsBeforeNextTrack[1]--;
	}

	//check if can play new secondary decorator
	if (BeatsBeforeNextTrack[2] <= 0 && SecondaryDecoratorComponent == nullptr)
	{
		StartNewDecorator(ValidDecorators, false);
	}
	if(SecondaryDecoratorComponent == nullptr)
	{
		BeatsBeforeNextTrack[2]--;
	}

}

void UAmbientMusicTrackComponent::SetupMusicComponent(FAmbientMusicTrack TrackData)
{
	if (!TrackData.Pads.IsEmpty())
	{
		//Set fade in/out settings
		bShouldFadeIn = TrackData.FadeSettings.bShouldFadeIn;
		FadeInDuration = TrackData.FadeSettings.FadeInDuration;
		bShouldFadeOut = TrackData.FadeSettings.bShouldFadeOut;
		FadeOutDuration = TrackData.FadeSettings.FadeOutDuration;
		DecoratorClock =TrackName = TrackData.TrackName;

		UWorld* world = GetWorld();
		QuartzSubsystem = world->GetSubsystem<UQuartzSubsystem>();
		
		if (!QuartzSubsystem)
		{
			UE_LOG(LogTemp, Error, TEXT("Quartz clock failed to initialize, destroying component"));
			DestroyComponent();
		}

		ThisTrack = TrackData;

		PopulateValidPadArray();
		PopulateValidDecoratorArray();

		SetupQuartzClock(TrackData.BeatsPerMinute);

	}

}


void UAmbientMusicTrackComponent::SetupQuartzClock_Implementation(float BeatsPerMinute)
{
	//overridden in blueprint
}

void UAmbientMusicTrackComponent::StartNewPad(TArray<FAmbientPad> pads)
{
	if (!bTrackFadingOut)
	{
		FAmbientPad newPad = SelectRandomAmbientPad(pads);

		if (newPad.PadName != "" && newPad.PadName.ToString().Len() > 0)
		{
			PadComponent = NewObject<UDecoratorComponent>(this, UDecoratorComponent::StaticClass(), FName("PadDecorator"), RF_Transient);
			PadComponent->RegisterComponent();
			PadComponent->OnDecoratorFinished.AddDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
			SetBeatsBeforeNextTrack(0);
			PadComponent->SetupDecoratorComponent(newPad.Track,
				newPad.LoopSettings.bShouldLoop,
				newPad.LoopSettings.MinNumLoops,
				newPad.LoopSettings.MaxNumLoops,
				nullptr,
				FadeInDuration,
				FadeOutDuration,
				newPad.PadName,
				TArray<FName>{}
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Pad track collision, skipping creating new pad (not sure that's possible, but juuuuust in case."));
		}
	}
	

}

void UAmbientMusicTrackComponent::OnDecoratorFinished(UDecoratorComponent* FinishedTrack)
{
	if (FinishedTrack)
	{
		RemoveProhibitedDecorators(FinishedTrack->ProhibitedDecorators);

		if (PadComponent && FinishedTrack->DecoratorName == PadComponent->DecoratorName)
		{
			PadComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
			PadComponent = nullptr;
		}
		else if (PrimaryDecoratorComponent && FinishedTrack->DecoratorName == PrimaryDecoratorComponent->DecoratorName)
		{
			PrimaryDecoratorComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
			PrimaryDecoratorComponent = nullptr;
		}
		else if (SecondaryDecoratorComponent && FinishedTrack->DecoratorName == SecondaryDecoratorComponent->DecoratorName)
		{
			SecondaryDecoratorComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
			SecondaryDecoratorComponent = nullptr;
		}

		FinishedTrack->DestroyComponent();

	}

}

void UAmbientMusicTrackComponent::StartNewDecorator(TArray<FAmbientDecorator> decorators, bool PrimaryDecorator)
{
	if (!bTrackFadingOut)
	{
		FAmbientDecorator newDecoratorWrapper = SelectRandomAmbientDecorator(decorators);

		if (newDecoratorWrapper.Decorator.Num() > 0)
		{
			if (PrimaryDecorator)
			{
				PrimaryDecoratorComponent = NewObject<UDecoratorComponent>(this, UDecoratorComponent::StaticClass(), FName("PrimaryDecorator"), RF_Transient);
				PrimaryDecoratorComponent->OnDecoratorFinished.AddDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
				SetBeatsBeforeNextTrack(1);
				PrimaryDecoratorComponent->RegisterComponent();
				AddProhibitedDecorators(newDecoratorWrapper.ProhibitedDecorators);

				TSoftObjectPtr<USoundBase> IndividualDecorator = SelectRandomTrack(newDecoratorWrapper.Decorator);
				RemoveIndividualItemFromDecorator(IndividualDecorator);

				PrimaryDecoratorComponent->SetupDecoratorComponent(IndividualDecorator,
					newDecoratorWrapper.LoopSettings.bShouldLoop,
					newDecoratorWrapper.LoopSettings.MinNumLoops,
					newDecoratorWrapper.LoopSettings.MaxNumLoops,
					newDecoratorWrapper.DecoratorOut,
					FadeInDuration,
					FadeOutDuration,
					newDecoratorWrapper.DecoratorName,
					newDecoratorWrapper.ProhibitedDecorators
				);
			}
			else
			{
				SecondaryDecoratorComponent = NewObject<UDecoratorComponent>(this, UDecoratorComponent::StaticClass(), FName("SecondaryDecorator"), RF_Transient);

				SetBeatsBeforeNextTrack(2);
				SecondaryDecoratorComponent->RegisterComponent();
				AddProhibitedDecorators(newDecoratorWrapper.ProhibitedDecorators);
				SecondaryDecoratorComponent->OnDecoratorFinished.AddDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
				TSoftObjectPtr<USoundBase> IndividualDecorator = SelectRandomTrack(newDecoratorWrapper.Decorator);
				RemoveIndividualItemFromDecorator(IndividualDecorator);

				SecondaryDecoratorComponent->SetupDecoratorComponent(IndividualDecorator,
					newDecoratorWrapper.LoopSettings.bShouldLoop,
					newDecoratorWrapper.LoopSettings.MinNumLoops,
					newDecoratorWrapper.LoopSettings.MaxNumLoops,
					newDecoratorWrapper.DecoratorOut,
					FadeInDuration,
					FadeOutDuration,
					newDecoratorWrapper.DecoratorName,
					newDecoratorWrapper.ProhibitedDecorators
				);

			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Decorator track collision, skipping creating new decorator."));
		}
	}
	
}

TSoftObjectPtr<USoundBase> UAmbientMusicTrackComponent::SelectRandomTrack(TArray<TSoftObjectPtr<USoundBase>> tracks)
{
	TSoftObjectPtr<USoundBase> SelectedTrack = nullptr;

	if (tracks.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Unable to select random track, empty array."));
	}

	if (tracks.Num() == 1)
	{
		SelectedTrack = tracks[0];
	}
	else
	{
		int index = FMath::RandRange(0, tracks.Num() - 1);
		SelectedTrack = tracks[index];
	}

	return SelectedTrack;

}

FAmbientPad UAmbientMusicTrackComponent::SelectRandomAmbientPad(TArray<FAmbientPad> pads)
{
	//if the array of valid pads is empty, repopulate it
	if (ValidPads.Num() <= 0)
	{
		PopulateValidPadArray();
	}

	FAmbientPad SelectedPad = ValidPads[FMath::RandRange(0, ValidPads.Num() - 1)];

	//remove the selected pad from the valid pads array
	ValidPads.Remove(SelectedPad);

	//return a random pad from the array of remaining valid pads
	return SelectedPad;

}

FAmbientDecorator UAmbientMusicTrackComponent::SelectRandomAmbientDecorator(TArray<FAmbientDecorator> decorators)
{
	
	if (decorators.Num() <= 0)
	{
		PopulateValidDecoratorArray();
	}

	TArray<FAmbientDecorator> TempValidDecorators;

	//if the decorator type is not playing and there are still valid decorators, add to temp valid array
	for (const FAmbientDecorator& decorator : decorators)
	{
		if ((!IsTrackCurrentlyPlaying(decorator.DecoratorName) && decorator.Decorator.Num() > 0) && !IsDecoratorCurrentlyProhibited(decorator.DecoratorName))
		{
			TempValidDecorators.Add(decorator);
		}
	}

	// select random pad
	int index = FMath::RandRange(0, TempValidDecorators.Num() - 1);

	return TempValidDecorators[index];
}

bool UAmbientMusicTrackComponent::IsTrackCurrentlyPlaying(FName track)
{
	if (ValidDecorators.Num() > 1)
	{
		return (PadComponent && PadComponent->DecoratorName == track) ||
			(PrimaryDecoratorComponent && PrimaryDecoratorComponent->DecoratorName == track) ||
			(SecondaryDecoratorComponent && SecondaryDecoratorComponent->DecoratorName == track);
	}
	else
	{
		return false;
	}
	
}

bool UAmbientMusicTrackComponent::IsDecoratorCurrentlyProhibited(FName decorator)
{
	return ProhibitiedDecorators.Contains(decorator);
}


void UAmbientMusicTrackComponent::AddProhibitedDecorators(TArray<FName> decorators)
{
	for (FName decorator : decorators)
	{
		ProhibitiedDecorators.AddUnique(decorator);
	}
}

void UAmbientMusicTrackComponent::RemoveProhibitedDecorators(TArray<FName> decorators)
{
	for (FName decorator : decorators)
	{
		ProhibitiedDecorators.Remove(decorator);
	}
}

void UAmbientMusicTrackComponent::OnAllTracksFadedOut()
{
	//make sure all decorator components are nullptrs before destroying, then broadcast that this components is finished
	if (!PadComponent && !PrimaryDecoratorComponent && !SecondaryDecoratorComponent)
	{
		OnTrackEnd.Broadcast(TrackName, NextTrack);
		DestroyComponent();
	}

}

void UAmbientMusicTrackComponent::PopulateValidPadArray()
{
	ValidPads.Empty();

	for (const FAmbientPad pad : ThisTrack.Pads)
	{
		if (!ValidPads.Contains(pad))
		{
			ValidPads.Add(pad);
		}
		
	}
	
}

void UAmbientMusicTrackComponent::PopulateValidDecoratorArray()
{
	ValidDecorators.Empty();

	for (const FAmbientDecorator decorator : ThisTrack.Decorators)
	{
		if (!ValidDecorators.Contains(decorator))
		{
			ValidDecorators.Add(decorator);
		}
		
	}

}

void UAmbientMusicTrackComponent::RemoveIndividualItemFromDecorator(TSoftObjectPtr<USoundBase> DecoratorToRemove)
{
	
	for (int32 i = 0; i < ValidDecorators.Num(); ++i)
	{
		FAmbientDecorator& Dec = ValidDecorators[i];

		if (Dec.Decorator.Contains(DecoratorToRemove))
		{
			Dec.Decorator.Remove(DecoratorToRemove);

			if (Dec.Decorator.Num() <= 0)
			{
				// Create a temp wrapper with the same name
				FAmbientDecorator Temp;
				Temp.DecoratorName = Dec.DecoratorName;

				const FAmbientDecorator* Original = Algo::Find(ThisTrack.Decorators, Temp);

				if (Original)
				{
					ValidDecorators[i] = *Original;
				}
				else
				{
					ValidDecorators.RemoveAt(i);
					--i; // prevent skipping next item
				}
			}

			break;
		}
	}

}

void UAmbientMusicTrackComponent::SetTrackFrequency(int InNewFreq)
{
	if (InNewFreq >= MusicFreqMin && InNewFreq <= MusicFreqMax)
	{
		CurrentMusicFrequency = InNewFreq;
	}
	else
	{
		//set freq to default of 5 if out of range
		CurrentMusicFrequency = 5;
	}
}

void UAmbientMusicTrackComponent::SetBeatsBeforeNextTrack(int Index)
{
	if (Index == 0)
	{
		BeatsBeforeNextTrack[Index] = FMath::RandRange(MusicFrequencySettings[CurrentMusicFrequency][0], MusicFrequencySettings[CurrentMusicFrequency][1]);
	}
	else
	{
		BeatsBeforeNextTrack[Index] = FMath::RandRange(MusicFrequencySettings[CurrentMusicFrequency][2], MusicFrequencySettings[CurrentMusicFrequency][3]);
	}
	
}


