// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameEnd.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API UGameEnd : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void MenuSetUp();

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ExpBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KillHunterNum;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KillMonsterNum;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* KillBossNum;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* BountyText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CrystalShard;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Crystal;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Level;

protected:
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnPlayerLeftGame();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ReturnButton;

	UFUNCTION()
	void ReturnButtonClicked();

	UPROPERTY()
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
	class APlayerController* PlayerController;

};
