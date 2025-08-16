// Fill out your copyright notice in the Description page of Project Settings.


#include "AmbientMusicTrackComponent.h"


UAmbientMusicTrackComponent::UAmbientMusicTrackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}



void UAmbientMusicTrackComponent::BeginPlay()
{
	Super::BeginPlay();

	QuantizationBoundary.Quantization = EQuartzCommandQuantization::Beat;					 //Could be Bar, HalfNote, QuarterNote, etc.
	QuantizationBoundary.Multiplier = 1.0f;													 //How many of that quantization unit to wait
	QuantizationBoundary.CountingReferencePoint = EQuarztQuantizationReference::Count;	     

}



void UAmbientMusicTrackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UAmbientMusicTrackComponent::FadeTrackOut()
{	
	bTrackFadingOut = true;

	PopAllSoundMixes();

	//if any of the decorator components are valid, instruct them to fade out
	if (PadComponent)
	{
		PadComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
		PadComponent->FadeOutDuration = ThisTrack.FadeSettings.FadeOutDuration;
		PadComponent->FadeDecoratorOut();
		PadComponent = nullptr;
	}
	if (PrimaryDecoratorComponent)
	{
		PrimaryDecoratorComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
		PrimaryDecoratorComponent->FadeOutDuration = ThisTrack.FadeSettings.FadeOutDuration;
		PrimaryDecoratorComponent->FadeDecoratorOut();
		PrimaryDecoratorComponent = nullptr;
	}
	if (SecondaryDecoratorComponent)
	{
		SecondaryDecoratorComponent->OnDecoratorFinished.RemoveDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
		SecondaryDecoratorComponent->FadeOutDuration = ThisTrack.FadeSettings.FadeOutDuration;
		SecondaryDecoratorComponent->FadeDecoratorOut();
		SecondaryDecoratorComponent = nullptr;
	}

	//destroy this component after handling all spawned components
	OnAllTracksFadedOut();

}

void UAmbientMusicTrackComponent::PlayTrack()
{
	//check if can play new pad
	if (BeatsBeforeNextTrack[0] <= 0 && PadComponent == nullptr)
	{
		StartNewPad(ValidPads);
		BeatsSinceLastTrack[0] = 0;
	}
	else if(PadComponent == nullptr)
	{
		BeatsBeforeNextTrack[0]--;
		BeatsSinceLastTrack[0]++;
	}

	//check if can play new primary decorator
	if (BeatsBeforeNextTrack[1] <= 0 && PrimaryDecoratorComponent == nullptr)
	{
		StartNewDecorator(ValidDecorators, true);
		BeatsSinceLastTrack[1] = 0;
	}
	else if(PrimaryDecoratorComponent == nullptr)
	{
		BeatsBeforeNextTrack[1]--;
		BeatsSinceLastTrack[1]++;
	}

	//check if can play new secondary decorator
	if (BeatsBeforeNextTrack[2] <= 0 && SecondaryDecoratorComponent == nullptr)
	{
		StartNewDecorator(ValidDecorators, false);
		BeatsSinceLastTrack[2] = 0;
	}
	else if(SecondaryDecoratorComponent == nullptr)
	{
		BeatsBeforeNextTrack[2]--;
		BeatsSinceLastTrack[2]++;
	}

}

void UAmbientMusicTrackComponent::SetupMusicComponent(FAmbientMusicTrack TrackData)
{
	if (!TrackData.Pads.IsEmpty())
	{
		//Set fade out settings - ambient track as a whole cannot fade in, only out
		DecoratorClock = TrackName = TrackData.TrackName;
		FadeOutDuration = TrackData.FadeSettings.FadeOutDuration;

		UWorld* world = GetWorld();
		QuartzSubsystem = world->GetSubsystem<UQuartzSubsystem>();
		
		if (!QuartzSubsystem)
		{
			UE_LOG(LogTemp, Error, TEXT("Quartz clock failed to initialize, destroying component"));
			DestroyComponent();
		}

		ThisTrack = TrackData;

		BeatsBeforeNextTrack[0] = ThisTrack.StartingDelay_Pad;
		BeatsBeforeNextTrack[1] = ThisTrack.StartingDelay_PrimaryDecorator;
		BeatsBeforeNextTrack[2] = ThisTrack.StartingDelay_SecondaryDecorator;

		PopulateValidPadArray();
		PopulateValidDecoratorArray();

		//setup Quartz clock, done in Blueprint child  class
		SetupQuartzClock(TrackData.BeatsPerMinute);

	}

}

