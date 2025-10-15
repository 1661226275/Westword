// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CowBoyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API ACowBoyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDWeaponAmmo(int32 Ammo);
	void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

private:
	class ACowBoyHUD* CowboyHUD;
	
};
