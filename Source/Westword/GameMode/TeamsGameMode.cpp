// Fill out your copyright notice in the Description page of Project Settings.



#include "TeamsGameMode.h"
#include "GameState/WestWorldGameState.h"
#include "PlayerState/CowBoyPlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATeamsGameMode::Logout(AController* Exiting)
{
	AWestWorldGameState* WGameState = Cast<AWestWorldGameState>(UGameplayStatics::GetGameState(this));
	ACowBoyPlayerState* CPState = Exiting->GetPlayerState<ACowBoyPlayerState>();
	if (WGameState && CPState)
	{
		if (WGameState->RedTeam.Contains(CPState)) WGameState->RedTeam.Remove(CPState);
		if (WGameState->BlueTeam.Contains(CPState)) WGameState->BlueTeam.Remove(CPState);
	}
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
	AWestWorldGameState* WGameState = Cast<AWestWorldGameState>(UGameplayStatics::GetGameState(this));
	if (WGameState)
	{
		for (auto PState : WGameState->PlayerArray)
		{
			ACowBoyPlayerState* CPState = Cast<ACowBoyPlayerState>(PState.Get());
			if (CPState && CPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (WGameState->BlueTeam.Num() >= WGameState->RedTeam.Num())
				{
					WGameState->RedTeam.AddUnique(CPState);
					CPState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					WGameState->BlueTeam.AddUnique(CPState);
					CPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
			
		}
		
	}
}