void UAmbientMusicTrackComponent::OnDecoratorFinished(UDecoratorComponent* FinishedTrack)
{
	if (FinishedTrack)
	{
		RemoveProhibitedDecorators(FinishedTrack->ProhibitedDecorators);
		PopSoundMixes(FinishedTrack->DecoratorName);

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
				newPad.FadeSettings.FadeInDuration,
				newPad.FadeSettings.FadeOutDuration,
				newPad.PadName,
				newPad.ProhibitedDecorators,
				newPad.Volume,
				QuantizationBoundary,
				DecoratorClockHandle,
				QuartzSubsystem
			);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Pad track collision, skipping creating new pad (not sure that's possible, but juuuuust in case."));
		}
	}
	

}

void UAmbientMusicTrackComponent::StartNewDecorator(TArray<FAmbientDecorator> decorators, bool PrimaryDecorator)
{
	if (!bTrackFadingOut)
	{
		FAmbientDecorator newDecorator = SelectRandomAmbientDecorator(decorators);

		if (newDecorator.Decorator.Num() > 0)
		{
			if (PrimaryDecorator)
			{
				PrimaryDecoratorComponent = NewObject<UDecoratorComponent>(this, UDecoratorComponent::StaticClass(), FName("PrimaryDecorator"), RF_Transient);
				PrimaryDecoratorComponent->OnDecoratorFinished.AddDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);
				SetBeatsBeforeNextTrack(1);
				PrimaryDecoratorComponent->RegisterComponent();
				AddProhibitedDecorators(newDecorator.ProhibitedDecorators);

				PushSoundMixes(newDecorator.DecoratorName, newDecorator.SoundMixes);

				TSoftObjectPtr<USoundBase> IndividualDecorator = SelectTrack(newDecorator.Decorator, newDecorator.bShouldPlayInOrder);
				RemoveIndividualItemFromDecorator(IndividualDecorator);

				PrimaryDecoratorComponent->SetupDecoratorComponent(IndividualDecorator,
					newDecorator.LoopSettings.bShouldLoop,
					newDecorator.LoopSettings.MinNumLoops,
					newDecorator.LoopSettings.MaxNumLoops,
					newDecorator.DecoratorOut,
					newDecorator.FadeSettings.FadeInDuration,
					newDecorator.FadeSettings.FadeOutDuration,
					newDecorator.DecoratorName,
					newDecorator.ProhibitedDecorators,
					newDecorator.Volume,
					QuantizationBoundary,
					DecoratorClockHandle,
					QuartzSubsystem
				);
			}
			else
			{
				SecondaryDecoratorComponent = NewObject<UDecoratorComponent>(this, UDecoratorComponent::StaticClass(), FName("SecondaryDecorator"), RF_Transient);

				SetBeatsBeforeNextTrack(2);
				SecondaryDecoratorComponent->RegisterComponent();
				AddProhibitedDecorators(newDecorator.ProhibitedDecorators);
				SecondaryDecoratorComponent->OnDecoratorFinished.AddDynamic(this, &UAmbientMusicTrackComponent::OnDecoratorFinished);

				PushSoundMixes(newDecorator.DecoratorName, newDecorator.SoundMixes);

				TSoftObjectPtr<USoundBase> IndividualDecorator = SelectTrack(newDecorator.Decorator, newDecorator.bShouldPlayInOrder);
				RemoveIndividualItemFromDecorator(IndividualDecorator);

				SecondaryDecoratorComponent->SetupDecoratorComponent(IndividualDecorator,
					newDecorator.LoopSettings.bShouldLoop,
					newDecorator.LoopSettings.MinNumLoops,
					newDecorator.LoopSettings.MaxNumLoops,
					newDecorator.DecoratorOut,
					newDecorator.FadeSettings.FadeInDuration,
					newDecorator.FadeSettings.FadeOutDuration,
					newDecorator.DecoratorName,
					newDecorator.ProhibitedDecorators,
					newDecorator.Volume,
					QuantizationBoundary,
					DecoratorClockHandle,
					QuartzSubsystem
				);

			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Decorator track collision, skipping creating new decorator."));
		}
	}
	
}

