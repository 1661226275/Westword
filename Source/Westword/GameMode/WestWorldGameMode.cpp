// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/WestWorldGameMode.h"
#include "Character/CowBoyCharacter.h"
#include "PlayerController/CowBoyPlayerController.h"
#include "PlayerState/CowBoyPlayerState.h"
#include "GameState/WestWorldGameState.h"

namespace MatchState
{
	const FName Cooldown = FName(TEXT("Cooldown"));
}

AWestWorldGameMode::AWestWorldGameMode()
{
	bDelayedStart = true;
}



void AWestWorldGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AWestWorldGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();
	for(FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator();It;++It)
	{
		ACowBoyPlayerController* CowBoyPlayerController = Cast<ACowBoyPlayerController>(It->Get());
		if(CowBoyPlayerController)
		{
			CowBoyPlayerController->OnMatchStateSet(MatchState);
		}
	}
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
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MarchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if(CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	
	
}

float AWestWorldGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void AWestWorldGameMode::PlayerEliminated(ACowBoyCharacter* ElimmedCharacter, ACowBoyPlayerController* VictimController, AController* AttackController)
{
	ACowBoyPlayerState* VictimPlayerState = VictimController ? Cast<ACowBoyPlayerState>(VictimController->PlayerState) : nullptr;
	ACowBoyPlayerState* AttackerPlayerState = AttackController ? Cast<ACowBoyPlayerState>(AttackController->PlayerState) : nullptr;
	if (AttackerPlayerState && VictimPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
		AttackerPlayerState->KillHunterNum = AttackerPlayerState->KillHunterNum + 1.f;
		AttackerPlayerState->Bounty += VictimPlayerState->Bounty;
		AttackerPlayerState->CrystalShard += VictimPlayerState->CrystalShard;
		AttackerPlayerState->Crystal += VictimPlayerState->Crystal;
		VictimPlayerState->Bounty = 0.f;
		VictimPlayerState->CrystalShard = 0.f;
		VictimPlayerState->Crystal = 0.f;

	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}
	//ACowBoyPlayerController* CowBoyAttackController = Cast<ACowBoyPlayerController>(AttackController);
	//if (CowBoyAttackController)
	//{
	//	//来自玩家的攻击
	//	if (ElimmedCharacter)
	//	{
	//		ElimmedCharacter->Elim(false);
	//	}
	//}
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ACowBoyPlayerController* CowBoyPlayerController = Cast<ACowBoyPlayerController>(*It);
		if (CowBoyPlayerController && AttackerPlayerState && VictimPlayerState)
		{
			CowBoyPlayerController->BroadCastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

void AWestWorldGameMode::EnemyEliminated(AActor* ElimmedEnemy, AController* VictimController, AController* AttackController,bool bIsBOSS)
{
	ACowBoyPlayerState* AttackerPlayerState = AttackController ? Cast<ACowBoyPlayerState>(AttackController->PlayerState) : nullptr;
	if (AttackerPlayerState)
	{
		if (bIsBOSS)
		{
			AttackerPlayerState->KillBossNum = AttackerPlayerState->KillBossNum + 1.f;
			AttackerPlayerState->Bounty += 1000.f;
			AttackerPlayerState->CrystalShard += 200.f;
			AttackerPlayerState->Crystal += 1.f;
		}
		else
		{
			AttackerPlayerState->KillMonsterNum++;
			AttackerPlayerState->Bounty += 100.f;
			AttackerPlayerState->CrystalShard += 20.f;
			AttackerPlayerState->Crystal += 0.f;
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

void AWestWorldGameMode::PlayerLeftGame(ACowBoyPlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	AWestWorldGameState* WestWorldGameState = GetGameState<AWestWorldGameState>();
	ACowBoyCharacter*CharacterLeaving = Cast<ACowBoyCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}


