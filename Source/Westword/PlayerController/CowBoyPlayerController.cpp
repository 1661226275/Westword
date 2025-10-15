// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/CowBoyPlayerController.h"
#include "HUD/CowBoyHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/CowBoyCharacter.h"

void ACowBoyPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		const float HealthPercent = Health / MaxHealth;
		CowboyHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		CowboyHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ACowBoyPlayerController::SetHUDScore(float Score)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		CowboyHUD->CharacterOverlay->ScoreAmount->SetText(FText::AsNumber(FMath::CeilToInt(Score)));
	}
}

void ACowBoyPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		CowboyHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::AsNumber(Ammo));
	}
}

void ACowBoyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(InPawn);
	if (CowBoyCharacter)
	{
		SetHUDHealth(CowBoyCharacter->GetHealth(), CowBoyCharacter->GetMaxHealth());
	}
}

void ACowBoyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CowboyHUD = Cast<ACowBoyHUD>(GetHUD());
}