TSoftObjectPtr<USoundBase> UAmbientMusicTrackComponent::SelectTrack(TArray<TSoftObjectPtr<USoundBase>> tracks, bool bInPlayInOrder)
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
		int index = 0;

		if (!bInPlayInOrder)
		{
			index = FMath::RandRange(0, tracks.Num() - 1);
		}

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

	//add prohibited decorators to array
	AddProhibitedDecorators(SelectedPad.ProhibitedDecorators);

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

void UAmbientMusicTrackComponent::PushSoundMixes(FName InDecoratorName, TArray<USoundMix*> InSoundMixes)
{
	if (InSoundMixes.Num() > 0)
	{
		UWorld* World = GetWorld();

		if (World)
		{
			FAudioDeviceHandle AudioDevice = World->GetAudioDevice();

			for (USoundMix* Mix : InSoundMixes)
			{
				AppliedSoundMixes.Add(InDecoratorName, Mix);
				AudioDevice->PushSoundMixModifier(Mix);
			}

		}
	}
	
}

void UAmbientMusicTrackComponent::PopSoundMixes(FName InDecoratorName)
{
	//get world pointer
	UWorld* World = GetWorld();

	//if pointer is valid, pop soundmixes
	if (World)
	{
		FAudioDeviceHandle AudioDevice = World->GetAudioDevice();

		for (auto SoundMixIterator = AppliedSoundMixes.CreateConstIterator(); SoundMixIterator; ++SoundMixIterator)
		{
			//for each key in AppliedSoundMixes TMap
			const FName key = SoundMixIterator.Key();

			//if the decorator name is the same as the key
			if (key == InDecoratorName)
			{
				//get the value of the element in AppliedSoundMixes TMap and pop it from the audio device
				USoundMix* Mix = SoundMixIterator.Value();
				AudioDevice->PopSoundMixModifier(Mix);
				AppliedSoundMixes.Remove(key);
			}

		}

	}

}

void UAmbientMusicTrackComponent::PopAllSoundMixes()
{
	if (PadComponent)
	{
		PopSoundMixes(PadComponent->DecoratorName);
	}
	if (PrimaryDecoratorComponent)
	{
		PopSoundMixes(PrimaryDecoratorComponent->DecoratorName);
	}
	if (SecondaryDecoratorComponent)
	{
		PopSoundMixes(SecondaryDecoratorComponent->DecoratorName);
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
		BeatsBeforeNextTrack[Index] = FMath::RandRange(ThisTrack.FrequencySettings[CurrentMusicFrequency].MinDelayBeforeNext_Pad, 
			ThisTrack.FrequencySettings[CurrentMusicFrequency].MaxDelayBeforeNext_Pad);
	}
	else
	{
		BeatsBeforeNextTrack[Index] = FMath::RandRange(ThisTrack.FrequencySettings[CurrentMusicFrequency].MinDelayBeforeNext_Decorator, 
			ThisTrack.FrequencySettings[CurrentMusicFrequency].MaxDelayBeforeNext_Decorator);
	}
	
}

void UAmbientMusicTrackComponent::SetBeatsBeforeNextTrack_FrequencyChange(int Index)
{
	int NewBeats = 0;

	if (Index == 0)
	{
		NewBeats = FMath::RandRange(ThisTrack.FrequencySettings[CurrentMusicFrequency].MinDelayBeforeNext_Pad,
			ThisTrack.FrequencySettings[CurrentMusicFrequency].MaxDelayBeforeNext_Pad);
	}
	else
	{
		NewBeats = FMath::RandRange(ThisTrack.FrequencySettings[CurrentMusicFrequency].MinDelayBeforeNext_Decorator,
			ThisTrack.FrequencySettings[CurrentMusicFrequency].MaxDelayBeforeNext_Decorator);
	}

	NewBeats -= BeatsSinceLastTrack[Index];

	if (NewBeats < 0)
	{
		NewBeats = 0;
	}

	BeatsBeforeNextTrack[Index] = NewBeats;


}


