// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/WestWorldGameMode.h"
#include "Character/CowBoyCharacter.h"
#include "PlayerController/CowBoyPlayerController.h"
#include "PlayerState/CowBoyPlayerState.h"

AWestWorldGameMode::AWestWorldGameMode()
{
	bDelayedStart = true;
}

void AWestWorldGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AWestWorldGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
	
}

void AWestWorldGameMode::PlayerEliminated(ACowBoyCharacter* ElimmedCharacter, ACowBoyPlayerController* VictimController, AController* AttackController)
{
	ACowBoyPlayerState* VictimPlayerState = VictimController ? Cast<ACowBoyPlayerState>(VictimController->PlayerState) : nullptr;
	ACowBoyPlayerState* AttackerPlayerState = AttackController ? Cast<ACowBoyPlayerState>(AttackController->PlayerState) : nullptr;
	if (VictimPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}

	ACowBoyPlayerController* CowBoyAttackController = Cast<ACowBoyPlayerController>(AttackController);
	if (CowBoyAttackController)
	{
		//来自玩家的攻击
		if (ElimmedCharacter)
		{
			ElimmedCharacter->Elim();
		}
	}
}

void AWestWorldGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if(ElimmedCharacter)
	{
		ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(ElimmedCharacter);
		CowBoyCharacter->MulticastRespawn();

	}
}


