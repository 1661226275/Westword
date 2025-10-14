// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/CowBoyPlayerController.h"
#include "HUD/CowBoyHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ACowBoyPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		const float HealthPercent = Health / MaxHealth;
		CowboyHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		CowboyHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void ACowBoyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CowboyHUD = Cast<ACowBoyHUD>(GetHUD());
}
