// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/CowBoyPlayerController.h"
#include "HUD/CowBoyHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/CowBoyCharacter.h"


void ACowBoyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	CowboyHUD = Cast<ACowBoyHUD>(GetHUD());
}

void ACowBoyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);

}

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

void ACowBoyPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		CowboyHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::AsNumber(Ammo));
	}
}

void ACowBoyPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = FMath::CeilToInt(CountdownTime) - (Minutes * 60);
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		CowboyHUD->CharacterOverlay->MatchCountdown->SetText(FText::FromString(CountdownText));
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




void ACowBoyPlayerController::SetHUDTime()
{
	uint32 SecondsLeft = FMath::CeilToInt(MatchTime-GetServerTime());
	if(CountdownInt != SecondsLeft)
	{
		SetHUDMatchCountdown(SecondsLeft);
	}
	CountdownInt = SecondsLeft;
}

void ACowBoyPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ACowBoyPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

void ACowBoyPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	ClientReportServerTime(TimeOfClientRequest, GetWorld()->GetTimeSeconds());
}

float ACowBoyPlayerController::GetServerTime()
{
	if(HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

void ACowBoyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}
