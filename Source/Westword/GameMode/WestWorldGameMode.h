// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "WestWorldGameMode.generated.h"

namespace MatchState
{
	extern WESTWORD_API const FName Cooldown;//Ω·À„
}

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
	void PlayerLeftGame(class ACowBoyPlayerState* PlayerLeaving);
	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float MarchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	UPROPERTY(EditDefaultsOnly)
	float CountdownTime = 0.f;
};
