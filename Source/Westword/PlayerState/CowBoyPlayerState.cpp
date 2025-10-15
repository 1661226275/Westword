// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/CowBoyPlayerState.h"
#include "Character/CowBoyCharacter.h"
#include "PlayerController/CowBoyPlayerController.h"

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
