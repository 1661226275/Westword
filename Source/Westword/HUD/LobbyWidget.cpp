// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LobbyWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "PlayerController/LobbyPlayerController.h"
void ULobbyWidget::SetBounty(float num)
{
	FString BountyText = FString::Printf(TEXT( "ÉÍ½ð£º% d"), FMath::CeilToInt(num));
	Bounty->SetText(FText::FromString(BountyText));
}

void ULobbyWidget::SetCrystalShard(float num)
{
	FString CrystalShardText = FString::Printf(TEXT("ÑªÂö½á¾§ËéÆ¬£º% d"), FMath::CeilToInt(num));
	CrystalShard->SetText(FText::FromString(CrystalShardText));
}

void ULobbyWidget::SetCrystal(float num)
{
	FString CrystalText = FString::Printf(TEXT("ÑªÂö½á¾§ËéÆ¬£º% d"), FMath::CeilToInt(num));
	Crystal->SetText(FText::FromString(CrystalText));
}

void ULobbyWidget::SetUp()
{
	if (CowBoyButton && !CowBoyButton->OnClicked.IsBound())
	{
		CowBoyButton->OnClicked.AddDynamic(this, &ULobbyWidget::CowBoyButtonClicked);
	}
	if (RoundWeaponButton && !RoundWeaponButton->OnClicked.IsBound())
	{
		RoundWeaponButton->OnClicked.AddDynamic(this, &ULobbyWidget::RoundWeaponButtonClicked);
	}
	if (RIderWeaponButton && !RIderWeaponButton->OnClicked.IsBound())
	{
		RIderWeaponButton->OnClicked.AddDynamic(this, &ULobbyWidget::RIderWeaponButtonClicked);
	}
	if (BulletBotton && !BulletBotton->OnClicked.IsBound())
	{
		BulletBotton->OnClicked.AddDynamic(this, &ULobbyWidget::BulletBottonClicked);
	}
	if (EvilBulletButton && !EvilBulletButton->OnClicked.IsBound())
	{
		EvilBulletButton->OnClicked.AddDynamic(this, &ULobbyWidget::EvilBulletButtonClicked);
	}
}

void ULobbyWidget::CowBoyButtonClicked()
{
	if (LastCowBoyCost != 0)
	{
		TotalCrystalShard = TotalCrystalShard + LastCowBoyCost - 50;
	}
	else
	{
		TotalCrystalShard = TotalCrystalShard - 50;
	}
	LastCowBoyCost = 50;
	SetCrystalShard(TotalCrystalShard);
}

void ULobbyWidget::RoundWeaponButtonClicked()
{
	if (LastWeaponCost != 0)
	{
		TotalBounty = TotalBounty + LastWeaponCost - 200;
	}
	else
	{
		TotalBounty = TotalBounty - 200;
	}
	LastWeaponCost = 200;
	SetBounty(TotalBounty);
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->ClientUpdateWeaponDisplay(true);
	}
}

void ULobbyWidget::RIderWeaponButtonClicked()
{
	if (LastWeaponCost != 0)
	{
		TotalBounty = TotalBounty + LastWeaponCost - 500;
	}
	else
	{
		TotalBounty = TotalBounty - 500;
	}
	LastWeaponCost = 500;
	SetBounty(TotalBounty);
	ALobbyPlayerController* PC = Cast<ALobbyPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->ClientUpdateWeaponDisplay(false);
	}
}

void ULobbyWidget::BulletBottonClicked()
{
	if (LastBulletCost != 0)
	{
		TotalBounty = TotalBounty + LastBulletCost - 100;
	}
	else
	{
		TotalBounty = TotalBounty - 100;
	}
	LastBulletCost = 100;
	SetBounty(TotalBounty);
}

void ULobbyWidget::EvilBulletButtonClicked()
{
}
