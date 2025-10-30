// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/WestWorldGameState.h" 
#include "Net/UnrealNetwork.h"
#include "PlayerState/CowBoyPlayerState.h"

void AWestWorldGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWestWorldGameState, RedTeamScore);
	DOREPLIFETIME(AWestWorldGameState, BlueTeamScore);
}

void AWestWorldGameState::RedTeamScores()
{
	++RedTeamScore;
}

void AWestWorldGameState::BlueTeamScores()
{
	++BlueTeamScore;
}

void AWestWorldGameState::OnRep_RedTeamScore()
{
}

void AWestWorldGameState::OnRep_BlueTeamScore()
{
}
