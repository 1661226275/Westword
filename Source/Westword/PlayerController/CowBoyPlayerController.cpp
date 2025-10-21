// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/CowBoyPlayerController.h"
#include "HUD/CowBoyHUD.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/Announcement.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Character/CowBoyCharacter.h"
#include "GameMode/WestWorldGameMode.h"
#include "Kismet/GameplayStatics.h"

void ACowBoyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ServerCheckMatchState();
	CowboyHUD = Cast<ACowBoyHUD>(GetHUD());
	
}


void ACowBoyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();

}

void ACowBoyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACowBoyPlayerController, MatchState);
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
	else
	{
		bInitializedHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ACowBoyPlayerController::SetHUDSan(float San, float MaxSan)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		const float SanPercent = San / MaxSan;
		CowboyHUD->CharacterOverlay->SansBar->SetPercent(SanPercent);
		FString SansText = FString::Printf(TEXT("%d / %d"), FMath::CeilToInt(San), FMath::CeilToInt(MaxSan));
		CowboyHUD->CharacterOverlay->SansText->SetText(FText::FromString(SansText));
	}
	else
	{
		bInitializedSan = true;
		HUDSan = San;
		HUDMaxSan = MaxSan;
	}
}

void ACowBoyPlayerController::SetHUDScore(float Score)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		CowboyHUD->CharacterOverlay->ScoreAmount->SetText(FText::AsNumber(FMath::CeilToInt(Score)));
	}
	{
		bInitializedScore = true;
		HUDScore = Score;
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

void ACowBoyPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->Announcement)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = FMath::CeilToInt(CountdownTime) - (Minutes * 60);
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		CowboyHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ACowBoyPlayerController::SetRangeWeaponHUDVisible(bool bIsVisible)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		if (bIsVisible)
		{
			CowboyHUD->CharacterOverlay->RangeWeaponCanvas->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CowboyHUD->CharacterOverlay->RangeWeaponCanvas->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
void ACowBoyPlayerController::SetMeleeWeaponHUDVisible(bool bIsVisible)
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD && CowboyHUD->CharacterOverlay)
	{
		if (bIsVisible)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString("Set MeleeHud"));
			CowboyHUD->CharacterOverlay->MeleeWeaponCanvas->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			CowboyHUD->CharacterOverlay->MeleeWeaponCanvas->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
void ACowBoyPlayerController::SetCharacterDeBuffHUD()
{
	if (CowboyHUD)
	{
		CowboyHUD->AddCharacterDeBuffWidget();
	}
}

void ACowBoyPlayerController::DestoryCharacterDeBuffHUD()
{
	if (CowboyHUD)
	{
		CowboyHUD->RemoveCharacterDeBuffWidget();
	}
}

void ACowBoyPlayerController::SetCharacterHurtHUD()
{
}

void ACowBoyPlayerController::DestoryCharacterHurtHUD()
{
}

void ACowBoyPlayerController::AddDamageEffect()
{
	if (CowboyHUD)
	{
		CowboyHUD->AddDamageEffect();
	}
	
}

void ACowBoyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ACowBoyCharacter* CowBoyCharacter = Cast<ACowBoyCharacter>(InPawn);
	if (CowBoyCharacter)
	{
		SetHUDHealth(CowBoyCharacter->GetHealth(), CowBoyCharacter->GetMaxHealth());
		SetHUDSan(CowBoyCharacter->GetSan(), CowBoyCharacter->GetMaxSan());
	}
}




void ACowBoyPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	if(MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - (GetServerTime() - LevelStartingTime);
	}
	else if(MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime+MatchTime - (GetServerTime() - LevelStartingTime);
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if(CountdownInt != SecondsLeft)
	{
		if(MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(SecondsLeft);
		}
		else if(MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(SecondsLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void ACowBoyPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if(CowboyHUD && CowboyHUD->CharacterOverlay)
		{
			CharacterOverlay = CowboyHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializedHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializedSan)SetHUDSan(HUDSan, HUDMaxSan);
				if (bInitializedScore)SetHUDScore(HUDScore);
			}
		}
	}
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





void ACowBoyPlayerController::ServerCheckMatchState_Implementation()
{
	AWestWorldGameMode* WestWorldGameMode = Cast<AWestWorldGameMode>(UGameplayStatics::GetGameMode(this));
	if (WestWorldGameMode)
	{
		WarmupTime = WestWorldGameMode->WarmupTime;
		MatchTime = WestWorldGameMode->MarchTime;
		LevelStartingTime = WestWorldGameMode->LevelStartingTime;
		MatchState = WestWorldGameMode->GetMatchState();
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime);
	}
}


void ACowBoyPlayerController::ClientJoinMidGame_Implementation(FName State, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	MatchState = State;
	OnMatchStateSet(MatchState);
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

void ACowBoyPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State;
	if(MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if(MatchState == MatchState::WaitingToStart)
	{
		HandleWarmup();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
	
	
}



void ACowBoyPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if(MatchState == MatchState::WaitingToStart)
	{
		HandleWarmup();
	}
	else if(MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ACowBoyPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{


	if (CowboyHUD)
	{
		CowboyHUD->AddCharacterOverlay();
		CowboyHUD->AddCharacterHurtHUD();
		if (CowboyHUD->Announcement)
		{
			CowboyHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}

void ACowBoyPlayerController::HandleWarmup()
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;

	if (CowboyHUD)
	{
		CowboyHUD->AddAnnouncement();
	}

}


void ACowBoyPlayerController::HandleCooldown()
{
	CowboyHUD = CowboyHUD == nullptr ? Cast<ACowBoyHUD>(GetHUD()) : CowboyHUD;
	if (CowboyHUD)
	{
		CowboyHUD->CharacterOverlay->RemoveFromParent();
		CowboyHUD->RemoveCharacterHurtHUD();
		CowboyHUD->AddAnnouncement();
		
	}

}
