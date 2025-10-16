// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "WestWorldGameMode.generated.h"

/**
 * 
 */
UCLASS()
class WESTWORD_API AWestWorldGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AWestWorldGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class ACowBoyCharacter* ElimmedCharacter, class ACowBoyPlayerController* VictimController,AController* AttackController);
	virtual void RequestRespawn(class ACharacter* ElimmedCharacter, class AController* ElimmedController);

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	float CountdownTime = 0.f;
};
