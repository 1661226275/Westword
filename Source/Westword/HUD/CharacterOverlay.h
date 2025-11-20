// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	/*
	* 自己的UI部分
	*/
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SansBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SansText;


	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SelfPlayerName;

	/*
	* 队友的UI部分
	*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* TeammatePlayerCanvasPanel;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* TeammateHealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TeammateHealthText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* TeammateSansBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TeammateSansText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* TeammatePlayerName;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	class UCanvasPanel* RangeWeaponCanvas;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(BlueprintReadWrite,meta = (BindWidget))
	class UCanvasPanel* MeleeWeaponCanvas;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdown;


	UPROPERTY(meta = (BindWidget))
	UTextBlock* EscapeText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EscapeCountdown;
	

	UPROPERTY(meta = (BindWidget))
	class UImage* HighPingImage;

	UPROPERTY(meta = (BindWidgetAnim),Transient)
	UWidgetAnimation* HighPingAnimation;

};
