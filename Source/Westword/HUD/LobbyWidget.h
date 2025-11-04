// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Bounty;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CrystalShard;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Crystal;

	void SetBounty(float num);
	void SetCrystalShard(float num);
	void SetCrystal(float num);
	void SetUp();

private:

	float TotalBounty = 1000;
	float TotalCrystalShard = 100;
	float TotalCrystal = 1;
	float LastCowBoyCost = 0.f;
	float LastWeaponCost = 0.f;
	float LastBulletCost = 0.f;


	UPROPERTY(meta = (BindWidget))
	class UButton* CowBoyButton;

	UFUNCTION()
	void CowBoyButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UButton* RoundWeaponButton;
	UFUNCTION()
	void RoundWeaponButtonClicked();
	UPROPERTY(meta = (BindWidget))
	class UButton* RIderWeaponButton;
	UFUNCTION()
	void RIderWeaponButtonClicked();

	UPROPERTY(meta = (BindWidget))
	class UButton* BulletBotton;
	UFUNCTION()
	void BulletBottonClicked();
	UPROPERTY(meta = (BindWidget))
	class UButton* EvilBulletButton;
	UFUNCTION()
	void EvilBulletButtonClicked();
};
