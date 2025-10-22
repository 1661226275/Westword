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
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* SansBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SansText;

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
	class UImage* HighPingImage;

	UPROPERTY(meta = (BindWidgetAnim),Transient)
	UWidgetAnimation* HighPingAnimation;

};
