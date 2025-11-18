// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/CowBoyPlayerState.h"
#include "Character/CowBoyCharacter.h"
#include "PlayerController/CowBoyPlayerController.h"
#include "GameMode/TeamsGameMode.h"

void ACowBoyPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ACowBoyCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}

}

void ACowBoyPlayerState::AddToScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
	Character = Character == nullptr ? Cast<ACowBoyCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ACowBoyPlayerController>(Character->GetController()) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ACowBoyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACowBoyPlayerState, Team);
}

void ACowBoyPlayerState::SetCowBoyCharacter(ACowBoyCharacter* NewCharacter)
{
	Character = NewCharacter;

	if (Character)
	{
		// 广播委托通知Character已设置
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Character Set in PlayerState")));
		OnCharacterSet.Broadcast(Character);
	}
}

void ACowBoyPlayerState::SetCowBoyPlayerController(ACowBoyPlayerController* NewController)
{
	Controller = NewController;
}
