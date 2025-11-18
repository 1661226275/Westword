// Fill out your copyright notice in the Description page of Project Settings.



#include "TeamsGameMode.h"
#include "GameState/WestWorldGameState.h"
#include "PlayerState/CowBoyPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Character/CowBoyCharacter.h"
#include "PlayerController/CowBoyPlayerController.h"

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



void ATeamsGameMode::HandleTeammateHealthChanged(ACowBoyCharacter* ChangedCharacter, float NewHealth)
{
	AWestWorldGameState* WGameState = Cast<AWestWorldGameState>(UGameplayStatics::GetGameState(this));
	ACowBoyPlayerState* CPState = ChangedCharacter->GetPlayerState<ACowBoyPlayerState>();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Teammate Health Changed: %f"), NewHealth));
	if (WGameState && CPState)
	{
		if (CPState->GetTeam() == ETeam::ET_RedTeam)
		{
			for (auto TeammatePState : WGameState->RedTeam)
			{
				if (TeammatePState != CPState)
				{
					TeammatePState->GetCowBoyPlayerController()->Client_UpdateTeammateHealth(ChangedCharacter, NewHealth);
				}
			}
		}
		else if (CPState->GetTeam() == ETeam::ET_BlueTeam)
		{
			for (auto TeammatePState : WGameState->BlueTeam)
			{
				if (TeammatePState != CPState)
				{
					TeammatePState->GetCowBoyPlayerController()->Client_UpdateTeammateHealth(ChangedCharacter, NewHealth);
				}
			}
		}
	}
}

void ATeamsGameMode::HandleTeammateSanChanged(ACowBoyCharacter* ChangedCharacter, float NewSan)
{
	AWestWorldGameState* WGameState = Cast<AWestWorldGameState>(UGameplayStatics::GetGameState(this));
	ACowBoyPlayerState* CPState = ChangedCharacter->GetPlayerState<ACowBoyPlayerState>();
	
	if (WGameState && CPState)
	{
		if (CPState->GetTeam() == ETeam::ET_RedTeam)
		{
			for (auto TeammatePState : WGameState->RedTeam)
			{
				if (TeammatePState != CPState)
				{
					TeammatePState->GetCowBoyPlayerController()->Client_UpdateTeammateSan(ChangedCharacter, NewSan);
				}
			}
		}
		else if (CPState->GetTeam() == ETeam::ET_BlueTeam)
		{
			for (auto TeammatePState : WGameState->BlueTeam)
			{
				if (TeammatePState != CPState)
				{
					TeammatePState->GetCowBoyPlayerController()->Client_UpdateTeammateSan(ChangedCharacter, NewSan);
				}
			}
		}
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
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Assigning Teams for Player: %s"), *CPState->GetPlayerName()));
			CPState->GetCowBoyCharacter()->OnHealthChanged.AddDynamic(this, &ATeamsGameMode::HandleTeammateHealthChanged);
			CPState->GetCowBoyCharacter()->OnSansChanged.AddDynamic(this, &ATeamsGameMode::HandleTeammateSanChanged);
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

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	ACowBoyPlayerState* AttackerPState = Attacker->GetPlayerState<ACowBoyPlayerState>();
	ACowBoyPlayerState* VictimPState = Victim->GetPlayerState<ACowBoyPlayerState>();
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (VictimPState == AttackerPState)
	{
		return BaseDamage;
	}
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0.f;
	}
	return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(ACowBoyCharacter* ElimmedCharacter, ACowBoyPlayerController* VictimController, AController* AttackController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackController);
}
